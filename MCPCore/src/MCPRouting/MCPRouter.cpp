/**
 * @file MCPRouter.cpp
 * @brief MCP方法路由器实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPRouter.h"
#include "MCPContext.h"
#include "MCPServerMessage.h"
#include "MCPError.h"
#include "MCPLog.h"
#include "MCPMiddleware/IMCPMiddleware.h"

MCPRouter::MCPRouter(QObject* pParent)
    : QObject(pParent)
{
}

MCPRouter::~MCPRouter()
{
}

void MCPRouter::registerRoute(const QString& strMethod, RouteHandler handler)
{
    if (m_dictRoutes.contains(strMethod))
    {
        MCP_CORE_LOG_WARNING() << "MCPRouter: 路由已存在，将被覆盖:" << strMethod;
    }
    
    m_dictRoutes[strMethod] = handler;
}

void MCPRouter::unregisterRoute(const QString& strMethod)
{
    if (m_dictRoutes.remove(strMethod) == 0)
    {
        MCP_CORE_LOG_WARNING() << "MCPRouter: 尝试注销不存在的路由:" << strMethod;
    }
}

QSharedPointer<MCPServerMessage> MCPRouter::dispatch(const QString& strMethod, 
                                                      const QSharedPointer<MCPContext>& pContext)
{
    // 查找路由处理器
    auto it = m_dictRoutes.find(strMethod);
    if (it == m_dictRoutes.end())
    {
        MCP_CORE_LOG_WARNING() << "MCPRouter: 未找到路由:" << strMethod;
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext, 
            MCPError::methodNotFound(QString("未知方法: %1").arg(strMethod))
        );
    }
    
    RouteHandler finalHandler = it.value();
    // 构建中间件管道（从后向前包装）
    std::function<QSharedPointer<MCPServerMessage>()> pipeline = [finalHandler, pContext]()
    {
        return finalHandler(pContext);
    };
    
    // 倒序包装中间件（后添加的中间件在外层）
    for (int i = m_listMiddlewares.size() - 1; i >= 0; --i)
    {
        auto pMiddleware = m_listMiddlewares[i];
        auto nextPipeline = pipeline;  // 捕获当前pipeline
        
        pipeline = [pMiddleware, pContext, nextPipeline]()
        {
            return pMiddleware->process(pContext, nextPipeline);
        };
    }
    
    // 执行中间件管道，捕获异常
    try
    {
        return pipeline();
    }
    catch (const MCPError& error)
    {
        MCP_CORE_LOG_WARNING() << "MCPRouter: 路由处理失败:" << strMethod << "，错误:" << error.getMessage();
        return QSharedPointer<MCPServerErrorResponse>::create(pContext, error);
    }
    catch (const std::exception& e)
    {
        MCP_CORE_LOG_WARNING() << "MCPRouter: 路由处理异常:" << strMethod << "，异常:" << e.what();
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext, 
            MCPError::internalError(QString("处理失败: %1").arg(e.what()))
        );
    }
    catch (...)
    {
        MCP_CORE_LOG_WARNING() << "MCPRouter: 路由处理时发生未知异常:" << strMethod;
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext, 
            MCPError::internalError("发生未知异常")
        );
    }
}

bool MCPRouter::hasRoute(const QString& strMethod) const
{
    return m_dictRoutes.contains(strMethod);
}

QStringList MCPRouter::getRegisteredRoutes() const
{
    return m_dictRoutes.keys();
}

void MCPRouter::use(QSharedPointer<IMCPMiddleware> pMiddleware)
{
    m_listMiddlewares.append(pMiddleware);
}

void MCPRouter::clearMiddlewares()
{
    m_listMiddlewares.clear();
}

int MCPRouter::getMiddlewareCount() const
{
    return m_listMiddlewares.size();
}

