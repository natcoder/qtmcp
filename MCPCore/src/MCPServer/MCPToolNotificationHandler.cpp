/**
 * @file MCPToolNotificationHandler.cpp
 * @brief MCP工具通知处理器实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPToolNotificationHandler.h"
#include "MCPServer.h"
#include "MCPTools/MCPToolService.h"
#include "MCPLog/MCPLog.h"

MCPToolNotificationHandler::MCPToolNotificationHandler(MCPServer* pServer, QObject* pParent)
    : MCPNotificationHandlerBase(pServer, pParent)
{
}

MCPToolNotificationHandler::~MCPToolNotificationHandler()
{
}

void MCPToolNotificationHandler::onToolsListChanged()
{
    MCP_CORE_LOG_INFO() << "MCPToolNotificationHandler: 工具列表变化，向所有客户端发送通知";
    
    // 获取最新的工具列表
    QJsonArray arrTools = m_pServer->getToolService()->list();
    
    // 构建通知参数
    QJsonObject params;
    params["tools"] = arrTools;
    
    // 广播通知
    broadcastNotification("notifications/tools/list_changed", params);
    
    MCP_CORE_LOG_INFO() << "MCPToolNotificationHandler: 工具列表变化通知处理完成";
}

