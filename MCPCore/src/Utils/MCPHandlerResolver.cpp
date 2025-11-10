/**
 * @file MCPHandlerResolver.cpp
 * @brief MCP Handler解析器实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPHandlerResolver.h"
#include <QCoreApplication>
#include <QVariant>
QMap<QString, QObject*> MCPHandlerResolver::resolveHandlers(QObject* pSearchRoot)
{
    QMap<QString, QObject*> handlers;
    
    // 如果未指定搜索根，使用qApp
    if (!pSearchRoot)
    {
        pSearchRoot = qApp;
    }
    
    if (!pSearchRoot)
    {
        return handlers;
    }
    
    // 查找所有子对象
    QList<QObject*> allObjects = pSearchRoot->findChildren<QObject*>();
    
    for (QObject* pObj : allObjects)
    {
        // 方式1：通过objectName识别
        QString strObjectName = pObj->objectName();
        if (!strObjectName.isEmpty())
        {
            handlers[strObjectName] = pObj;
        }
        
        // 方式2：通过"MPCServerHandlerName"属性识别（用于工具Handler）
        QString strHandlerName = pObj->property("MPCServerHandlerName").toString();
        if (!strHandlerName.isEmpty())
        {
            handlers[strHandlerName] = pObj;
        }
        
        // 方式3：通过"MCPResourceHandlerName"属性识别（用于资源Handler）
        QString strResourceHandlerName = pObj->property("MCPResourceHandlerName").toString();
        if (!strResourceHandlerName.isEmpty())
        {
            handlers[strResourceHandlerName] = pObj;
        }
    }
    
    return handlers;
}

QMap<QString, QObject*> MCPHandlerResolver::resolveResourceHandlers(QObject* pSearchRoot)
{
    QMap<QString, QObject*> handlers;
    
    // 如果未指定搜索根，使用qApp
    if (!pSearchRoot)
    {
        pSearchRoot = qApp;
    }
    
    if (!pSearchRoot)
    {
        return handlers;
    }
    
    // 查找所有子对象
    QList<QObject*> allObjects = pSearchRoot->findChildren<QObject*>();
    
    for (QObject* pObj : allObjects)
    {
        // 方式1：通过objectName识别
        QString strObjectName = pObj->objectName();
        if (!strObjectName.isEmpty())
        {
            handlers[strObjectName] = pObj;
        }
        
        // 方式2：通过"MCPResourceHandlerName"属性识别
        QString strResourceHandlerName = pObj->property("MCPResourceHandlerName").toString();
        if (!strResourceHandlerName.isEmpty())
        {
            handlers[strResourceHandlerName] = pObj;
        }
    }
    
    return handlers;
}

QObject* MCPHandlerResolver::findHandler(const QString& strHandlerName, QObject* pSearchRoot)
{
    QMap<QString, QObject*> handlers = resolveHandlers(pSearchRoot);
    return handlers.value(strHandlerName, nullptr);
}

