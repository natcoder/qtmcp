/**
 * @file MCPAutoServer.cpp
 * @brief MCP自动服务器实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPAutoServer.h"
#include "IMCPServer.h"
#include "MCPLog/MCPLog.h"
#include "Utils/MCPHandlerResolver.h"
#include <QCoreApplication>
#include <QDir>
#include <QMap>
#include <windows.h>
#include "MCPConfig/MCPServerConfig.h"
MCPAutoServer::MCPAutoServer(QObject* pParent)
    : QObject(pParent)
    , m_pServer(nullptr)
{

}

MCPAutoServer::~MCPAutoServer()
{

}

void MCPAutoServer::performStart()
{
    MCP_CORE_LOG_INFO() << "MCPAutoServer: 开始自动配置...";

    // 创建配置管理器
    // 加载配置文件
	m_pServer = IMCPServer::createServer();
    auto pConfig = m_pServer->getConfig();
	QString strConfigDir = QCoreApplication::applicationDirPath() + "/MCPServerConfig";
    if (!pConfig->loadFromDirectory(strConfigDir))
    {
        MCP_CORE_LOG_WARNING() << "MCPAutoServer: 加载配置失败";
        return;
    }    
    // 通过start方法应用配置（start方法内部会调用initServer来应用配置）
    if (!m_pServer->start())
    {
        MCP_CORE_LOG_WARNING() << "MCPAutoServer: 服务器启动失败";
        IMCPServer::destroyServer(m_pServer);
        m_pServer = nullptr;
        return;
    }
    
    MCP_CORE_LOG_INFO() << "MCPAutoServer: 服务器已启动，端口:" << pConfig->getPort();
}

void MCPAutoServer::performStop()
{
    if (m_pServer != nullptr)
    {
        m_pServer->stop();
        IMCPServer::destroyServer(m_pServer);
        m_pServer = nullptr;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		// 增加模块引用计数
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
			GET_MODULE_HANDLE_EX_FLAG_PIN,
			(LPCTSTR)hModule, &hModule);
		break;
	}
	return TRUE;
}