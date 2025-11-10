/**
 * @file MCPHttpResponseBuilder.cpp
 * @brief HTTP响应构建器实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPHttpResponseBuilder.h"
#include "MCPSession/MCPSession.h"
#include <QSharedPointer>

QByteArray MCPHttpResponseBuilder::buildSseConnectResponse(const QString& strSessionUri)
{
    QByteArray arrResponse;
    arrResponse.append(buildSseHeaders());
    arrResponse.append("\r\n");
    
    arrResponse.append("event: endpoint");
    arrResponse.append("\n");
    arrResponse.append("data: " + strSessionUri.toUtf8());
    arrResponse.append("\n\n");
    
    return arrResponse;
}

QByteArray MCPHttpResponseBuilder::buildSseMessageResponse(const QByteArray& strMessageData)
{
    QByteArray arrResponse;
    arrResponse.append(buildSseHeaders());
    arrResponse.append("\r\n");
    
    arrResponse.append("event: message");
    arrResponse.append("\n");
    arrResponse.append("data: ");
    arrResponse.append(strMessageData);
    arrResponse.append("\n\n");
    
    return arrResponse;
}

QByteArray MCPHttpResponseBuilder::buildStreamableResponse(const QByteArray& strMessageData, const QSharedPointer<MCPSession>& pSession)
{
    QString strSessionId = pSession ? pSession->getSessionId() : QString();
    QString strProtocolVersion = pSession ? pSession->getProtocolVersion() : QString();
    
    QByteArray arrResponse;
    arrResponse.append(buildStreamableHeaders(strMessageData.size(), strSessionId, strProtocolVersion));
    arrResponse.append("\r\n");
    arrResponse.append(strMessageData);
    
    return arrResponse;
}

QByteArray MCPHttpResponseBuilder::buildAcceptResponse()
{
    QByteArray arrResponse;
    arrResponse.append(QString("HTTP/1.1 %1 %2\r\n").arg(202).arg("Accepted").toUtf8());
    arrResponse.append("Content-Length: 0\r\n");
    arrResponse.append("Connection: keep-alive\r\n");
    arrResponse.append(buildCorsHeaders());
    arrResponse.append("\r\n");
    
    return arrResponse;
}

QByteArray MCPHttpResponseBuilder::buildSseHeaders()
{
    QByteArray arrHeaders;
    arrHeaders.append("HTTP/1.1 200 OK\r\n");
    arrHeaders.append("Content-Type: text/event-stream\r\n");
    arrHeaders.append("Cache-Control: no-cache\r\n");
    arrHeaders.append("Connection: keep-alive\r\n");
    arrHeaders.append(buildCorsHeaders());
    
    return arrHeaders;
}

QByteArray MCPHttpResponseBuilder::buildStreamableHeaders(int nContentLength, const QString& strSessionId, const QString& strProtocolVersion)
{
    QByteArray arrHeaders;
    arrHeaders.append("HTTP/1.1 200 OK\r\n");
    arrHeaders.append("Content-Type: application/json\r\n");
    arrHeaders.append(QString("Content-Length: %1\r\n").arg(nContentLength).toUtf8());
    
    if (!strSessionId.isEmpty())
    {
        arrHeaders.append(QString("Mcp-Session-Id: %1\r\n").arg(strSessionId).toUtf8());
    }
    
    if (!strProtocolVersion.isEmpty())
    {
        arrHeaders.append(QString("MCP-Protocol-Version: %1\r\n").arg(strProtocolVersion).toUtf8());
    }
    
    arrHeaders.append("Connection: keep-alive\r\n");
    arrHeaders.append(buildCorsHeaders());
    
    return arrHeaders;
}

QByteArray MCPHttpResponseBuilder::buildCorsHeaders()
{
    QByteArray arrCors;
    arrCors.append("Access-Control-Allow-Origin: *\r\n");
    arrCors.append("Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n");
    arrCors.append("Access-Control-Allow-Headers: Content-Type, Authorization, X-Requested-With\r\n");
    arrCors.append("Access-Control-Expose-Headers: Content-Length, Content-Range\r\n");
    
    return arrCors;
}

