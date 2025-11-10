/**
 * @file MCPHelper.cpp
 * @brief MCP统一辅助工具类实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPHelper.h"
#include "MCPInvokeHelper.h"
#include "MCPMethodHelper.h"

void MCPHelper::syncInvoke(QObject* pTargetObj, const std::function<void()>& fun)
{
    MCPInvokeHelper::syncInvoke(pTargetObj, fun);
}

void MCPHelper::asyncInvoke(QObject* pTargetObj, const std::function<void()>& fun)
{
    MCPInvokeHelper::asynInvoke(pTargetObj, fun);
}

QVariant MCPHelper::callMethod(QObject* pHandler, const QString& strMethodName, const QVariantList& lstArguments)
{
    return MCPMethodHelper::syncCallMethod(pHandler, strMethodName, lstArguments);
}

QVariant MCPHelper::callMethod(QObject* pHandler, const QString& strMethodName, const QVariantMap& dictArguments)
{
    return MCPMethodHelper::syncCallMethod(pHandler, strMethodName, dictArguments);
}

void MCPHelper::setThreadName(unsigned long dwThreadID, const QString& strThreadName)
{
    MCPInvokeHelper::setThreadName(dwThreadID, strThreadName);
}

void MCPHelper::setCurrentThreadName(const QString& strThreadName)
{
    MCPInvokeHelper::setCurrentThreadName(strThreadName);
}

