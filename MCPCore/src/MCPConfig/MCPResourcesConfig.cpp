/**
 * @file MCPResourcesConfig.cpp
 * @brief MCP资源配置类实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPResourcesConfig.h"
#include "IMCPServer.h"
#include "MCPLog/MCPLog.h"
#include <QJsonDocument>
#include <QFile>
#include <QDir>

// ============================================================================
// MCPResourceConfig 实现
// ============================================================================

QJsonObject MCPResourceConfig::toJson() const
{
    QJsonObject json;
    json["uri"] = strUri;
    json["name"] = strName;
    json["description"] = strDescription;
    json["mimeType"] = strMimeType;
    
    if (!strType.isEmpty() && strType != "content")
    {
        json["type"] = strType;
    }
    
    if (!strContent.isEmpty())
    {
        json["content"] = strContent;
    }
    
    if (!strFilePath.isEmpty())
    {
        json["filePath"] = strFilePath;
    }
    
    if (!strHandlerName.isEmpty())
    {
        json["handlerName"] = strHandlerName;
    }
    
    // 添加 annotations（如果存在）
    if (!annotations.isEmpty())
    {
        json["annotations"] = annotations;
    }
    
    return json;
}

MCPResourceConfig MCPResourceConfig::fromJson(const QJsonObject& json)
{
    MCPResourceConfig config;
    config.strUri = json["uri"].toString();
    config.strName = json["name"].toString();
    config.strDescription = json["description"].toString();
    config.strMimeType = json["mimeType"].toString("text/plain");
    config.strType = json["type"].toString("content");  // 默认为content类型
    config.strContent = json["content"].toString();
    config.strFilePath = json["filePath"].toString();
    config.strHandlerName = json["handlerName"].toString();
    
    // 解析 annotations（如果存在）
    if (json.contains("annotations") && json["annotations"].isObject())
    {
        config.annotations = json["annotations"].toObject();
    }
    
    return config;
}

// ============================================================================
// MCPResourcesConfig 实现
// ============================================================================

MCPResourcesConfig::MCPResourcesConfig(QObject* pParent)
    : QObject(pParent)
{
}

MCPResourcesConfig::~MCPResourcesConfig()
{
}

void MCPResourcesConfig::addResource(const MCPResourceConfig& resourceConfig)
{
    m_listResourceConfigs.append(resourceConfig);
}

QList<MCPResourceConfig> MCPResourcesConfig::getResources() const
{
    return m_listResourceConfigs;
}

int MCPResourcesConfig::getResourceCount() const
{
    return m_listResourceConfigs.size();
}

void MCPResourcesConfig::clear()
{
    m_listResourceConfigs.clear();
}

void MCPResourcesConfig::loadFromJson(const QJsonArray& jsonArray)
{
    for (const QJsonValue& value : jsonArray)
    {
        if (value.isObject())
        {
            m_listResourceConfigs.append(MCPResourceConfig::fromJson(value.toObject()));
        }
    }
}

QJsonArray MCPResourcesConfig::toJson() const
{
    QJsonArray jsonArray;
    
    for (const MCPResourceConfig& resourceConfig : m_listResourceConfigs)
    {
        jsonArray.append(resourceConfig.toJson());
    }
    
    return jsonArray;
}

int MCPResourcesConfig::loadFromDirectory(const QString& strDirPath)
{
    QDir dir(strDirPath);
    if (!dir.exists())
    {
        MCP_CORE_LOG_WARNING() << "MCPResourcesConfig: 目录不存在:" << strDirPath;
        return 0;
    }
    
    m_listResourceConfigs.clear();
    
    // 查找所有.json文件
    QStringList filters;
    filters << "*.json";
    QStringList configFiles = dir.entryList(filters, QDir::Files);
    
    for (const QString& fileName : configFiles)
    {
        QString fullPath = dir.absoluteFilePath(fileName);
        
        QFile file(fullPath);
        if (!file.open(QIODevice::ReadOnly))
        {
            MCP_CORE_LOG_WARNING() << "MCPResourcesConfig: 无法打开文件:" << fullPath;
            continue;
        }
        
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
        file.close();
        
        if (parseError.error != QJsonParseError::NoError)
        {
            MCP_CORE_LOG_WARNING() << "MCPResourcesConfig: JSON解析错误:" << fullPath << parseError.errorString();
            continue;
        }
        
        if (doc.isObject())
        {
            m_listResourceConfigs.append(MCPResourceConfig::fromJson(doc.object()));
        }
    }
    
    MCP_CORE_LOG_INFO() << "MCPResourcesConfig: 从目录加载了" << m_listResourceConfigs.size() << "个资源配置";
    return m_listResourceConfigs.size();
}

