/**
 * @file MCPServerHttpMessage.cpp
 * @brief MCP服务器HTTP消息实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPHttpReplyMessage.h"
#include "MCPRouting/MCPContext.h"
#include "MCPHttpResponseBuilder.h"

MCPHttpReplyMessage::MCPHttpReplyMessage(const QSharedPointer<MCPServerMessage>& pServerMessage, MCPMessageType::Flags flags)
	: m_pServerMessage(pServerMessage)
	, m_flags(flags)
{
	if (pServerMessage != nullptr)
	{
		m_pContext = pServerMessage->getContext();
	}
}

QSharedPointer<MCPHttpReplyMessage> MCPHttpReplyMessage::CreateSseAcceptNotification()
{
	return QSharedPointer<MCPHttpReplyMessage>::create(QSharedPointer<MCPServerMessage>(), MCPMessageType::SseTransport | MCPMessageType::ResponseNotification);
}

QSharedPointer<MCPHttpReplyMessage> MCPHttpReplyMessage::CreateStreamableAcceptNotification()
{
	return QSharedPointer<MCPHttpReplyMessage>::create(QSharedPointer<MCPServerMessage>(), MCPMessageType::StreamableTransport | MCPMessageType::ResponseNotification);
}

QByteArray MCPHttpReplyMessage::toData()
{
	if (m_flags & MCPMessageType::Connect)
	{
		return toSseConnectResponseData();
	}

	if (m_flags & MCPMessageType::SseTransport)
	{
		if (m_flags & MCPMessageType::Response)
		{
			return toSseChannelData();
		}
		else if (m_flags & MCPMessageType::ResponseNotification)
		{
			return toAcceptData();
		}
		else if (m_flags & MCPMessageType::RequestNotification)
		{
			return toSseChannelData();
		}
	}

	if (m_flags & MCPMessageType::StreamableTransport)
	{
		if (m_flags & MCPMessageType::Response)
		{
			return toStreamableConnectData();
		}
		else if (m_flags & MCPMessageType::ResponseNotification)
		{
			return toAcceptData();
		}
		else if (m_flags & MCPMessageType::RequestNotification)
		{
			return toStreamableNotificationData();
		}
	}

	return toAcceptData();
}

QByteArray MCPHttpReplyMessage::toSseConnectResponseData()
{
	if (m_pServerMessage == nullptr || m_pServerMessage->getContext() == nullptr)
	{
		return QByteArray();
	}

	auto pSession = m_pServerMessage->getContext()->getSession();
	if (pSession == nullptr)
	{
		return QByteArray();
	}

	QString strSessionUri = "/sse?Mcp-Session-Id=" + pSession->getSessionId();
	return MCPHttpResponseBuilder::buildSseConnectResponse(strSessionUri);
}

QByteArray MCPHttpReplyMessage::toSseChannelData()
{
	if (m_pServerMessage == nullptr)
	{
		return QByteArray();
	}

	return MCPHttpResponseBuilder::buildSseMessageResponse(m_pServerMessage->toData());
}

QByteArray MCPHttpReplyMessage::toSseRequestData()
{
	return QByteArray();
}

QByteArray MCPHttpReplyMessage::toSseNotificationData()
{
	return toAcceptData();
}

QByteArray MCPHttpReplyMessage::toStreamableConnectData()
{
	if (m_pServerMessage == nullptr || m_pServerMessage->getContext() == nullptr)
	{
		return QByteArray();
	}

	auto pSession = m_pServerMessage->getContext()->getSession();
	if (pSession == nullptr)
	{
		return QByteArray();
	}

	auto rpcResponseData = m_pServerMessage->toData();
	return MCPHttpResponseBuilder::buildStreamableResponse(rpcResponseData, pSession);
}

QByteArray MCPHttpReplyMessage::toStreamableRequestData()
{
	return QByteArray();
}

QByteArray MCPHttpReplyMessage::toStreamableNotificationData()
{
	if (m_pServerMessage == nullptr)
	{
		return QByteArray();
	}

	auto pContext = m_pServerMessage->getContext();
	if (pContext == nullptr)
	{
		return QByteArray();
	}

	auto pSession = pContext->getSession();
	if (pSession == nullptr)
	{
		return QByteArray();
	}

	auto rpcResponseData = m_pServerMessage->toData();
	return MCPHttpResponseBuilder::buildStreamableResponse(rpcResponseData, pSession);
}

QByteArray MCPHttpReplyMessage::toAcceptData()
{
	return MCPHttpResponseBuilder::buildAcceptResponse();
}
