/**
 * @file MCPError.cpp
 * @brief MCP错误类实现
 * @author zhangheng
 * @date 2025-01-08
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPError.h"
#include <QJsonObject>

MCPError::MCPError()
    : m_code(MCPErrorCode::SUCCESS)
    , m_message(getErrorMessage(MCPErrorCode::SUCCESS))
    , m_data(QJsonValue::Null)
{
}

MCPError::MCPError(MCPErrorCode code, const QString& message, const QJsonValue& data)
    : m_code(code)
    , m_message(message.isEmpty() ? getErrorMessage(code) : message)
    , m_data(data)
{
}

MCPError::MCPError(const MCPError& other)
    : m_code(other.m_code)
    , m_message(other.m_message)
    , m_data(other.m_data)
{
}

MCPError& MCPError::operator=(const MCPError& other)
{
    if (this != &other)
    {
        m_code = other.m_code;
        m_message = other.m_message;
        m_data = other.m_data;
    }
    return *this;
}

MCPError::~MCPError()
{
}

MCPErrorCode MCPError::getCode() const
{
    return m_code;
}

void MCPError::setCode(MCPErrorCode code)
{
    m_code = code;
    // 如果消息为空，则使用默认消息
    if (m_message.isEmpty())
    {
        m_message = getErrorMessage(code);
    }
}

QString MCPError::getMessage() const
{
    return m_message;
}

void MCPError::setMessage(const QString& message)
{
    m_message = message;
}

QJsonValue MCPError::getData() const
{
    return m_data;
}

void MCPError::setData(const QJsonValue& data)
{
    m_data = data;
}

bool MCPError::isServerError() const
{
    return ::isServerError(m_code);
}

QJsonObject MCPError::toJson() const
{
    QJsonObject errorObj;
    errorObj.insert("code", static_cast<int>(m_code));
    errorObj.insert("message", m_message);

    if (!m_data.isNull())
    {
        errorObj.insert("data", m_data);
    }

    return errorObj;
}

QJsonObject MCPError::toJsonResponse(const QJsonValue& requestId) const
{
    QJsonObject response;
    response.insert("jsonrpc", "2.0");
    response.insert("error", toJson());

    if (!requestId.isNull())
    {
        response.insert("id", requestId);
    }

    return response;
}

MCPError MCPError::fromJson(const QJsonObject& json)
{
    MCPErrorCode code = static_cast<MCPErrorCode>(json.value("code").toInt());
    QString strMessage = json.value("message").toString();
    QJsonValue data = json.value("data");

    return MCPError(code, strMessage, data);
}

// 静态工厂方法

MCPError MCPError::parseError(const QString& details)
{
    QString message = getErrorMessage(MCPErrorCode::PARSE_ERROR);
    if (!details.isEmpty())
    {
        message += QString(" - ") + details;
    }
    return MCPError(MCPErrorCode::PARSE_ERROR, message);
}

MCPError MCPError::invalidRequest(const QString& details)
{
    QString message = getErrorMessage(MCPErrorCode::INVALID_REQUEST);
    if (!details.isEmpty())
    {
        message += QString(" - ") + details;
    }
    return MCPError(MCPErrorCode::INVALID_REQUEST, message);
}

MCPError MCPError::methodNotFound(const QString& methodName)
{
    // 根据 JSON-RPC 2.0 和 MCP 协议规范，错误消息应该使用英文
    QString message = getErrorMessage(MCPErrorCode::METHOD_NOT_FOUND);
    if (!methodName.isEmpty())
    {
        message += QString(": %1").arg(methodName);
    }
    return MCPError(MCPErrorCode::METHOD_NOT_FOUND, message);
}

MCPError MCPError::invalidParams(const QString& details)
{
    QString message = getErrorMessage(MCPErrorCode::INVALID_PARAMS);
    if (!details.isEmpty())
    {
        message += QString(" - ") + details;
    }
    return MCPError(MCPErrorCode::INVALID_PARAMS, message);
}

MCPError MCPError::internalError(const QString& details)
{
    QString message = getErrorMessage(MCPErrorCode::INTERNAL_ERROR);
    if (!details.isEmpty())
    {
        message += QString(" - ") + details;
    }
    return MCPError(MCPErrorCode::INTERNAL_ERROR, message);
}

MCPError MCPError::toolNotFound(const QString& toolName)
{
    // 根据 MCP 协议规范，工具不存在的错误消息应该是英文
    QString message = "Tool not found";
    
    // 根据 MCP 协议规范，data 字段可以包含工具名称
    QJsonObject data;
    if (!toolName.isEmpty())
    {
        data["name"] = toolName;
    }
    
    return MCPError(MCPErrorCode::TOOL_NOT_FOUND, message, data);
}

MCPError MCPError::toolExecutionFailed(const QString& details)
{
    // 根据 MCP 协议规范，工具执行失败的错误消息应该是英文
    QString message = getErrorMessage(MCPErrorCode::TOOL_EXECUTION_FAILED);
    if (!details.isEmpty())
    {
        message += QString(" - ") + details;
    }
    return MCPError(MCPErrorCode::TOOL_EXECUTION_FAILED, message);
}

MCPError MCPError::resourceNotFound(const QString& resourceUri)
{
    // 根据 MCP 协议规范，资源不存在的错误消息应该是英文
    QString message = "Resource not found";
    
    // 根据 MCP 协议规范，data 字段应包含 uri 对象
    QJsonObject data;
    data["uri"] = resourceUri;
    
    return MCPError(MCPErrorCode::RESOURCE_NOT_FOUND, message, data);
}

MCPError MCPError::sessionNotFound(const QString& sessionId)
{
    QString message = QString("会话未找到：%1").arg(sessionId);
    return MCPError(MCPErrorCode::SESSION_NOT_FOUND, message);
}

MCPError MCPError::authenticationFailed(const QString& details)
{
    QString message = getErrorMessage(MCPErrorCode::AUTHENTICATION_FAILED);
    if (!details.isEmpty())
    {
        message += QString(" - ") + details;
    }
    return MCPError(MCPErrorCode::AUTHENTICATION_FAILED, message);
}

MCPError MCPError::authorizationFailed(const QString& details)
{
    QString message = getErrorMessage(MCPErrorCode::AUTHORIZATION_FAILED);
    if (!details.isEmpty())
    {
        message += QString(" - ") + details;
    }
    return MCPError(MCPErrorCode::AUTHORIZATION_FAILED, message);
}
