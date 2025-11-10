/**
 * @file MCPMessageParser.cpp
 * @brief MCP消息解析器实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPHttpMessageParser.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include "MCPClientinitializeMessage.h"

//这里为子线程操作，虽然不太符合层次，但还是尽量把能处理都在这里处理了
QSharedPointer<MCPClientMessage> MCPHttpMessageParser::genClientMessageFromHttp(const QSharedPointer<MCPHttpRequestData>& pHttpRequestData)
{
	/*
    POST:
        2025-06-18: 
        Recv:
			HTTP_HEADER: 
                    MUST include an Accept header, listing both application/json and text/event-stream as supported content types.
                    {Accept: application/json, text/event-stream}
			HTTP_BODY: A single JSON-RPC request, notification, or response
        SEND:
			response -> accepts:HTTP status code 202 Accepted: with no body.
			            cannot accept:HTTP status code 400(Bad Request): no body or JSON-RPC error response that has no id.
            request  ->
                       SSE:Content-Type: text/event-stream + ONE JSON OBJECT
                       COMMON:Content-Type: application/json + ONE JSON OBJECT
		2025-03-26:
        Recv:
		    HTTP_HEADER:
					MUST include an Accept header, listing both application/json and text/event-stream as supported content types.
					{Accept: application/json, text/event-stream}
            HTTP_BODY: 
                     A single JSON-RPC request, notification, or response
                     An array batching one or more requests and/or notifications
                     An array batching one or more responses
        SEND:
        https://modelcontextprotocol.io/specification/2025-03-26/basic/transports
        If the input contains any number of JSON-RPC requests, 
            the server MUST either return Content-Type: text/event-stream, to initiate an SSE stream, 
            or Content-Type: application/json, to return one JSON object.
		If the input consists solely of (any number of) JSON-RPC responses or notifications:
        If the server accepts the input, the server MUST return HTTP status code 202 Accepted with no body.
        If the server cannot accept the input, it MUST return an HTTP error status code (e.g., 400 Bad Request).
            The HTTP response body MAY comprise a JSON-RPC error response that has no id.
            ......

    //GET
    //DELETE 关闭 2025-06-18
    */
    //
    /*2024-11-15：
        two endpoints:SSE or Regular HTTP POST
        Open SSE connection GET {Accept: text/event-stream}
    
    2025-03-26 - 
        a single HTTP endpoint supports both POST and GET methods.
        MUST POST {Accept: application/json, text/event-stream}
    */
    //GET POST DELETE
    auto strHttpMethod = pHttpRequestData->getMethod();
	auto strConnection = pHttpRequestData->getHeader("connection"); //keep-alive
	//application/json,text/event-stream
	auto strAcceptHeader = pHttpRequestData->getHeader("Accept");
	auto setAcceptTypes = strAcceptHeader.split(",").toSet();
	// 清理空格
	QSet<QString> cleanedAcceptTypes;
	for (const QString& type : setAcceptTypes)
	{
		cleanedAcceptTypes.insert(type.trimmed());
	}
	setAcceptTypes = cleanedAcceptTypes;
	//application/json,text/event-stream
	auto strContentType = pHttpRequestData->getHeader("content-type");

	//sse?Mcp-Session-Id=123456789 NOT 2025-06-18
	auto strQuerySessionId = pHttpRequestData->getQueryParameter("Mcp-Session-Id");
	auto strMcpSessionId = pHttpRequestData->getHeader("Mcp-Session-Id");
	auto strProtocolVersion = pHttpRequestData->getHeader("MCP-Protocol-Version");

	//1、先把偷摸进来的http干掉 - 这里先固定，不支持设置了
    auto strPath = pHttpRequestData->getPath();
    if (strPath != "/sse" && strPath != "/mcp")
    {
		return QSharedPointer<MCPClientMessage>();
    }
    
    // 验证Accept头（对于POST请求，必须包含application/json和text/event-stream）
    if (strHttpMethod == "POST")
    {
        static QSet<QString> setRequiredTypes = {"application/json", "text/event-stream"};
        if (!setRequiredTypes.contains(setAcceptTypes) //2015-06-18
            && !setAcceptTypes.contains("*/*"))
        {
            // Accept头不符合MCP规范要求
            return QSharedPointer<MCPClientMessage>();
        }
    }

	//2、再把协议暂时不支持的干掉
    /* 断流恢复 - 2025-03-26
	https://modelcontextprotocol.io/specification/2025-03-26/basic/transports#resumability-and-redelivery
	设计Last-Event-ID = SessionId + EventId-123，从这个里面解析出 SessionId和EventId 一点都支持不了 太麻烦了乱糟糟的 还没有啥用
    */
	auto strLastEventId = pHttpRequestData->getHeader("Last-Event-ID");//Last-Event-ID
	if (strHttpMethod == "GET" && !strLastEventId.isEmpty())
	{
		return QSharedPointer<MCPClientMessage>();
	}
    /* 客户端关闭 - 2025-03-26
    https://modelcontextprotocol.io/specification/2025-03-26/basic/transports#streamable-http
    没太明白意义，关就关吧，感觉也没啥用
    */
	if (strHttpMethod == "DELETE")
	{
		return QSharedPointer<MCPClientMessage>();
	}

	auto pClientMessage = QSharedPointer<MCPClientMessage>::create(MCPMessageType::None);

    pClientMessage->m_strMcpSessionId = strQuerySessionId.isEmpty() ? strMcpSessionId : strQuerySessionId;
    //
    //为了兼容[2024-11-15]先判断是否是SSE的连上来了 - 只有使用SSE链接的才会这么干
    //https://modelcontextprotocol.io/specification/2024-11-05/basic/transports
	if (strHttpMethod == "GET" 
        && strQuerySessionId.isEmpty() // no sse sessionid
        && strMcpSessionId.isEmpty() //no mcp sessionid
        && strLastEventId.isEmpty() // 不是重连
        && setAcceptTypes.size() == 1 && setAcceptTypes.contains("text/event-stream") //only sse
        && strConnection == "keep-alive") //sse keep-alive
    {
        //这个connect 让也模拟 模拟下rpc调用
        pClientMessage->m_jsonRpc.insert("method", "connect");
		pClientMessage->appendType(MCPMessageType::SseTransport | MCPMessageType::Connect);
        return pClientMessage;
    }
    //批量操作放弃支持 - 2025-06-18已经明确放弃了
    //https://modelcontextprotocol.io/specification/2025-03-26/basic/transports#streamable-http
    if (strHttpMethod == "POST" && strContentType == "application/json")
    {
	    auto jsonRpc = QJsonDocument::fromJson(pHttpRequestData->getBody()).object();
        
        // 验证JSON-RPC版本字段（必须是字符串"2.0"）
        QJsonValue jsonrpcValue = jsonRpc.value("jsonrpc");
        if (!jsonrpcValue.isString() || jsonrpcValue.toString() != "2.0")
        {
            // JSON-RPC版本字段缺失或格式错误
            return QSharedPointer<MCPClientMessage>();
        }
        
        auto strXMethodName = jsonRpc.value("method").toString();
        auto bRequest = jsonRpc.contains("id") && jsonRpc.contains("method");
		auto bResponse = jsonRpc.contains("id") && ((jsonRpc.contains("result") + jsonRpc.contains("error")) == 1);
        auto bNotification = !jsonRpc.contains("id");
        if (bRequest || bResponse || bNotification)
        {
			pClientMessage->m_jsonRpc = jsonRpc;
            //
			bRequest&& pClientMessage->appendType(MCPMessageType::Request);
            bResponse && pClientMessage->appendType(MCPMessageType::Response);
            bNotification && pClientMessage->appendType(MCPMessageType::Notification);
			//
			pClientMessage->appendType(strQuerySessionId.isEmpty() 
                ? MCPMessageType::StreamableTransport 
                : MCPMessageType::SseTransport);
			//这个先放在这里吧
            return genXXClientMessage(pClientMessage);
        }
	}

	return QSharedPointer<MCPClientMessage>();
}

QSharedPointer<MCPClientMessage> MCPHttpMessageParser::genXXClientMessage(const QSharedPointer<MCPClientMessage>& pClientMessage)
{
    if (pClientMessage->getMethodName() == "ping")
    {
        pClientMessage->appendType(MCPMessageType::Ping);
    }
    if (pClientMessage->getMethodName() == "initialize")
    {
        pClientMessage->appendType(MCPMessageType::Initialize);
        return QSharedPointer<MCPClientInitializeMessage>::create(*pClientMessage);
    }
    return pClientMessage;
}
