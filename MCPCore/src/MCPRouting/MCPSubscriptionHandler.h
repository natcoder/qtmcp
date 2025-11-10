/**
 * @file MCPSubscriptionHandler.h
 * @brief MCP订阅处理器
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QSharedPointer>

class MCPContext;
class MCPServerMessage;
class MCPResourceService;

/**
 * @brief MCP订阅处理器
 * 
 * 职责：
 * - 处理订阅请求（resources/subscribe，符合MCP协议规范）
 * - 处理取消订阅请求（resources/unsubscribe，符合MCP协议规范）
 * - 同时支持 notifications/subscribe 和 notifications/unsubscribe（向后兼容）
 * - 参数验证和错误处理
 * - 调用资源服务执行订阅操作
 * 
 * 设计说明：
 * - 从MCPResourceService获取订阅功能（只有资源支持订阅）
 * - 处理MCP协议中的订阅相关请求
 * - 提供统一的订阅管理接口
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 指针类型添加 p 前缀
 * - { 和 } 要单独一行
 */
class MCPSubscriptionHandler : public QObject
{
    Q_OBJECT
    
public:
    explicit MCPSubscriptionHandler(MCPResourceService* pResourceService, QObject* pParent = nullptr);
    virtual ~MCPSubscriptionHandler();
    
    /**
     * @brief 处理订阅请求
     * @param pContext 请求上下文
     * @return 响应消息
     */
    QSharedPointer<MCPServerMessage> handleSubscribe(const QSharedPointer<MCPContext>& pContext);
    
    /**
     * @brief 处理取消订阅请求
     * @param pContext 请求上下文
     * @return 响应消息
     */
    QSharedPointer<MCPServerMessage> handleUnsubscribe(const QSharedPointer<MCPContext>& pContext);
    
private:
    MCPResourceService* m_pResourceService;
};

