/**
 * @file MCPInitializeHandler.cpp
 * @brief MCP初始化处理器实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPInitializeHandler.h"
#include "MCPContext.h"
#include "MCPMessage/MCPClientMessage.h"
#include "MCPMessage/MCPServerMessage.h"
#include "MCPSession/MCPSession.h"
#include "IMCPServerConfig.h"
#include "MCPLog/MCPLog.h"
#include "MCPClientinitializeMessage.h"
#include "MCPError/MCPError.h"
#include "MCPError/MCPErrorCode.h"

MCPInitializeHandler::MCPInitializeHandler(IMCPServerConfig* pConfig, QObject* pParent)
    : QObject(pParent)
    , m_pConfig(pConfig)
{
}

MCPInitializeHandler::~MCPInitializeHandler()
{
}

QSharedPointer<MCPServerMessage> MCPInitializeHandler::handleInitialize(const QSharedPointer<MCPContext>& pContext)
{
    const auto pInitializeMessage = pContext->getClientMessage().dynamicCast<MCPClientInitializeMessage>();
    if (!pInitializeMessage)
    {
        MCP_CORE_LOG_WARNING() << "无法转换为MCPClientInitializeMessage";
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext,
            MCPError::invalidRequest("Invalid initialize message")
        );
    }
    
    // 验证protocolVersion参数（必需）
    if (!pInitializeMessage->hasProtocolVersion())
    {
        MCP_CORE_LOG_WARNING() << "initialize请求缺少必需参数: protocolVersion";
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext,
            MCPError::invalidParams("Missing required parameter: protocolVersion")
        );
    }
    
    // 验证protocolVersion格式
    if (!pInitializeMessage->isProtocolVersionFormatValid())
    {
        QString strProtocolVersion = pInitializeMessage->getClientProtocolVersion();
        MCP_CORE_LOG_WARNING() << "protocolVersion格式错误:" << strProtocolVersion;
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext,
            MCPError::invalidParams("Invalid protocolVersion format, expected YYYY-MM-DD")
        );
    }
    
    // 验证协议版本是否支持
    // 支持的协议版本列表（按时间顺序，从新到旧）
    QStringList supportedVersions = {"2025-06-18", "2025-03-26", "2024-11-05"};
    if (!pInitializeMessage->isProtocolVersionSupported(supportedVersions))
    {
        QString strProtocolVersion = pInitializeMessage->getClientProtocolVersion();
        MCP_CORE_LOG_WARNING() << "不支持的协议版本:" << strProtocolVersion << "，支持的版本:" << supportedVersions.join(", ");
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext,
            MCPError::invalidParams(QString("Unsupported protocol version: %1. Supported versions: %2")
                .arg(strProtocolVersion)
                .arg(supportedVersions.join(", ")))
        );
    }
    
    // 验证capabilities参数（如果提供，必须是对象）
    if (!pInitializeMessage->isCapabilitiesValid())
    {
        MCP_CORE_LOG_WARNING() << "capabilities参数类型错误";
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext,
            MCPError::invalidParams("Invalid capabilities parameter, must be an object")
        );
    }
    
    // 验证clientInfo参数（如果提供，必须是对象）
    if (!pInitializeMessage->isClientInfoValid())
    {
        MCP_CORE_LOG_WARNING() << "clientInfo参数类型错误";
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext,
            MCPError::invalidParams("Invalid clientInfo parameter, must be an object")
        );
    }
    
    // 协商协议版本
    QString negotiatedVersion = pInitializeMessage->getClientProtocolVersion();
    
    // 获取服务器信息并验证
    QString strServerName = m_pConfig->getServerName();
    QString strServerTitle = m_pConfig->getServerTitle();
    QString strServerVersion = m_pConfig->getServerVersion();
    
    // 验证serverInfo字段非空（根据MCP规范，这些字段应该是非空的）
    if (strServerName.isEmpty() || strServerTitle.isEmpty() || strServerVersion.isEmpty())
    {
        MCP_CORE_LOG_WARNING() << "serverInfo配置不完整，name:" << strServerName 
                                << ", title:" << strServerTitle 
                                << ", version:" << strServerVersion;
        return QSharedPointer<MCPServerErrorResponse>::create(
            pContext,
            MCPError::internalError("Server configuration incomplete: serverInfo fields cannot be empty")
        );
    }
    
    // 构建初始化响应
    QJsonObject result = {
        {"protocolVersion", negotiatedVersion},
        {"capabilities", QJsonObject{
            {"prompts", QJsonObject{ {"listChanged", true}}},      // 支持提示词
            {"resources", QJsonObject{ {"listChanged", true}, {"subscribe", true}}},    // 支持资源
            {"tools", QJsonObject{ {"listChanged", true}}}         // 支持工具
        }},
        {"serverInfo", QJsonObject{
            {"name", strServerName},
            {"title", strServerTitle},
            {"version", strServerVersion}
        }}
    };
    
    // instructions字段是可选的，只有当非空时才添加
    QString strInstructions = m_pConfig->getInstructions();
    if (!strInstructions.isEmpty())
    {
        result["instructions"] = strInstructions;
    }
    
    // 更新会话状态
    pContext->getSession()->setStatus(EnumSessionStatus::enInitializing);
    pContext->getSession()->setProtocolVersion(negotiatedVersion);
    
    MCP_CORE_LOG_INFO() << "initialize成功，协议版本:" << negotiatedVersion;
    
    return QSharedPointer<MCPServerMessage>::create(pContext, result);
}

QSharedPointer<MCPServerMessage> MCPInitializeHandler::handleInitialized(const QSharedPointer<MCPContext>& pContext)
{
    // initialized 是一个通知（notification），根据 MCP 协议规范，通知不应该有响应
    // 服务器应该静默处理这个通知，只更新会话状态即可
    pContext->getSession()->setStatus(EnumSessionStatus::enInitialized);
    MCP_CORE_LOG_INFO() << "收到 initialized 通知，会话状态已更新为已初始化";
    
    // 返回空指针表示不发送响应（符合 JSON-RPC 2.0 和 MCP 协议规范）
    return QSharedPointer<MCPServerMessage>::create(pContext, (MCPMessageType::Flags)MCPMessageType::ResponseNotification);
}

