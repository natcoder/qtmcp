/**
 * @file MCPToolService.cpp
 * @brief MCP工具服务实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPToolService.h"
#include "MCPLog/MCPLog.h"
#include "MCPTool.h"
#include "Utils/MCPMethodHelper.h"
#include "MCPError/MCPError.h"
#include "Utils/MCPInvokeHelper.h"
#include "MCPConfig/MCPToolsConfig.h"
#include "Utils/MCPHandlerResolver.h"

MCPToolService::MCPToolService(QObject* pParent)
    : IMCPToolService(pParent)
{

}

MCPToolService::~MCPToolService()
{
}

bool MCPToolService::add(const QString& strName,
                          const QString& strTitle,
                          const QString& strDescription,
                          const QJsonObject& jsonInputSchema,
                          const QJsonObject& jsonOutputSchema,
                          QObject* pHandler,
                          const QString& strMethodName)
{
    return MCPInvokeHelper::syncInvokeReturn(this, [this, strName, strTitle, strDescription, jsonInputSchema, jsonOutputSchema, pHandler, strMethodName]()
    {
        return doAddImpl(strName, strTitle, strDescription, jsonInputSchema, jsonOutputSchema, pHandler, strMethodName) != nullptr;
    });
}

bool MCPToolService::add(const QString& strName,
                          const QString& strTitle,
                          const QString& strDescription,
                          const QJsonObject& jsonInputSchema,
                          const QJsonObject& jsonOutputSchema,
                          std::function<QJsonObject()> execFun)
{
    return MCPInvokeHelper::syncInvokeReturn(this, [this, strName, strTitle, strDescription, jsonInputSchema, jsonOutputSchema, execFun]()
    {
        return doAddImpl(strName, strTitle, strDescription, jsonInputSchema, jsonOutputSchema, execFun) != nullptr;
    });
}

bool MCPToolService::remove(const QString& strName)
{
    return MCPInvokeHelper::syncInvokeReturn(this, [this, strName]()
    {
        return doRemoveImpl(strName);
    });
}

QJsonArray MCPToolService::list() const
{
    return MCPInvokeHelper::syncInvokeReturnT<QJsonArray>(const_cast<MCPToolService*>(this), [this]()->QJsonArray
        {
            return doListImpl();
        });
}

bool MCPToolService::addFromJson(const QJsonObject& jsonTool, QObject* pSearchRoot)
{
    return MCPInvokeHelper::syncInvokeReturn(this, [this, jsonTool, pSearchRoot]()
    {
        // 将JSON对象转换为MCPToolConfig
        MCPToolConfig toolConfig = MCPToolConfig::fromJson(jsonTool);
        
        // 如果提供了pSearchRoot，先解析handlers；否则传递空map，让addFromConfig从qApp搜索
        QMap<QString, QObject*> dictHandlers;
        if (pSearchRoot)
        {
            dictHandlers = MCPHandlerResolver::resolveHandlers(pSearchRoot);
        }
        
        return addFromConfig(toolConfig, dictHandlers);
    });
}

bool MCPToolService::addFromConfig(const MCPToolConfig& toolConfig, const QMap<QString, QObject*>& dictHandlers)
{
    // 从dictHandlers映射表中查找Handler，如果dictHandlers为空则从qApp搜索
    QObject* pHandler = nullptr;
    if (!dictHandlers.isEmpty())
    {
        pHandler = dictHandlers.value(toolConfig.strExecHandler, nullptr);
    }
    else
    {
        pHandler = MCPHandlerResolver::findHandler(toolConfig.strExecHandler, nullptr);
    }
    
    if (pHandler == nullptr)
    {
        MCP_TOOLS_LOG_WARNING() << "MCPToolService: 工具配置的Handler未找到:" << toolConfig.strExecHandler
                               << ", 工具:" << toolConfig.strName;
        return false;
    }
    
    // 添加工具
    MCPTool* pTool = doAddImpl(toolConfig.strName, toolConfig.strTitle, toolConfig.strDescription,
                               toolConfig.jsonInputSchema, toolConfig.jsonOutputSchema,
                               pHandler, toolConfig.strExecMethod);
    
    // 如果配置中包含 annotations，设置到工具中
    if (pTool != nullptr && !toolConfig.annotations.isEmpty())
    {
        pTool->withAnnotations(toolConfig.annotations);
    }
    
    return pTool != nullptr;
}

MCPTool* MCPToolService::doAddImpl(const QString& strName,
                                   const QString& strTitle,
                                   const QString& strDescription,
                                   const QJsonObject& jsonInputSchema,
                                   const QJsonObject& jsonOutputSchema,
                                   QObject* pHandler,
                                   const QString& strMethodName)
{
    if (pHandler == nullptr)
    {
        MCP_TOOLS_LOG_WARNING() << "Handler对象为空，工具:" << strName;
        return nullptr;
    }
    
    // 创建工具对象（父对象设为this，便于生命周期管理）
    MCPTool* pTool = new MCPTool(strName, this);
    pTool->withTitle(strTitle)
         ->withDescription(strDescription)
         ->withInputSchema(jsonInputSchema)
         ->withOutputSchema(jsonOutputSchema);
    
    // 注册到服务（会自动连接handlerDestroyed信号）
    // 如果注册失败，删除Tool对象避免内存泄漏
    if (!registerTool(pTool, pHandler, strMethodName))
    {
        pTool->deleteLater();
        return nullptr;
    }
    
    return pTool;
}

MCPTool* MCPToolService::doAddImpl(const QString& strName,
                                   const QString& strTitle,
                                   const QString& strDescription,
                                   const QJsonObject& jsonInputSchema,
                                   const QJsonObject& jsonOutputSchema,
                                   std::function<QJsonObject()> execFun)
{
    if (execFun == nullptr)
    {
        MCP_TOOLS_LOG_WARNING() << "执行函数为空，工具:" << strName;
        return nullptr;
    }
    
    // 创建工具对象（父对象设为this，便于生命周期管理）
    MCPTool* pTool = new MCPTool(strName, this);
    pTool->withTitle(strTitle)
         ->withDescription(strDescription)
         ->withInputSchema(jsonInputSchema)
         ->withOutputSchema(jsonOutputSchema);
    
    // 注册到服务（使用函数方式）
    // 如果注册失败，删除Tool对象避免内存泄漏
    if (!registerTool(pTool, execFun))
    {
        pTool->deleteLater();
        return nullptr;
    }
    
    return pTool;
}

bool MCPToolService::doRemoveImpl(const QString& strName, bool bEmitSignal)
{
    if (!m_dictTools.contains(strName))
    {
        MCP_TOOLS_LOG_WARNING() << "未找到工具:" << strName;
        return false;
    }

    MCPTool* pTool = m_dictTools.take(strName);
    if (pTool)
    {
        pTool->deleteLater();
    }

    MCP_TOOLS_LOG_INFO() << "工具已注销:" << strName;
    if (bEmitSignal)
    {
        emit toolsListChanged();
    }
    return true;
}

QJsonArray MCPToolService::doListImpl() const
{
    QJsonArray toolsArray;

    for (auto pTool : m_dictTools.values())
    {        
        toolsArray.append(pTool->getSchema());
    }

    return toolsArray;
}

bool MCPToolService::registerTool(MCPTool* pTool, QObject* pExecHandler, const QString& strMethodName)
{
	// 如果已存在，先删除旧的（覆盖），不发送信号，因为后面注册新对象时会发送
	if (m_dictTools.contains(pTool->getName()))
	{
		MCP_TOOLS_LOG_INFO() << "工具已存在，覆盖旧工具:" << pTool->getName();
		doRemoveImpl(pTool->getName(), false);
	}
    
    // 设置执行Handler
    pTool->withExecHandler(pExecHandler, strMethodName);
    
    // 监听Tool的handlerDestroyed信号，自动注销工具
	QObject::connect(pTool, &MCPTool::handlerDestroyed, this, &MCPToolService::onHandlerDestroyed);    
	m_dictTools.insert(pTool->getName(), pTool);
	MCP_TOOLS_LOG_INFO() << "工具已注册:" << pTool->getName();
	emit toolsListChanged();
    return true;
}

bool MCPToolService::registerTool(MCPTool* pTool, std::function<QJsonObject()> execFun)
{
	// 如果已存在，先删除旧的（覆盖），不发送信号，因为后面注册新对象时会发送
	if (m_dictTools.contains(pTool->getName()))
	{
		MCP_TOOLS_LOG_INFO() << "工具已存在，覆盖旧工具:" << pTool->getName();
		doRemoveImpl(pTool->getName(), false);
	}
	//
	pTool->withExecFun(execFun);
	//
	m_dictTools.insert(pTool->getName(), pTool);
	MCP_TOOLS_LOG_INFO() << "工具已注册:" << pTool->getName();
	emit toolsListChanged();
	return true;
}

bool MCPToolService::registerTool(MCPTool* pTool)
{
	// 如果已存在，先删除旧的（覆盖），不发送信号，因为后面注册新对象时会发送
	if (m_dictTools.contains(pTool->getName()))
	{
		MCP_TOOLS_LOG_INFO() << "工具已存在，覆盖旧工具:" << pTool->getName();
		doRemoveImpl(pTool->getName(), false);
	}
	m_dictTools.insert(pTool->getName(), pTool);
	MCP_TOOLS_LOG_INFO() << "工具已注册:" << pTool->getName();
	emit toolsListChanged();
	return true;
}

void MCPToolService::onHandlerDestroyed(const QString& strToolName)
{
	MCP_TOOLS_LOG_INFO() << "工具Handler已销毁，自动注销工具:" << strToolName;
	remove(strToolName);
}

MCPTool* MCPToolService::getTool(const QString& strToolName) const
{
	return m_dictTools.value(strToolName, nullptr);
}

QJsonObject MCPToolService::callTool(const QString& strToolName, const QJsonObject& jsonCallArguments)
{
	auto pTool = getTool(strToolName);
	if (pTool == nullptr)
	{
		MCP_TOOLS_LOG_CRITICAL() << QString("未知工具: %1").arg(strToolName);
		// 根据 MCP 协议规范，工具不存在的错误消息应该是英文
		throw MCPError::toolNotFound(strToolName);
	}
	try
	{
		return pTool->execute(jsonCallArguments);
	}
	catch (const MCPError& e)
	{
		// 重新抛出MCPError异常
		throw e;
	}
	catch (const std::exception& e)
	{
		MCP_TOOLS_LOG_CRITICAL() << "MCPToolService: 工具执行异常 - " << strToolName << ":" << e.what();
		// 根据 MCP 协议规范，内部错误的错误消息应该是英文
		throw MCPError::internalError(QString("Tool execution failed: %1").arg(e.what()));
	}
	catch (...)
	{
		MCP_TOOLS_LOG_CRITICAL() << "MCPToolService: 工具执行时发生未知异常 - " << strToolName;
		// 根据 MCP 协议规范，内部错误的错误消息应该是英文
		throw MCPError::internalError("Tool execution failed: Unknown error");
	}
}
