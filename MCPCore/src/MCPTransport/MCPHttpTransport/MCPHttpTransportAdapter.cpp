/**
 * @file MCPHttpTransportAdapter.cpp
 * @brief MCP HTTP传输适配器实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPHttpTransportAdapter.h"
#include "MCPHttpTransport.h"
#include "MCPMessage.h"

MCPHttpTransportAdapter::MCPHttpTransportAdapter(QObject* pParent)
    : IMCPTransport(pParent)
    , m_pHttpTransport(new MCPHttpTransport(this))
{
    // 转发信号：将内部对象的信号转发为接口的信号
    QObject::connect(m_pHttpTransport, &MCPHttpTransport::messageReceived,
                     this, &IMCPTransport::messageReceived);
}

MCPHttpTransportAdapter::~MCPHttpTransportAdapter()
{
    // m_pHttpTransport由Qt父子关系自动管理，无需手动delete
}

bool MCPHttpTransportAdapter::start(quint16 nPort)
{
    // 转发调用到内部的HTTP传输对象
    return m_pHttpTransport->start(nPort);
}

bool MCPHttpTransportAdapter::stop()
{
    // 转发调用到内部的HTTP传输对象
    return m_pHttpTransport->stop();
}

bool MCPHttpTransportAdapter::isRunning()
{
    // 转发调用到内部的HTTP传输对象
    return m_pHttpTransport->isRunning();
}

void MCPHttpTransportAdapter::sendMessage(quint64 nConnectionId, QSharedPointer<MCPMessage> pMessage)
{
    // 转发调用到内部的HTTP传输对象
    m_pHttpTransport->sendMessage(nConnectionId, pMessage);
}

void MCPHttpTransportAdapter::sendCloseMessage(quint64 nConnectionId, QSharedPointer<MCPMessage> pMessage)
{
    // 转发调用到内部的HTTP传输对象
    m_pHttpTransport->sendCloseMessage(nConnectionId, pMessage);
}

