/**
 * @file MCPServerConfig.h
 * @brief MCP服务器配置接口类（抽象基类）
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include "MCPCore_global.h.h"
#include <QObject>
#include <QString>
#include <QSharedPointer>

class MCPToolsConfig;
class MCPResourcesConfig;
class MCPPromptsConfig;

/**
 * @brief MCP服务器配置接口类（抽象基类）
 * 
 * 职责：
 * - 定义服务器配置的公共接口
 * - 不包含具体实现和数据
 * 
 * 使用示例：
 * @code
 * // 从目录加载
 * MCPXServerConfig config;
 * config.loadFromDirectory("MCPServerConfig");
 * pServer->start(config.getPort(), &config);
 * @endcode
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 数值类型添加 n 前缀
 * - { 和 } 要单独一行
 */
class MCPCORE_EXPORT IMCPServerConfig : public QObject
{
    Q_OBJECT

public:
    explicit IMCPServerConfig(QObject* pParent = nullptr);
    virtual ~IMCPServerConfig();

public:
    // ============ 配置加载/保存 ============
    
    /**
     * @brief 从配置目录加载所有配置
     * @param strConfigDir 配置根目录（包含ServerConfig.json和子目录）
     * @return true表示加载成功，false表示失败
     * 
     * 目录结构：
     * MCPServerConfig/
     *   ├── ServerConfig.json      # 主配置
     *   ├── Tools/                 # 工具目录
     *   │   ├── calculator.json
     *   │   └── ...
     *   ├── Resources/             # 资源目录
     *   │   └── ...
     *   └── Prompts/               # 提示词目录
     *       └── ...
     */
    virtual bool loadFromDirectory(const QString& strConfigDir) = 0;
    
public:
    // ============ 配置访问 ============
    
    virtual void setPort(quint16 nPort) = 0;
    virtual quint16 getPort() const = 0;
    
    virtual void setServerName(const QString& strName) = 0;
    virtual QString getServerName() const = 0;
    
    virtual void setServerTitle(const QString& strTitle) = 0;
    virtual QString getServerTitle() const = 0;
    
    virtual void setServerVersion(const QString& strVersion) = 0;
    virtual QString getServerVersion() const = 0;
    
    virtual void setInstructions(const QString& strInstructions) = 0;
    virtual QString getInstructions() const = 0;
    
signals:
    /**
     * @brief 配置加载完成信号
     * 当配置从目录或文件加载完成后发出此信号
     * @param pToolsConfig 工具配置对象（使用QSharedPointer管理生命周期）
     * @param pResourcesConfig 资源配置对象（使用QSharedPointer管理生命周期）
     * @param pPromptsConfig 提示词配置对象（使用QSharedPointer管理生命周期）
     * MCPServer 会监听此信号并自动应用配置
     */
    void configLoaded(QSharedPointer<MCPToolsConfig> pToolsConfig,
                      QSharedPointer<MCPResourcesConfig> pResourcesConfig,
                      QSharedPointer<MCPPromptsConfig> pPromptsConfig);
};
