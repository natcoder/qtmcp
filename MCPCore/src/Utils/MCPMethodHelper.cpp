/**
 * @file MCPMethodHelper.cpp
 * @brief MCP方法助手实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPMethodHelper.h"
#include "MCPInvokeHelper.h"
#include <QDebug>
#include <QThread>
#include <QMetaMethod>
#include <QCoreApplication>
#include <QScopedPointer>
QVariant MCPMethodHelper::syncCallMethod(QObject* pHandler, const QString& strMethodName, const QVariantList& lstArguments)
{
	if (pHandler->thread() == QThread::currentThread())
	{
		return callMethod(pHandler, strMethodName, lstArguments);
	}
	QVariant retValue;
	MCPInvokeHelper::syncInvoke(pHandler, [&]()
		{
			retValue = callMethod(pHandler, strMethodName, lstArguments);
		});
	return retValue;
}

QVariant MCPMethodHelper::syncCallMethod(QObject* pHandler, const QString& strMethodName, const QVariantMap& dictArguments)
{
	if (pHandler->thread() == QThread::currentThread())
	{
		return callMethod(pHandler, strMethodName, dictArguments);
	}
	QVariant retValue;
	MCPInvokeHelper::syncInvoke(pHandler, [&]()
		{
			retValue = callMethod(pHandler, strMethodName, dictArguments);
		});
	return retValue;
}

QVariant MCPMethodHelper::callMethod(QObject* pHandler, const QString& strMethodName, const QVariantList& lstArguments)
{
	if (auto pMetaMethod = findMethod(pHandler, strMethodName))
	{
		if (auto pLstMethodArguments = createMethodArguments(pMetaMethod, lstArguments))
		{
			return directCallMethod(pHandler, pMetaMethod, pLstMethodArguments);
		}
	}
	return QVariant();
}


QVariant MCPMethodHelper::callMethod(QObject* pHandler, const QString& strMethodName, const QVariantMap& dictArguments)
{
	if (auto pMetaMethod = findMethod(pHandler, strMethodName))
	{
		if (auto pLstMethodArguments = createMethodArguments(pMetaMethod, dictArguments))
		{
			return directCallMethod(pHandler, pMetaMethod, pLstMethodArguments);
		}
	}
	return QVariant();
}

QSharedPointer<QList<QGenericArgument>> MCPMethodHelper::createMethodArguments(const QSharedPointer<QMetaMethod>& pMetaMethod, const QVariantList& lstArguments)
{
	//无法运行时获取默认值，所以严格相等
	auto lstMethodParameterTypes = pMetaMethod->parameterTypes();
	if (lstMethodParameterTypes.size() != lstArguments.size())
	{
		qDebug().noquote() << "MCPMethodHelper::createMethodArguments: " << lstArguments << " = false";
		return QSharedPointer<QList<QGenericArgument>>();
	}
	QScopedPointer<QVariantList> pConvertedStorage(new QVariantList());
	QScopedPointer<QList<QGenericArgument>> pLstMethodArguments(new QList<QGenericArgument>());
	for (int i = 0; i < lstMethodParameterTypes.size(); ++i)
	{
		pConvertedStorage->append(lstArguments[i]);
		const auto& strParameterType = lstMethodParameterTypes[i];
		auto nParameterType = QMetaType::type(strParameterType);

		if (pConvertedStorage->last().userType() == nParameterType)
		{
			pLstMethodArguments->append(QGenericArgument(strParameterType, pConvertedStorage->last().constData()));
		}
		else if (pConvertedStorage->last().canConvert(nParameterType))
		{
			pConvertedStorage->last().convert(nParameterType);
			pLstMethodArguments->append(QGenericArgument(strParameterType, pConvertedStorage->last().constData()));
		}
		else
		{
			qDebug().noquote() << "MCPMethodHelper::createMethodArguments: " << lstArguments << " = false";
			return QSharedPointer<QList<QGenericArgument>>();
		}
	}
	auto pStorage = pConvertedStorage.take();
	return QSharedPointer<QList<QGenericArgument>>(pLstMethodArguments.take(), [pStorage](QList<QGenericArgument>* pArgs)
		{
			delete pStorage;
			delete pArgs;
		});
}

QSharedPointer<QList<QGenericArgument>> MCPMethodHelper::createMethodArguments(const QSharedPointer<QMetaMethod>& pMetaMethod, const QVariantMap& dictArguments)
{
	//处理默认值算了，严格相等吧
	auto lstMethodParameterNames = pMetaMethod->parameterNames();
	if (dictArguments.size() != lstMethodParameterNames.size())
	{
		qDebug().noquote() << "MCPMethodHelper::createMethodArguments: " << dictArguments.values() << " = false";
		return QSharedPointer<QList<QGenericArgument>>();
	}
	QVariantList lstArguments;
	for (const auto& strMethodParameterName : lstMethodParameterNames)
	{
		auto it = dictArguments.find(strMethodParameterName);
		if (it == dictArguments.end())
		{
			return QSharedPointer<QList<QGenericArgument>>();
		}
		lstArguments.append(it.value());
	}
	return createMethodArguments(pMetaMethod, lstArguments);
}



QSharedPointer<QMetaMethod> MCPMethodHelper::findMethod(QObject* pHandler, const QString& strMethodName)
{
	auto pHanderMetaObject = pHandler->metaObject();
	auto nMethodCount = pHanderMetaObject->methodCount();
	auto nXXMethodCount = pHandler->staticMetaObject.methodCount();
	for (int i = nXXMethodCount; i < nMethodCount; ++i)
	{
		const QMetaMethod method = pHanderMetaObject->method(i);
		auto strCurMethodName = method.name();
		if (strCurMethodName == strMethodName)
		{
			return QSharedPointer<QMetaMethod>(new QMetaMethod(method));
		}
	}
	qDebug().noquote() << "MCPMethodHelper::findMethod: " << strMethodName << " = false";
	return QSharedPointer<QMetaMethod>();
}

QVariant MCPMethodHelper::directCallMethod(QObject* pHandler, const QSharedPointer<QMetaMethod>& pMetaMethod, const QSharedPointer<QList<QGenericArgument>>& pLstMethodArguments)
{
	if (pHandler == nullptr || pMetaMethod == nullptr || pLstMethodArguments == nullptr)
	{
		return QVariant();
	}

	QVariant returnValue(QMetaType::type(pMetaMethod->typeName()), static_cast<void*>(nullptr));
	QGenericReturnArgument returnArgument(pMetaMethod->typeName(), const_cast<void*>(returnValue.constData()));
	pMetaMethod->invoke(pHandler, Qt::DirectConnection, returnArgument,
		pLstMethodArguments->value(0),
		pLstMethodArguments->value(1),
		pLstMethodArguments->value(2),
		pLstMethodArguments->value(3),
		pLstMethodArguments->value(4),
		pLstMethodArguments->value(5),
		pLstMethodArguments->value(6),
		pLstMethodArguments->value(7),
		pLstMethodArguments->value(8),
		pLstMethodArguments->value(9)
	);
	qDebug().noquote() << "MainThread(" << (QThread::currentThread() == QCoreApplication::instance()->thread()) << ") "
		<< "MCPMethodHelper::directCallMethod: " << pMetaMethod->name() << " = " << returnValue;
	return returnValue;
}