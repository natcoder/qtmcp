#pragma once

/**
 * @file MCPErrorHandler.h
 * @brief MCP错误处理工具类
 * @author zhangheng
 * @date 2025-01-08
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include <QObject>
#include <QJsonObject>
#include <functional>
#include "MCPError.h"

/**
 * @brief MCP错误处理工具类
 *
 * 提供简单的错误响应生成和日志记录功能。
 */
class MCPErrorHandler : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     */
    explicit MCPErrorHandler(QObject* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~MCPErrorHandler();

public:
    /**
     * @brief 创建成功响应
     * @param result 结果数据
     * @param requestId 请求ID
     * @return JSON-RPC 2.0格式的成功响应
     */
    static QJsonObject createSuccessResponse(const QJsonValue& result, const QJsonValue& requestId = QJsonValue::Null);

    /**
     * @brief 记录错误到日志
     * @param error 错误对象
     * @param context 错误上下文信息
     */
    static void logError(const MCPError& error, const QString& context = QString());
};

// 全局错误处理器实例
MCPErrorHandler* getGlobalErrorHandler();
