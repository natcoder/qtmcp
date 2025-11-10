/**
 * @file MCPMiddlewares.h
 * @brief MCP基础中间件集合
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include "IMCPMiddleware.h"
#include <QElapsedTimer>

/**
 * @brief 日志中间件
 * 
 * 职责：
 * - 记录每个请求的方法名
 * - 记录请求开始和结束
 * - 便于调试和追踪
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - { 和 } 要单独一行
 */
class MCPLoggingMiddleware : public IMCPMiddleware
{
public:
    MCPLoggingMiddleware();
    virtual ~MCPLoggingMiddleware();
    
    QSharedPointer<MCPServerMessage> process(
        const QSharedPointer<MCPContext>& pContext,
        std::function<QSharedPointer<MCPServerMessage>()> next) override;
};

/**
 * @brief 性能监控中间件
 * 
 * 职责：
 * - 监控请求处理时间
 * - 识别和记录慢请求
 * - 提供性能统计数据
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 数值类型添加 n 前缀
 * - { 和 } 要单独一行
 */
class MCPPerformanceMiddleware : public IMCPMiddleware
{
public:
    /**
     * @brief 构造函数
     * @param nSlowThresholdMs 慢请求阈值（毫秒），默认1000ms
     */
    explicit MCPPerformanceMiddleware(qint64 nSlowThresholdMs = 1000);
    virtual ~MCPPerformanceMiddleware();
    
    QSharedPointer<MCPServerMessage> process(
        const QSharedPointer<MCPContext>& pContext,
        std::function<QSharedPointer<MCPServerMessage>()> next) override;
        
private:
    qint64 m_nSlowThresholdMs;
};

/**
 * @brief 会话验证中间件
 * 
 * 职责：
 * - 验证会话状态
 * - 确保请求在正确的会话状态下执行
 * - 拦截未授权的请求
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - { 和 } 要单独一行
 */
class MCPSessionValidationMiddleware : public IMCPMiddleware
{
public:
    MCPSessionValidationMiddleware();
    virtual ~MCPSessionValidationMiddleware();
    
    QSharedPointer<MCPServerMessage> process(
        const QSharedPointer<MCPContext>& pContext,
        std::function<QSharedPointer<MCPServerMessage>()> next) override;
};

