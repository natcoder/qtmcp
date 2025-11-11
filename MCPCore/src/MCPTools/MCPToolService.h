/**
 * @file MCPToolService.h
 * @brief MCP工具服务类（内部实现）
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <functional>
#include "IMCPToolService.h"

class MCPTool;
class MCPError;
struct MCPToolConfig;

/**
 * @brief MCP 工具服务实现类
 * 
 * 职责：
 * - 工具注册和管理
 * - 工具调用执行
 * - 工具列表提供
 * - 工具Schema管理
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 指针类型添加 p 前缀
 * - { 和 } 要单独一行
 */
class MCPToolService : public IMCPToolService
{
    Q_OBJECT

public:
    explicit MCPToolService(QObject* pParent = nullptr);
    virtual ~MCPToolService();

public:
    // 实现 IMCPToolService 接口
    bool add(const QString& strName,
             const QString& strTitle,
             const QString& strDescription,
             const QJsonObject& jsonInputSchema,
             const QJsonObject& jsonOutputSchema,
             QObject* pHandler,
             const QString& strMethodName) override;
    
    bool add(const QString& strName,
             const QString& strTitle,
             const QString& strDescription,
             const QJsonObject& jsonInputSchema,
             const QJsonObject& jsonOutputSchema,
             std::function<QJsonObject()> execFun) override;
    
    bool remove(const QString& strName) override;
    QJsonArray list() const override;
    
    bool addFromJson(const QJsonObject& jsonTool, QObject* pSearchRoot = nullptr) override;
    
public:
    // 内部方法（供内部使用）
    bool registerTool(MCPTool* pTool, QObject* pExecHandler, const QString& strMethodName = QString());
    bool registerTool(MCPTool* pTool, std::function<QJsonObject()> execFun);
    QJsonObject callTool(const QString& strMethodName, const QJsonObject& jsonCallArguments);
    
signals:
    /**
     * @brief 工具列表变化信号
     * 当工具注册或注销时发出此信号
     */
    void toolsListChanged();
    
private:
	void onHandlerDestroyed(const QString& strToolName);

private:
    MCPTool* getTool(const QString& strToolName) const;
	bool registerTool(MCPTool* pTool); // 用于已经设置好处理器的工具
	
	/**
	 * @brief 内部方法：实际执行添加工具操作
	 * @return 成功返回工具对象指针，失败返回nullptr
	 */
	MCPTool* doAddImpl(const QString& strName,
	                   const QString& strTitle,
	                   const QString& strDescription,
	                   const QJsonObject& jsonInputSchema,
	                   const QJsonObject& jsonOutputSchema,
	                   QObject* pHandler,
	                   const QString& strMethodName);
	
	/**
	 * @brief 内部方法：实际执行添加工具操作（使用函数）
	 * @return 成功返回工具对象指针，失败返回nullptr
	 */
	MCPTool* doAddImpl(const QString& strName,
	                   const QString& strTitle,
	                   const QString& strDescription,
	                   const QJsonObject& jsonInputSchema,
	                   const QJsonObject& jsonOutputSchema,
	                   std::function<QJsonObject()> execFun);
	
	/**
	 * @brief 内部方法：实际执行删除工具操作
	 * @param strName 工具名称
	 * @param bEmitSignal 是否发送信号，默认为true
	 */
	bool doRemoveImpl(const QString& strName, bool bEmitSignal = true);
	
	/**
	 * @brief 内部方法：实际执行获取工具列表操作
	 */
	QJsonArray doListImpl() const;
	
	/**
	 * @brief 从配置对象添加工具（内部方法，供MCPServer使用）
	 * @param toolConfig 工具配置对象
	 * @param dictHandlers Handler名称到对象的映射表，如果为空则从qApp搜索
	 * @return true表示注册成功，false表示失败
	 * 
	 * @warning 死锁风险：在服务运行过程中调用此方法添加工具时，如果调用方与Handler对象在同一线程，
	 *          且Handler对象正在等待某些操作完成，可能导致死锁。建议在服务初始化阶段调用此方法，
	 *          避免在运行过程中动态添加工具。
	 */
	bool addFromConfig(const MCPToolConfig& toolConfig, const QMap<QString, QObject*>& dictHandlers = QMap<QString, QObject*>());
	
private:
    QMap<QString, MCPTool*> m_dictTools;
    
private:
	friend class MCPAutoServer;
	friend class MCPServer;
};

