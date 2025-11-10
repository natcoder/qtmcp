#pragma once
#include <QObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QSet>
#include <QString>
#include <QSharedPointer>
#include "MCPMessageType.h"
/**
 * @brief MCP 消息基类
 * 
 * 职责：
 * - 定义MCP消息的通用接口
 * - 提供JSON序列化/反序列化
 * - 管理消息ID和类型
 * 
 */

/**
 * @brief 协议版本枚举
 */
class MCPMessage
{
public:
	explicit MCPMessage(MCPMessageType::Flags enMessageType = MCPMessageType::None);
	virtual ~MCPMessage() {}
public:
	MCPMessageType::Flags getType();
	MCPMessageType::Flags appendType(MCPMessageType::Flags enType);
public:
	virtual QByteArray toData();
protected:
	MCPMessageType::Flags m_enType;
};
Q_DECLARE_METATYPE(MCPMessage*)
Q_DECLARE_METATYPE(QSharedPointer<MCPMessage>)