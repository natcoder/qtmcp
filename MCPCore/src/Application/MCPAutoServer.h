#pragma once
#include "MCPCore_global.h.h"
#include <QObject>
#include <QThread>

class IMCPServer;
class IMCPServerConfig;
class MCPTool;

/**
 * @brief MCP自动服务器类 - 基于配置文件自动启动和管理MCP服务器
 *
 * 该类提供了一种零配置的方式来启动MCP服务器：
 * - 自动检测程序目录下的MCPServerConfig.json配置文件
 * - 解析配置文件中的服务器端口和工具定义
 * - 自动查找并绑定工具对应的处理器对象
 * - 在单独的线程中启动服务器，提供高并发处理能力
 * - 服务器启动后持续运行，无需手动停止
 *
 * 使用方式：
 * @code
 * // 在main函数中创建Handler对象
 * MyHandler* handler = new MyHandler(qApp);
 * handler->setObjectName("MyHandler");
 *
 * // 创建自动服务器（自动启动）
 * MCPAutoServer server;
 * @endcode
 */
class MCPAutoServer : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数 - 自动检测并启动MCP服务器
     *
     * 在构造函数中会自动执行以下操作：
     * 1. 检测MCPServerConfig.json配置文件是否存在
     * 2. 解析配置文件中的服务器配置和工具定义
     * 3. 查找并绑定工具对应的处理器对象
     * 4. 在单独的线程中启动服务器
     *
     * 如果任何步骤失败，会输出警告信息但不会抛出异常。
     *
     * @param pParent 父对象指针
     */
    explicit MCPAutoServer(QObject* pParent = nullptr);

    /**
     * @brief 析构函数 - 清理服务器资源
     *
     * 停止服务器线程并清理相关资源。
     */
    virtual ~MCPAutoServer();

public:
    /**
     * @brief 执行自动启动逻辑
     * @deprecated 请直接创建MCPAutoServer对象来自动启动
     */
    void performStart();

    /**
     * @brief 执行停止逻辑
     * @deprecated 服务器会持续运行直到程序结束
     */
    void performStop();

private:
    IMCPServer* m_pServer;           // 服务器实例
};
