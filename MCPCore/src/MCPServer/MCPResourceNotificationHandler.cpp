/**
 * @file MCPResourceNotificationHandler.cpp
 * @brief MCP资源通知处理器实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPResourceNotificationHandler.h"
#include "MCPServer.h"
#include "MCPResource/MCPResourceService.h"
#include "MCPResource/MCPResource.h"
#include "MCPLog/MCPLog.h"

MCPResourceNotificationHandler::MCPResourceNotificationHandler(MCPServer* pServer, QObject* pParent)
    : MCPNotificationHandlerBase(pServer, pParent)
{
}

MCPResourceNotificationHandler::~MCPResourceNotificationHandler()
{
}

void MCPResourceNotificationHandler::onResourceContentChanged(const QString& strUri)
{
    MCP_CORE_LOG_INFO() << "MCPResourceNotificationHandler: 资源内容变化，通知订阅者:" << strUri;
    
    // 获取订阅该URI的所有会话ID
	auto pResourceService = m_pServer->getResourceService();
	QSet<QString> lstSubscribedSessionIds = pResourceService->getSubscribedSessionIds(strUri);
    if (lstSubscribedSessionIds.isEmpty())
    {
        MCP_CORE_LOG_DEBUG() << "MCPResourceNotificationHandler: URI没有订阅者:" << strUri;
        return;
    }
    
    // 读取资源内容和元数据
    QJsonObject resourceInfo = pResourceService->readResource(strUri);
    // 获取资源元数据（name、description、mimeType）
    auto pResource = pResourceService->getResource(strUri);
	QJsonObject metadata = pResource->getMetadata();
	resourceInfo["name"] = metadata["name"];
	resourceInfo["description"] = metadata["description"];
	resourceInfo["mimeType"] = metadata["mimeType"];
    
    // 构建通知参数
    QJsonObject params;
    params["uri"] = strUri;
    QJsonObject resourceData;
    resourceData["resource"] = resourceInfo;
    params["data"] = resourceData;
    
    // 发送通知到订阅者
    // 根据 MCP 协议规范，资源更新通知方法名是 "notifications/resources/updated"
    sendNotificationToSubscribers("notifications/resources/updated", params, lstSubscribedSessionIds);
    
    MCP_CORE_LOG_INFO() << "MCPResourceNotificationHandler: URI" << strUri 
                       << "的内容变化通知已处理，共" << lstSubscribedSessionIds.size() << "个订阅者";
}

void MCPResourceNotificationHandler::onResourceDeleted(const QString& strUri)
{
    if (strUri.isEmpty())
    {
        return;
    }
    
    auto pResourceService = m_pServer->getResourceService();
    
    MCP_CORE_LOG_INFO() << "MCPResourceNotificationHandler: 资源删除，通知订阅者:" << strUri;
    
    // 获取订阅该URI的所有会话ID
    QSet<QString> subscribedSessionIds = pResourceService->getSubscribedSessionIds(strUri);
    if (subscribedSessionIds.isEmpty())
    {
        MCP_CORE_LOG_DEBUG() << "MCPResourceNotificationHandler: URI没有订阅者:" << strUri;
        return;
    }
    
    // 构建删除通知参数
    QJsonObject params;
    params["uri"] = strUri;
    QJsonObject resourceData;
    resourceData["deleted"] = true;
    params["data"] = resourceData;
    
    // 发送通知到订阅者
    // 根据 MCP 协议规范，资源更新通知方法名是 "notifications/resources/updated"
    sendNotificationToSubscribers("notifications/resources/updated", params, subscribedSessionIds);
    
    MCP_CORE_LOG_INFO() << "MCPResourceNotificationHandler: URI" << strUri 
                       << "的删除通知已处理，共" << subscribedSessionIds.size() << "个订阅者";
}

void MCPResourceNotificationHandler::onResourcesListChanged()
{
    auto pResourceService = m_pServer->getResourceService();
    
    MCP_CORE_LOG_INFO() << "MCPResourceNotificationHandler: 资源列表变化，向所有客户端发送通知";
    
    // 获取最新的资源列表
    QJsonArray arrResources = pResourceService->list();
    
    // 构建通知参数
    QJsonObject params;
    params["resources"] = arrResources;
    
    // 广播通知
    broadcastNotification("notifications/resources/list_changed", params);
    
    MCP_CORE_LOG_INFO() << "MCPResourceNotificationHandler: 资源列表变化通知处理完成";
}

