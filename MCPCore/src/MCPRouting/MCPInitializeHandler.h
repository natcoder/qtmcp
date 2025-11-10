/**
 * @file MCPInitializeHandler.h
 * @brief MCP初始化处理器
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QSharedPointer>
#include <QJsonObject>

class MCPContext;
class MCPServerMessage;
class IMCPServerConfig;

/**
 * @brief MCP初始化处理器
 * 
 * 职责：
 * - 处理initialize握手请求
 * - 协商协议版本
 * - 声明服务器能力（capabilities）
 * - 提供服务器信息（serverInfo）
 * 
 * 设计说明：
 * - 从MCPServerConfig读取服务器信息
 * - 处理协议版本协商
 * - 设置会话状态
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 指针类型添加 p 前缀
 * - { 和 } 要单独一行
 */
class MCPInitializeHandler : public QObject
{
    Q_OBJECT
    
public:
    explicit MCPInitializeHandler(IMCPServerConfig* pConfig, QObject* pParent = nullptr);
    virtual ~MCPInitializeHandler();
    
    /**
     * @brief 处理initialize请求
     * @param pContext 请求上下文
     * @return 响应消息
     */
    QSharedPointer<MCPServerMessage> handleInitialize(const QSharedPointer<MCPContext>& pContext);
    
    /**
     * @brief 处理initialized通知
     * @param pContext 请求上下文
     * @return 响应消息
     */
    QSharedPointer<MCPServerMessage> handleInitialized(const QSharedPointer<MCPContext>& pContext);
    
private:
    IMCPServerConfig* m_pConfig;
};

