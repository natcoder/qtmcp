/**
 * @file MCPNotificationHandlerBase.h
 * @brief MCP通知处理器基类
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QSharedPointer>
#include <QSet>

class MCPServer;
class MCPSession;

/**
 * @brief MCP通知处理器基类
 * 
 * 职责：
 * - 提供通用的通知发送逻辑
 * - 处理StreamableTransport和SSE传输的通知发送
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 指针类型添加 p 前缀
 * - { 和 } 要单独一行
 */
class MCPNotificationHandlerBase : public QObject
{
    Q_OBJECT
    
public:
    /**
     * @brief 构造函数
     * @param pServer 服务器对象（必需）
     * @param pParent 父对象
     */
    explicit MCPNotificationHandlerBase(MCPServer* pServer, QObject* pParent = nullptr);
    
    virtual ~MCPNotificationHandlerBase();
    
signals:
    /**
     * @brief 发送通知信号
     * @param strSessionId 会话ID（空字符串表示广播到所有会话）
     * @param objNotification 通知消息
     */
    void notificationRequested(const QString& strSessionId, const QJsonObject& objNotification);
    
protected:
    /**
     * @brief 发送通知到所有会话（广播通知）
     * @param strMethod 通知方法名
     * @param objParams 通知参数
     */
    void broadcastNotification(const QString& strMethod, const QJsonObject& objParams);
    
    /**
     * @brief 发送通知到订阅的会话（订阅通知）
     * @param strMethod 通知方法名
     * @param objParams 通知参数
     * @param setSubscribedSessionIds 订阅的会话ID集合
     */
    void sendNotificationToSubscribers(const QString& strMethod, 
                                       const QJsonObject& objParams,
                                       const QSet<QString>& setSubscribedSessionIds);
    
protected:
    MCPServer* m_pServer;  // 服务器对象，通过它获取各个服务
};

