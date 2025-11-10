/**
 * @file MCPToolsConfig.cpp
 * @brief MCP工具配置类实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPToolsConfig.h"
#include "IMCPServer.h"
#include "MCPLog/MCPLog.h"
#include <QJsonDocument>
#include <QFile>
#include <QDir>

// ============================================================================
// MCPToolConfig 实现
// ============================================================================

QJsonObject MCPToolConfig::toJson() const
{
    QJsonObject json;
    json["name"] = strName;
    json["title"] = strTitle;
    json["description"] = strDescription;
    json["inputSchema"] = jsonInputSchema;
    json["outputSchema"] = jsonOutputSchema;
    json["execHandler"] = strExecHandler;
    json["execMethod"] = strExecMethod;
    
    // 添加 annotations（如果存在）
    if (!annotations.isEmpty())
    {
        json["annotations"] = annotations;
    }
    
    return json;
}

MCPToolConfig MCPToolConfig::fromJson(const QJsonObject& json)
{
    MCPToolConfig config;
    config.strName = json["name"].toString();
    config.strTitle = json["title"].toString();
    config.strDescription = json["description"].toString();
    config.jsonInputSchema = json["inputSchema"].toObject();
    config.jsonOutputSchema = json["outputSchema"].toObject();
    config.strExecHandler = json["execHandler"].toString();
    config.strExecMethod = json["execMethod"].toString();
    
    // 解析 annotations（如果存在）
    if (json.contains("annotations") && json["annotations"].isObject())
    {
        config.annotations = json["annotations"].toObject();
    }
    
    return config;
}

// ============================================================================
// MCPToolsConfig 实现
// ============================================================================

MCPToolsConfig::MCPToolsConfig(QObject* pParent)
    : QObject(pParent)
{
}

MCPToolsConfig::~MCPToolsConfig()
{
}

void MCPToolsConfig::addTool(const MCPToolConfig& toolConfig)
{
    m_listToolConfigs.append(toolConfig);
}

QList<MCPToolConfig> MCPToolsConfig::getTools() const
{
    return m_listToolConfigs;
}

int MCPToolsConfig::getToolCount() const
{
    return m_listToolConfigs.size();
}

void MCPToolsConfig::clear()
{
    m_listToolConfigs.clear();
}

void MCPToolsConfig::loadFromJson(const QJsonArray& jsonArray)
{
    m_listToolConfigs.clear();
    
    for (const QJsonValue& toolValue : jsonArray)
    {
        if (toolValue.isObject())
        {
            m_listToolConfigs.append(MCPToolConfig::fromJson(toolValue.toObject()));
        }
    }
}

QJsonArray MCPToolsConfig::toJson() const
{
    QJsonArray jsonArray;
    
    for (const MCPToolConfig& toolConfig : m_listToolConfigs)
    {
        jsonArray.append(toolConfig.toJson());
    }
    
    return jsonArray;
}

int MCPToolsConfig::loadFromDirectory(const QString& strDirPath)
{
    QDir dir(strDirPath);
    if (!dir.exists())
    {
        MCP_CORE_LOG_WARNING() << "MCPToolsConfig: 目录不存在:" << strDirPath;
        return 0;
    }
    
    m_listToolConfigs.clear();
    
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
            MCP_CORE_LOG_WARNING() << "MCPToolsConfig: 无法打开文件:" << fullPath;
            continue;
        }
        
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
        file.close();
        
        if (parseError.error != QJsonParseError::NoError)
        {
            MCP_CORE_LOG_WARNING() << "MCPToolsConfig: JSON解析错误:" << fullPath << parseError.errorString();
            continue;
        }
        
        if (doc.isObject())
        {
            m_listToolConfigs.append(MCPToolConfig::fromJson(doc.object()));
        }
    }
    
    MCP_CORE_LOG_INFO() << "MCPToolsConfig: 从目录加载了" << m_listToolConfigs.size() << "个工具配置";
    return m_listToolConfigs.size();
}

