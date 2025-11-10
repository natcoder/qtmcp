#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include "MCPMessage.h"
#include "MCPServerMessage.h"

/**
 * @brief HTTP 传输层实现
 * 
 * 职责：
 * - 基于HTTP/1.1协议实现传输层
 * - 支持POST请求
 * - 管理HTTP连接生命周期
 * - 处理HTTP协议细节
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 指针类型添加 p 前缀
 * - { 和 } 要单独一行
 */
class MCPMessage;
class MCPThreadPool;
class MCPHttpConnection;
class MCPHttpTransport : public QTcpServer
{
    Q_OBJECT

public:
    explicit MCPHttpTransport(QObject* pParent = nullptr);
    virtual ~MCPHttpTransport();
    
public:
    bool start(quint16 nPort);
    bool stop();
    bool isRunning();
signals:
    void messageReceived(quint64 nConnectionId, const QSharedPointer<MCPMessage>& pMessage);
    
public slots:
    void sendMessage(quint64 nConnectionId, QSharedPointer<MCPMessage> pMessage);
    void sendCloseMessage(quint64 nConnectionId, QSharedPointer<MCPMessage> pMessage);
private slots:
    void onDisconnected();
private:
	void incomingConnection(qintptr handle);
private:
    QMap<quint64, MCPHttpConnection*> m_dictConnections;
private:
    MCPThreadPool* m_pThreadPool;
};