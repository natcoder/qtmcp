#pragma once
#include <QObject>
#include <QSet>
#include <QMap>
#include <QThread>
#include <QSharedPointer>
class MCPThreadPool : public QObject
{
    Q_OBJECT
public:
    explicit MCPThreadPool(int nThreadCount = QThread::idealThreadCount(), QObject* parent = nullptr);
    ~MCPThreadPool();
public:
	void addWorker(QObject* pWorker);
	void removeWorker(QObject* pWorker);
private:
	QMap<QThread*, QSharedPointer<QSet<QObject*>>> m_dictThreadWorkers;
};