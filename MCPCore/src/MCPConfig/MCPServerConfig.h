/**
 * @file MCPXServerConfig.h
 * @brief MCP X服务器配置实现类
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include "IMCPServerConfig.h"
#include <QJsonObject>
#include <QMap>

class IMCPServer;

/**
 * @brief MCP X服务器配置实现类
 * 
 * 职责：
 * - 管理服务器配置（端口、服务器信息等）
 * - 加载配置文件
 * - 应用配置到Server
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 数值类型添加 n 前缀
 * - { 和 } 要单独一行
 */
class MCPServerConfig : public IMCPServerConfig
{
    Q_OBJECT
    friend class IMCPServer;  // 允许MCPServer访问private方法
    friend class MCPServer; // 允许MCPXServer访问private方法

public:
    explicit MCPServerConfig(QObject* pParent = nullptr);
    virtual ~MCPServerConfig();

public:
    // ============ 配置加载/保存 ============
    
    bool loadFromDirectory(const QString& strConfigDir) override;
    
public:
    // ============ 配置访问 ============
    
    void setPort(quint16 nPort) override;
    quint16 getPort() const override;
    
    void setServerName(const QString& strName) override;
    QString getServerName() const override;
    
    void setServerTitle(const QString& strTitle) override;
    QString getServerTitle() const override;
    
    void setServerVersion(const QString& strVersion) override;
    QString getServerVersion() const override;
    
    void setInstructions(const QString& strInstructions) override;
    QString getInstructions() const override;

private:
    // 内部使用的方法
    bool loadFromFile(const QString& strFilePath);
    bool saveToFile(const QString& strFilePath) const;
    bool loadFromJson(const QJsonObject& jsonConfig);
    QJsonObject toJson() const;

private:
    quint16 m_nPort;
    QString m_strServerName;
    QString m_strServerTitle;
    QString m_strServerVersion;
    QString m_strInstructions;
private:
    friend class MCPServer;
};

