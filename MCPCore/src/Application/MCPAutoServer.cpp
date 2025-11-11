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
#include <QJsonDocument>
#include "IMCPToolService.h"
#include "MCPConfig/MCPServerConfig.h"
#include "MCPInvokeHelper.h"
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

    // 创建服务器实例
	m_pServer = IMCPServer::createServer();
    auto pConfig = m_pServer->getConfig();
	QString strConfigDir = QCoreApplication::applicationDirPath() + "/MCPServerConfig";
    
    // 加载配置文件（加载完成后会自动通过信号槽机制应用配置）
    if (!pConfig->loadFromDirectory(strConfigDir))
    {
        MCP_CORE_LOG_WARNING() << "MCPAutoServer: 加载配置失败";
        return;
    }
    
    // 启动服务器（配置已在加载时自动应用）
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

void MCPAutoServer::loadTool(const QString& strToolConfigFile)
{
    if (m_pServer != nullptr)
    {
        QFile file(strToolConfigFile);
        if (!file.open(QIODevice::ReadOnly))
        {
            MCP_CORE_LOG_WARNING() << "MCPAutoServer: 无法打开工具配置文件:" << strToolConfigFile;
            return;
        }
        QByteArray jsonData = file.readAll();
        file.close();
        auto jsonTool = QJsonDocument::fromJson(jsonData).object();
        MCPInvokeHelper::asynInvoke(m_pServer, [this, jsonTool]()
            {
                m_pServer->getToolService()->addFromJson(jsonTool);
            });
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