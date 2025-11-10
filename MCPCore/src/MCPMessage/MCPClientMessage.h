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
class MCPClientMessage : public MCPMessage
{
public:
	explicit MCPClientMessage(MCPMessageType::Flags enMessageType);
	virtual ~MCPClientMessage();
public:
	QString getSessionId();
public:
	QJsonValue getMethodId();
	QString getMethodName();
	QJsonValue getParmams();
protected:
	QString m_strMcpSessionId;
	QString m_strProtocolVersion;
protected:
	QJsonObject m_jsonRpc;
private:
	friend class MCPHttpRequestData;
	friend class MCPHttpMessageParser;
};