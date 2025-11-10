/**
 * @file MCPMiddlewares.cpp
 * @brief MCP基础中间件实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPMiddlewares.h"
#include "MCPRouting/MCPContext.h"
#include "MCPClientMessage.h"
#include "MCPServerMessage.h"
#include "MCPSession.h"
#include "MCPError.h"
#include "MCPLog.h"
#include <QElapsedTimer>

// ============================================================================
// MCPLoggingMiddleware - 日志中间件
// ============================================================================

MCPLoggingMiddleware::MCPLoggingMiddleware()
{
}

MCPLoggingMiddleware::~MCPLoggingMiddleware()
{
}

QSharedPointer<MCPServerMessage> MCPLoggingMiddleware::process(
    const QSharedPointer<MCPContext>& pContext,
    std::function<QSharedPointer<MCPServerMessage>()> next)
{
    auto strMethod = pContext->getClientMessage()->getMethodName();
    
    MCP_CORE_LOG_DEBUG() << "[Request] " << strMethod;
    
    // 调用下一个中间件/处理器
    auto pResponse = next();
    
    MCP_CORE_LOG_DEBUG() << "[Response] " << strMethod;
    
    return pResponse;
}

// ============================================================================
// MCPPerformanceMiddleware - 性能监控中间件
// ============================================================================

MCPPerformanceMiddleware::MCPPerformanceMiddleware(qint64 nSlowThresholdMs)
    : m_nSlowThresholdMs(nSlowThresholdMs)
{
}

MCPPerformanceMiddleware::~MCPPerformanceMiddleware()
{
}

QSharedPointer<MCPServerMessage> MCPPerformanceMiddleware::process(
    const QSharedPointer<MCPContext>& pContext,
    std::function<QSharedPointer<MCPServerMessage>()> next)
{
    auto strMethod = pContext->getClientMessage()->getMethodName();
    
    // 启动计时器
    QElapsedTimer timer;
    timer.start();
    
    // 调用下一个中间件/处理器
    auto pResponse = next();
    
    // 计算耗时
    qint64 nElapsed = timer.elapsed();
    
    // 只记录慢请求
    if (nElapsed > m_nSlowThresholdMs)
    {
        MCP_CORE_LOG_WARNING() << "[慢请求] " << strMethod 
                               << " 耗时:" << nElapsed << "ms";
    }
    
    return pResponse;
}

// ============================================================================
// MCPSessionValidationMiddleware - 会话验证中间件
// ============================================================================

MCPSessionValidationMiddleware::MCPSessionValidationMiddleware()
{
}

MCPSessionValidationMiddleware::~MCPSessionValidationMiddleware()
{
}

QSharedPointer<MCPServerMessage> MCPSessionValidationMiddleware::process(
    const QSharedPointer<MCPContext>& pContext,
    std::function<QSharedPointer<MCPServerMessage>()> next)
{
    auto strMethod = pContext->getClientMessage()->getMethodName();
    
    // 这些方法不需要会话验证
    if (strMethod == "connect" || 
        strMethod == "ping" || 
        strMethod == "initialize")
    {
        return next();
    }
    
    // 验证会话存在
    auto pSession = pContext->getSession();
    if (!pSession)
    {
        MCP_CORE_LOG_WARNING() << "[会话验证失败] " << strMethod << " - 会话不存在";
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext, 
            MCPError::invalidRequest("会话不存在")
        );
    }
    
    // 验证会话已初始化（除了初始化通知）
    if (strMethod != "notifications/initialized")
    {
        auto enStatus = pSession->getSessionStatus();
        if (enStatus != EnumSessionStatus::enInitialized)
        {
            MCP_CORE_LOG_WARNING() << "[会话验证失败] " << strMethod << " - 会话未初始化";
            return QSharedPointer<MCPServerErrorResponse>::create(
                pContext, 
                MCPError::invalidRequest("会话未初始化")
            );
        }
    }
    
    // 验证通过，继续执行
    return next();
}

