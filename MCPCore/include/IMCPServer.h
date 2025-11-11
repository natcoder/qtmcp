/**
 * @file MCPServer.h
 * @brief MCP服务器基类
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QString>
#include "MCPCore_global.h.h"

class IMCPServerConfig;
class IMCPToolService;
class IMCPResourceService;
class IMCPPromptService;

/**
 * @brief MCP服务器基类
 * 
 * 职责：
 * - 定义MCP服务器的公开接口
 * - 提供统一的注册API
 * - 管理服务器配置
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - { 和 } 要单独一行
 */
class MCPCORE_EXPORT IMCPServer : public QObject
{
    Q_OBJECT
    
public:
    /**
     * @brief 创建服务器实例
     * @return 服务器实例指针，失败时返回nullptr
     */
    static IMCPServer* createServer();

    /**
     * @brief 销毁服务器实例（推荐使用此方法）
     * @param pServer 服务器实例指针
     * 
     * 注意：不要直接使用 delete 删除服务器实例，应使用此方法
     */
    static void destroyServer(IMCPServer* pServer);

public:
    /**
     * @brief 启动服务器
     * @param nPort 监听端口号，默认8888
     * @param pConfig 配置对象（可选），如果提供则应用配置中的工具
     * @return true表示启动成功，false表示启动失败
     * 
     * 使用示例：
     * @code
     * // 方式1：直接启动（默认端口）
     * pServer->start();
     * @endcode
     */
    virtual bool start() = 0;

    /**
     * @brief 停止服务器
     */
    virtual void stop() = 0;

    /**
     * @brief 是否正在运行
     * @return true表示正在运行，false表示未运行
     */
    virtual bool isRunning() = 0;

public:
    /**
     * @brief 获取配置对象
     * @return 配置对象指针（保证非空）
     * 
     * 使用示例：
     * @code
     * auto pConfig = pServer->getConfig();
     * pConfig->setServerName("My Server");
     * @endcode
     */
    virtual IMCPServerConfig* getConfig() = 0;
    
    /**
     * @brief 获取工具服务接口
     * @return 工具服务接口指针（保证非空）
     * 
     * 注意：Service 对象由 Server 管理，不要直接删除
     * 
     * 使用示例：
     * @code
     * auto pToolService = pServer->getToolService();
     * pToolService->add("myTool", "My Tool", "Tool description",
     *     inputSchema, outputSchema,
     *     []() -> QJsonObject {
     *         QJsonObject result;
     *         result["content"] = QJsonArray();
     *         result["structuredContent"] = QJsonObject();
     *         return result;
     *     });
     * @endcode
     */
    virtual IMCPToolService* getToolService() = 0;
    
    /**
     * @brief 获取资源服务接口
     * @return 资源服务接口指针（保证非空）
     * 
     * 注意：Service 对象由 Server 管理，不要直接删除
     * 
     * 使用示例：
     * @code
     * auto pResourceService = pServer->getResourceService();
     * pResourceService->add("file:///path/to/file.txt", "My File", "A text file", "/path/to/file.txt");
     * @endcode
     */
    virtual IMCPResourceService* getResourceService() = 0;
    
    /**
     * @brief 获取提示词服务接口
     * @return 提示词服务接口指针（保证非空）
     * 
     * 注意：Service 对象由 Server 管理，不要直接删除
     * 
     * 使用示例：
     * @code
     * auto pPromptService = pServer->getPromptService();
     * QList<QPair<QString, QPair<QString, bool>>> args;
     * args.append(qMakePair("name", qMakePair("用户名", true)));
     * pPromptService->add("greeting", "问候提示词", args, "Hello {{name}}, welcome!");
     * @endcode
     */
    virtual IMCPPromptService* getPromptService() = 0;

protected:
    /**
     * @brief 构造函数（protected，只能通过 createServer 创建）
     */
    explicit IMCPServer(QObject* pParent = nullptr);
    
    /**
     * @brief 析构函数（protected，只能通过 destroyServer 销毁）
     */
    virtual ~IMCPServer() override;
};


extern "C" MCPCORE_EXPORT void __stdcall  StartAutoMCPServer();
extern "C" MCPCORE_EXPORT void __stdcall  LoadAutoMCPServerTool(const char* szToolConfigFile);
extern "C" MCPCORE_EXPORT void __stdcall  StopAutoMCPServer();