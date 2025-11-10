/**
 * @file MCPServerHandler.cpp
 * @brief MCP服务器业务处理器类实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPServerHandler.h"
#include "MCPServer.h"
#include "MCPMessage/MCPMessage.h"
#include "MCPMessage/MCPClientMessage.h"
#include "MCPMessage/MCPServerMessage.h"
#include "MCPMessage/MCPMessageType.h"
#include "MCPMessage/MCPMessageSender.h"
#include "MCPTransport/IMCPTransport.h"
#include "MCPSession/MCPSessionService.h"
#include "MCPRouting/MCPRequestDispatcher.h"
#include "MCPRouting/MCPContext.h"
#include "MCPResource/MCPResourceService.h"
#include "MCPResource/MCPResource.h"
#include "MCPTools/MCPToolService.h"
#include "MCPPrompt/MCPPromptService.h"
#include "MCPTransport/MCPHttpTransport/impl/MCPHttpReplyMessage.h"
#include "MCPLog/MCPLog.h"
#include "MCPSession/MCPSession.h"
#include "MCPSession/MCPPendingNotification.h"
#include "MCPResourceNotificationHandler.h"
#include "MCPToolNotificationHandler.h"
#include "MCPPromptNotificationHandler.h"
#include <QJsonArray>

MCPServerHandler::MCPServerHandler(MCPServer* pServer,
                                   QObject* pParent)
    : QObject(pParent)
    , m_pServer(pServer)
    , m_pRequestDispatcher(nullptr)
    , m_pMessageSender(nullptr)
    , m_pResourceNotificationHandler(nullptr)
    , m_pToolNotificationHandler(nullptr)
    , m_pPromptNotificationHandler(nullptr)
{
    // 创建消息发送器（统一处理消息发送逻辑）
    m_pMessageSender = new MCPMessageSender(pServer->getTransport(), this);
    
    // 创建请求调度器（负责路由和分发客户端请求）
    m_pRequestDispatcher = new MCPRequestDispatcher(pServer, this);
    
    // 连接请求调度器的服务器消息信号到本Handler
    QObject::connect(m_pRequestDispatcher, &MCPRequestDispatcher::serverMessageReceived,
                     this, &MCPServerHandler::onServerMessageReceived);
    
    // 创建各个通知处理器
    m_pResourceNotificationHandler = new MCPResourceNotificationHandler(pServer, this);
    m_pToolNotificationHandler = new MCPToolNotificationHandler(pServer, this);
    m_pPromptNotificationHandler = new MCPPromptNotificationHandler(pServer, this);
    
    // 连接各个通知处理器的通知请求信号到本Handler
    QObject::connect(m_pResourceNotificationHandler, &MCPNotificationHandlerBase::notificationRequested,
                     this, &MCPServerHandler::onNotificationRequested);
    QObject::connect(m_pToolNotificationHandler, &MCPNotificationHandlerBase::notificationRequested,
                     this, &MCPServerHandler::onNotificationRequested);
    QObject::connect(m_pPromptNotificationHandler, &MCPNotificationHandlerBase::notificationRequested,
                     this, &MCPServerHandler::onNotificationRequested);
}

MCPServerHandler::~MCPServerHandler()
{
}

void MCPServerHandler::onClientMessageReceived(quint64 nConnectionId, const QSharedPointer<MCPMessage>& pMessage)
{
    if (auto pClientMessage = pMessage.dynamicCast<MCPClientMessage>())
    {
		if (auto pSession = m_pServer->getSessionService()->getSession(nConnectionId, pClientMessage))
		{
			auto strMethodName = pClientMessage->getMethodName();
			auto pContext = QSharedPointer<MCPContext>::create(nConnectionId, pSession, pClientMessage);
			if (auto pResponse = m_pRequestDispatcher->handleClientMessage(pContext))
			{
				onServerMessageReceived(pResponse);
			}
		}
    }
}

void MCPServerHandler::onServerMessageReceived(const QSharedPointer<MCPMessage>& pMessage)
{
    if (auto pServerMessage = pMessage.dynamicCast<MCPServerMessage>())
    {
		// 对于Streamable传输的响应，需要先发送待处理的通知
		auto enMessageType = pServerMessage->getType();
		if ((enMessageType & MCPMessageType::StreamableTransport) && 
		    (enMessageType & MCPMessageType::Response))
		{
			sendStreamableTransportPendingNotifications(pServerMessage);
		}
  
		// 使用消息发送器统一发送消息
		m_pMessageSender->sendMessage(pServerMessage);
    }
}

void MCPServerHandler::onConnectionClosed(quint64 nConnectionId)
{
    // 先获取会话，以便取消订阅
    auto pSession = m_pServer->getSessionService()->getSessionByConnectionId(nConnectionId);
    if (pSession)
    {
        QString strSessionId = pSession->getSessionId();
        // 取消该会话的所有订阅（使用sessionId）
        m_pServer->getResourceService()->unsubscribeAll(strSessionId);
    }
    
    // 移除会话
    m_pServer->getSessionService()->removeSessionBySSEConnectId(nConnectionId);
}

// 注意：onSseTransportServerMessageReceived 和 onStreamableTransportServerMessageReceived 
// 方法已被移除，消息发送逻辑统一由 MCPMessageSender 处理

void MCPServerHandler::sendStreamableTransportPendingNotifications(const QSharedPointer<MCPServerMessage>& pServerMessage)
{   
	auto pContext = pServerMessage->getContext();
	if (pContext == nullptr)
	{
		return;
	}

	auto pSession = pContext->getSession();
	if (pSession == nullptr)
	{
		return;
	}

	// 检查是否有待发送的通知
    if (!pSession->hasPendingNotifications())
    {
        return;
    }

	// 根据通知对象生成并发送通知
	auto lstPendingNotifications = pSession->takePendingNotifications();
	for (const MCPPendingNotification& notification : lstPendingNotifications)
	{
		QJsonObject notificationObj;
		if (notification.isResourceChanged())
		{
			notificationObj = generateResourceChangedNotification(notification);
		}
		else if (notification.isResourcesListChanged())
		{
			// 资源列表变化通知：通过方法名生成
			notificationObj = generateNotificationByMethod(notification.getMethod());
		}
		else if (notification.isToolsListChanged())
		{
			// 工具列表变化通知：通过方法名生成
			notificationObj = generateNotificationByMethod(notification.getMethod());
		}
		else if (notification.isPromptsListChanged())
		{
			// 提示词列表变化通知：通过方法名生成
			notificationObj = generateNotificationByMethod(notification.getMethod());
		}
		else
		{
			// 未知的通知类型
			MCP_CORE_LOG_WARNING() << "MCPServerHandler: 未知的通知类型:" << static_cast<int>(notification.getType());
			continue;
		}

		if (notificationObj.isEmpty())
		{
			MCP_CORE_LOG_WARNING() << "MCPServerHandler: 无法生成通知:" << notification.getMethod();
			continue;
		}

		// 创建通知消息
		auto pNotificationMessage = QSharedPointer<MCPServerMessage>::create(
            pContext, notificationObj,
			MCPMessageType::StreamableTransport | MCPMessageType::RequestNotification
		);

		// 使用消息发送器发送通知
		m_pMessageSender->sendMessage(pNotificationMessage);

		MCP_CORE_LOG_DEBUG() << "MCPServerHandler: 已发送StreamableTransport通知:" << notification.getMethod();
	}
}

void MCPServerHandler::onSubscriptionNotification(const QString& strSessionId, const QJsonObject& objNotification)
{
    // 通过sessionId获取会话
    auto pSession = m_pServer->getSessionService()->getSessionBySessionId(strSessionId);
    
    if (pSession == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPServerHandler: 无法发送订阅通知，会话" << strSessionId << "不存在";
        return;
    }
    
    // 根据传输类型决定处理方式
	QString strMethod = objNotification.value("method").toString();
	if (pSession->isStreamableTransport())
    {
        // StreamableTransport：缓存通知标记
        if (!strMethod.isEmpty())
        {
            // 根据方法名判断通知类型
            if (strMethod == "notifications/resources/updated")
            {
                QJsonObject params = objNotification.value("params").toObject();
                QString strUri = params.value("uri").toString();
                pSession->addResourceChangedNotification(strUri);
                MCP_CORE_LOG_DEBUG() << "MCPServerHandler: 资源变化通知标记已缓存到StreamableTransport会话:" << strUri;
            }
            else if (strMethod == "notifications/resources/list_changed")
            {
                pSession->addResourcesListChangedNotification();
                MCP_CORE_LOG_DEBUG() << "MCPServerHandler: 资源列表变化通知标记已缓存到StreamableTransport会话";
            }
            else if (strMethod == "notifications/tools/list_changed")
            {
                pSession->addToolsListChangedNotification();
                MCP_CORE_LOG_DEBUG() << "MCPServerHandler: 工具列表变化通知标记已缓存到StreamableTransport会话";
            }
            else if (strMethod == "notifications/prompts/list_changed")
            {
                pSession->addPromptsListChangedNotification();
                MCP_CORE_LOG_DEBUG() << "MCPServerHandler: 提示词列表变化通知标记已缓存到StreamableTransport会话";
            }
            else
            {
                // 未知的通知类型
                MCP_CORE_LOG_WARNING() << "MCPServerHandler: 未知的通知方法:" << strMethod;
            }
        }
    }
    else
    {
        // SSE传输：立即发送通知
        // 获取SSE连接ID
        quint64 nSseConnectionId = pSession->getSseConnectionId();
        if (nSseConnectionId == 0)
        {
            MCP_CORE_LOG_WARNING() << "MCPServerHandler: 无法发送订阅通知，会话" << strSessionId << "没有SSE连接ID";
            return;
        }
        
        auto pClientMessage = QSharedPointer<MCPClientMessage>::create(
            MCPMessageType::SseTransport | MCPMessageType::Notification
        );
        
        // 创建Context
        auto pContext = QSharedPointer<MCPContext>::create(nSseConnectionId, pSession, pClientMessage);
        
        // 创建通知消息（通知消息不需要id字段）
        auto pNotificationMessage = QSharedPointer<MCPServerMessage>::create(
            pContext, 
            objNotification,
            MCPMessageType::SseTransport | MCPMessageType::RequestNotification
        );
        
        // 使用消息发送器发送通知
        m_pMessageSender->sendMessage(pNotificationMessage);
    }
}

void MCPServerHandler::onNotificationRequested(const QString& strSessionId, const QJsonObject& objNotification)
{
    // 直接调用onSubscriptionNotification处理通知
    onSubscriptionNotification(strSessionId, objNotification);
}

void MCPServerHandler::onResourceContentChanged(const QString& strUri)
{
    // 转发到资源通知处理器
    m_pResourceNotificationHandler->onResourceContentChanged(strUri);
}

void MCPServerHandler::onResourceDeleted(const QString& strUri)
{
    // 转发到资源通知处理器
    m_pResourceNotificationHandler->onResourceDeleted(strUri);
}

void MCPServerHandler::onResourcesListChanged()
{
    // 转发到资源通知处理器
    m_pResourceNotificationHandler->onResourcesListChanged();
}

void MCPServerHandler::onToolsListChanged()
{
    // 转发到工具通知处理器
    m_pToolNotificationHandler->onToolsListChanged();
}

void MCPServerHandler::onPromptsListChanged()
{
    // 转发到提示词通知处理器
    m_pPromptNotificationHandler->onPromptsListChanged();
}

MCPResourceNotificationHandler* MCPServerHandler::getResourceNotificationHandler() const
{
    return m_pResourceNotificationHandler;
}

MCPToolNotificationHandler* MCPServerHandler::getToolNotificationHandler() const
{
    return m_pToolNotificationHandler;
}

MCPPromptNotificationHandler* MCPServerHandler::getPromptNotificationHandler() const
{
    return m_pPromptNotificationHandler;
}

QJsonObject MCPServerHandler::generateNotificationByMethod(const QString& strMethod)
{
    QJsonObject notification;
    notification["method"] = strMethod;
    
    if (strMethod == "notifications/resources/list_changed")
    {
        // 资源列表变化通知：包含当前资源列表
        QJsonArray resources = m_pServer->getResourceService()->list();
        QJsonObject params;
        params["resources"] = resources;
        notification["params"] = params;
    }
    else if (strMethod == "notifications/tools/list_changed")
    {
        // 工具列表变化通知：包含当前工具列表
        QJsonArray tools = m_pServer->getToolService()->list();
        QJsonObject params;
        params["tools"] = tools;
        notification["params"] = params;
    }
    else if (strMethod == "notifications/prompts/list_changed")
    {
        // 提示词列表变化通知：包含当前提示词列表
        QJsonArray prompts = m_pServer->getPromptService()->list();
        QJsonObject params;
        params["prompts"] = prompts;
        notification["params"] = params;
    }
    else
    {
        // 未知的通知方法，返回空对象
        MCP_CORE_LOG_WARNING() << "MCPServerHandler: 未知的通知方法:" << strMethod;
        return QJsonObject();
    }
    
    return notification;
}

QJsonObject MCPServerHandler::generateResourceChangedNotification(const MCPPendingNotification& notification)
{
    // 资源变化通知：重新读取资源数据和元数据
    QString strUri = notification.getUri();
    if (strUri.isEmpty())
    {
        MCP_CORE_LOG_WARNING() << "MCPServerHandler: 资源变化通知缺少URI，返回空对象";
        return QJsonObject();
    }
    
    auto pResourceService = m_pServer->getResourceService();
    QJsonObject resourceData;
    
    if (pResourceService->has(strUri))
    {
        // 获取资源内容（包含contents数组）
        QJsonObject resourceInfo = pResourceService->readResource(strUri);
        // 获取资源元数据（name、description、mimeType）
        if (auto pResource = pResourceService->getResource(strUri))
        {
            QJsonObject metadata = pResource->getMetadata();
            // 合并元数据到资源信息中
            resourceInfo["name"] = metadata["name"];
            resourceInfo["description"] = metadata["description"];
            resourceInfo["mimeType"] = metadata["mimeType"];
        }
        resourceData["resource"] = resourceInfo;
    }
    else
    {
        resourceData["deleted"] = true;
    }

    QJsonObject notificationObj;
    notificationObj["method"] = notification.getMethod();
    QJsonObject params;
    params["uri"] = strUri;
    params["data"] = resourceData;
    notificationObj["params"] = params;
    return notificationObj;
}

