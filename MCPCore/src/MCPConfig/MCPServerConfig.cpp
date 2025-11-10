/**
 * @file MCPXServerConfig.cpp
 * @brief MCP X服务器配置实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPServerConfig.h"
#include "MCPToolsConfig.h"
#include "MCPResourcesConfig.h"
#include "MCPPromptsConfig.h"
#include "IMCPServer.h"
#include "MCPLog/MCPLog.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDir>

// ============================================================================
// MCPServerConfig 实现
// ============================================================================

MCPServerConfig::MCPServerConfig(QObject* pParent)
    : IMCPServerConfig(pParent)
    , m_nPort(8888)
    , m_strServerName("C++ MCPServer")
    , m_strServerTitle("C++ MCP Server Implementation")
    , m_strServerVersion("1.0.0")
    , m_strInstructions("这是一个使用C++和Qt实现的MCP服务器，支持工具、资源和提示词功能")
    , m_pToolsConfig(new MCPToolsConfig(this))
    , m_pResourcesConfig(new MCPResourcesConfig(this))
    , m_pPromptsConfig(new MCPPromptsConfig(this))
{
}

MCPServerConfig::~MCPServerConfig()
{
}

bool MCPServerConfig::loadFromDirectory(const QString& strConfigDir)
{
    QDir configDir(strConfigDir);
    if (!configDir.exists())
    {
        MCP_CORE_LOG_WARNING() << "MCPXServerConfig: 配置目录不存在:" << strConfigDir;
        return false;
    }
    
    // 1. 加载主配置文件 ServerConfig.json
    QString strServerConfigPath = configDir.absoluteFilePath("ServerConfig.json");
    if (QFile::exists(strServerConfigPath))
    {
        if (!loadFromFile(strServerConfigPath))
        {
            MCP_CORE_LOG_WARNING() << "MCPXServerConfig: 主配置文件加载失败:" << strServerConfigPath;
            return false;
        }
    }
    else
    {
        MCP_CORE_LOG_WARNING() << "MCPXServerConfig: 主配置文件不存在:" << strServerConfigPath;
    }
    
    // 2. 加载工具配置目录
    QString strToolsDir = configDir.absoluteFilePath("Tools");
    if (QDir(strToolsDir).exists())
    {
        m_pToolsConfig->loadFromDirectory(strToolsDir);
    }
    
    // 3. 加载资源配置目录
    QString strResourcesDir = configDir.absoluteFilePath("Resources");
    if (QDir(strResourcesDir).exists())
    {
        m_pResourcesConfig->loadFromDirectory(strResourcesDir);
    }
    
    // 4. 加载提示词配置目录
    QString strPromptsDir = configDir.absoluteFilePath("Prompts");
    if (QDir(strPromptsDir).exists())
    {
        m_pPromptsConfig->loadFromDirectory(strPromptsDir);
    }
    
    MCP_CORE_LOG_INFO() << "MCPXServerConfig: 配置目录加载完成 - 端口:" << m_nPort
                        << ", 工具:" << m_pToolsConfig->getToolCount()
                        << ", 资源:" << m_pResourcesConfig->getResourceCount()
                        << ", 提示词:" << m_pPromptsConfig->getPromptCount();
    
    return true;
}

bool MCPServerConfig::loadFromFile(const QString& strFilePath)
{
    QFile configFile(strFilePath);
    if (!configFile.open(QIODevice::ReadOnly))
    {
        MCP_CORE_LOG_WARNING() << "MCPXServerConfig: 无法打开配置文件:" << strFilePath;
        return false;
    }

    QByteArray jsonData = configFile.readAll();
    configFile.close();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError)
    {
        MCP_CORE_LOG_WARNING() << "MCPXServerConfig: JSON解析错误:" << parseError.errorString();
        return false;
    }

    if (!jsonDoc.isObject())
    {
        MCP_CORE_LOG_WARNING() << "MCPXServerConfig: 配置文件根元素不是对象";
        return false;
    }

    return loadFromJson(jsonDoc.object());
}

bool MCPServerConfig::loadFromJson(const QJsonObject& jsonConfig)
{
    // 读取端口
    m_nPort = static_cast<quint16>(jsonConfig.value("port").toInt(8888));
    // 读取服务器信息
    if (jsonConfig.contains("serverInfo"))
    {
        QJsonObject serverInfo = jsonConfig["serverInfo"].toObject();
        m_strServerName = serverInfo["name"].toString("C++ MCPServer");
        m_strServerTitle = serverInfo["title"].toString("C++ MCP Server Implementation");
        m_strServerVersion = serverInfo["version"].toString("1.0.0");
    }
    
    // 读取使用说明
    if (jsonConfig.contains("instructions"))
    {
        m_strInstructions = jsonConfig["instructions"].toString();
    }

    MCP_CORE_LOG_INFO() << "MCPXServerConfig: 主配置加载成功 - 端口:" << m_nPort 
                        << ", 服务器:" << m_strServerName;
    return true;
}

bool MCPServerConfig::saveToDirectory(const QString& strConfigDir) const
{
    QDir configDir(strConfigDir);
    
    // 确保配置目录存在
    if (!configDir.exists())
    {
        if (!configDir.mkpath("."))
        {
            MCP_CORE_LOG_WARNING() << "MCPXServerConfig: 无法创建配置目录:" << strConfigDir;
            return false;
        }
    }
    
    // 1. 保存主配置文件 ServerConfig.json
    QString strServerConfigPath = configDir.absoluteFilePath("ServerConfig.json");
    if (!saveToFile(strServerConfigPath))
    {
        MCP_CORE_LOG_WARNING() << "MCPXServerConfig: 主配置文件保存失败:" << strServerConfigPath;
        return false;
    }
    
    // 2. 保存工具配置目录
    QString strToolsDir = configDir.absoluteFilePath("Tools");
    QDir toolsDir(strToolsDir);
    if (!toolsDir.exists())
    {
        if (!toolsDir.mkpath("."))
        {
            MCP_CORE_LOG_WARNING() << "MCPXServerConfig: 无法创建工具配置目录:" << strToolsDir;
            return false;
        }
    }
    
    // 保存每个工具配置为单独的文件
    QJsonArray toolsJson = m_pToolsConfig->toJson();
    for (const QJsonValue& toolValue : toolsJson)
    {
        if (toolValue.isObject())
        {
            QJsonObject toolObj = toolValue.toObject();
            QString strToolName = toolObj["name"].toString();
            if (strToolName.isEmpty())
            {
                continue;
            }
            
            QString strToolFilePath = toolsDir.absoluteFilePath(strToolName + ".json");
            QFile toolFile(strToolFilePath);
            if (!toolFile.open(QIODevice::WriteOnly))
            {
                MCP_CORE_LOG_WARNING() << "MCPXServerConfig: 无法创建工具配置文件:" << strToolFilePath;
                continue;
            }
            
            QJsonDocument toolDoc(toolObj);
            toolFile.write(toolDoc.toJson(QJsonDocument::Indented));
            toolFile.close();
        }
    }
    
    // 3. 保存资源配置目录
    QString strResourcesDir = configDir.absoluteFilePath("Resources");
    QDir resourcesDir(strResourcesDir);
    if (!resourcesDir.exists())
    {
        if (!resourcesDir.mkpath("."))
        {
            MCP_CORE_LOG_WARNING() << "MCPXServerConfig: 无法创建资源配置目录:" << strResourcesDir;
            return false;
        }
    }
    
    // 保存每个资源配置为单独的文件
    QJsonArray resourcesJson = m_pResourcesConfig->toJson();
    for (const QJsonValue& resourceValue : resourcesJson)
    {
        if (resourceValue.isObject())
        {
            QJsonObject resourceObj = resourceValue.toObject();
            QString strResourceName = resourceObj["name"].toString();
            if (strResourceName.isEmpty())
            {
                // 如果没有名称，尝试使用URI的一部分作为文件名
                QString strUri = resourceObj["uri"].toString();
                strResourceName = strUri.split("/").last();
                if (strResourceName.isEmpty())
                {
                    continue;
                }
            }
            
            QString strResourceFilePath = resourcesDir.absoluteFilePath(strResourceName + ".json");
            QFile resourceFile(strResourceFilePath);
            if (!resourceFile.open(QIODevice::WriteOnly))
            {
                MCP_CORE_LOG_WARNING() << "MCPXServerConfig: 无法创建资源配置文件:" << strResourceFilePath;
                continue;
            }
            
            QJsonDocument resourceDoc(resourceObj);
            resourceFile.write(resourceDoc.toJson(QJsonDocument::Indented));
            resourceFile.close();
        }
    }
    
    // 4. 保存提示词配置目录
    QString strPromptsDir = configDir.absoluteFilePath("Prompts");
    QDir promptsDir(strPromptsDir);
    if (!promptsDir.exists())
    {
        if (!promptsDir.mkpath("."))
        {
            MCP_CORE_LOG_WARNING() << "MCPXServerConfig: 无法创建提示词配置目录:" << strPromptsDir;
            return false;
        }
    }
    
    // 保存每个提示词配置为单独的文件
    QJsonArray promptsJson = m_pPromptsConfig->toJson();
    for (const QJsonValue& promptValue : promptsJson)
    {
        if (promptValue.isObject())
        {
            QJsonObject promptObj = promptValue.toObject();
            QString strPromptName = promptObj["name"].toString();
            if (strPromptName.isEmpty())
            {
                continue;
            }
            
            QString strPromptFilePath = promptsDir.absoluteFilePath(strPromptName + ".json");
            QFile promptFile(strPromptFilePath);
            if (!promptFile.open(QIODevice::WriteOnly))
            {
                MCP_CORE_LOG_WARNING() << "MCPXServerConfig: 无法创建提示词配置文件:" << strPromptFilePath;
                continue;
            }
            
            QJsonDocument promptDoc(promptObj);
            promptFile.write(promptDoc.toJson(QJsonDocument::Indented));
            promptFile.close();
        }
    }
    
    MCP_CORE_LOG_INFO() << "MCPXServerConfig: 配置目录保存完成 - 端口:" << m_nPort
                        << ", 工具:" << m_pToolsConfig->getToolCount()
                        << ", 资源:" << m_pResourcesConfig->getResourceCount()
                        << ", 提示词:" << m_pPromptsConfig->getPromptCount();
    
    return true;
}

bool MCPServerConfig::saveToFile(const QString& strFilePath) const
{
    QFile configFile(strFilePath);
    if (!configFile.open(QIODevice::WriteOnly))
    {
        MCP_CORE_LOG_WARNING() << "MCPXServerConfig: 无法创建配置文件:" << strFilePath;
        return false;
    }

    QJsonDocument jsonDoc(toJson());
    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Indented);
    
    qint64 nWritten = configFile.write(jsonData);
    configFile.close();

    if (nWritten != jsonData.size())
    {
        MCP_CORE_LOG_WARNING() << "MCPXServerConfig: 配置文件写入不完整";
        return false;
    }

    MCP_CORE_LOG_INFO() << "MCPXServerConfig: 配置已保存到:" << strFilePath;
    return true;
}

QJsonObject MCPServerConfig::toJson() const
{
    QJsonObject json;
    json["port"] = m_nPort;
    
    // 导出服务器信息
    QJsonObject serverInfo;
    serverInfo["name"] = m_strServerName;
    serverInfo["title"] = m_strServerTitle;
    serverInfo["version"] = m_strServerVersion;
    json["serverInfo"] = serverInfo;
    
    json["instructions"] = m_strInstructions;
    
    return json;
}

void MCPServerConfig::setPort(quint16 nPort)
{
    m_nPort = nPort;
}

quint16 MCPServerConfig::getPort() const
{
    return m_nPort;
}

void MCPServerConfig::setServerName(const QString& strName)
{
    m_strServerName = strName;
}

QString MCPServerConfig::getServerName() const
{
    return m_strServerName;
}

void MCPServerConfig::setServerTitle(const QString& strTitle)
{
    m_strServerTitle = strTitle;
}

QString MCPServerConfig::getServerTitle() const
{
    return m_strServerTitle;
}

void MCPServerConfig::setServerVersion(const QString& strVersion)
{
    m_strServerVersion = strVersion;
}

QString MCPServerConfig::getServerVersion() const
{
    return m_strServerVersion;
}

void MCPServerConfig::setInstructions(const QString& strInstructions)
{
    m_strInstructions = strInstructions;
}

QString MCPServerConfig::getInstructions() const
{
    return m_strInstructions;
}

