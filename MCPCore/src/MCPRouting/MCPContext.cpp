/**
 * @file MCPContext.cpp
 * @brief MCP上下文实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPContext.h"

MCPContext::MCPContext(quint64 nConnectionId, const QSharedPointer<MCPSession>& pSession, const QSharedPointer<MCPClientMessage>& pClientMessage)
	: m_nConnectionId(nConnectionId)
	, m_pSession(pSession)
	, m_pClientMessage(pClientMessage)
{

}

quint64 MCPContext::getConnectionId() const
{
	return m_nConnectionId;
}

QSharedPointer<MCPClientMessage> MCPContext::getClientMessage() const
{
	return m_pClientMessage;
}

QSharedPointer<MCPSession> MCPContext::getSession() const
{
	return m_pSession;
}

