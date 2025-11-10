/**
 * @file MCPHttpResponseBuilder.h
 * @brief HTTP响应构建器 - 统一构建HTTP响应格式
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QByteArray>
#include <QString>
#include <QSharedPointer>

class MCPSession;

/**
 * @brief HTTP响应构建器
 * 
 * 职责：
 * - 统一构建HTTP响应格式
 * - 减少HTTP响应构建代码的重复
 * - 提供清晰的接口构建不同类型的HTTP响应
 * 
 * 编码规范：
 * - 静态方法用于构建响应
 * - { 和 } 要单独一行
 */
class MCPHttpResponseBuilder
{
public:
    /**
     * @brief 构建SSE连接响应
     * @param strSessionUri 会话URI
     * @return HTTP响应数据
     */
    static QByteArray buildSseConnectResponse(const QString& strSessionUri);

    /**
     * @brief 构建SSE消息响应
     * @param strMessageData 消息数据（JSON格式）
     * @return HTTP响应数据
     */
    static QByteArray buildSseMessageResponse(const QByteArray& strMessageData);

    /**
     * @brief 构建Streamable连接/响应
     * @param strMessageData 消息数据（JSON格式）
     * @param pSession 会话对象（用于获取SessionId和ProtocolVersion）
     * @return HTTP响应数据
     */
    static QByteArray buildStreamableResponse(const QByteArray& strMessageData, const QSharedPointer<MCPSession>& pSession);

    /**
     * @brief 构建接受通知响应（202 Accepted）
     * @return HTTP响应数据
     */
    static QByteArray buildAcceptResponse();

private:
    /**
     * @brief 构建SSE响应头
     * @return SSE响应头
     */
    static QByteArray buildSseHeaders();

    /**
     * @brief 构建Streamable响应头
     * @param nContentLength 内容长度
     * @param strSessionId 会话ID
     * @param strProtocolVersion 协议版本
     * @return Streamable响应头
     */
    static QByteArray buildStreamableHeaders(int nContentLength, const QString& strSessionId, const QString& strProtocolVersion);

    /**
     * @brief 构建通用CORS头
     * @return CORS头字符串
     */
    static QByteArray buildCorsHeaders();
};

