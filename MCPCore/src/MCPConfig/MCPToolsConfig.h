/**
 * @file MCPToolsConfig.h
 * @brief MCP工具配置类
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
 * @brief 工具配置结构
 */
struct MCPToolConfig
{
    QString strName;
    QString strTitle;
    QString strDescription;
    QJsonObject jsonInputSchema;
    QJsonObject jsonOutputSchema;
    QString strExecHandler;
    QString strExecMethod;
    
    // 工具注解（Annotations），根据 MCP 协议规范，可选
    QJsonObject annotations;   // 包含 audience、priority、lastModified 等字段
    
    MCPToolConfig() {}
    
    QJsonObject toJson() const;
    static MCPToolConfig fromJson(const QJsonObject& json);
};

/**
 * @brief MCP工具配置类
 * 
 * 职责：
 * - 管理工具配置列表
 * - 加载/保存工具配置
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 字符串类型添加 str 前缀
 * - { 和 } 要单独一行
 */
class MCPToolsConfig : public QObject
{
    Q_OBJECT

public:
    explicit MCPToolsConfig(QObject* pParent = nullptr);
    virtual ~MCPToolsConfig();

    /**
     * @brief 添加工具配置
     * @param toolConfig 工具配置
     */
    void addTool(const MCPToolConfig& toolConfig);
    
    /**
     * @brief 获取所有工具配置
     * @return 工具配置列表
     */
    QList<MCPToolConfig> getTools() const;
    
    /**
     * @brief 获取工具数量
     * @return 工具数量
     */
    int getToolCount() const;
    
    /**
     * @brief 清空所有工具配置
     */
    void clear();
    
    /**
     * @brief 从JSON数组加载工具配置
     * @param jsonArray 工具配置JSON数组
     */
    void loadFromJson(const QJsonArray& jsonArray);
    
    /**
     * @brief 导出为JSON数组
     * @return 工具配置JSON数组
     */
    QJsonArray toJson() const;
    
    /**
     * @brief 从目录加载所有工具配置文件
     * @param strDirPath 工具配置目录路径
     * @return 成功加载的工具数量
     */
    int loadFromDirectory(const QString& strDirPath);

private:
    QList<MCPToolConfig> m_listToolConfigs;
};

