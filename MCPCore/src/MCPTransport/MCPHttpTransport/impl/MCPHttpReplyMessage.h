#pragma once
#include <QObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QSet>
#include <QString>
#include <QSharedPointer>
#include "MCPMessage.h"
#include "MCPSession.h"
#include "MCPRouting/MCPContext.h"
#include "MCPServerMessage.h"

class MCPHttpReplyMessage : public MCPServerMessage
{
public:
	MCPHttpReplyMessage(const QSharedPointer<MCPServerMessage>& pServerMessage, MCPMessageType::Flags flags);
public:
	static QSharedPointer<MCPHttpReplyMessage> CreateSseAcceptNotification();
	static QSharedPointer<MCPHttpReplyMessage> CreateStreamableAcceptNotification();
public:
	virtual QByteArray toData() override;
private:
	QByteArray toSseConnectResponseData();
	QByteArray toSseRequestData();
	QByteArray toSseNotificationData();
	//
	QByteArray toStreamableConnectData();
	QByteArray toStreamableRequestData();
	QByteArray toStreamableNotificationData();
private:
	QByteArray toSseChannelData();
	QByteArray toAcceptData();
protected:
	MCPMessageType::Flags m_flags;
	QSharedPointer<MCPServerMessage>  m_pServerMessage;
};
