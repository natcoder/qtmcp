/**
 * @file MCPPromptNotificationHandler.cpp
 * @brief MCP提示词通知处理器实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPPromptNotificationHandler.h"
#include "MCPServer.h"
#include "MCPPrompt/MCPPromptService.h"
#include "MCPLog/MCPLog.h"

MCPPromptNotificationHandler::MCPPromptNotificationHandler(MCPServer* pServer, QObject* pParent)
    : MCPNotificationHandlerBase(pServer, pParent)
{
}

MCPPromptNotificationHandler::~MCPPromptNotificationHandler()
{
}

void MCPPromptNotificationHandler::onPromptsListChanged()
{
    MCP_CORE_LOG_INFO() << "MCPPromptNotificationHandler: 提示词列表变化，向所有客户端发送通知";
    
    // 获取最新的提示词列表
    QJsonArray arrPrompts = m_pServer->getPromptService()->list();
    
    // 构建通知参数
    QJsonObject params;
    params["prompts"] = arrPrompts;
    
    // 广播通知
    broadcastNotification("notifications/prompts/list_changed", params);
    
    MCP_CORE_LOG_INFO() << "MCPPromptNotificationHandler: 提示词列表变化通知处理完成";
}

