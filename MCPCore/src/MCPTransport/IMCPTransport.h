/**
 * @file IMCPTransport.h
 * @brief MCP传输层抽象接口
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QSharedPointer>

class MCPMessage;

/**
 * @brief MCP传输层抽象接口
 * 
 * 职责：
 * - 定义传输层的通用行为
 * - 支持多种传输实现（HTTP、WebSocket、Stdio等）
 * - 提供统一的消息收发接口
 * 
 * 设计说明：
 * - 继承QObject以支持信号槽机制
 * - 具体实现通过适配器模式聚合底层传输类，避免菱形继承
 * - 所有方法使用纯虚函数，由子类实现
 * 
 * 编码规范：
 * - 接口方法使用纯虚函数
 * - 参数命名遵循项目规范（n前缀表示数值、p前缀表示指针、str前缀表示字符串）
 * - { 和 } 单独一行
 */
class IMCPTransport : public QObject
{
    Q_OBJECT
    
public:
    explicit IMCPTransport(QObject* pParent = nullptr) : QObject(pParent) {}
    virtual ~IMCPTransport() {}
    
    /**
     * @brief 启动传输层
     * @param nPort 监听端口号（对于网络传输）
     * @return true表示启动成功，false表示启动失败
     */
    virtual bool start(quint16 nPort = 8888) = 0;
    
    /**
     * @brief 停止传输层
     * @return true表示停止成功，false表示停止失败
     */
    virtual bool stop() = 0;
    
    /**
     * @brief 是否正在运行
     * @return true表示正在运行，false表示未运行
     */
    virtual bool isRunning() = 0;
    /**
     * @brief 发送消息
     * @param nConnectionId 连接ID
     * @param pMessage 消息对象指针
     */
    virtual void sendMessage(quint64 nConnectionId, QSharedPointer<MCPMessage> pMessage) = 0;
    
    /**
     * @brief 发送消息后关闭连接
     * @param nConnectionId 连接ID
     * @param pMessage 消息对象指针
     */
    virtual void sendCloseMessage(quint64 nConnectionId, QSharedPointer<MCPMessage> pMessage) = 0;
    
signals:
    /**
     * @brief 收到消息信号
     * @param nConnectionId 连接ID
     * @param pMessage 消息对象指针
     */
    void messageReceived(quint64 nConnectionId, const QSharedPointer<MCPMessage>& pMessage);
};

