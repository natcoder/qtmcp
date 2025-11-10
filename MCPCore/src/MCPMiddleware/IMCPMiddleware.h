/**
 * @file IMCPMiddleware.h
 * @brief MCP中间件接口
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QSharedPointer>
#include <functional>

class MCPContext;
class MCPServerMessage;

/**
 * @brief MCP中间件接口
 * 
 * 职责：
 * - 定义中间件的通用行为
 * - 支持请求处理前后的拦截
 * - 实现横切关注点（日志、监控、验证等）
 * 
 * 设计模式：
 * - 责任链模式（Chain of Responsibility）
 * - 中间件模式（Middleware Pattern）
 * 
 * 使用说明：
 * - process方法中调用next()继续执行下一个中间件
 * - 可以在next()前后添加处理逻辑
 * - 返回nullptr表示继续执行，返回非空则中断管道
 * 
 * 编码规范：
 * - 指针类型添加 p 前缀
 * - { 和 } 要单独一行
 */
class IMCPMiddleware
{
public:
    virtual ~IMCPMiddleware() {}
    
    /**
     * @brief 处理请求
     * @param pContext 请求上下文
     * @param next 下一个中间件/处理器函数
     * @return 响应消息。返回nullptr表示继续执行next，返回非空则使用该响应
     * 
     * 使用示例：
     * @code
     * QSharedPointer<MCPServerMessage> process(...)
     * {
     *     // 前置处理
     *     qDebug() << "Before processing";
     *     
     *     // 调用下一个中间件
     *     auto pResponse = next();
     *     
     *     // 后置处理
     *     qDebug() << "After processing";
     *     
     *     return pResponse;
     * }
     * @endcode
     */
    virtual QSharedPointer<MCPServerMessage> process(
        const QSharedPointer<MCPContext>& pContext,
        std::function<QSharedPointer<MCPServerMessage>()> next) = 0;
};

