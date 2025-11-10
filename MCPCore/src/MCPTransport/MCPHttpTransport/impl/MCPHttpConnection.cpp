/**
 * @file MCPHttpConnection.cpp
 * @brief MCP HTTP连接实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPHttpConnection.h"
#include "MCPLog.h"
#include <QThread>
#include <QTcpSocket>
#include <QHostAddress>
#include "MCPMessage.h"
#include "MCPHttpRequestData.h"
#include "MCPHttpRequestParser.h"
#include "MCPHttpMessageParser.h"
#include "Utils/MCPInvokeHelper.h"
static quint64 SERVER_CONNECTION_ID = 1000;
MCPHttpConnection::MCPHttpConnection(qintptr nSocketDescriptor, QObject* parent)
    : QObject(parent)
    , m_nId(SERVER_CONNECTION_ID++)
    , m_pHttpRequestParser(new MCPHttpRequestParser(this))
{
    m_pSocket = new QTcpSocket(this);
    m_pSocket->setSocketDescriptor(nSocketDescriptor);
	QObject::connect(m_pSocket, &QTcpSocket::readyRead, this, &MCPHttpConnection::onReadyRead);
    QObject::connect(m_pSocket, &QTcpSocket::disconnected, this, &MCPHttpConnection::onDisconnected);
    QObject::connect(m_pSocket, static_cast<void (QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
		this, &MCPHttpConnection::onError);
    QObject::connect(m_pHttpRequestParser, &MCPHttpRequestParser::httpRequestReceived, this, &MCPHttpConnection::onHttpRequestReceived);

	MCP_TRANSPORT_LOG_INFO() << "Socket创建完成，描述符:" << nSocketDescriptor
		<< ", 来自:" << m_pSocket->peerAddress().toString()
		<< ", 端口:" << m_pSocket->peerPort();
}

MCPHttpConnection::~MCPHttpConnection()
{

}

quint64 MCPHttpConnection::getConnectionId()
{
    return m_nId;
}

void MCPHttpConnection::sendMessage(QSharedPointer<MCPMessage> pMessage)
{
    auto data = pMessage->toData();
	MCP_TRANSPORT_LOG_INFO() << "发送HTTP响应到" << m_pSocket->peerAddress().toString()
		<< ":" << m_pSocket->peerPort() << ", 大小:" << data.size();

	// 记录详细的HTTP响应内容
	MCP_TRANSPORT_LOG_DEBUG().noquote() << "HTTP响应详情:\n" << data;

	m_pSocket->write(data);
}

void MCPHttpConnection::disconnectFromHost()
{
    MCP_TRANSPORT_LOG_INFO() << "正在断开客户端连接:" << m_pSocket->peerAddress().toString()
        << ":" << m_pSocket->peerPort();
    m_pSocket->disconnectFromHost();
}

void MCPHttpConnection::onReadyRead()
{
    QByteArray data = m_pSocket->readAll();
    m_pHttpRequestParser->appendData(data);
}


void MCPHttpConnection::onError(QAbstractSocket::SocketError error)
{
    m_pSocket->disconnectFromHost();
}


void MCPHttpConnection::onHttpRequestReceived(QByteArray data, QSharedPointer<MCPHttpRequestData> pRequestData)
{
    if (auto pMessage = MCPHttpMessageParser::genClientMessageFromHttp(pRequestData))
    {
		emit messageReceived(m_nId, pMessage);
    }

}

void MCPHttpConnection::onDisconnected()
{
    MCP_TRANSPORT_LOG_INFO() << "客户端断开连接:" << m_pSocket->peerAddress().toString()
        << ":" << m_pSocket->peerPort();
    emit disconnected();
}

