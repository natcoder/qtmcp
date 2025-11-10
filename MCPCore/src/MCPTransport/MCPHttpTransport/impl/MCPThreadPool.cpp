/**
 * @file MCPThreadPool.cpp
 * @brief MCP线程池实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPThreadPool.h"
#include <QThread>
#include <algorithm>
MCPThreadPool::MCPThreadPool(int nThreadCount, QObject* parent)
    : QObject(parent)
{
    for (int i = 0; i < nThreadCount; ++i)
    {
		QThread* pThread = new QThread(this);
        m_dictThreadWorkers[pThread] = QSharedPointer<QSet<QObject*>>::create();
		pThread->start();
    }
}

MCPThreadPool::~MCPThreadPool()
{
    // 停止所有线程
    for (auto pThread : m_dictThreadWorkers.keys())
    {
        if (pThread != nullptr && pThread->isRunning())
        {
            pThread->quit();
            pThread->wait(3000);
        }
    }
}

void MCPThreadPool::addWorker(QObject* pWorker)
{
	auto it = m_dictThreadWorkers.constBegin();
	auto pTargetThread = it.key();
	int nMinSize = it.value()->size();
	// 继续遍历剩余元素
	while (++it != m_dictThreadWorkers.constEnd()) 
    {
		int nCurrentSize = it.value()->size();
		if (nCurrentSize < nMinSize) 
        {
			nMinSize = nCurrentSize;
			pTargetThread = it.key();
		}
	}
	pWorker->moveToThread(pTargetThread);
    m_dictThreadWorkers[pTargetThread]->insert(pWorker);
}

void MCPThreadPool::removeWorker(QObject* pWorker)
{
    if (m_dictThreadWorkers.contains(pWorker->thread()))
    {
		m_dictThreadWorkers[pWorker->thread()]->remove(pWorker);
    }
}
