/**
 * @file MCPRequestDispatcher.h
 * @brief MCP请求调度器
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QSharedPointer>
#include <QJsonArray>
#include <QString>
#include "MCPServerMessage.h"

class MCPToolService;
class MCPResourceService;
class MCPPromptService;
class MCPSessionService;
class MCPRouter;
class MCPContext;
class MCPInitializeHandler;
class MCPSubscriptionHandler;
class IMCPServerConfig;
class MCPServer;

/**
 * @brief MCP请求调度器
 * 
 * 职责：
 * - 配置和管理路由表
 * - 接收客户端请求并调度到对应的处理方法
 * - 提供MCP协议的各种处理方法实现
 * - 协调各个服务层（Tool、Resource、Session等）
 * 
 * 设计说明：
 * - 使用MCPRouter进行路由分发
 * - 通过Lambda将处理方法注册到路由器
 * - 保持协议处理方法的集中管理
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 指针类型添加 p 前缀
 * - { 和 } 要单独一行
 */
class MCPRequestDispatcher : public QObject
{
    Q_OBJECT

public:
    explicit MCPRequestDispatcher(MCPServer* pServer,
                                   QObject* pParent = nullptr);
    virtual ~MCPRequestDispatcher();
    
signals:
    void serverMessageReceived(const QSharedPointer<MCPServerMessage>& pServerMessage);
    
public:
    /**
     * @brief 处理客户端消息
     * @param pContext 请求上下文
     * @return 服务器响应消息
     */
    QSharedPointer<MCPServerMessage> handleClientMessage(const QSharedPointer<MCPContext>& pContext);
    
private:
    /**
     * @brief 初始化路由表
     * 注册所有MCP协议方法到路由器
     */
    void initializeRoutes();
    
private:
    // MCP 协议方法处理器
	QSharedPointer<MCPServerMessage> handleConnect(const QSharedPointer<MCPContext>& pContext);
    QSharedPointer<MCPServerMessage> handleToolsList(const QSharedPointer<MCPContext>& pContext);
    QSharedPointer<MCPServerMessage> handleToolsCall(const QSharedPointer<MCPContext>& pContext);
    QSharedPointer<MCPServerMessage> handleListResources(const QSharedPointer<MCPContext>& pContext);
    QSharedPointer<MCPServerMessage> handleListResourceTemplates(const QSharedPointer<MCPContext>& pContext);
    QSharedPointer<MCPServerMessage> handleReadResource(const QSharedPointer<MCPContext>& pContext);
    QSharedPointer<MCPServerMessage> handleListPrompts(const QSharedPointer<MCPContext>& pContext);
    QSharedPointer<MCPServerMessage> handleGetPrompt(const QSharedPointer<MCPContext>& pContext);
    QSharedPointer<MCPServerMessage> handlePing(const QSharedPointer<MCPContext>& pContext);
    
private:
	QSharedPointer<MCPServerMessage> syncHandleToolsCall(const QSharedPointer<MCPContext>& pContext);
    
private:
    MCPServer* m_pServer;
    MCPRouter* m_pRouter;
    MCPInitializeHandler* m_pInitializeHandler;
    MCPSubscriptionHandler* m_pSubscriptionHandler;
};

