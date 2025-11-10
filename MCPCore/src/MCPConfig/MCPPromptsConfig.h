/**
 * @file MCPPromptsConfig.h
 * @brief MCP提示词配置类
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
#include <QPair>

/**
 * @brief 提示词参数配置
 */
struct MCPPromptArgumentConfig
{
    QString strName;
    QString strDescription;
    bool bRequired;
    
    MCPPromptArgumentConfig() : bRequired(false) {}
};

/**
 * @brief 提示词配置结构
 */
struct MCPPromptConfig
{
    QString strName;
    QString strDescription;
    QList<MCPPromptArgumentConfig> listArguments;
    QString strTemplate;  // 提示词模板
    
    MCPPromptConfig() {}
    
    QJsonObject toJson() const;
    static MCPPromptConfig fromJson(const QJsonObject& json);
};

/**
 * @brief MCP提示词配置类
 * 
 * 职责：
 * - 管理提示词配置列表
 * - 从目录加载多个提示词配置文件
 */
class MCPPromptsConfig : public QObject
{
    Q_OBJECT

public:
    explicit MCPPromptsConfig(QObject* pParent = nullptr);
    virtual ~MCPPromptsConfig();

    void addPrompt(const MCPPromptConfig& promptConfig);
    QList<MCPPromptConfig> getPrompts() const;
    int getPromptCount() const;
    void clear();
    
    void loadFromJson(const QJsonArray& jsonArray);
    QJsonArray toJson() const;
    
    /**
     * @brief 从目录加载所有提示词配置文件
     * @param strDirPath 提示词配置目录路径
     * @return 成功加载的提示词数量
     */
    int loadFromDirectory(const QString& strDirPath);

private:
    QList<MCPPromptConfig> m_listPromptConfigs;
};

