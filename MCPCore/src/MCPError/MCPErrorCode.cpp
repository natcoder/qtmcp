/**
 * @file MCPErrorCode.cpp
 * @brief MCP错误码实现
 * @author zhangheng
 * @date 2025-01-08
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPErrorCode.h"

QString getErrorMessage(MCPErrorCode code)
{
    switch (code)
    {
    case MCPErrorCode::SUCCESS:
        return QString("成功");

    // JSON-RPC 2.0 预定义错误（根据 JSON-RPC 2.0 规范，错误消息应该使用英文）
    case MCPErrorCode::PARSE_ERROR:
        return QString("Parse error");
    case MCPErrorCode::INVALID_REQUEST:
        return QString("Invalid Request");
    case MCPErrorCode::METHOD_NOT_FOUND:
        return QString("Method not found");
    case MCPErrorCode::INVALID_PARAMS:
        return QString("Invalid params");
    case MCPErrorCode::INTERNAL_ERROR:
        return QString("Internal error");

    // MCP 特定错误（根据 MCP 协议规范，错误消息应该使用英文）
    case MCPErrorCode::TOOL_NOT_FOUND:
        return QString("Tool not found");
    case MCPErrorCode::RESOURCE_NOT_FOUND:
        return QString("Resource not found");  // 根据 MCP 协议规范，使用英文消息
    case MCPErrorCode::TOOL_EXECUTION_FAILED:
        return QString("Tool execution failed");
    case MCPErrorCode::SESSION_NOT_FOUND:
        return QString("会话未找到：指定的会话不存在");
    case MCPErrorCode::SESSION_EXPIRED:
        return QString("会话已过期：会话已失效，请重新建立连接");
    case MCPErrorCode::AUTHENTICATION_FAILED:
        return QString("认证失败：身份验证未通过");
    case MCPErrorCode::AUTHORIZATION_FAILED:
        return QString("授权失败：权限验证未通过");
    case MCPErrorCode::RATE_LIMIT_EXCEEDED:
        return QString("频率限制：请求过于频繁，请稍后重试");
    case MCPErrorCode::CONFIGURATION_ERROR:
        return QString("配置错误：服务器配置异常");

    // 网络和传输错误
    case MCPErrorCode::CONNECTION_CLOSED:
        return QString("连接已关闭：网络连接意外断开");
    case MCPErrorCode::CONNECTION_TIMEOUT:
        return QString("连接超时：网络请求超时");
    case MCPErrorCode::PROTOCOL_ERROR:
        return QString("协议错误：协议格式不符合规范");
    case MCPErrorCode::MESSAGE_TOO_LARGE:
        return QString("消息过大：消息大小超过限制");

    // 业务逻辑错误
    case MCPErrorCode::INVALID_TOOL_SCHEMA:
        return QString("无效的工具模式：工具定义不符合规范");
    case MCPErrorCode::TOOL_VALIDATION_FAILED:
        return QString("工具验证失败：工具参数验证未通过");
    case MCPErrorCode::RESOURCE_ACCESS_DENIED:
        return QString("资源访问被拒绝：无权限访问该资源");
    case MCPErrorCode::SUBSCRIPTION_NOT_SUPPORTED:
        return QString("不支持订阅：该资源不支持订阅操作");

    default:
        return QString("未知错误：未定义的错误码");
    }
}

bool isServerError(MCPErrorCode code)
{
    // 服务器错误码范围：-32099 到 -32600，以及 -32700 (PARSE_ERROR)
    int intCode = static_cast<int>(code);
    return (intCode >= -32700 && intCode <= -32000) || intCode == -32603;
}
