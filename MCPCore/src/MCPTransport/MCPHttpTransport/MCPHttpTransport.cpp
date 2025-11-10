/**
 * @file MCPHttpTransport.cpp
 * @brief MCP HTTP传输实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPHttpTransport.h"
#include "MCPLog/MCPLog.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QDateTime>
#include <QJsonDocument>
#include <QCoreApplication>
#include "Utils/MCPInvokeHelper.h"
#include "MCPClientMessage.h"
#include "impl/MCPHttpRequestData.h"
#include "impl/MCPHttpConnection.h"
#include "impl/MCPThreadPool.h"
MCPHttpTransport::MCPHttpTransport(QObject* pParent)
    : QTcpServer(pParent)
    , m_pThreadPool(new MCPThreadPool(2, this))
{
	qRegisterMetaType<QSharedPointer<MCPHttpRequestData>>("QSharedPointer<HttpRequestData>");
	qRegisterMetaType<QSharedPointer<MCPMessage>>("QSharedPointer<MCPMessage>");
	qRegisterMetaType<QSharedPointer<MCPClientMessage>>("QSharedPointer<MCPClientMessage>");
	qRegisterMetaType<QSharedPointer<MCPServerMessage>>("QSharedPointer<MCPResponse>");
}


MCPHttpTransport::~MCPHttpTransport()
{
}

bool MCPHttpTransport::start(quint16 nPort)
{
    if (isListening()) 
    {
        return true; // 已经启动
    }
    
	//if (!listen(QHostAddress::Any, nPort))
	if (!listen(QHostAddress("0.0.0.0"), nPort))
    {
        MCP_TRANSPORT_LOG_WARNING() << "启动服务器失败，端口：" << nPort << "，错误：" << errorString();
        return false;
    }
    MCP_TRANSPORT_LOG_INFO() << "MCP HTTP服务器已在端口" << nPort << "启动";
    return true;
}

bool MCPHttpTransport::stop()
{
    MCP_TRANSPORT_LOG_INFO() << "MCP HTTP服务器正在停止";
    close();
    MCP_TRANSPORT_LOG_INFO() << "MCP HTTP服务器已停止";
    return true;
}

bool MCPHttpTransport::isRunning()
{
    return isListening();
}

void MCPHttpTransport::sendMessage(quint64 nConnectionId, QSharedPointer<MCPMessage> pResponse)
{
    if (auto pConnection = m_dictConnections.value(nConnectionId))
    {
		MCPInvokeHelper::syncInvoke(pConnection, [pConnection, pResponse]()
			{
                pConnection->sendMessage(pResponse);
			});
    }
}

void MCPHttpTransport::sendCloseMessage(quint64 nConnectionId, QSharedPointer<MCPMessage> pResponse)
{
	if (auto pConnection = m_dictConnections.value(nConnectionId))
	{
		MCPInvokeHelper::asynInvoke(pConnection, [pConnection, pResponse]()
			{
				pConnection->sendMessage(pResponse);
                //pConnection->disconnectFromHost();
			});
	}
}


void MCPHttpTransport::incomingConnection(qintptr handle)
{
    MCP_TRANSPORT_LOG_INFO() << "新传入连接，句柄:" << handle;
    MCPHttpConnection* pConnection = new MCPHttpConnection(handle, nullptr);
	QObject::connect(pConnection, &MCPHttpConnection::messageReceived, this, &MCPHttpTransport::messageReceived);
	QObject::connect(pConnection, &MCPHttpConnection::disconnected, this, &MCPHttpTransport::onDisconnected);
    m_dictConnections[pConnection->getConnectionId()] = pConnection;
    m_pThreadPool->addWorker(pConnection);
}


void MCPHttpTransport::onDisconnected()
{
    if (auto pConnection = qobject_cast<MCPHttpConnection*>(sender()))
    {
        MCP_TRANSPORT_LOG_INFO() << "连接清理完成，ID:" << pConnection->getConnectionId();
        m_dictConnections.remove(pConnection->getConnectionId());
        m_pThreadPool->removeWorker(pConnection);
        pConnection->deleteLater();
    }
}