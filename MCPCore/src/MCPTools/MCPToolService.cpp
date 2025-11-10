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
        return doAddImpl(strName, strTitle, strDescription, jsonInputSchema, jsonOutputSchema, pHandler, strMethodName);
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
        return doAddImpl(strName, strTitle, strDescription, jsonInputSchema, jsonOutputSchema, execFun);
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
    QJsonArray arrResult;
    MCPInvokeHelper::syncInvoke(const_cast<MCPToolService*>(this), [this, &arrResult]()
    {
        arrResult = doListImpl();
    });
    return arrResult;
}

bool MCPToolService::doAddImpl(const QString& strName,
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
        return false;
    }
    
    // 创建工具对象（父对象设为this，便于生命周期管理）
    MCPTool* pTool = new MCPTool(strName, this);
    pTool->withTitle(strTitle)
         ->withDescription(strDescription)
         ->withInputSchema(jsonInputSchema)
         ->withOutputSchema(jsonOutputSchema);
    
    // 注册到服务（会自动连接handlerDestroyed信号）
    bool bSuccess = registerTool(pTool, pHandler, strMethodName);
    
    // 如果注册失败，删除Tool对象避免内存泄漏
    if (!bSuccess)
    {
        pTool->deleteLater();
    }
    
    return bSuccess;
}

bool MCPToolService::doAddImpl(const QString& strName,
                               const QString& strTitle,
                               const QString& strDescription,
                               const QJsonObject& jsonInputSchema,
                               const QJsonObject& jsonOutputSchema,
                               std::function<QJsonObject()> execFun)
{
    if (!execFun)
    {
        MCP_TOOLS_LOG_WARNING() << "执行函数为空，工具:" << strName;
        return false;
    }
    
    // 创建工具对象（父对象设为this，便于生命周期管理）
    MCPTool* pTool = new MCPTool(strName, this);
    pTool->withTitle(strTitle)
         ->withDescription(strDescription)
         ->withInputSchema(jsonInputSchema)
         ->withOutputSchema(jsonOutputSchema);
    
    // 注册到服务（使用函数方式）
    bool bSuccess = registerTool(pTool, execFun);
    
    // 如果注册失败，删除Tool对象避免内存泄漏
    if (!bSuccess)
    {
        pTool->deleteLater();
    }
    
    return bSuccess;
}

bool MCPToolService::doRemoveImpl(const QString& strName)
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
    emit toolsListChanged();
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
	if (m_dictTools.contains(pTool->getName()))
	{
		MCP_TOOLS_LOG_WARNING() << "工具已注册:" << pTool->getName();
		return false;
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
	if (m_dictTools.contains(pTool->getName()))
	{
		MCP_TOOLS_LOG_WARNING() << "工具已注册:" << pTool->getName();
		return false;
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
	if (m_dictTools.contains(pTool->getName()))
	{
		MCP_TOOLS_LOG_WARNING() << "工具已注册:" << pTool->getName();
		return false;
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
