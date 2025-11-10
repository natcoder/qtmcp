/**
 * @file MCPHttpTransportAdapter.h
 * @brief MCP HTTP传输适配器
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include "IMCPTransport.h"

class MCPHttpTransport;

/**
 * @brief MCP HTTP传输适配器
 * 
 * 职责：
 * - 实现IMCPTransport接口
 * - 聚合MCPHttpTransport，避免菱形继承问题
 * - 转发接口调用到内部的MCPHttpTransport
 * - 转发MCPHttpTransport的信号
 * 
 * 设计模式：
 * - 适配器模式（Adapter Pattern）
 * - 通过组合而不是继承来复用MCPHttpTransport
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 指针类型添加 p 前缀
 * - { 和 } 要单独一行
 */
class MCPHttpTransportAdapter : public IMCPTransport
{
    Q_OBJECT
    
public:
    explicit MCPHttpTransportAdapter(QObject* pParent = nullptr);
    virtual ~MCPHttpTransportAdapter();
    
public:
    // 实现IMCPTransport接口
    virtual bool start(quint16 nPort = 8888) override;
    virtual bool stop() override;
    virtual bool isRunning() override;
    virtual void sendMessage(quint64 nConnectionId, QSharedPointer<MCPMessage> pMessage) override;
    virtual void sendCloseMessage(quint64 nConnectionId, QSharedPointer<MCPMessage> pMessage) override;
    
private:
    MCPHttpTransport* m_pHttpTransport;
};

