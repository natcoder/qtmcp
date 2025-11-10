/**
 * @file MCPPrompt.h
 * @brief MCP提示词类（内部实现）
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <functional>

/**
 * @brief MCP提示词参数
 */
struct MCPPromptArgument
{
    QString strName;
    QString strDescription;
    bool bRequired;
    
    MCPPromptArgument(const QString& name, const QString& desc, bool required = false)
        : strName(name), strDescription(desc), bRequired(required) {}
};

/**
 * @brief MCP提示词类
 * 
 * 职责：
 * - 定义提示词的元数据（名称、描述、参数等）
 * - 提供提示词内容生成接口
 * - 支持参数化的提示词模板
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 字符串类型添加 str 前缀
 * - { 和 } 要单独一行
 */
class MCPPrompt : public QObject
{
    Q_OBJECT
    
public:
    explicit MCPPrompt(const QString& strName, QObject* pParent = nullptr);
    virtual ~MCPPrompt();
    
public:
    MCPPrompt* withDescription(const QString& strDescription);
    MCPPrompt* withArgument(const QString& strName, 
                            const QString& strDescription,
                            bool bRequired = false);
    MCPPrompt* withGenerator(std::function<QString(const QMap<QString, QString>&)> generator);
    
    /**
     * @brief 设置提示词模板
     * @param strTemplate 模板字符串，支持 {{变量名}} 格式的占位符
     * @return 返回this指针，支持链式调用
     * 
     * 如果设置了模板但没有设置生成器，将使用默认的模板替换生成器
     */
    MCPPrompt* withTemplate(const QString& strTemplate);
    
public:
    QString getName() const;
    QString getDescription() const;
    QJsonObject getMetadata() const;
    QJsonArray generate(const QMap<QString, QString>& arguments);
    
    /**
     * @brief 默认的模板替换生成器（静态方法）
     * @param strTemplate 模板字符串，支持 {{变量名}} 格式的占位符
     * @param arguments 参数映射
     * @return 替换后的字符串
     * 
     * 这是一个静态工具方法，用于替换模板中的 {{变量名}} 占位符
     */
    static QString defaultTemplateGenerator(const QString& strTemplate, const QMap<QString, QString>& arguments);
    
private:
    QString m_strName;
    QString m_strDescription;
    QList<MCPPromptArgument> m_listArguments;
    std::function<QString(const QMap<QString, QString>&)> m_generator;
    QString m_strTemplate;  // 提示词模板（如果设置了模板但没有设置生成器，将使用默认的模板替换）
};

