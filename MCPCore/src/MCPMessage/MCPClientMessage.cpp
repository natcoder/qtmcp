/**
 * @file MCPClientMessage.cpp
 * @brief MCP客户端消息实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPClientMessage.h"
#include <QJsonDocument>

// mcpMessage 基类实现
MCPClientMessage::MCPClientMessage(MCPMessageType::Flags enMessageType)
	: MCPMessage(enMessageType)
{

}

MCPClientMessage::~MCPClientMessage()
{

}

QString MCPClientMessage::getSessionId()
{
	return m_strMcpSessionId;
}

QJsonValue MCPClientMessage::getMethodId()
{
	auto jsonId = m_jsonRpc.value("id");
	return jsonId;
}

QString MCPClientMessage::getMethodName()
{
	auto strMethodName = m_jsonRpc.value("method").toString();
	return strMethodName;
}

QJsonValue MCPClientMessage::getParmams()
{
	return m_jsonRpc.value("params");
}