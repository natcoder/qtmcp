/**
 * @file MCPResourcesConfig.h
 * @brief MCP资源配置类
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QList>

/**
 * @brief 资源配置结构
 */
struct MCPResourceConfig
{
    QString strUri;           // 资源URI
    QString strName;          // 资源名称
    QString strDescription;   // 资源描述
    QString strMimeType;      // MIME类型
    QString strType;          // 资源类型："file"（文件资源）、"wrapper"（包装资源）、"content"（内容资源，默认）
    QString strContent;       // 静态内容（可选，用于content类型）
    QString strFilePath;      // 文件路径（可选，用于file类型）
    QString strHandlerName;  // Handler名称（可选，用于wrapper类型，通过MCPResourceHandlerName属性查找QObject）
    
    // 资源注解（Annotations），根据 MCP 协议规范，可选
    QJsonObject annotations;   // 包含 audience、priority、lastModified 等字段
    
    MCPResourceConfig() : strMimeType("text/plain"), strType("content") {}
    
    QJsonObject toJson() const;
    static MCPResourceConfig fromJson(const QJsonObject& json);
};

/**
 * @brief MCP资源配置类
 * 
 * 职责：
 * - 管理资源配置列表
 * - 从目录加载多个资源配置文件
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 字符串类型添加 str 前缀
 * - { 和 } 要单独一行
 */
class MCPResourcesConfig : public QObject
{
    Q_OBJECT

public:
    explicit MCPResourcesConfig(QObject* pParent = nullptr);
    virtual ~MCPResourcesConfig();

    void addResource(const MCPResourceConfig& resourceConfig);
    QList<MCPResourceConfig> getResources() const;
    int getResourceCount() const;
    void clear();
    
    void loadFromJson(const QJsonArray& jsonArray);
    QJsonArray toJson() const;
    
    /**
     * @brief 从目录加载所有资源配置文件
     * @param strDirPath 资源配置目录路径
     * @return 成功加载的资源数量
     */
    int loadFromDirectory(const QString& strDirPath);

private:
    QList<MCPResourceConfig> m_listResourceConfigs;
};

