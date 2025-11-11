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
#include "MCPConfig/MCPPromptsConfig.h"

MCPPromptService::MCPPromptService(QObject* pParent)
    : IMCPPromptService(pParent)
{
}

MCPPromptService::~MCPPromptService()
{
}

bool MCPPromptService::add(const QString& strName,
                           const QString& strDescription,
                           const QList<QPair<QString, QPair<QString, bool>>>& arguments,
                           std::function<QString(const QMap<QString, QString>&)> generator)
{
    return MCPInvokeHelper::syncInvokeReturn(this, [this, strName, strDescription, arguments, generator]()
    {
        return doAddImpl(strName, strDescription, arguments, generator) != nullptr;
    });
}

bool MCPPromptService::add(const QString& strName,
                           const QString& strDescription,
                           const QList<QPair<QString, QPair<QString, bool>>>& arguments,
                           const QString& strTemplate)
{
    return MCPInvokeHelper::syncInvokeReturn(this, [this, strName, strDescription, arguments, strTemplate]()
    {
        return doAddImpl(strName, strDescription, arguments, strTemplate) != nullptr;
    });
}

bool MCPPromptService::registerPrompt(MCPPrompt* pPrompt)
{
    QString strName = pPrompt->getName();
    
    // 如果已存在，先删除旧的（覆盖），不发送信号，因为后面注册新对象时会发送
    if (m_dictPrompts.contains(strName))
    {
        MCP_CORE_LOG_INFO() << "MCPPromptService: 提示词已存在，覆盖旧提示词:" << strName;
        doRemoveImpl(strName, false);
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
    return MCPInvokeHelper::syncInvokeReturnT<QJsonArray>(const_cast<MCPPromptService*>(this), [this]()->QJsonArray
    {
        return doListImpl();
    });
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

bool MCPPromptService::addFromJson(const QJsonObject& jsonPrompt)
{
    return MCPInvokeHelper::syncInvokeReturn(this, [this, jsonPrompt]()
    {
        // 将JSON对象转换为MCPPromptConfig
        MCPPromptConfig promptConfig = MCPPromptConfig::fromJson(jsonPrompt);
        return addFromConfig(promptConfig);
    });
}

bool MCPPromptService::addFromConfig(const MCPPromptConfig& promptConfig)
{
    // 转换参数格式
    QList<QPair<QString, QPair<QString, bool>>> arguments;
    for (const MCPPromptArgumentConfig& arg : promptConfig.listArguments)
    {
        arguments.append(qMakePair(
            arg.strName,
            qMakePair(arg.strDescription, arg.bRequired)
        ));
    }
    
    // 使用模板方式注册（会自动使用默认的模板替换生成器）
    return doAddImpl(
        promptConfig.strName,
        promptConfig.strDescription,
        arguments,
        promptConfig.strTemplate) != nullptr;
}

MCPPrompt* MCPPromptService::createAndConfigurePrompt(const QString& strName,
                                                       const QString& strDescription,
                                                       const QList<QPair<QString, QPair<QString, bool>>>& arguments)
{
    // 创建提示词对象（父对象设为this）
    MCPPrompt* pPrompt = new MCPPrompt(strName, this);
    pPrompt->withDescription(strDescription);
    
    // 添加参数
    for (const auto& arg : arguments)
    {
        pPrompt->withArgument(arg.first, arg.second.first, arg.second.second);
    }
    
    return pPrompt;
}

MCPPrompt* MCPPromptService::doAddImpl(const QString& strName,
                                       const QString& strDescription,
                                       const QList<QPair<QString, QPair<QString, bool>>>& arguments,
                                       std::function<QString(const QMap<QString, QString>&)> generator)
{
    // 创建并配置提示词对象
    MCPPrompt* pPrompt = createAndConfigurePrompt(strName, strDescription, arguments);
    pPrompt->withGenerator(generator);
    
    // 注册到服务
    if (!registerPrompt(pPrompt))
    {
        // 如果注册失败，删除Prompt对象避免内存泄漏
        pPrompt->deleteLater();
        return nullptr;
    }
    
    return pPrompt;
}

MCPPrompt* MCPPromptService::doAddImpl(const QString& strName,
                                       const QString& strDescription,
                                       const QList<QPair<QString, QPair<QString, bool>>>& arguments,
                                       const QString& strTemplate)
{
    // 创建并配置提示词对象
    MCPPrompt* pPrompt = createAndConfigurePrompt(strName, strDescription, arguments);
    pPrompt->withTemplate(strTemplate);
    
    // 注册到服务
    if (!registerPrompt(pPrompt))
    {
        // 如果注册失败，删除Prompt对象避免内存泄漏
        pPrompt->deleteLater();
        return nullptr;
    }
    
    return pPrompt;
}

bool MCPPromptService::doRemoveImpl(const QString& strName, bool bEmitSignal)
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
    if (bEmitSignal)
    {
        emit promptChanged(strName);
        emit promptsListChanged();
    }
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

