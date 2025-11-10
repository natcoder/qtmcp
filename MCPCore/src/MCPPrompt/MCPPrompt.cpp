/**
 * @file MCPPrompt.cpp
 * @brief MCP提示词类实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPPrompt.h"

MCPPrompt::MCPPrompt(const QString& strName, QObject* pParent)
    : QObject(pParent)
    , m_strName(strName)
{
}

MCPPrompt::~MCPPrompt()
{
}

MCPPrompt* MCPPrompt::withDescription(const QString& strDescription)
{
    m_strDescription = strDescription;
    return this;
}

MCPPrompt* MCPPrompt::withArgument(const QString& strName, 
                                    const QString& strDescription,
                                    bool bRequired)
{
    m_listArguments.append(MCPPromptArgument(strName, strDescription, bRequired));
    return this;
}

MCPPrompt* MCPPrompt::withGenerator(std::function<QString(const QMap<QString, QString>&)> generator)
{
    m_generator = generator;
    return this;
}

MCPPrompt* MCPPrompt::withTemplate(const QString& strTemplate)
{
    m_strTemplate = strTemplate;
    return this;
}

QString MCPPrompt::getName() const
{
    return m_strName;
}

QString MCPPrompt::getDescription() const
{
    return m_strDescription;
}

QJsonObject MCPPrompt::getMetadata() const
{
    QJsonObject metadata;
    metadata["name"] = m_strName;
    
    if (!m_strDescription.isEmpty())
    {
        metadata["description"] = m_strDescription;
    }
    
    // 添加参数定义
    if (!m_listArguments.isEmpty())
    {
        QJsonArray arguments;
        for (const MCPPromptArgument& arg : m_listArguments)
        {
            QJsonObject argObj;
            argObj["name"] = arg.strName;
            argObj["description"] = arg.strDescription;
            argObj["required"] = arg.bRequired;
            arguments.append(argObj);
        }
        metadata["arguments"] = arguments;
    }
    
    return metadata;
}

QJsonArray MCPPrompt::generate(const QMap<QString, QString>& arguments)
{
    QJsonArray messages;
    
    QString strContent;
    
    if (m_generator)
    {
        // 使用自定义生成器
        strContent = m_generator(arguments);
    }
    else if (!m_strTemplate.isEmpty())
    {
        // 使用默认的模板替换生成器
        strContent = defaultTemplateGenerator(m_strTemplate, arguments);
    }
    
    if (!strContent.isEmpty())
    {
        // 生成标准的MCP消息格式
        QJsonObject message;
        message["role"] = "user";
        
        QJsonObject content;
        content["type"] = "text";
        content["text"] = strContent;
        
        message["content"] = content;
        messages.append(message);
    }
    
    return messages;
}

QString MCPPrompt::defaultTemplateGenerator(const QString& strTemplate, const QMap<QString, QString>& arguments)
{
    QString result = strTemplate;
    
    // 替换模板中的变量 {{变量名}}
    for (auto it = arguments.begin(); it != arguments.end(); ++it)
    {
        QString placeholder = QString("{{%1}}").arg(it.key());
        result.replace(placeholder, it.value());
    }
    
    return result;
}

