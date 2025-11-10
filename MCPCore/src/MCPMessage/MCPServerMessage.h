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
#include "MCPError.h"
#include "MCPMessageType.h"

class MCPServerMessage : public MCPMessage
{
public:
	MCPServerMessage();
	
	//
	MCPServerMessage(const QSharedPointer<MCPContext>& pContext);
	//
	MCPServerMessage(const QSharedPointer<MCPContext>& pContext,
		const QJsonValue& rpcValue);
	//
	MCPServerMessage(const QSharedPointer<MCPContext>& pContext,
		MCPMessageType::Flags enType);
	//
	MCPServerMessage(const QSharedPointer<MCPContext>& pContext,
		const QJsonValue& rpcValue,
		MCPMessageType::Flags enType);

	MCPServerMessage(const QJsonValue& rpcValue,
		MCPMessageType::Flags enType);
public:
	QSharedPointer<MCPContext> getContext() const;
public:
	virtual QByteArray toData() override;
protected:
	QSharedPointer<MCPContext> m_pContext;
protected:
    QJsonValue m_rpcValue;
};
Q_DECLARE_METATYPE(MCPServerMessage*)
Q_DECLARE_METATYPE(QSharedPointer<MCPServerMessage>)

class MCPServerErrorResponse : public MCPServerMessage
{
public:
	// 保留原有构造函数以保持兼容性
	MCPServerErrorResponse(const QSharedPointer<MCPContext>& pContext, int nCode, const QString& strMessage, const QString& strData = QString());

	// 新增接受MCPError对象的构造函数
	MCPServerErrorResponse(const QSharedPointer<MCPContext>& pContext, const MCPError& error);

public:
	QByteArray toData() override;
private:
	QJsonValue m_rpcValue;
};