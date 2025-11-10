/**
 * @file MCPPendingNotification.h
 * @brief MCP待发送通知结构
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QString>
#include <QJsonObject>

/**
 * @brief 待发送通知类型枚举
 */
enum class MCPPendingNotificationType
{
    ResourceChanged,        // 资源变化通知（单个资源的内容或元数据变化）
    ResourcesListChanged,   // 资源列表变化通知（资源列表的增删）
    ToolsListChanged,       // 工具列表变化通知（工具列表的增删）
    PromptsListChanged      // 提示词列表变化通知（提示词列表的增删）
};

/**
 * @brief MCP待发送通知结构
 * 
 * 职责：
 * - 结构化表示待发送的通知信息
 * - 支持不同类型的通知（资源变化、列表变化等）
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 字符串类型添加 str 前缀
 * - { 和 } 要单独一行
 */
class MCPPendingNotification
{
public:
    /**
     * @brief 构造函数（资源变化通知）
     * @param strUri 资源URI
     */
    explicit MCPPendingNotification(MCPPendingNotificationType type, const QString& strUri);
    
    /**
     * @brief 构造函数（列表变化通知）
     * @param type 通知类型（ResourcesListChanged、ToolsListChanged、PromptsListChanged）
     */
    explicit MCPPendingNotification(MCPPendingNotificationType type);
    
    /**
     * @brief 默认构造函数
     */
    MCPPendingNotification();
    
    /**
     * @brief 拷贝构造函数
     */
    MCPPendingNotification(const MCPPendingNotification& other);
    
    /**
     * @brief 赋值运算符
     */
    MCPPendingNotification& operator=(const MCPPendingNotification& other);
    
    /**
     * @brief 比较运算符（用于去重）
     */
    bool operator==(const MCPPendingNotification& other) const;
    
    /**
     * @brief 小于运算符（用于排序）
     */
    bool operator<(const MCPPendingNotification& other) const;
    
    /**
     * @brief 获取通知类型
     * @return 通知类型
     */
    MCPPendingNotificationType getType() const;
    
    /**
     * @brief 获取通知方法名（用于生成通知消息）
     * @return 通知方法名
     */
    QString getMethod() const;
    
    /**
     * @brief 获取资源URI（仅用于资源变化通知）
     * @return 资源URI，如果不是资源变化通知则返回空字符串
     */
    QString getUri() const;
    
    /**
     * @brief 检查是否为资源变化通知
     * @return true表示是资源变化通知
     */
    bool isResourceChanged() const;
    
    /**
     * @brief 检查是否为资源列表变化通知
     * @return true表示是资源列表变化通知
     */
    bool isResourcesListChanged() const;
    
    /**
     * @brief 检查是否为工具列表变化通知
     * @return true表示是工具列表变化通知
     */
    bool isToolsListChanged() const;
    
    /**
     * @brief 检查是否为提示词列表变化通知
     * @return true表示是提示词列表变化通知
     */
    bool isPromptsListChanged() const;
    
    /**
     * @brief 检查是否有URI（仅资源变化通知有URI）
     * @return true表示有URI
     */
    bool hasUri() const;
    
private:
    /**
     * @brief 根据通知类型获取方法名
     * @param type 通知类型
     * @return 方法名
     */
    static QString getMethodByType(MCPPendingNotificationType type);
    
private:
    MCPPendingNotificationType m_type;  // 通知类型
    QString m_strUri;                   // 资源URI（仅用于资源变化通知）
};

