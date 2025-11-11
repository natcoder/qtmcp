#pragma once
#include <QObject>
#include <QJsonObject>
#include <QString>
#include <QSharedPointer>
#include <functional>
#include "MCPClientMessage.h"
#include "IMCPServer.h"
#include "IMCPToolService.h"
#include "IMCPResourceService.h"
#include "IMCPPromptService.h"
#include "MCPTools/MCPToolService.h"
#include "MCPResource/MCPResourceService.h"
#include "MCPPrompt/MCPPromptService.h"
class MCPAbstractServerTransport;
class MCPMessage;
class MCPSessionService;
class IMCPTransport;
class MCPThreadPool;
class MCPServerMessage;
class MCPServerConfig;
class MCPServerHandler;
class MCPToolsConfig;
class MCPResourcesConfig;
class MCPPromptsConfig;
/**
 * @brief MCP 服务器实现类
 * 
 * 职责：
 * - 服务器生命周期管理
 * - 组件初始化协调
 * - 扩展注册接口
 * - 信号槽连接管理
 * - 独立线程运行
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 指针类型添加 p 前缀
 * - { 和 } 要单独一行
 */
class MCPServer : public IMCPServer
{
    Q_OBJECT

public:
    explicit MCPServer(QObject* pParent = nullptr);

protected:
    /**
     * @brief 析构函数（protected，只能通过 destroyServer 销毁）
     */
    virtual ~MCPServer();

public:
    /**
     * @brief 启动服务器
     * @return 是否启动成功
     */
    bool start() override;

    /**
     * @brief 停止服务器
     */
    void stop();

    /**
     * @brief 是否正在运行
     * @return true表示正在运行，false表示未运行
     */
    bool isRunning() override;

public:
    /**
     * @brief 获取配置对象
     * @return 配置对象指针
     */
    IMCPServerConfig* getConfig() override;
    
    /**
     * @brief 获取工具服务（返回具体实现类指针，方便内部使用）
     * @return 工具服务实现类指针
     */
    MCPToolService* getToolService() override;
    
    /**
     * @brief 获取资源服务（返回具体实现类指针，方便内部使用）
     * @return 资源服务实现类指针
     */
    MCPResourceService* getResourceService() override;
    
    /**
     * @brief 获取提示词服务（返回具体实现类指针，方便内部使用）
     * @return 提示词服务实现类指针
     */
    MCPPromptService* getPromptService() override;
    
public:
	IMCPTransport* getTransport() const;
	MCPSessionService* getSessionService() const;
private slots:
	void onThreadReady();
	void onConfigLoaded(QSharedPointer<MCPToolsConfig> pToolsConfig,
	                   QSharedPointer<MCPResourcesConfig> pResourcesConfig,
	                   QSharedPointer<MCPPromptsConfig> pPromptsConfig);
private:
    bool doStart();
    bool doStop();
	bool initServer(QSharedPointer<MCPToolsConfig> pToolsConfig,
	               QSharedPointer<MCPResourcesConfig> pResourcesConfig,
	               QSharedPointer<MCPPromptsConfig> pPromptsConfig);
private:
    IMCPTransport* m_pTransport;
    MCPSessionService* m_pSessionService;
    MCPToolService* m_pToolService;
    MCPResourceService* m_pResourceService;
    MCPPromptService* m_pPromptService;
    MCPServerConfig* m_pConfig;
    MCPServerHandler* m_pHandler;  // 业务处理器，负责消息处理和业务逻辑
private:
    QThread* m_pThread;
};
