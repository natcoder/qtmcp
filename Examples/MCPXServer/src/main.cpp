/**
 * @file main.cpp
 * @brief MCP X服务器主程序
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include <QCoreApplication>
#include "MyExampleHandler.h"
#include "MyResourceHandler.h"
#include "IMCPServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("MCPXServer");
    app.setApplicationVersion("1.0.0");

    // 创建Handler对象（用于处理工具调用）
    // Handler 必须创建，MCPHandlerResolver 会通过 objectName 或 "MPCServerHandlerName" 属性找到它
    // MyExampleHandler 构造函数中已设置 "MPCServerHandlerName" 属性
    MyExampleHandler* pHandler = new MyExampleHandler(qApp);
    // 同时设置 objectName 以确保能够被找到
    pHandler->setObjectName("MyExampleHandler");

    // 创建资源Handler对象（用于验证MCPResourceWrapper）
    // 资源Handler必须创建，MCPHandlerResolver会通过objectName或"MCPResourceHandlerName"属性找到它
    // MyResourceHandler构造函数中已设置"MCPResourceHandlerName"属性
    MyResourceHandler* pResourceHandler = new MyResourceHandler(qApp);
    // 同时设置objectName作为备用标识
    pResourceHandler->setObjectName("MyResourceHandler");

    // 使用自动启动方式，从配置文件加载并启动服务器
    // 配置文件位于应用程序目录下的 MCPServerConfig 文件夹
    StartAutoMCPServer();
    
    return app.exec();
}