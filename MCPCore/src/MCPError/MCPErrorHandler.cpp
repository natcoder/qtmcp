/**
 * @file MCPErrorHandler.cpp
 * @brief MCP错误处理工具类实现
 * @author zhangheng
 * @date 2025-01-08
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPErrorHandler.h"
#include "MCPLog.h"
#include <QJsonObject>
#include <QMutex>
#include <QMutexLocker>

MCPErrorHandler::MCPErrorHandler(QObject* parent)
    : QObject(parent)
{
}

MCPErrorHandler::~MCPErrorHandler()
{
}

QJsonObject MCPErrorHandler::createSuccessResponse(const QJsonValue& result, const QJsonValue& requestId)
{
    QJsonObject response;
    response.insert("jsonrpc", "2.0");
    response.insert("result", result);

    if (!requestId.isNull())
    {
        response.insert("id", requestId);
    }

    return response;
}

void MCPErrorHandler::logError(const MCPError& error, const QString& context)
{
    QString strLogMessage = QString("MCP错误 [%1]: %2")
                           .arg(static_cast<int>(error.getCode()))
                           .arg(error.getMessage());

    if (!context.isEmpty())
    {
        strLogMessage += QString(" (上下文: %1)").arg(context);
    }

    // 根据错误级别记录不同级别的日志
    if (error.isServerError())
    {
        MCP_CORE_LOG_CRITICAL() << strLogMessage;
    }
    else
    {
        MCP_CORE_LOG_WARNING() << strLogMessage;
    }
}

// 全局错误处理器
static QMutex g_errorHandlerMutex;
static MCPErrorHandler* g_errorHandlerInstance = nullptr;

MCPErrorHandler* getGlobalErrorHandler()
{
    if (!g_errorHandlerInstance)
    {
        QMutexLocker locker(&g_errorHandlerMutex);
        if (!g_errorHandlerInstance)
        {
            g_errorHandlerInstance = new MCPErrorHandler();
        }
    }
    return g_errorHandlerInstance;
}
