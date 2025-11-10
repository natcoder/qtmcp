/**
 * @file MCPRequestType.cpp
 * @brief MCP请求类型实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPMessageType.h"
#include <QStringList>

// 预定义组合实现
namespace MCPMessagePresets
{
	// 控制消息
	const MCPMessageType::Flags SseConnection =
		MCPMessageType::SseTransport | MCPMessageType::Connect;

	const MCPMessageType::Flags SseInitialize =
		MCPMessageType::SseTransport | MCPMessageType::Initialize;

	const MCPMessageType::Flags SsePing =
		MCPMessageType::SseTransport | MCPMessageType::Ping;

	// 单个调用
	const MCPMessageType::Flags SseSingleRequest =
		MCPMessageType::SseTransport | MCPMessageType::Request | MCPMessageType::Single;

	const MCPMessageType::Flags SseSingleResponse =
		MCPMessageType::SseTransport | MCPMessageType::Response | MCPMessageType::Single;

	const MCPMessageType::Flags SseSingleNotification =
		MCPMessageType::SseTransport | MCPMessageType::Notification | MCPMessageType::Single;

	// 批量调用
	const MCPMessageType::Flags SseBatchCall =
		MCPMessageType::SseTransport | MCPMessageType::Batch;

	const MCPMessageType::Flags StreamBatchCall =
		MCPMessageType::StreamableTransport | MCPMessageType::Batch;
}

// 工具函数实现
QString MCPMessageType::toString(Flags type)
{
	if (type == None) return "None";

	QStringList parts;

	// 传输类型
	if (type & SseTransport) parts << "SSE";
	if (type & StreamableTransport) parts << "Stream";

	// 单个调用的消息类型
	if (type & Request) parts << "Request";
	if (type & Response) parts << "Response";
	if (type & Notification) parts << "Notification";

	// 控制消息
	if (type & Connect) parts << "Connect";
	if (type & Initialize) parts << "Initialize";
	if (type & Ping) parts << "Ping";

	// 调用模式
	if (type & Single) parts << "SingleCall";
	if (type & Batch) parts << "BatchCall";

	return parts.join("|");
}

bool MCPMessageType::isControlMessage(Flags type)
{
	return (type & ControlMask) != None;
}


MCPMessageType::Flags MCPMessageType::getTransportType(Flags type)
{
	return type & TransportMask;
}

MCPMessageType::Flags MCPMessageType::getContentType(Flags type)
{
	return type & ContentTypeMask;
}