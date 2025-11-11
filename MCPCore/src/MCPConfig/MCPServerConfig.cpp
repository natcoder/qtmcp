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
#include "MCPServer/MCPServer.h"
#include <QJsonDocument>
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
    QSharedPointer<MCPToolsConfig> pToolsConfig(new MCPToolsConfig());
    QString strToolsDir = configDir.absoluteFilePath("Tools");
    if (QDir(strToolsDir).exists())
    {
        pToolsConfig->loadFromDirectory(strToolsDir);
    }
    
    // 3. 加载资源配置目录
    QSharedPointer<MCPResourcesConfig> pResourcesConfig(new MCPResourcesConfig());
    QString strResourcesDir = configDir.absoluteFilePath("Resources");
    if (QDir(strResourcesDir).exists())
    {
        pResourcesConfig->loadFromDirectory(strResourcesDir);
    }
    
    // 4. 加载提示词配置目录
    QSharedPointer<MCPPromptsConfig> pPromptsConfig(new MCPPromptsConfig());
    QString strPromptsDir = configDir.absoluteFilePath("Prompts");
    if (QDir(strPromptsDir).exists())
    {
        pPromptsConfig->loadFromDirectory(strPromptsDir);
    }
    
    MCP_CORE_LOG_INFO() << "MCPXServerConfig: 配置目录加载完成 - 端口:" << m_nPort
                        << ", 服务器:" << m_strServerName
                        << ", 工具:" << pToolsConfig->getToolCount()
                        << ", 资源:" << pResourcesConfig->getResourceCount()
                        << ", 提示词:" << pPromptsConfig->getPromptCount();
    
    // 发送配置加载完成信号，传递配置对象
    emit configLoaded(pToolsConfig, pResourcesConfig, pPromptsConfig);
    
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

