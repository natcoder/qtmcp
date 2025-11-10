/**
 * @file MCPMessage.cpp
 * @brief MCP消息基类实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPMessage.h"
MCPMessage::MCPMessage(MCPMessageType::Flags enMessageType /*= MCPRequestType::None*/)
	: m_enType(enMessageType)
{

}

MCPMessageType::Flags MCPMessage::getType()
{
	return m_enType;
}

MCPMessageType::Flags MCPMessage::appendType(MCPMessageType::Flags enType)
{
	m_enType |= enType;
	return m_enType;
}

QByteArray MCPMessage::toData()
{
	return QByteArray();
}
