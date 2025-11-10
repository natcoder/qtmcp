/**
 * @file MCPMetaObjectHelper.cpp
 * @brief MCP元对象助手类实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPMetaObjectHelper.h"
#include "MCPLog/MCPLog.h"
#include <QJsonObject>

QMetaMethod MCPMetaObjectHelper::getSignal(QObject* pObject, const QString& strSignalSignature)
{
    if (pObject == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPMetaObjectHelper::getSignal: 对象指针为空";
        return QMetaMethod();
    }
    
    const QMetaObject* pMetaObject = pObject->metaObject();
    if (pMetaObject == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPMetaObjectHelper::getSignal: 无法获取对象的元对象信息";
        return QMetaMethod();
    }
    
    int nSignalIndex = pMetaObject->indexOfSignal(strSignalSignature.toUtf8().constData());
    if (nSignalIndex < 0)
    {
        MCP_CORE_LOG_DEBUG() << "MCPMetaObjectHelper::getSignal: 对象没有信号:" << strSignalSignature;
        return QMetaMethod();
    }
    
    return pMetaObject->method(nSignalIndex);
}

QMetaMethod MCPMetaObjectHelper::getMethod(QObject* pObject, const QString& strMethodSignature)
{
    if (pObject == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPMetaObjectHelper::getMethod: 对象指针为空";
        return QMetaMethod();
    }
    
    const QMetaObject* pMetaObject = pObject->metaObject();
    if (pMetaObject == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPMetaObjectHelper::getMethod: 无法获取对象的元对象信息";
        return QMetaMethod();
    }
    
    int nMethodIndex = pMetaObject->indexOfMethod(strMethodSignature.toUtf8().constData());
    if (nMethodIndex < 0)
    {
        MCP_CORE_LOG_DEBUG() << "MCPMetaObjectHelper::getMethod: 对象没有方法:" << strMethodSignature;
        return QMetaMethod();
    }
    
    return pMetaObject->method(nMethodIndex);
}

bool MCPMetaObjectHelper::hasSignal(QObject* pObject, const QString& strSignalSignature)
{
    QMetaMethod signal = getSignal(pObject, strSignalSignature);
    return signal.isValid();
}

bool MCPMetaObjectHelper::hasMethod(QObject* pObject, const QString& strMethodSignature)
{
    QMetaMethod method = getMethod(pObject, strMethodSignature);
    return method.isValid();
}

bool MCPMetaObjectHelper::hasSlot(QObject* pObject, const QString& strMethodName, int nReturnType)
{
    if (pObject == nullptr)
    {
        return false;
    }
    
    const QMetaObject* pMetaObject = pObject->metaObject();
    if (pMetaObject == nullptr)
    {
        return false;
    }
    
    // 查找指定名称和返回类型的slot方法
    int nMethodCount = pMetaObject->methodCount();
    for (int i = 0; i < nMethodCount; ++i)
    {
        QMetaMethod method = pMetaObject->method(i);
        
        if (method.methodType() == QMetaMethod::Slot &&
            method.name() == strMethodName &&
            method.parameterCount() == 0 &&
            method.returnType() == nReturnType)
        {
            return true;
        }
    }
    
    return false;
}

bool MCPMetaObjectHelper::invokeMethod(QObject* pObject, const QString& strMethodName)
{
    if (pObject == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPMetaObjectHelper::invokeMethod: 对象指针为空";
        return false;
    }
    
    bool bSuccess = QMetaObject::invokeMethod(pObject, strMethodName.toUtf8().constData(),
                                              Qt::DirectConnection);
    
    if (!bSuccess)
    {
        MCP_CORE_LOG_WARNING() << "MCPMetaObjectHelper::invokeMethod: 无法调用方法:" << strMethodName;
    }
    
    return bSuccess;
}

bool MCPMetaObjectHelper::invokeMethod(QObject* pObject, const QString& strMethodName, QString& strResult)
{
    if (pObject == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPMetaObjectHelper::invokeMethod: 对象指针为空";
        return false;
    }
    
    bool bSuccess = QMetaObject::invokeMethod(pObject, strMethodName.toUtf8().constData(),
                                              Qt::DirectConnection,
                                              Q_RETURN_ARG(QString, strResult));
    
    if (!bSuccess)
    {
        MCP_CORE_LOG_WARNING() << "MCPMetaObjectHelper::invokeMethod: 无法调用方法:" << strMethodName;
    }
    
    return bSuccess;
}

bool MCPMetaObjectHelper::invokeMethod(QObject* pObject, const QString& strMethodName, QJsonObject& objResult)
{
    if (pObject == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPMetaObjectHelper::invokeMethod: 对象指针为空";
        return false;
    }
    
    bool bSuccess = QMetaObject::invokeMethod(pObject, strMethodName.toUtf8().constData(),
                                              Qt::DirectConnection,
                                              Q_RETURN_ARG(QJsonObject, objResult));
    
    if (!bSuccess)
    {
        MCP_CORE_LOG_WARNING() << "MCPMetaObjectHelper::invokeMethod: 无法调用方法:" << strMethodName;
    }
    
    return bSuccess;
}

const QMetaObject* MCPMetaObjectHelper::getMetaObject(QObject* pObject)
{
    if (pObject == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPMetaObjectHelper::getMetaObject: 对象指针为空";
        return nullptr;
    }
    
    return pObject->metaObject();
}

