/**
 * @file MCPPromptService.cpp
 * @brief MCP提示词服务实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPPromptService.h"
#include "MCPPrompt.h"
#include "MCPLog.h"
#include "Utils/MCPInvokeHelper.h"

MCPPromptService::MCPPromptService(QObject* pParent)
    : IMCPPromptService(pParent)
{
}

MCPPromptService::~MCPPromptService()
{
    // 清理所有提示词
    for (MCPPrompt* pPrompt : m_dictPrompts)
    {
        if (pPrompt)
        {
            pPrompt->deleteLater();
        }
    }
    m_dictPrompts.clear();
}

bool MCPPromptService::add(const QString& strName,
                           const QString& strDescription,
                           const QList<QPair<QString, QPair<QString, bool>>>& arguments,
                           std::function<QString(const QMap<QString, QString>&)> generator)
{
    return MCPInvokeHelper::syncInvokeReturn(this, [this, strName, strDescription, arguments, generator]()
    {
        return doAddImpl(strName, strDescription, arguments, generator);
    });
}

bool MCPPromptService::add(const QString& strName,
                           const QString& strDescription,
                           const QList<QPair<QString, QPair<QString, bool>>>& arguments,
                           const QString& strTemplate)
{
    return MCPInvokeHelper::syncInvokeReturn(this, [this, strName, strDescription, arguments, strTemplate]()
    {
        return doAddImpl(strName, strDescription, arguments, strTemplate);
    });
}

bool MCPPromptService::registerPrompt(MCPPrompt* pPrompt)
{
    QString strName = pPrompt->getName();
    
    if (m_dictPrompts.contains(strName))
    {
        MCP_CORE_LOG_WARNING() << "MCPPromptService: 提示词已存在:" << strName;
        return false;
    }
    
    m_dictPrompts[strName] = pPrompt;
    MCP_CORE_LOG_INFO() << "MCPPromptService: 提示词已注册:" << strName;
    
    emit promptChanged(strName);
    emit promptsListChanged();
    return true;
}

bool MCPPromptService::remove(const QString& strName)
{
    return MCPInvokeHelper::syncInvokeReturn(this, [this, strName]()
    {
        return doRemoveImpl(strName);
    });
}

bool MCPPromptService::has(const QString& strName) const
{
    return MCPInvokeHelper::syncInvokeReturn(const_cast<MCPPromptService*>(this), [this, strName]()
    {
        return doHasImpl(strName);
    });
}

QJsonArray MCPPromptService::list() const
{
    QJsonArray arrResult;
    MCPInvokeHelper::syncInvoke(const_cast<MCPPromptService*>(this), [this, &arrResult]()
    {
        arrResult = doListImpl();
    });
    return arrResult;
}

QJsonObject MCPPromptService::getPrompt(const QString& strName, const QMap<QString, QString>& arguments)
{
    QJsonObject objResult;
    MCPInvokeHelper::syncInvoke(this, [this, &objResult, strName, arguments]()
    {
        objResult = doGetPromptImpl(strName, arguments);
    });
    return objResult;
}

bool MCPPromptService::doAddImpl(const QString& strName,
                                const QString& strDescription,
                                const QList<QPair<QString, QPair<QString, bool>>>& arguments,
                                std::function<QString(const QMap<QString, QString>&)> generator)
{
    // 创建提示词对象（父对象设为this）
    MCPPrompt* pPrompt = new MCPPrompt(strName, this);
    pPrompt->withDescription(strDescription);
    
    // 添加参数
    for (const auto& arg : arguments)
    {
        const QString& strArgName = arg.first;
        const QString& strArgDesc = arg.second.first;
        bool bRequired = arg.second.second;
        pPrompt->withArgument(strArgName, strArgDesc, bRequired);
    }
    
    pPrompt->withGenerator(generator);
    
    // 注册到服务
    bool bSuccess = registerPrompt(pPrompt);
    
    // 如果注册失败，删除Prompt对象避免内存泄漏
    if (!bSuccess)
    {
        pPrompt->deleteLater();
    }
    
    return bSuccess;
}

bool MCPPromptService::doAddImpl(const QString& strName,
                                const QString& strDescription,
                                const QList<QPair<QString, QPair<QString, bool>>>& arguments,
                                const QString& strTemplate)
{
    // 创建提示词对象（父对象设为this）
    MCPPrompt* pPrompt = new MCPPrompt(strName, this);
    pPrompt->withDescription(strDescription);
    
    // 添加参数
    for (const auto& arg : arguments)
    {
        const QString& strArgName = arg.first;
        const QString& strArgDesc = arg.second.first;
        bool bRequired = arg.second.second;
        pPrompt->withArgument(strArgName, strArgDesc, bRequired);
    }
    
    // 设置模板（会自动使用默认的模板替换生成器）
    pPrompt->withTemplate(strTemplate);
    
    // 注册到服务
    bool bSuccess = registerPrompt(pPrompt);
    
    // 如果注册失败，删除Prompt对象避免内存泄漏
    if (!bSuccess)
    {
        pPrompt->deleteLater();
    }
    
    return bSuccess;
}

bool MCPPromptService::doRemoveImpl(const QString& strName)
{
    if (!m_dictPrompts.contains(strName))
    {
        MCP_CORE_LOG_WARNING() << "MCPPromptService: 提示词不存在:" << strName;
        return false;
    }
    
    MCPPrompt* pPrompt = m_dictPrompts.take(strName);
    if (pPrompt)
    {
        pPrompt->deleteLater();
    }
    
    MCP_CORE_LOG_INFO() << "MCPPromptService: 提示词已注销:" << strName;
    emit promptChanged(strName);
    emit promptsListChanged();
    return true;
}

bool MCPPromptService::doHasImpl(const QString& strName) const
{
    return m_dictPrompts.contains(strName);
}

QJsonArray MCPPromptService::doListImpl() const
{
    QJsonArray arrPrompts;
    
    for (auto it = m_dictPrompts.constBegin(); it != m_dictPrompts.constEnd(); ++it)
    {
        MCPPrompt* pPrompt = it.value();
        arrPrompts.append(pPrompt->getMetadata());
    }
    
    return arrPrompts;
}

QJsonObject MCPPromptService::doGetPromptImpl(const QString& strName, const QMap<QString, QString>& arguments)
{
    if (!m_dictPrompts.contains(strName))
    {
        MCP_CORE_LOG_WARNING() << "MCPPromptService: 尝试获取不存在的提示词:" << strName;
        return QJsonObject();
    }
    
    MCPPrompt* pPrompt = m_dictPrompts[strName];
    QJsonArray messages = pPrompt->generate(arguments);
    
    QJsonObject result;
    result["description"] = pPrompt->getDescription();
    result["messages"] = messages;
    
    return result;
}

