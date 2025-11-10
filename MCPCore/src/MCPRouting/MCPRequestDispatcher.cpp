/**
 * @file MCPRequestDispatcher.cpp
 * @brief MCP请求调度器实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPRequestDispatcher.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include "MCPMessage/MCPMessage.h"
#include "MCPContext.h"
#include "MCPTools/MCPToolService.h"
#include "MCPResource/MCPResourceService.h"
#include "MCPPrompt/MCPPromptService.h"
#include "MCPError/MCPError.h"
#include "MCPLog/MCPLog.h"
#include "MCPRouter.h"
#include "MCPInitializeHandler.h"
#include "MCPSubscriptionHandler.h"
#include "MCPMiddleware/MCPMiddlewares.h"
#include "MCPServer/MCPServer.h"
#include <QtConcurrent>

MCPRequestDispatcher::MCPRequestDispatcher(MCPServer* pServer,
                                           QObject* pParent)
    : QObject(pParent)
    , m_pServer(pServer)
    , m_pRouter(new MCPRouter(this))
    , m_pInitializeHandler(new MCPInitializeHandler(m_pServer->getConfig(), this))
    , m_pSubscriptionHandler(new MCPSubscriptionHandler(m_pServer->getResourceService(), this))
{
    // 初始化路由表
    initializeRoutes();
}

MCPRequestDispatcher::~MCPRequestDispatcher()
{

}

void MCPRequestDispatcher::initializeRoutes()
{
    // 注册中间件（按执行顺序）
    m_pRouter->use(QSharedPointer<MCPLoggingMiddleware>::create());
    m_pRouter->use(QSharedPointer<MCPPerformanceMiddleware>::create(500));  // 500ms慢请求阈值
    m_pRouter->use(QSharedPointer<MCPSessionValidationMiddleware>::create());
    
    // 注册所有路由（使用Lambda绑定成员函数）
    m_pRouter->registerRoute("ping", [this](const QSharedPointer<MCPContext>& pContext)
    {
        return handlePing(pContext);
    });
    
    m_pRouter->registerRoute("connect", [this](const QSharedPointer<MCPContext>& pContext)
    {
        return handleConnect(pContext);
    });
    
    m_pRouter->registerRoute("initialize", [this](const QSharedPointer<MCPContext>& pContext)
    {
        return m_pInitializeHandler->handleInitialize(pContext);
    });
    
    m_pRouter->registerRoute("notifications/initialized", [this](const QSharedPointer<MCPContext>& pContext)
    {
        return m_pInitializeHandler->handleInitialized(pContext);
    });
    
    m_pRouter->registerRoute("tools/list", [this](const QSharedPointer<MCPContext>& pContext)
    {
        return handleToolsList(pContext);
    });
    
    m_pRouter->registerRoute("tools/call", [this](const QSharedPointer<MCPContext>& pContext)
    {
        return handleToolsCall(pContext);
    });
    
    m_pRouter->registerRoute("resources/list", [this](const QSharedPointer<MCPContext>& pContext)
    {
        return handleListResources(pContext);
    });
    
    m_pRouter->registerRoute("resources/templates/list", [this](const QSharedPointer<MCPContext>& pContext)
    {
        return handleListResourceTemplates(pContext);
    });
    
    m_pRouter->registerRoute("resources/read", [this](const QSharedPointer<MCPContext>& pContext)
    {
        return handleReadResource(pContext);
    });
    
    m_pRouter->registerRoute("resources/subscribe", [this](const QSharedPointer<MCPContext>& pContext)
    {
        return m_pSubscriptionHandler->handleSubscribe(pContext);
    });
    
    m_pRouter->registerRoute("resources/unsubscribe", [this](const QSharedPointer<MCPContext>& pContext)
    {
        return m_pSubscriptionHandler->handleUnsubscribe(pContext);
    });
    
    m_pRouter->registerRoute("prompts/list", [this](const QSharedPointer<MCPContext>& pContext)
    {
        return handleListPrompts(pContext);
    });
    
    m_pRouter->registerRoute("prompts/get", [this](const QSharedPointer<MCPContext>& pContext)
    {
        return handleGetPrompt(pContext);
    });
    
    m_pRouter->registerRoute("notifications/subscribe", [this](const QSharedPointer<MCPContext>& pContext)
    {
        return m_pSubscriptionHandler->handleSubscribe(pContext);
    });
    
    m_pRouter->registerRoute("notifications/unsubscribe", [this](const QSharedPointer<MCPContext>& pContext)
    {
        return m_pSubscriptionHandler->handleUnsubscribe(pContext);
    });
}

QSharedPointer<MCPServerMessage> MCPRequestDispatcher::handleClientMessage(const QSharedPointer<MCPContext>& pContext)
{
	QString strMethod = pContext->getClientMessage()->getMethodName();
    return m_pRouter->dispatch(strMethod, pContext);
}

QSharedPointer<MCPServerMessage> MCPRequestDispatcher::handleConnect(const QSharedPointer<MCPContext>& pContext)
{
	return QSharedPointer<MCPServerMessage>::create(pContext, (MCPMessageType::Flags)MCPMessageType::Connect);
}

QSharedPointer<MCPServerMessage> MCPRequestDispatcher::handleToolsList(const QSharedPointer<MCPContext>& pContext)
{
    QJsonArray arrTools = m_pServer->getToolService()->list();
    return QSharedPointer<MCPServerMessage>::create(pContext, QJsonObject{ {"tools", arrTools} });
}

QSharedPointer<MCPServerMessage> MCPRequestDispatcher::handleToolsCall(const QSharedPointer<MCPContext>& pContext)
{
	QtConcurrent::run([this, pContext]()
		{
			auto pServerMessage = syncHandleToolsCall(pContext);
			emit this->serverMessageReceived(pServerMessage);
		});
	return QSharedPointer<MCPServerMessage>();
}

QSharedPointer<MCPServerMessage> MCPRequestDispatcher::syncHandleToolsCall(const QSharedPointer<MCPContext>& pContext)
{
	auto pClientMesage = pContext->getClientMessage().dynamicCast<MCPClientMessage>();
    auto jsonCall = pClientMesage->getParmams().toObject();
    QString strToolName = jsonCall.value("name").toString();
    QJsonObject jsonCallArguments = jsonCall.value("arguments").toObject();

    // 验证必需参数
    if (strToolName.isEmpty())
    {
        // 根据 JSON-RPC 2.0 和 MCP 协议规范，错误消息应该使用英文
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext, 
            MCPError::invalidParams("Missing required parameter: name")
        );
    }

    try
    {
        //https://modelcontextprotocol.io/docs/learn/architecture
        //https://modelcontextprotocol.io/specification/2025-06-18/server/tools#structured-content
        QJsonObject result = m_pServer->getToolService()->callTool(strToolName, jsonCallArguments);
        return QSharedPointer<MCPServerMessage>::create(pContext, result);
    }
    catch (const MCPError& error)
    {
        return QSharedPointer<MCPServerErrorResponse>::create(pContext, error);
    }
    catch (const std::exception& e)
    {
        MCP_CORE_LOG_WARNING() << "工具调用时发生未知异常:" << e.what();
        // 根据 MCP 协议规范，内部错误的错误消息应该是英文
        MCPError error = MCPError::internalError(QString("Tool execution failed: %1").arg(e.what()));
        return QSharedPointer<MCPServerErrorResponse>::create(pContext, error);
    }
}


QSharedPointer<MCPServerMessage> MCPRequestDispatcher::handleListResources(const QSharedPointer<MCPContext>& pContext)
{
    QJsonArray arrResources = m_pServer->getResourceService()->list();
    QJsonObject result;
    result["resources"] = arrResources;
    return QSharedPointer<MCPServerMessage>::create(pContext, result);
}

QSharedPointer<MCPServerMessage> MCPRequestDispatcher::handleListResourceTemplates(const QSharedPointer<MCPContext>& pContext)
{
    // 当前实现不支持资源模板，返回空列表
    // 根据MCP规范，resources/templates/list应该返回资源模板列表
    QJsonObject result;
    result["resourceTemplates"] = QJsonArray();
    return QSharedPointer<MCPServerMessage>::create(pContext, result);
}

QSharedPointer<MCPServerMessage> MCPRequestDispatcher::handleReadResource(const QSharedPointer<MCPContext>& pContext)
{
    auto pClientMessage = pContext->getClientMessage().dynamicCast<MCPClientMessage>();
    auto jsonParams = pClientMessage->getParmams().toObject();
    QString strUri = jsonParams.value("uri").toString();
    
    if (strUri.isEmpty())
    {
        // 根据 JSON-RPC 2.0 和 MCP 协议规范，错误消息应该使用英文
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext, 
            MCPError::invalidParams("Missing required parameter: uri")
        );
    }
    
    QJsonObject result = m_pServer->getResourceService()->readResource(strUri);
    
    if (result.isEmpty())
    {
        // 根据 MCP 协议规范，资源不存在应返回 -32002 错误码，包含 uri 在 data 字段中
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext, 
            MCPError::resourceNotFound(strUri)
        );
    }
    
    return QSharedPointer<MCPServerMessage>::create(pContext, result);
}

QSharedPointer<MCPServerMessage> MCPRequestDispatcher::handleListPrompts(const QSharedPointer<MCPContext>& pContext)
{
    QJsonArray arrPrompts = m_pServer->getPromptService()->list();
    QJsonObject result;
    result["prompts"] = arrPrompts;
    return QSharedPointer<MCPServerMessage>::create(pContext, result);
}

QSharedPointer<MCPServerMessage> MCPRequestDispatcher::handleGetPrompt(const QSharedPointer<MCPContext>& pContext)
{
    auto pClientMessage = pContext->getClientMessage().dynamicCast<MCPClientMessage>();
    auto jsonParams = pClientMessage->getParmams().toObject();
    QString strName = jsonParams.value("name").toString();
    
    if (strName.isEmpty())
    {
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext, 
            MCPError::invalidParams("缺少name参数")
        );
    }
    
    // 解析参数
    QMap<QString, QString> arguments;
    QJsonObject jsonArguments = jsonParams.value("arguments").toObject();
    for (auto it = jsonArguments.begin(); it != jsonArguments.end(); ++it)
    {
        arguments[it.key()] = it.value().toString();
    }
    
    QJsonObject result = m_pServer->getPromptService()->getPrompt(strName, arguments);
    
    if (result.isEmpty())
    {
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext, 
            MCPError::invalidRequest(QString("提示词不存在: %1").arg(strName))
        );
    }
    
    return QSharedPointer<MCPServerMessage>::create(pContext, result);
}

QSharedPointer<MCPServerMessage> MCPRequestDispatcher::handlePing(const QSharedPointer<MCPContext>& pContext)
{
    return QSharedPointer<MCPServerMessage>::create(pContext);
}

