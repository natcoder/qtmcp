/**
 * @file MCPSubscriptionHandler.cpp
 * @brief MCP订阅处理器实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPSubscriptionHandler.h"
#include "MCPContext.h"
#include "MCPMessage/MCPClientMessage.h"
#include "MCPMessage/MCPServerMessage.h"
#include "MCPResource/MCPResourceService.h"
#include "MCPError/MCPError.h"
#include "MCPLog/MCPLog.h"

MCPSubscriptionHandler::MCPSubscriptionHandler(MCPResourceService* pResourceService, QObject* pParent)
    : QObject(pParent)
    , m_pResourceService(pResourceService)
{
    if (!m_pResourceService)
    {
        MCP_CORE_LOG_WARNING() << "MCPSubscriptionHandler: 资源服务为空";
    }
}

MCPSubscriptionHandler::~MCPSubscriptionHandler()
{
}

QSharedPointer<MCPServerMessage> MCPSubscriptionHandler::handleSubscribe(const QSharedPointer<MCPContext>& pContext)
{
    if (!m_pResourceService)
    {
        // 根据 MCP 协议规范，内部错误应返回 -32603，错误消息使用英文
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext, 
            MCPError::internalError("Resource service not initialized")
        );
    }
    
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
    
    // 验证资源是否存在（可选，根据MCP协议规范）
    // 如果资源不存在，仍然允许订阅，以便在资源创建时收到通知
    
    // 执行订阅（使用sessionId而不是connectionId）
    auto pSession = pContext->getSession();
    if (!pSession)
    {
        // 根据 JSON-RPC 2.0 和 MCP 协议规范，错误消息应该使用英文
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext, 
            MCPError::invalidRequest("Session not found")
        );
    }
    
    QString strSessionId = pSession->getSessionId();
    bool bSuccess = m_pResourceService->subscribe(strUri, strSessionId);
    
    if (!bSuccess)
    {
        // 根据 MCP 协议规范，内部错误应返回 -32603，错误消息使用英文
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext, 
            MCPError::internalError("Subscription failed")
        );
    }
    
    MCP_CORE_LOG_INFO() << "MCPSubscriptionHandler: 会话" << strSessionId << "已订阅URI:" << strUri;
    
    // 返回成功响应（空结果表示成功）
    return QSharedPointer<MCPServerMessage>::create(pContext, QJsonObject());
}

QSharedPointer<MCPServerMessage> MCPSubscriptionHandler::handleUnsubscribe(const QSharedPointer<MCPContext>& pContext)
{
    if (!m_pResourceService)
    {
        // 根据 MCP 协议规范，内部错误应返回 -32603，错误消息使用英文
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext, 
            MCPError::internalError("Resource service not initialized")
        );
    }
    
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
    
    // 执行取消订阅（使用sessionId而不是connectionId）
    auto pSession = pContext->getSession();
    if (!pSession)
    {
        // 根据 JSON-RPC 2.0 和 MCP 协议规范，错误消息应该使用英文
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext, 
            MCPError::invalidRequest("Session not found")
        );
    }
    
    QString strSessionId = pSession->getSessionId();
    bool bSuccess = m_pResourceService->unsubscribe(strUri, strSessionId);
    
    if (!bSuccess)
    {
        // 如果未订阅，返回错误（根据 MCP 协议规范，错误消息应该使用英文）
        QJsonObject data;
        data["uri"] = strUri;
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext, 
            MCPError(MCPErrorCode::INVALID_REQUEST, "Not subscribed to URI", data)
        );
    }
    
    MCP_CORE_LOG_INFO() << "MCPSubscriptionHandler: 会话" << strSessionId << "已取消订阅URI:" << strUri;
    
    // 返回成功响应（空结果表示成功）
    return QSharedPointer<MCPServerMessage>::create(pContext, QJsonObject());
}

