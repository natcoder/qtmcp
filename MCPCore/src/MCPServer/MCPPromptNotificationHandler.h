/**
 * @file MCPPromptNotificationHandler.h
 * @brief MCP提示词通知处理器
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include "MCPNotificationHandlerBase.h"

/**
 * @brief MCP提示词通知处理器
 * 
 * 职责：
 * - 处理提示词列表变化通知（广播通知）
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 指针类型添加 p 前缀
 * - { 和 } 要单独一行
 */
class MCPPromptNotificationHandler : public MCPNotificationHandlerBase
{
    Q_OBJECT
    
public:
    /**
     * @brief 构造函数
     * @param pServer 服务器对象（必需）
     * @param pParent 父对象
     */
    explicit MCPPromptNotificationHandler(MCPServer* pServer, QObject* pParent = nullptr);
    
    virtual ~MCPPromptNotificationHandler();
    
public slots:
    /**
     * @brief 处理提示词列表变化事件（广播通知）
     */
    void onPromptsListChanged();
};

