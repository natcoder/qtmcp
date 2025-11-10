#pragma once

/**
 * @file MCPError.h
 * @brief MCP错误类定义
 * @author zhangheng
 * @date 2025-01-08
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QMetaType>
#include "MCPErrorCode.h"

/**
 * @brief MCP错误类
 *
 * 封装完整的错误信息，包括错误码、消息和额外数据。
 * 支持JSON序列化和反序列化，用于网络传输。
 */
class MCPError
{
public:
    /**
     * @brief 默认构造函数
     */
    MCPError();

    /**
     * @brief 构造函数
     * @param code 错误码
     * @param message 错误消息（可选，默认使用标准消息）
     * @param data 额外错误数据（可选）
     */
    explicit MCPError(MCPErrorCode code,
                     const QString& message = QString(),
                     const QJsonValue& data = QJsonValue());

    /**
     * @brief 复制构造函数
     */
    MCPError(const MCPError& other);

    /**
     * @brief 赋值操作符
     */
    MCPError& operator=(const MCPError& other);

    /**
     * @brief 析构函数
     */
    ~MCPError();

public:
    /**
     * @brief 获取错误码
     */
    MCPErrorCode getCode() const;

    /**
     * @brief 设置错误码
     */
    void setCode(MCPErrorCode code);

    /**
     * @brief 获取错误消息
     */
    QString getMessage() const;

    /**
     * @brief 设置错误消息
     */
    void setMessage(const QString& message);

    /**
     * @brief 获取额外数据
     */
    QJsonValue getData() const;

    /**
     * @brief 设置额外数据
     */
    void setData(const QJsonValue& data);

    /**
     * @brief 检查是否为服务器错误
     */
    bool isServerError() const;


public:
    /**
     * @brief 转换为JSON对象
     * @return 符合JSON-RPC 2.0规范的错误对象
     */
    QJsonObject toJson() const;

    /**
     * @brief 创建JSON-RPC错误响应
     * @param requestId 请求ID
     * @return JSON-RPC 2.0格式的错误响应
     */
    QJsonObject toJsonResponse(const QJsonValue& requestId = QJsonValue::Null) const;

    /**
     * @brief 从JSON对象创建错误
     * @param json JSON对象
     * @return MCPError实例
     */
    static MCPError fromJson(const QJsonObject& json);


    /**
     * @brief 创建标准错误对象
     */
    static MCPError parseError(const QString& details = QString());
    static MCPError invalidRequest(const QString& details = QString());
    static MCPError methodNotFound(const QString& methodName);
    static MCPError invalidParams(const QString& details = QString());
    static MCPError internalError(const QString& details = QString());

    /**
     * @brief 创建MCP特定错误对象
     */
    static MCPError toolNotFound(const QString& toolName);
    static MCPError toolExecutionFailed(const QString& details = QString());
    static MCPError resourceNotFound(const QString& resourceUri);
    static MCPError sessionNotFound(const QString& sessionId);
    static MCPError authenticationFailed(const QString& details = QString());
    static MCPError authorizationFailed(const QString& details = QString());

private:
    MCPErrorCode m_code;        // 错误码
    QString m_message;          // 错误消息
    QJsonValue m_data;          // 额外数据
};

// Qt元类型声明
Q_DECLARE_METATYPE(MCPError)
