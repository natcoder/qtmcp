/**
 * @file MCPInvokeHelper.h
 * @brief MCP方法调用助手类（内部实现）
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <functional>
#include <QSharedPointer>
#include <QMetaMethod>
#include <QCoreApplication>

/**
 * @brief MCP方法调用助手类
 * 
 * Qt低版本不支持跨对象进行invoke，QTimer要求较严格，此类用来模拟
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 指针类型添加 p 前缀
 * - { 和 } 要单独一行
 */
class MCPInvokeHelper : public QObject
{
	Q_OBJECT
public:
	// 同步调用返回bool值	
	static bool syncInvokeReturn(QObject *pTargetObj, const std::function<bool()>& fun);
	//
	static void syncInvoke(QObject *pTargetObj, const std::function<void()>& fun);
	static void asynInvoke(QObject *pTargetObj, const std::function<void()>& fun);
public:
	static void setThreadName(unsigned long dwThreadID, QString strThreadName);
	static void setCurrentThreadName(QString strThreadName);
public:
	MCPInvokeHelper(QObject *pTargetObj);
	void operator+(const std::function<void()>& fun);
	void operator-(const std::function<void()>& fun);
private:
	~MCPInvokeHelper();
private slots:
	void invoke();
private:
	std::function<void()> m_fun;
};

#define SYNC_SAFE_THREAD_INVOKE *(new MCPInvokeHelper(this))+ [&]()
#define SYNC_MAIN_THREAD_INVOKE *(new MCPInvokeHelper(QCoreApplication::instance()))+ [&]()

