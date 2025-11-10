/**
 * @file MCServerRpcMessage.cpp
 * @brief MCP服务器RPC消息实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPServerMessage.h"
#include "MCPClientMessage.h"
#include "MCPError.h"

MCPServerMessage::MCPServerMessage()
	: MCPMessage(MCPMessageType::None)
	, m_pContext(nullptr)
{

}

MCPServerMessage::MCPServerMessage(const QSharedPointer<MCPContext>& pContext)
	: MCPServerMessage(pContext, QJsonObject(), MCPMessageType::None)
{

}

MCPServerMessage::MCPServerMessage(const QSharedPointer<MCPContext>& pContext, const QJsonValue& rpcValue)
	: MCPServerMessage(pContext, rpcValue, MCPMessageType::None)
{

}

MCPServerMessage::MCPServerMessage(const QSharedPointer<MCPContext>& pContext, MCPMessageType::Flags enType)
	: MCPServerMessage(pContext, QJsonObject(), enType)
{

}

MCPServerMessage::MCPServerMessage(const QSharedPointer<MCPContext>& pContext,
	const QJsonValue& rpcValue, MCPMessageType::Flags enType)
	: MCPMessage(enType)
	, m_pContext(pContext)
{
	auto enClientMessageType = (m_pContext && m_pContext->getClientMessage())
		? m_pContext->getClientMessage()->getType() : MCPMessageType::None;
	appendType(enClientMessageType & MCPMessageType::TransportMask);
	//
	if (enClientMessageType & MCPMessageType::Request)
	{
		appendType(MCPMessageType::Response);
		auto pRpcRequest = m_pContext->getClientMessage().staticCast<MCPClientMessage>();
		m_rpcValue = QJsonObject{
		{"jsonrpc", "2.0"},
		{"id", pRpcRequest->getMethodId()},
		{"result", 	rpcValue} };
	}
	else if (enClientMessageType & MCPMessageType::Notification)
	{
		appendType(MCPMessageType::Notification);
		// 如果rpcValue是对象且包含method字段，说明是服务器主动发送的通知
		if (rpcValue.isObject())
		{
			QJsonObject objValue = rpcValue.toObject();
			if (objValue.contains("method"))
			{
				// 服务器主动发送的通知消息
				m_rpcValue = QJsonObject{
					{"jsonrpc", "2.0"},
					{"method", objValue.value("method")},
					{"params", objValue.value("params")}
				};
			}
			else
			{
				// 客户端通知的响应
				m_rpcValue = QJsonObject{
					{"jsonrpc", "2.0"},
					{"result", rpcValue}
				};
			}
		}
		else
		{
			m_rpcValue = QJsonObject{
				{"jsonrpc", "2.0"},
				{"result", rpcValue}
			};
		}
	}
	else if (enClientMessageType & MCPMessageType::Response)
	{
		appendType(MCPMessageType::Notification);
		m_rpcValue = QJsonObject{
		{"jsonrpc", "2.0"},
		{"result", 	rpcValue} };
	}
}


QSharedPointer<MCPContext > MCPServerMessage::getContext() const
{
	return m_pContext;
}

QByteArray MCPServerMessage::toData()
{
	auto data = m_rpcValue.isArray()
		? QJsonDocument(m_rpcValue.toArray()).toJson(QJsonDocument::Compact)
		: QJsonDocument(m_rpcValue.toObject()).toJson(QJsonDocument::Compact);
	return data;
}

MCPServerErrorResponse::MCPServerErrorResponse(const QSharedPointer<MCPContext>& pContext, int nCode, const QString& strMessage, const QString& strData)
	: MCPServerMessage(pContext)
{
	auto pRpcRequest = pContext->getClientMessage().staticCast<MCPClientMessage>();
	QJsonObject objError{
	{"code", nCode},
	{"message", strMessage}
	};

	if (!strData.isEmpty())
	{
		objError["data"] = strData;
	}

	m_rpcValue = QJsonObject{
		{"jsonrpc", "2.0"},
		{"id", pRpcRequest->getMethodId()},
		{"error", objError}
	};
}

QByteArray MCPServerErrorResponse::toData()
{
    auto data = m_rpcValue.isArray()
        ? QJsonDocument(m_rpcValue.toArray()).toJson(QJsonDocument::Compact)
        : QJsonDocument(m_rpcValue.toObject()).toJson(QJsonDocument::Compact);
    return data;
}

MCPServerErrorResponse::MCPServerErrorResponse(const QSharedPointer<MCPContext>& pContext, const MCPError& error)
    : MCPServerMessage(pContext)
{
    auto pRpcRequest = pContext->getClientMessage().staticCast<MCPClientMessage>();

    m_rpcValue = QJsonObject{
        {"jsonrpc", "2.0"},
        {"id", pRpcRequest->getMethodId()},
        {"error", error.toJson()}
    };
}
