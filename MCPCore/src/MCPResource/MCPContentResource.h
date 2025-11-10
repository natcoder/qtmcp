/**
 * @file MCPContentResource.h
 * @brief MCP内容资源类（继承自MCPResource基类）
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include "MCPResource.h"
#include <functional>

/**
 * @brief MCP内容资源类
 * 
 * 职责：
 * - 实现资源内容的读取逻辑
 * - 支持内容提供函数方式
 * - 支持文本和二进制资源
 * 
 * 注意：
 * - 文件资源请使用 MCPFileResource 类
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 字符串类型添加 str 前缀
 * - { 和 } 要单独一行
 */
class MCPContentResource : public MCPResource
{
    Q_OBJECT
    
public:
    explicit MCPContentResource(const QString& strUri, 
                                QObject* pParent = nullptr);
    virtual ~MCPContentResource();
    
public:
    /**
     * @brief 设置资源名称（链式调用）
     * @param strName 资源名称
     * @return 返回this指针，支持链式调用
     */
    MCPContentResource* withName(const QString& strName);
    
    /**
     * @brief 设置资源描述（链式调用）
     * @param strDescription 资源描述
     * @return 返回this指针，支持链式调用
     */
    MCPContentResource* withDescription(const QString& strDescription);
    
    /**
     * @brief 设置MIME类型（链式调用）
     * @param strMimeType MIME类型
     * @return 返回this指针，支持链式调用
     */
    MCPContentResource* withMimeType(const QString& strMimeType);
    
    /**
     * @brief 设置内容提供函数（链式调用）
     * @param contentProvider 内容提供函数
     * @return 返回this指针，支持链式调用
     */
    MCPContentResource* withContentProvider(std::function<QString()> contentProvider);
    
public:
    /**
     * @brief 读取资源内容（实现基类纯虚函数）
     * @return 资源内容字符串
     */
    QString readContent() const override;
    
private:
    std::function<QString()> m_contentProvider;
};

