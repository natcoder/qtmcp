#pragma once
#include <QObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QSet>
#include <QString>
#include <QSharedPointer>
#include "MCPSession.h"
#include "MCPClientMessage.h"
class MCPContext
{
public:
	MCPContext(quint64 nConnectionId, const QSharedPointer<MCPSession>& pSession, const QSharedPointer<MCPClientMessage>& pClientRequest);
public:
	quint64 getConnectionId() const ;
	QSharedPointer<MCPClientMessage> getClientMessage() const ;
	QSharedPointer<MCPSession> getSession() const;
private:
	quint64 m_nConnectionId;
	const QSharedPointer<MCPClientMessage> m_pClientMessage;
	const QSharedPointer<MCPSession> m_pSession;
};

