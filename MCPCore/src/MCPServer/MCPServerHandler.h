/**
 * @file MCPServerHandler.h
 * @brief MCP服务器业务处理器类（内部实现）
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QSharedPointer>
#include <QSet>

class MCPMessage;
class MCPServerMessage;
class MCPSessionService;
class MCPToolService;
class MCPResourceService;
class MCPPromptService;
class MCPRequestDispatcher;
class IMCPTransport;
class MCPServerConfig;
class MCPServer;
class MCPResourceNotificationHandler;
class MCPToolNotificationHandler;
class MCPPromptNotificationHandler;
class MCPPendingNotification;
class MCPMessageSender;

/**
 * @brief MCP服务器业务处理器类
 * 
 * 职责：
 * - 处理客户端消息接收和响应
 * - 管理请求调度器（MCPRequestDispatcher），负责路由和分发客户端请求
 * - 处理服务器消息发送（通过MCPMessageSender统一处理）
 * - 处理连接生命周期事件
 * - 处理订阅通知
 * - 协调各个通知处理器（资源、工具、提示词）
 * - 生成和发送通知消息（用于StreamableTransport）
 * 
 * 注意：
 * - 资源、工具、提示词的具体通知处理由对应的NotificationHandler负责
 * - 请求路由和分发由内部的MCPRequestDispatcher负责
 * - 消息发送由MCPMessageSender统一处理，简化了发送逻辑
 * - 本类主要负责消息处理流程的协调和通知发送
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 指针类型添加 p 前缀
 * - { 和 } 要单独一行
 */
class MCPServerHandler : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param pServer 服务器对象（必需）
     * @param pParent 父对象
     */
    explicit MCPServerHandler(MCPServer* pServer,
                               QObject* pParent = nullptr);
    
    virtual ~MCPServerHandler();

public slots:
    /**
     * @brief 处理客户端消息接收
     * @param nConnectionId 连接ID
     * @param pMessage 消息对象
     */
    void onClientMessageReceived(quint64 nConnectionId, const QSharedPointer<MCPMessage>& pMessage);

    /**
     * @brief 处理服务器消息接收
     * @param pMessage 服务器消息对象
     */
    void onServerMessageReceived(const QSharedPointer<MCPMessage>& pMessage);

    /**
     * @brief 处理连接关闭
     * @param nConnectionId 连接ID
     */
    void onConnectionClosed(quint64 nConnectionId);
    
    /**
     * @brief 处理订阅通知
     * @param strSessionId 会话ID
     * @param objNotification 通知消息
     */
    void onSubscriptionNotification(const QString& strSessionId, const QJsonObject& objNotification);
    
    /**
     * @brief 处理资源内容变化事件（转发到资源通知处理器）
     * @param strUri 资源URI
     */
    void onResourceContentChanged(const QString& strUri);
    
    /**
     * @brief 处理资源删除事件（转发到资源通知处理器）
     * @param strUri 资源URI
     */
    void onResourceDeleted(const QString& strUri);
    
    /**
     * @brief 处理资源列表变化事件（转发到资源通知处理器）
     */
    void onResourcesListChanged();
    
    /**
     * @brief 处理工具列表变化事件（转发到工具通知处理器）
     */
    void onToolsListChanged();
    
    /**
     * @brief 处理提示词列表变化事件（转发到提示词通知处理器）
     */
    void onPromptsListChanged();
    
    /**
     * @brief 获取资源通知处理器
     * @return 资源通知处理器指针
     */
    MCPResourceNotificationHandler* getResourceNotificationHandler() const;
    
    /**
     * @brief 获取工具通知处理器
     * @return 工具通知处理器指针
     */
    MCPToolNotificationHandler* getToolNotificationHandler() const;
    
    /**
     * @brief 获取提示词通知处理器
     * @return 提示词通知处理器指针
     */
    MCPPromptNotificationHandler* getPromptNotificationHandler() const;

private slots:
    /**
     * @brief 处理通知请求（由各个通知处理器发出）
     * @param strSessionId 会话ID
     * @param objNotification 通知消息
     */
    void onNotificationRequested(const QString& strSessionId, const QJsonObject& objNotification);
    
private:
    /**
     * @brief 发送Streamable传输的待处理通知
     * @param pServerMessage 服务器消息
     * 
     * 注意：此方法在发送响应消息之前调用，用于发送待处理的通知
     */
    void sendStreamableTransportPendingNotifications(const QSharedPointer<MCPServerMessage>& pServerMessage);
private:
    /**
     * @brief 根据通知方法名生成通知消息
     * @param strMethod 通知方法名（如"notifications/tools/list_changed"）
     * @return 通知消息JSON对象
     */
    QJsonObject generateNotificationByMethod(const QString& strMethod);
    
    /**
     * @brief 生成资源变化通知消息
     * @param notification 资源变化通知对象
     * @return 通知消息JSON对象，如果URI为空则返回空对象
     */
    QJsonObject generateResourceChangedNotification(const MCPPendingNotification& notification);

private:
    MCPServer* m_pServer;  // 服务器对象，通过它获取各个服务
    
    // 请求调度器（负责路由和分发客户端请求）
    MCPRequestDispatcher* m_pRequestDispatcher;
    
    // 消息发送器（统一处理消息发送逻辑）
    MCPMessageSender* m_pMessageSender;
    
    // 各个通知处理器
    MCPResourceNotificationHandler* m_pResourceNotificationHandler;
    MCPToolNotificationHandler* m_pToolNotificationHandler;
    MCPPromptNotificationHandler* m_pPromptNotificationHandler;
};

