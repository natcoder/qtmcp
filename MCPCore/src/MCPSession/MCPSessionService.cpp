/**
 * @file MCPSessionService.cpp
 * @brief MCP会话管理器实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPSessionService.h"
#include "MCPLog.h"
#include "MCPSession.h"
#include <QDateTime>
#include "MCPClientMessage.h"

MCPSessionService::MCPSessionService(QObject* parent)
    : QObject(parent)
{
}

MCPSessionService::~MCPSessionService()
{

}

void MCPSessionService::removeSessionBySSEConnectId(quint64 nConnectionId)
{

}

QSharedPointer<MCPSession> MCPSessionService::getSession(quint64 nConnectionId, const QSharedPointer<MCPClientMessage> pClientMessage)
{
    auto strSessionId = pClientMessage->getSessionId();
    if (auto pSession = m_dictSessions.value(strSessionId))
    {
        return pSession;
    }
    if (strSessionId.isEmpty())
    {
		auto enMsgType = pClientMessage->getType();
		if ((enMsgType & MCPMessageType::SseTransport) && (enMsgType & MCPMessageType::Connect))
		{
			auto pSeesion = QSharedPointer<MCPSession>::create();
			pSeesion->setSseConnectionId(nConnectionId);
			pSeesion->setTransportType(false);  // SSE传输
			m_dictSessions[pSeesion->getSessionId()] = pSeesion;
			return pSeesion;
		}
		else if ((enMsgType & MCPMessageType::StreamableTransport) && (enMsgType & MCPMessageType::Initialize))
		{
			auto pSeesion = QSharedPointer<MCPSession>::create();
			pSeesion->setTransportType(true);  // StreamableTransport传输
			pSeesion->setConnectionId(nConnectionId);  // 存储连接ID
			m_dictSessions[pSeesion->getSessionId()] = pSeesion;
			return pSeesion;
		}
		else if (enMsgType & MCPMessageType::Ping)
		{
			auto pSeesion = QSharedPointer<MCPSession>::create();
			return pSeesion;
		}
    }
	return QSharedPointer<MCPSession>();
}

QList<quint64> MCPSessionService::getAllActiveConnectionIds() const
{
    QList<quint64> connectionIds;
    for (const auto& pSession : m_dictSessions.values())
    {
        if (pSession && pSession->getSseConnectionId() > 0)
        {
            connectionIds.append(pSession->getSseConnectionId());
        }
    }
    return connectionIds;
}

QSharedPointer<MCPSession> MCPSessionService::getSessionBySessionId(const QString& strSessionId) const
{
    return m_dictSessions.value(strSessionId);
}

QSharedPointer<MCPSession> MCPSessionService::getSessionByConnectionId(quint64 nConnectionId) const
{
    // 首先尝试通过SSE连接ID查找
    for (const auto& pSession : m_dictSessions.values())
    {
        if (pSession)
        {
            if (pSession->getSseConnectionId() == nConnectionId)
            {
                return pSession;
            }
            // 也检查StreamableTransport的连接ID
            if (pSession->isStreamableTransport() && pSession->getConnectionId() == nConnectionId)
            {
                return pSession;
            }
        }
    }
    return QSharedPointer<MCPSession>();
}

QList<QSharedPointer<MCPSession>> MCPSessionService::getAllSessions() const
{
    return m_dictSessions.values();
}