/**
 * @file MCPClientinitializeMessage.cpp
 * @brief MCP客户端初始化消息实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPClientInitializeMessage.h"
#include <QJsonDocument>
#include <QRegExp>

// mcpMessage 基类实现
MCPClientInitializeMessage::MCPClientInitializeMessage(const MCPClientMessage& clientMessage)
	: MCPClientMessage(clientMessage)
{
	auto params = m_jsonRpc.value("params").toObject();
	m_strClientProtocolVersion = params.value("protocolVersion").toString();
	auto clientInfo = params.value("clientInfo").toObject();
	m_strClientName = clientInfo.value("name").toString();
	m_strClientTitle = clientInfo.value("title").toString();
	m_strClientVersion = clientInfo.value("version").toString();
}

QString MCPClientInitializeMessage::getClientName()
{
	return m_strClientName;
}

QString MCPClientInitializeMessage::getClientTitle()
{
	return m_strClientTitle;
}

QString MCPClientInitializeMessage::getClientVersion()
{
	return m_strClientVersion;
}

QString MCPClientInitializeMessage::getClientProtocolVersion()
{
	return m_strClientProtocolVersion;
}

bool MCPClientInitializeMessage::hasProtocolVersion() const
{
	return !m_strClientProtocolVersion.isEmpty();
}

bool MCPClientInitializeMessage::isProtocolVersionFormatValid() const
{
	// 如果为空，格式无效
	if (m_strClientProtocolVersion.isEmpty())
	{
		return false;
	}
	
	// 验证格式（应该是YYYY-MM-DD格式）
	QRegExp versionRegex("^\\d{4}-\\d{2}-\\d{2}$");
	return versionRegex.exactMatch(m_strClientProtocolVersion);
}

bool MCPClientInitializeMessage::isProtocolVersionSupported(const QStringList& supportedVersions) const
{
	return supportedVersions.contains(m_strClientProtocolVersion);
}


bool MCPClientInitializeMessage::isCapabilitiesValid() const
{
	auto params = m_jsonRpc.value("params").toObject();
	// 如果提供了capabilities，必须是对象
	if (params.contains("capabilities"))
	{
		return params.value("capabilities").isObject();
	}
	// 如果没有提供，也是有效的（可选参数）
	return true;
}

bool MCPClientInitializeMessage::isClientInfoValid() const
{
	auto params = m_jsonRpc.value("params").toObject();
	// 如果提供了clientInfo，必须是对象
	if (params.contains("clientInfo"))
	{
		return params.value("clientInfo").isObject();
	}
	// 如果没有提供，也是有效的（可选参数）
	return true;
}
