/**
 * @file MCPMessageSender.h
 * @brief MCP消息发送器 - 统一处理消息发送逻辑
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QSharedPointer>
#include "MCPMessage.h"
#include "MCPServerMessage.h"

class IMCPTransport;
class MCPServerMessage;

/**
 * @brief MCP消息发送器
 * 
 * 职责：
 * - 统一处理服务器消息的发送逻辑
 * - 根据消息类型和传输类型选择合适的发送策略
 * - 封装消息转换和发送的细节
 * - 简化MCPServerHandler中的发送逻辑
 * 
 * 设计说明：
 * - 使用策略模式处理不同传输类型的消息发送
 * - 将消息构建和发送逻辑分离
 * - 提供清晰的接口，隐藏实现细节
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 指针类型添加 p 前缀
 * - { 和 } 要单独一行
 */
class MCPMessageSender : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param pTransport 传输层接口
     * @param pParent 父对象
     */
    explicit MCPMessageSender(IMCPTransport* pTransport, QObject* pParent = nullptr);
    
    virtual ~MCPMessageSender();

    /**
     * @brief 发送服务器消息
     * @param pServerMessage 服务器消息对象
     * 
     * 根据消息类型自动选择合适的发送策略：
     * - SSE传输：通过SSE连接发送
     * - Streamable传输：通过原始连接发送
     * - 自动处理连接关闭等逻辑
     */
    void sendMessage(const QSharedPointer<MCPServerMessage>& pServerMessage);

    /**
     * @brief 发送接受通知（202 Accepted响应）
     * @param nConnectionId 连接ID
     * @param enTransportType 传输类型
     */
    void sendAcceptNotification(quint64 nConnectionId, MCPMessageType::Flags enTransportType);

private:
    /**
     * @brief 发送SSE传输的消息
     * @param pServerMessage 服务器消息
     */
    void sendSseMessage(const QSharedPointer<MCPServerMessage>& pServerMessage);

    /**
     * @brief 发送Streamable传输的消息
     * @param pServerMessage 服务器消息
     */
    void sendStreamableMessage(const QSharedPointer<MCPServerMessage>& pServerMessage);

private:
    IMCPTransport* m_pTransport;  // 传输层接口
};

