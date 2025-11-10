/**
 * @file MCPResourceContentGenerator.cpp
 * @brief MCP资源内容生成器实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPResourceContentGenerator.h"
#include "MCPLog/MCPLog.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QTextStream>

QJsonObject MCPResourceContentGenerator::generateResourceContent(const QString& strMimeType, 
                                                                  const QString& strFilePath,
                                                                  const QString& strUri)
{
    QJsonObject result;
    
    // 检查文件是否存在
    QFileInfo fileInfo(strFilePath);
    if (!fileInfo.exists() || !fileInfo.isFile())
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceContentGenerator: 文件不存在或不是文件:" << strFilePath;
        return result;
    }
    
    // 生成或使用提供的URI
    QString strResourceUri = strUri;
    if (strResourceUri.isEmpty())
    {
        strResourceUri = generateUriFromFilePath(strFilePath);
    }
    
    result["uri"] = strResourceUri;
    
    // 读取文件内容
    QJsonArray contents;
    QJsonObject contentObj;
    contentObj["uri"] = strResourceUri;
    contentObj["mimeType"] = strMimeType;
    
    if (isTextMimeType(strMimeType))
    {
        // 文本类型，使用text字段
        QString strTextContent = readFileAsText(strFilePath);
        if (strTextContent.isEmpty() && fileInfo.size() > 0)
        {
            MCP_CORE_LOG_WARNING() << "MCPResourceContentGenerator: 文本文件读取失败或为空:" << strFilePath;
            return QJsonObject();
        }
        contentObj["text"] = strTextContent;
    }
    else
    {
        // 二进制类型，使用blob字段（Base64编码）
        QString strBase64Content = readFileAsBase64(strFilePath);
        if (strBase64Content.isEmpty())
        {
            MCP_CORE_LOG_WARNING() << "MCPResourceContentGenerator: 二进制文件读取失败:" << strFilePath;
            return QJsonObject();
        }
        contentObj["blob"] = strBase64Content;
    }
    
    contents.append(contentObj);
    result["contents"] = contents;
    
    MCP_CORE_LOG_DEBUG() << "MCPResourceContentGenerator: 成功生成资源内容 - URI:" << strResourceUri
                         << ", MIME类型:" << strMimeType
                         << ", 文件路径:" << strFilePath;
    
    return result;
}

bool MCPResourceContentGenerator::isTextMimeType(const QString& strMimeType)
{
    // 文本类型MIME类型列表
    QString strLowerMimeType = strMimeType.toLower();
    
    // 以text/开头的都是文本类型
    if (strLowerMimeType.startsWith("text/"))
    {
        return true;
    }
    
    // 常见的文本类型
    static const QStringList commonTextMimeTypes = {
        "application/json",
        "application/xml",
        "application/javascript",
        "application/x-javascript",
        "application/ecmascript",
        "application/x-ecmascript",
        "application/typescript",
        "application/x-typescript",
        "application/x-sh",
        "application/x-shellscript",
        "application/x-python",
        "application/x-c",
        "application/x-cpp",
        "application/x-c++",
        "application/x-csharp",
        "application/x-java",
        "application/x-html",
        "application/x-css",
        "application/x-sql",
        "application/x-yaml",
        "application/x-toml",
        "application/x-markdown",
        "application/x-svg+xml",
        "application/x-json",
        "application/x-ld+json",
        "application/x-jsonld",
        "application/x-rtf",
        "application/x-rtfd",
        "application/x-tex",
        "application/x-latex",
        "application/x-postscript",
        "application/x-ps",
        "application/x-eps"
    };
    
    return commonTextMimeTypes.contains(strLowerMimeType);
}

QString MCPResourceContentGenerator::readFileAsText(const QString& strFilePath)
{
    QFile file(strFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceContentGenerator: 无法打开文本文件:" << strFilePath
                               << ", 错误:" << file.errorString();
        return QString();
    }
    
    // 使用UTF-8编码读取
    QTextStream textStream(&file);
    textStream.setCodec("UTF-8");
    QString strContent = textStream.readAll();
    file.close();
    
    return strContent;
}

QString MCPResourceContentGenerator::readFileAsBase64(const QString& strFilePath)
{
    QFile file(strFilePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceContentGenerator: 无法打开二进制文件:" << strFilePath
                               << ", 错误:" << file.errorString();
        return QString();
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    if (data.isEmpty())
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceContentGenerator: 二进制文件为空:" << strFilePath;
        return QString();
    }
    
    return base64Encode(data);
}

QString MCPResourceContentGenerator::base64Encode(const QByteArray& data)
{
    return QString::fromLatin1(data.toBase64());
}

QString MCPResourceContentGenerator::generateUriFromFilePath(const QString& strFilePath)
{
    QFileInfo fileInfo(strFilePath);
    QString strAbsolutePath = fileInfo.absoluteFilePath();
    
    // 将路径转换为file://协议的URI
    QUrl url = QUrl::fromLocalFile(strAbsolutePath);
    return url.toString();
}

