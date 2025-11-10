#pragma once

/**
 * @file MCPErrorCode.h
 * @brief MCP错误码定义
 * @author zhangheng
 * @date 2025-01-08
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include <QString>

/**
 * @brief MCP错误码枚举
 *
 * 遵循JSON-RPC 2.0错误码规范：
 * -32600 to -32699: 服务器错误
 * -32700 to -32000: 预定义错误
 */
enum class MCPErrorCode {
    // 成功
    SUCCESS = 0,

    // JSON-RPC 2.0 预定义错误码
    PARSE_ERROR = -32700,          // 解析错误
    INVALID_REQUEST = -32600,      // 无效请求
    METHOD_NOT_FOUND = -32601,     // 方法未找到
    INVALID_PARAMS = -32602,       // 无效参数
    INTERNAL_ERROR = -32603,       // 内部错误

    // MCP 特定错误码 (-32000 到 -32099)
    SERVER_ERROR_BASE = -32000,
    TOOL_NOT_FOUND = -32001,       // 工具未找到
    RESOURCE_NOT_FOUND = -32002,   // 资源未找到（符合 MCP 协议规范）
    TOOL_EXECUTION_FAILED = -32003, // 工具执行失败
    SESSION_NOT_FOUND = -32004,    // 会话未找到
    SESSION_EXPIRED = -32005,      // 会话已过期
    AUTHENTICATION_FAILED = -32006, // 认证失败
    AUTHORIZATION_FAILED = -32007,  // 授权失败
    RATE_LIMIT_EXCEEDED = -32008,   // 频率限制
    CONFIGURATION_ERROR = -32009,   // 配置错误

    // 网络和传输错误 (-32100 到 -32199)
    NETWORK_ERROR_BASE = -32100,
    CONNECTION_CLOSED = -32101,     // 连接已关闭
    CONNECTION_TIMEOUT = -32102,    // 连接超时
    PROTOCOL_ERROR = -32103,        // 协议错误
    MESSAGE_TOO_LARGE = -32104,     // 消息过大

    // 业务逻辑错误 (-32200 到 -32299)
    BUSINESS_ERROR_BASE = -32200,
    INVALID_TOOL_SCHEMA = -32201,   // 无效的工具模式
    TOOL_VALIDATION_FAILED = -32202, // 工具验证失败
    RESOURCE_ACCESS_DENIED = -32203, // 资源访问被拒绝
    SUBSCRIPTION_NOT_SUPPORTED = -32204, // 不支持订阅
};

/**
 * @brief 获取错误码对应的描述信息
 * @param code 错误码
 * @return 错误描述字符串
 */
QString getErrorMessage(MCPErrorCode code);

/**
 * @brief 检查错误码是否为服务器错误
 * @param code 错误码
 * @return true表示是服务器错误，false表示是客户端错误
 */
bool isServerError(MCPErrorCode code);
