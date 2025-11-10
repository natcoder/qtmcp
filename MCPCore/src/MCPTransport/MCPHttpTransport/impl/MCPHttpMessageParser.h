#pragma once
#include <QObject>
#include <QByteArray>
#include <QString>
#include "MCPMessage.h"
#include "MCPHttpRequestData.h"
#include "MCPClientMessage.h"

/**
 * @brief MCP HTTP 解析器
 * 
 * 职责：
 * - 从HTTP请求中提取MCP消息
 * - 处理HTTP协议解析
 * - 验证HTTP格式和内容类型
 * - 支持JSON-RPC模式
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 指针类型添加 p 前缀
 * - { 和 } 要单独一行
 */
class MCPHttpMessageParser
{
public:
    /**
     * @brief 从HTTP请求中提取MCP消息
     * @param arrHttpData HTTP请求数据
     * @param pRequest 输出的MCP消息指针
     * @return 是否提取成功
     */
	static QSharedPointer<MCPClientMessage> genClientMessageFromHttp(const QSharedPointer<MCPHttpRequestData>& pHttpRequestData);
private:
    static QSharedPointer<MCPClientMessage> genXXClientMessage(const QSharedPointer<MCPClientMessage>& pClientMessage);
};