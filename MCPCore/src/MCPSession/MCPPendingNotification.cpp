/**
 * @file MCPPendingNotification.cpp
 * @brief MCP待发送通知结构实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPPendingNotification.h"

MCPPendingNotification::MCPPendingNotification(MCPPendingNotificationType type, const QString& strUri)
    : m_type(type)
    , m_strUri(strUri)
{
    // 验证：只有资源变化通知才应该有URI
    if (type != MCPPendingNotificationType::ResourceChanged)
    {
        m_strUri.clear();
    }
}

MCPPendingNotification::MCPPendingNotification(MCPPendingNotificationType type)
    : m_type(type)
    , m_strUri("")
{
    // 验证：列表变化通知不应该有URI
    // ResourceChanged类型必须使用带URI的构造函数
    if (type == MCPPendingNotificationType::ResourceChanged)
    {
        // 资源变化通知必须提供URI，使用此构造函数会创建一个无效的通知
        // 但为了兼容性，我们允许创建，只是URI为空
        m_strUri = "";
    }
}

MCPPendingNotification::MCPPendingNotification()
    : m_type(MCPPendingNotificationType::ResourceChanged)
    , m_strUri("")
{
}

MCPPendingNotification::MCPPendingNotification(const MCPPendingNotification& other)
    : m_type(other.m_type)
    , m_strUri(other.m_strUri)
{
}

MCPPendingNotification& MCPPendingNotification::operator=(const MCPPendingNotification& other)
{
    if (this != &other)
    {
        m_type = other.m_type;
        m_strUri = other.m_strUri;
    }
    return *this;
}

bool MCPPendingNotification::operator==(const MCPPendingNotification& other) const
{
    return m_type == other.m_type && m_strUri == other.m_strUri;
}

bool MCPPendingNotification::operator<(const MCPPendingNotification& other) const
{
    if (m_type != other.m_type)
    {
        return static_cast<int>(m_type) < static_cast<int>(other.m_type);
    }
    return m_strUri < other.m_strUri;
}

MCPPendingNotificationType MCPPendingNotification::getType() const
{
    return m_type;
}

QString MCPPendingNotification::getMethod() const
{
    return getMethodByType(m_type);
}

QString MCPPendingNotification::getUri() const
{
    return m_strUri;
}

bool MCPPendingNotification::isResourceChanged() const
{
    return m_type == MCPPendingNotificationType::ResourceChanged;
}

bool MCPPendingNotification::isResourcesListChanged() const
{
    return m_type == MCPPendingNotificationType::ResourcesListChanged;
}

bool MCPPendingNotification::isToolsListChanged() const
{
    return m_type == MCPPendingNotificationType::ToolsListChanged;
}

bool MCPPendingNotification::isPromptsListChanged() const
{
    return m_type == MCPPendingNotificationType::PromptsListChanged;
}

bool MCPPendingNotification::hasUri() const
{
    return !m_strUri.isEmpty();
}

QString MCPPendingNotification::getMethodByType(MCPPendingNotificationType type)
{
    switch (type)
    {
    case MCPPendingNotificationType::ResourceChanged:
        return "notifications/resources/updated";  // 根据 MCP 协议规范，资源更新通知方法名
    case MCPPendingNotificationType::ResourcesListChanged:
        return "notifications/resources/list_changed";
    case MCPPendingNotificationType::ToolsListChanged:
        return "notifications/tools/list_changed";
    case MCPPendingNotificationType::PromptsListChanged:
        return "notifications/prompts/list_changed";
    default:
        return "";
    }
}

