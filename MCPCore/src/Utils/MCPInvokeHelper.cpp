/**
 * @file MCPInvokeHelper.cpp
 * @brief MCP调用助手实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPInvokeHelper.h"
#include <QThread>
MCPInvokeHelper::MCPInvokeHelper(QObject *pTargetObj)
	: QObject(nullptr)
	, m_fun(nullptr)
{
	if (pTargetObj != nullptr)
	{
		moveToThread(pTargetObj->thread());
	}
}

void MCPInvokeHelper::operator+(const std::function<void()>& fun)
{
	this->thread() == QThread::currentThread()
		? (fun(), true) : (m_fun = fun, QMetaObject::invokeMethod(this, "invoke", Qt::BlockingQueuedConnection));
	deleteLater();
}

void MCPInvokeHelper::operator-(const std::function<void()>& fun)
{
	m_fun = fun;
	QMetaObject::invokeMethod(this, "invoke", Qt::QueuedConnection);
	deleteLater();
}

MCPInvokeHelper::~MCPInvokeHelper()
{

}

void MCPInvokeHelper::invoke()
{
	m_fun && (m_fun(), true);
}

void MCPInvokeHelper::syncInvoke(QObject *pTargetObj, const std::function<void()>& fun)
{
	*(new MCPInvokeHelper(pTargetObj)) + fun;
}

void MCPInvokeHelper::asynInvoke(QObject *pTargetObj, const std::function<void()>& fun)
{
	*(new MCPInvokeHelper(pTargetObj)) - fun;
}

bool MCPInvokeHelper::syncInvokeReturn(QObject* pTargetObj, const std::function<bool()>& fun)
{
	bool bResult = false;
	// 直接使用现有的 syncInvoke，通过 lambda 捕获返回值
	syncInvoke(pTargetObj, [&bResult, fun]()
		{
			bResult = fun();
		});
	return bResult;
}


#include <windows.h>  
const DWORD MS_VC_EXCEPTION = 0x406D1388;  
#pragma pack(push,8)  
typedef struct tagTHREADNAME_INFO  
{  
    DWORD dwType; // Must be 0x1000.  
    LPCSTR szName; // Pointer to name (in user addr space).  
    DWORD dwThreadID; // Thread ID (-1=caller thread).  
    DWORD dwFlags; // Reserved for future use, must be zero.  
 } THREADNAME_INFO;  
#pragma pack(pop)  

void SetThreadName(DWORD dwThreadID, const char* threadName) {  
    THREADNAME_INFO info;  
    info.dwType = 0x1000;  
    info.szName = threadName;  
    info.dwThreadID = dwThreadID;  
    info.dwFlags = 0;  
#pragma warning(push)  
#pragma warning(disable: 6320 6322)  
    __try{  
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);  
    }  
    __except (EXCEPTION_EXECUTE_HANDLER){  
    }  
#pragma warning(pop)  
}  
void MCPInvokeHelper::setThreadName(unsigned long dwThreadID, QString threadName)
{
	auto strStdThreadName = threadName.toStdString();
	SetThreadName(dwThreadID, strStdThreadName.c_str());
}

void MCPInvokeHelper::setCurrentThreadName(QString strThreadName)
{
	setThreadName((DWORD)QThread::currentThreadId(), strThreadName);
}
