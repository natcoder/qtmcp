/**
 * @file MCPXServer.cpp
 * @brief MCP X服务器实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPServer.h"
#include <QJsonDocument>
#include <QThread>
#include <QEventLoop>
#include <QMetaObject>
#include "MCPTransport/IMCPTransport.h"
#include "MCPTransport/MCPHttpTransport/MCPHttpTransportAdapter.h"
#include "MCPSession/MCPSessionService.h"
#include "MCPTools/MCPToolService.h"
#include "MCPTools/MCPTool.h"
#include "MCPResource/MCPResourceService.h"
#include "MCPPrompt/MCPPromptService.h"
#include "MCPResource/MCPResource.h"
#include "MCPResource/MCPResourceWrapper.h"
#include "MCPPrompt/MCPPrompt.h"
#include "Utils/MCPInvokeHelper.h"
#include "Utils/MCPHandlerResolver.h"
#include "MCPLog/MCPLog.h"
#include "MCPMessage/MCPServerMessage.h"
#include "MCPTransport/MCPHttpTransport/impl/MCPHttpReplyMessage.h"
#include "MCPConfig/MCPServerConfig.h"
#include "MCPRouting/MCPContext.h"
#include "MCPConfig/MCPToolsConfig.h"
#include "MCPConfig/MCPResourcesConfig.h"
#include "MCPConfig/MCPPromptsConfig.h"
#include "MCPServerHandler.h"
#include "IMCPToolService.h"
#include "IMCPResourceService.h"
#include "IMCPPromptService.h"

MCPServer::MCPServer(QObject* pParent)
    : IMCPServer(pParent)
    , m_pTransport(new MCPHttpTransportAdapter(this))
    , m_pResourceService(new MCPResourceService(this))
    , m_pSessionService(new MCPSessionService(this))
    , m_pToolService(new MCPToolService(this))
    , m_pPromptService(new MCPPromptService(this))
	, m_pConfig(new MCPServerConfig(this))
    , m_pHandler(nullptr)
    , m_pThread(new QThread(this))
{
	// 初始化模块日志系统
	MCPLog::instance()->initialize("mcpserver.log", LogLevel::Debug, true);
		
	// 创建业务处理器，负责消息处理和业务逻辑（内部会创建请求调度器）
	m_pHandler = new MCPServerHandler(this, this);
	
	// 连接传输层的消息接收信号到业务处理器
	QObject::connect(m_pTransport, &IMCPTransport::messageReceived,
        m_pHandler, &MCPServerHandler::onClientMessageReceived);
	
	// 连接资源服务的信号到业务处理器（MCPServerHandler内部会转发到对应的子Handler）
	QObject::connect(m_pResourceService, &MCPResourceService::resourceContentChanged,
		m_pHandler, &MCPServerHandler::onResourceContentChanged);
	QObject::connect(m_pResourceService, &MCPResourceService::resourceDeleted,
		m_pHandler, &MCPServerHandler::onResourceDeleted);
	QObject::connect(m_pResourceService, &MCPResourceService::resourcesListChanged,
		m_pHandler, &MCPServerHandler::onResourcesListChanged);
	
	// 连接工具服务的信号到业务处理器（MCPServerHandler内部会转发到对应的子Handler）
	QObject::connect(m_pToolService, &MCPToolService::toolsListChanged,
		m_pHandler, &MCPServerHandler::onToolsListChanged);
	
	// 连接提示词服务的信号到业务处理器（MCPServerHandler内部会转发到对应的子Handler）
	QObject::connect(m_pPromptService, &MCPPromptService::promptsListChanged,
		m_pHandler, &MCPServerHandler::onPromptsListChanged);

	m_pThread->setObjectName("MCPServer-WorkerThread");
	MCP_CORE_LOG_INFO() << "MCPServer: 服务器组件初始化完成";
}

MCPServer::~MCPServer()
{
    stop();
}

bool MCPServer::start()
{
	// 如果线程已经在运行，直接调用 doStart
	if (!m_pThread->isRunning())
	{
		// 启动工作线程
		moveToThread(m_pThread);
		m_pThread->start();
		// 在工作线程中启动服务器
		MCPInvokeHelper::asynInvoke(this, [this]() { doStart();});
	}
	return true;
}

void MCPServer::stop()
{
	if (m_pThread->isRunning())
	{
		MCPInvokeHelper::syncInvoke(this, [this](){doStop();});
		m_pThread->quit();
		m_pThread->wait();
		setParent(nullptr);
		m_pThread->deleteLater();
		m_pThread = nullptr;
	}
}

bool MCPServer::isRunning()
{
	return m_pTransport->isRunning();
}

IMCPServerConfig* MCPServer::getConfig()
{
    return m_pConfig;
}

MCPToolService* MCPServer::getToolService()
{
    return m_pToolService;
}

MCPResourceService* MCPServer::getResourceService()
{
    return m_pResourceService;
}

MCPPromptService* MCPServer::getPromptService()
{
    return m_pPromptService;
}

IMCPTransport* MCPServer::getTransport() const
{
    return m_pTransport;
}

MCPSessionService* MCPServer::getSessionService() const
{
    return m_pSessionService;
}

bool MCPServer::doStart()
{
	// 应用配置
	initServer();
	
	// 启动传输层
	auto nPort = m_pConfig->getPort();
	if (!m_pTransport->start(nPort))
	{
		MCP_CORE_LOG_WARNING() << "MCPServer: 传输层启动失败";
		return false;
	}
	
	MCP_CORE_LOG_INFO() << "MCPServer: 传输层启动成功，端口:" << nPort;
	return true;
}

bool MCPServer::doStop()
{
	MCP_CORE_LOG_INFO() << "MCPServer: 正在停止服务器...";
	m_pTransport->stop();
	MCP_CORE_LOG_INFO() << "MCPServer: 传输层已停止";
	return true;
}

bool MCPServer::initServer()
{
	int nToolsApplied = 0;
	int nResourcesApplied = 0;
	int nPromptsApplied = 0;
	// 应用工具配置
	auto dictHandlers = MCPHandlerResolver::resolveHandlers();
		for (const auto& toolConfig : m_pConfig->m_pToolsConfig->getTools())
	{
		auto pHandler = dictHandlers.value(toolConfig.strExecHandler, nullptr);
		if (pHandler != nullptr)
		{
			bool bSuccess = m_pToolService->add(toolConfig.strName, toolConfig.strTitle, toolConfig.strDescription,
				toolConfig.jsonInputSchema, toolConfig.jsonOutputSchema,
				pHandler, toolConfig.strExecMethod);
			
			// 如果配置中包含 annotations，设置到工具中
			if (bSuccess && !toolConfig.annotations.isEmpty())
			{
				MCPTool* pTool = m_pToolService->getTool(toolConfig.strName);
				if (pTool)
				{
					pTool->withAnnotations(toolConfig.annotations);
				}
			}
			
			if (bSuccess)
			{
				nToolsApplied++;
			}
		}
		else
		{
			MCP_CORE_LOG_WARNING() << "MCPServer: 工具配置的Handler未找到:" << toolConfig.strExecHandler
								   << ", 工具:" << toolConfig.strName;
		}
	}

	// 应用资源配置
	for (const auto& resourceConfig : m_pConfig->m_pResourcesConfig->getResources())
	{
		bool bSuccess = false;
		QString strType = resourceConfig.strType.isEmpty() ? "content" : resourceConfig.strType;
		
		if (strType == "file")
		{
			// 文件资源类型
			if (resourceConfig.strFilePath.isEmpty())
			{
				MCP_CORE_LOG_WARNING() << "MCPServer: 文件资源配置无效（缺少filePath）:" 
									   << resourceConfig.strUri;
				continue;
			}
			
			// 使用文件路径方式（支持自动MIME类型推断和二进制文件）
			bSuccess = m_pResourceService->add(
				resourceConfig.strUri,
				resourceConfig.strName,
				resourceConfig.strDescription,
				resourceConfig.strFilePath,
				resourceConfig.strMimeType.isEmpty() ? QString() : resourceConfig.strMimeType);
			
			// 如果配置中包含 annotations，设置到资源中
			if (bSuccess && !resourceConfig.annotations.isEmpty())
			{
				MCPResource* pResource = m_pResourceService->getResource(resourceConfig.strUri);
				if (pResource)
				{
					pResource->setAnnotations(resourceConfig.annotations);
				}
			}
		}
		else if (strType == "wrapper")
		{
			// 包装资源类型
			if (resourceConfig.strHandlerName.isEmpty())
			{
				MCP_CORE_LOG_WARNING() << "MCPServer: 包装资源配置无效（缺少handlerName）:" 
									   << resourceConfig.strUri;
				continue;
			}
			
			// 查找资源Handler对象
			QObject* pHandler = MCPHandlerResolver::findHandler(resourceConfig.strHandlerName);
			if (pHandler == nullptr)
			{
				MCP_CORE_LOG_WARNING() << "MCPServer: 资源Handler未找到:" << resourceConfig.strHandlerName
									   << ", 资源URI:" << resourceConfig.strUri;
				continue;
			}
			
			// 创建MCPResourceWrapper
			MCPResourceWrapper* pWrapper = MCPResourceWrapper::create(
				resourceConfig.strUri,
				pHandler,
				m_pResourceService);
			
			if (pWrapper == nullptr)
			{
				MCP_CORE_LOG_WARNING() << "MCPServer: 创建资源包装器失败:" << resourceConfig.strUri
									   << ", Handler:" << resourceConfig.strHandlerName;
				continue;
			}
			
			// 注册资源
			bSuccess = m_pResourceService->registerResource(resourceConfig.strUri, pWrapper);
			
			// 如果配置中包含 annotations，设置到资源中
			if (bSuccess && !resourceConfig.annotations.isEmpty())
			{
				pWrapper->setAnnotations(resourceConfig.annotations);
			}
		}
		else
		{
			// 内容资源类型（默认）
			if (resourceConfig.strContent.isEmpty())
			{
				MCP_CORE_LOG_WARNING() << "MCPServer: 内容资源配置无效（缺少content）:" 
									   << resourceConfig.strUri;
				continue;
			}
			
			// 使用静态内容方式
			QString strContent = resourceConfig.strContent;
			std::function<QString()> contentProvider = [strContent]() { return strContent; };
			
			bSuccess = m_pResourceService->add(
				resourceConfig.strUri,
				resourceConfig.strName,
				resourceConfig.strDescription,
				resourceConfig.strMimeType.isEmpty() ? "text/plain" : resourceConfig.strMimeType,
				contentProvider);
			
			// 如果配置中包含 annotations，设置到资源中
			if (bSuccess && !resourceConfig.annotations.isEmpty())
			{
				MCPResource* pResource = m_pResourceService->getResource(resourceConfig.strUri);
				if (pResource)
				{
					pResource->setAnnotations(resourceConfig.annotations);
				}
			}
		}
		
		if (bSuccess)
		{
			nResourcesApplied++;
		}
		else
		{
			MCP_CORE_LOG_WARNING() << "MCPServer: 资源配置注册失败:" << resourceConfig.strUri;
		}
	}

	// 应用提示词配置
	for (const auto& promptConfig : m_pConfig->m_pPromptsConfig->getPrompts())
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
		if (m_pPromptService->add(
			promptConfig.strName,
			promptConfig.strDescription,
			arguments,
			promptConfig.strTemplate))
		{
			nPromptsApplied++;
		}
		else
		{
			MCP_CORE_LOG_WARNING() << "MCPServer: 提示词配置注册失败:" << promptConfig.strName;
		}
	}

	MCP_CORE_LOG_INFO() << "MCPServer: 配置应用完成 - 工具:" << nToolsApplied
						<< ", 资源:" << nResourcesApplied
						<< ", 提示词:" << nPromptsApplied;

	return (nToolsApplied + nResourcesApplied + nPromptsApplied) > 0;
}

void MCPServer::onThreadReady()
{
	// 空实现，仅用于确保工作线程事件循环已启动
}
