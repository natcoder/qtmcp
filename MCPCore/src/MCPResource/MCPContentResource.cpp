/**
 * @file MCPContentResource.cpp
 * @brief MCP内容资源类实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPContentResource.h"
#include "MCPLog/MCPLog.h"

MCPContentResource::MCPContentResource(const QString& strUri, 
                                       QObject* pParent)
    : MCPResource(strUri, pParent)
{
}

MCPContentResource::~MCPContentResource()
{
}

MCPContentResource* MCPContentResource::withName(const QString& strName)
{
    setName(strName);
    return this;
}

MCPContentResource* MCPContentResource::withDescription(const QString& strDescription)
{
    setDescription(strDescription);
    return this;
}

MCPContentResource* MCPContentResource::withMimeType(const QString& strMimeType)
{
    setMimeType(strMimeType);
    return this;
}

MCPContentResource* MCPContentResource::withContentProvider(std::function<QString()> contentProvider)
{
    m_contentProvider = contentProvider;
    return this;
}

QString MCPContentResource::readContent() const
{
    // 使用contentProvider获取内容
    if (m_contentProvider)
    {
        return m_contentProvider();
    }
    
    // 如果没有contentProvider，返回空字符串
    MCP_CORE_LOG_WARNING() << "MCPContentResource: 未设置内容提供函数，无法读取内容";
    return QString();
}

