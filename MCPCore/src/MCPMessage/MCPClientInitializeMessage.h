#pragma once
#include <QObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QSet>
#include <QString>
#include <QSharedPointer>
#include "MCPClientMessage.h"

class MCPClientInitializeMessage : public MCPClientMessage
{
public:
	explicit MCPClientInitializeMessage(const MCPClientMessage&clientMessage);
public:
	//
	QString getClientName();
	QString getClientTitle();
	QString getClientVersion();
	//
	QString getClientProtocolVersion();
	
	/**
	 * @brief 检查是否有protocolVersion参数
	 * @return true表示存在，false表示不存在
	 */
	bool hasProtocolVersion() const;
	
	/**
	 * @brief 检查protocolVersion格式是否有效（YYYY-MM-DD格式）
	 * @return true表示格式有效，false表示格式无效
	 */
	bool isProtocolVersionFormatValid() const;
	
	/**
	 * @brief 检查协议版本是否在支持列表中
	 * @param supportedVersions 支持的协议版本列表
	 * @return true表示支持，false表示不支持
	 */
	bool isProtocolVersionSupported(const QStringList& supportedVersions) const;
	
	
	/**
	 * @brief 检查capabilities参数是否有效（如果提供，必须是对象）
	 * @return true表示有效，false表示无效
	 */
	bool isCapabilitiesValid() const;
	
	/**
	 * @brief 检查clientInfo参数是否有效（如果提供，必须是对象）
	 * @return true表示有效，false表示无效
	 */
	bool isClientInfoValid() const;
	
protected:
	QString m_strClientName;
	QString m_strClientTitle;
	QString m_strClientVersion;
	//
	QString m_strClientProtocolVersion;
private:
	friend class MCPHttpRequestData;
	friend class MCPHttpMessageParser;
};