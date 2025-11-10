/**
 * @file MCPFileResource.cpp
 * @brief MCP文件资源类实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPFileResource.h"
#include "Utils/MCPResourceContentGenerator.h"
#include "MCPLog/MCPLog.h"
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>

MCPFileResource::MCPFileResource(const QString& strUri,
                                 const QString& strFilePath,
                                 const QString& strName,
                                 QObject* pParent)
    : MCPContentResource(strUri, pParent)
    , m_strFilePath(strFilePath)
{
    // 如果提供了名称，设置名称；否则使用文件名作为名称
    if (!strName.isEmpty())
    {
        withName(strName);
    }
    else
    {
        QFileInfo fileInfo(strFilePath);
        withName(fileInfo.fileName());
    }
    
    // 推断MIME类型
    inferMimeType();
    
    // 设置文件内容提供函数
    withContentProvider(createFileContentProvider());
}

MCPFileResource::~MCPFileResource()
{
}

QString MCPFileResource::getFilePath() const
{
    return m_strFilePath;
}

std::function<QString()> MCPFileResource::createFileContentProvider() const
{
    return [this]() -> QString
    {
        if (m_strFilePath.isEmpty())
        {
            MCP_CORE_LOG_WARNING() << "MCPFileResource: 文件路径为空，无法读取内容";
            return QString();
        }
        
        // 根据MIME类型决定读取方式
        if (MCPResourceContentGenerator::isTextMimeType(getMimeType()))
        {
            // 文本类型，直接读取文本内容
            return MCPResourceContentGenerator::readFileAsText(m_strFilePath);
        }
        else
        {
            // 二进制类型，返回Base64编码的内容
            return MCPResourceContentGenerator::readFileAsBase64(m_strFilePath);
        }
    };
}

void MCPFileResource::inferMimeType()
{
    if (m_strFilePath.isEmpty())
    {
        withMimeType("text/plain");
        return;
    }
    
    // 如果MIME类型已经设置，不重新推断
    if (!getMimeType().isEmpty())
    {
        return;
    }
    
    // 使用QMimeDatabase根据文件扩展名推断MIME类型
    QMimeDatabase mimeDb;
    QMimeType mimeType = mimeDb.mimeTypeForFile(m_strFilePath);
    if (mimeType.isValid())
    {
        withMimeType(mimeType.name());
    }
    else
    {
        // 如果无法推断，默认使用text/plain
        withMimeType("text/plain");
        MCP_CORE_LOG_WARNING() << "MCPFileResource: 无法推断文件MIME类型，使用默认值text/plain:" << m_strFilePath;
    }
}

