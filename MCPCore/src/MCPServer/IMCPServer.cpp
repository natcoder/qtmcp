/**
 * @file MCPServer.cpp
 * @brief MCP服务器实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "IMCPServer.h"
#include "Application/MCPAutoServer.h"
#include "MCPServer/MCPServer.h"

IMCPServer::IMCPServer(QObject* pParent)
    : QObject(pParent)
{
}

IMCPServer* IMCPServer::createServer()
{
	auto pXServer = new MCPServer();
	return pXServer;
}

void IMCPServer::destroyServer(IMCPServer* pServer)
{
	if (pServer != nullptr)
	{
		// 先停止服务器
		pServer->stop();
		// 使用 deleteLater 安全删除（符合 Qt 对象生命周期管理）
		pServer->deleteLater();
	}
}

IMCPServer::~IMCPServer()
{
}

static MCPAutoServer autoServer;
void StartAutoMCPServer()
{
	autoServer.performStart();
}
void StopAutoMCPServer()
{
	autoServer.performStop();
}
