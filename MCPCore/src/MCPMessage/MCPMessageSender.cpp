/**
 * @file MCPMessageSender.cpp
 * @brief MCP消息发送器实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPMessageSender.h"
#include "MCPTransport/IMCPTransport.h"
#include "MCPTransport/MCPHttpTransport/impl/MCPHttpReplyMessage.h"
#include "MCPMessageType.h"
#include "MCPSession/MCPSession.h"
#include "MCPRouting/MCPContext.h"
#include "MCPLog/MCPLog.h"

MCPMessageSender::MCPMessageSender(IMCPTransport* pTransport, QObject* pParent)
    : QObject(pParent)
    , m_pTransport(pTransport)
{
    if (m_pTransport == nullptr)
    {
        MCP_CORE_LOG_CRITICAL() << "MCPMessageSender: 传输层接口不能为空";
    }
}

MCPMessageSender::~MCPMessageSender()
{
}

void MCPMessageSender::sendMessage(const QSharedPointer<MCPServerMessage>& pServerMessage)
{
    if (pServerMessage == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPMessageSender: 尝试发送空消息";
        return;
    }

    auto enMessageType = pServerMessage->getType();
    
    // 根据传输类型选择发送策略
    if (enMessageType & MCPMessageType::SseTransport)
    {
        sendSseMessage(pServerMessage);
    }
    else if (enMessageType & MCPMessageType::StreamableTransport)
    {
        sendStreamableMessage(pServerMessage);
    }
    else
    {
        MCP_CORE_LOG_WARNING() << "MCPMessageSender: 未知的传输类型:" << MCPMessageType::toString(enMessageType);
    }
}

void MCPMessageSender::sendAcceptNotification(quint64 nConnectionId, MCPMessageType::Flags enTransportType)
{
    QSharedPointer<MCPHttpReplyMessage> pReplyMessage;
    
    if (enTransportType & MCPMessageType::SseTransport)
    {
        pReplyMessage = MCPHttpReplyMessage::CreateSseAcceptNotification();
    }
    else if (enTransportType & MCPMessageType::StreamableTransport)
    {
        pReplyMessage = MCPHttpReplyMessage::CreateStreamableAcceptNotification();
    }
    else
    {
        MCP_CORE_LOG_WARNING() << "MCPMessageSender: 未知的传输类型，无法发送接受通知";
        return;
    }

    m_pTransport->sendMessage(nConnectionId, pReplyMessage);
}

void MCPMessageSender::sendSseMessage(const QSharedPointer<MCPServerMessage>& pServerMessage)
{
    auto pContext = pServerMessage->getContext();
    if (pContext == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPMessageSender: SSE消息缺少上下文";
        return;
    }

    auto enMessageType = pServerMessage->getType();
    auto pTransport = m_pTransport;

    if (enMessageType & MCPMessageType::Connect)
    {
        // SSE连接响应：发送到原始连接
        pTransport->sendMessage(pContext->getConnectionId(),
            QSharedPointer<MCPHttpReplyMessage>::create(pServerMessage, enMessageType));
    }
    else if (enMessageType & MCPMessageType::Response)
    {
        // SSE响应：发送到SSE连接，然后关闭原始连接
        auto pSession = pContext->getSession();
        if (pSession == nullptr)
        {
            MCP_CORE_LOG_WARNING() << "MCPMessageSender: SSE响应消息缺少会话";
            return;
        }

        pTransport->sendMessage(pSession->getSseConnectionId(),
            QSharedPointer<MCPHttpReplyMessage>::create(pServerMessage, enMessageType));
        
        // 发送接受通知并关闭原始连接
        pTransport->sendCloseMessage(pContext->getConnectionId(),
            MCPHttpReplyMessage::CreateStreamableAcceptNotification());
    }
    else if (enMessageType & MCPMessageType::ResponseNotification)
    {
        // SSE通知接受响应：发送202 Accepted
        pTransport->sendMessage(pContext->getConnectionId(),
            MCPHttpReplyMessage::CreateSseAcceptNotification());
    }
    else if (enMessageType & MCPMessageType::RequestNotification)
    {
        // SSE主动通知：发送到原始连接
        pTransport->sendMessage(pContext->getConnectionId(),
            QSharedPointer<MCPHttpReplyMessage>::create(pServerMessage, enMessageType));
    }
}

void MCPMessageSender::sendStreamableMessage(const QSharedPointer<MCPServerMessage>& pServerMessage)
{
    auto pContext = pServerMessage->getContext();
    if (pContext == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPMessageSender: Streamable消息缺少上下文";
        return;
    }

    auto enMessageType = pServerMessage->getType();
    auto pTransport = m_pTransport;

    // 发送响应消息
    if (enMessageType & MCPMessageType::Response)
    {
        pTransport->sendMessage(pContext->getConnectionId(),
            QSharedPointer<MCPHttpReplyMessage>::create(pServerMessage, enMessageType));
    }
    else if (enMessageType & MCPMessageType::ResponseNotification)
    {
        // Streamable通知接受响应：发送202 Accepted
        pTransport->sendMessage(pContext->getConnectionId(),
            MCPHttpReplyMessage::CreateStreamableAcceptNotification());
    }
    else if (enMessageType & MCPMessageType::RequestNotification)
    {
        // Streamable主动通知：发送通知消息
        pTransport->sendMessage(pContext->getConnectionId(),
            QSharedPointer<MCPHttpReplyMessage>::create(pServerMessage, enMessageType));
    }
}

