/**
 * @file MCPRouter.h
 * @brief MCP方法路由器
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QMap>
#include <QList>
#include <QString>
#include <QSharedPointer>
#include <functional>

class MCPContext;
class MCPServerMessage;
class IMCPMiddleware;

/**
 * @brief MCP方法路由器
 * 
 * 职责：
 * - 将方法名映射到处理函数
 * - 提供路由注册和注销接口
 * - 调度请求到对应的处理函数
 * - 统一错误处理
 * 
 * 设计模式：
 * - 命令模式（Command Pattern）
 * - 使用std::function实现动态路由注册
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 指针类型添加 p 前缀
 * - 字符串类型添加 str 前缀
 * - { 和 } 要单独一行
 */
class MCPRouter : public QObject
{
    Q_OBJECT
    
public:
    // 路由处理函数类型定义
    using RouteHandler = std::function<QSharedPointer<MCPServerMessage>(const QSharedPointer<MCPContext>&)>;
    
public:
    explicit MCPRouter(QObject* pParent = nullptr);
    virtual ~MCPRouter();
    
    /**
     * @brief 注册路由
     * @param strMethod 方法名（如"tools/list"、"ping"等）
     * @param handler 处理函数
     */
    void registerRoute(const QString& strMethod, RouteHandler handler);
    
    /**
     * @brief 注销路由
     * @param strMethod 方法名
     */
    void unregisterRoute(const QString& strMethod);
    
    /**
     * @brief 调度请求到对应的处理函数
     * @param strMethod 方法名
     * @param pContext 请求上下文
     * @return 响应消息，如果路由不存在返回错误消息
     */
    QSharedPointer<MCPServerMessage> dispatch(const QString& strMethod, 
                                               const QSharedPointer<MCPContext>& pContext);
    
    /**
     * @brief 检查是否已注册某个路由
     * @param strMethod 方法名
     * @return 是否已注册
     */
    bool hasRoute(const QString& strMethod) const;
    
    /**
     * @brief 获取所有已注册的路由方法名
     * @return 方法名列表
     */
    QStringList getRegisteredRoutes() const;
    
    /**
     * @brief 添加中间件
     * @param pMiddleware 中间件对象指针
     * 
     * 说明：
     * - 中间件按添加顺序执行
     * - 先添加的中间件先执行
     */
    void use(QSharedPointer<IMCPMiddleware> pMiddleware);
    
    /**
     * @brief 清空所有中间件
     */
    void clearMiddlewares();
    
    /**
     * @brief 获取中间件数量
     * @return 中间件数量
     */
    int getMiddlewareCount() const;
    
private:
    // 路由表：方法名 -> 处理函数
    QMap<QString, RouteHandler> m_dictRoutes;
    
    // 中间件列表（按添加顺序执行）
    QList<QSharedPointer<IMCPMiddleware>> m_listMiddlewares;
};

