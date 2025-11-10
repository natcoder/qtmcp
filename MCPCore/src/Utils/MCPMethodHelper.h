/**
 * @file MCPMethodHelper.h
 * @brief MCP方法助手类（内部实现）
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QSharedPointer>
#include <QList>
#include <QVariant>
#include <QVariantMap>
#include <QString>

class QMetaMethod;
class QObject;

/**
 * @brief MCP方法助手类
 * 
 * 职责：
 * - 通过反射同步调用对象方法
 * - 支持参数列表和参数字典两种方式
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 指针类型添加 p 前缀
 * - 字符串类型添加 str 前缀
 * - { 和 } 要单独一行
 */
class MCPMethodHelper
{
public:
	static QVariant syncCallMethod(QObject* pHandler, const QString& strMethodName, const QVariantList& lstArguments);
	static QVariant syncCallMethod(QObject* pHandler, const QString& strMethodName, const QVariantMap& dictArguments);
	
private:
	static QVariant callMethod(QObject* pHandler, const QString& strMethodName, const QVariantList& lstArguments);
	static QVariant callMethod(QObject* pHandler, const QString& strMethodName, const QVariantMap& dictArguments);
	static QSharedPointer<QMetaMethod> findMethod(QObject* pHandler, const QString& strMethodName);
	static QSharedPointer<QList<QGenericArgument>> createMethodArguments(const QSharedPointer<QMetaMethod>& pMetaMethod, const QVariantList& lstArguments);
	static QSharedPointer<QList<QGenericArgument>> createMethodArguments(const QSharedPointer<QMetaMethod>& pMetaMethod, const QVariantMap& dictArguments);
	static QVariant directCallMethod(QObject* pHandler, const QSharedPointer<QMetaMethod>& pMetaMethod, const QSharedPointer<QList<QGenericArgument>>& pLstMethodArguments);
};

