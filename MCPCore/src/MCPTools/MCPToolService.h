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
	 */
	bool doAddImpl(const QString& strName,
	               const QString& strTitle,
	               const QString& strDescription,
	               const QJsonObject& jsonInputSchema,
	               const QJsonObject& jsonOutputSchema,
	               QObject* pHandler,
	               const QString& strMethodName);
	
	/**
	 * @brief 内部方法：实际执行添加工具操作（使用函数）
	 */
	bool doAddImpl(const QString& strName,
	               const QString& strTitle,
	               const QString& strDescription,
	               const QJsonObject& jsonInputSchema,
	               const QJsonObject& jsonOutputSchema,
	               std::function<QJsonObject()> execFun);
	
	/**
	 * @brief 内部方法：实际执行删除工具操作
	 */
	bool doRemoveImpl(const QString& strName);
	
	/**
	 * @brief 内部方法：实际执行获取工具列表操作
	 */
	QJsonArray doListImpl() const;
	
private:
    QMap<QString, MCPTool*> m_dictTools;
    
private:
	friend class MCPAutoServer;
	friend class MCPServer;
};

