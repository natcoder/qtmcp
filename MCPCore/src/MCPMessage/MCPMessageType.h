#pragma once
#include <QFlags>
#include <QObject>

class MCPMessageType
{
	Q_GADGET
public:
	enum Value
	{
		// 基础类型
		None = 0,

		// 传输层 (bits 0-7)
		TransportMask = 0xFF,
		SseTransport = 1 << 0,           // Server-Sent Events 传输
		StreamableTransport = 1 << 1,    // 可流式传输
		StdioTransport = 1 << 2,         // Stdio传输

		// 消息内容类型 (bits 8-15) - 请求/响应/通知
		ContentTypeMask = 0xFF00,
		Request = 1 << 8,         // 请求消息 (client -> server)
		Response = 1 << 9,        // 响应消息 (server -> client)  
		Notification = 1 << 10,   // 通知消息 (双向)
		RequestNotification = 1 << 11,   // 通知消息 (双向)
		ResponseNotification = 1 << 12,   // 通知消息 (双向)

		// 控制消息 (bits 16-23)
		ControlMask = 0xFF0000,
		Connect = 1 << 16,            // 连接管理
		Initialize = 1 << 17,            // 初始化
		Ping = 1 << 18,                  // Ping
		Accept = 1 << 19,                  //Accept

		// 调用类型 (bits 24-31) - 单个/批量
		InvocationTypeMask = 0xFF000000,
		Single = 1 << 24,            // 单个
		Batch = 1 << 25,             // 批量
	};
	Q_ENUM(Value)
	Q_DECLARE_FLAGS(Flags, Value)

		// 简化工具函数
	static QString toString(Flags type);
	static bool isControlMessage(Flags type);
	static Flags getTransportType(Flags type);
	static Flags getContentType(Flags type);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MCPMessageType::Flags)

// 预定义组合
namespace MCPMessagePresets
{
	// 控制消息
	extern const MCPMessageType::Flags SseConnection;
	extern const MCPMessageType::Flags SseInitialize;
	extern const MCPMessageType::Flags SsePing;

	// 单个调用
	extern const MCPMessageType::Flags SseSingleRequest;
	extern const MCPMessageType::Flags SseSingleResponse;
	extern const MCPMessageType::Flags SseSingleNotification;

	// 批量调用
	extern const MCPMessageType::Flags SseBatchCall;
	extern const MCPMessageType::Flags StreamBatchCall;
}