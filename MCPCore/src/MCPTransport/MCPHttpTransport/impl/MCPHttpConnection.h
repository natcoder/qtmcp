#pragma once
#include <QObject>
#include <QByteArray>
#include <QSharedPointer>
#include <QAbstractSocket>
#include "MCPMessage.h"
#include "MCPHttpRequestData.h"
#include "MCPServerMessage.h"
class QTcpSocket;
class MCPHttpRequestParser;
class MCPHttpConnection : public QObject
{
    Q_OBJECT
public:
    explicit MCPHttpConnection(qintptr nSocketDescriptor, QObject* parent = nullptr);
    ~MCPHttpConnection();

signals:
	// HTTP请求接收信号
	void messageReceived(quint64 nConnectionId, const QSharedPointer<MCPMessage>& pMessage);

    void disconnected();
public:
    //
    quint64 getConnectionId();
    //
public slots:
	// 发送数据
	void sendMessage(QSharedPointer<MCPMessage> pResponse);
    void disconnectFromHost();
private slots:
    // 处理就绪读取
    void onReadyRead();
    // 处理错误
    void onError(QAbstractSocket::SocketError error);
	void onDisconnected();
private slots:
	void onHttpRequestReceived(QByteArray data, QSharedPointer<MCPHttpRequestData> pRequestData);
private:
    quint64 m_nId;
    QTcpSocket* m_pSocket;
private:
    MCPHttpRequestParser* m_pHttpRequestParser;
};
