/**
 * @file IMCPToolService.h
 * @brief MCP工具服务接口
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include "MCPCore_global.h.h"
#include <QObject>
#include <QString>
#include <QJsonObject>
#include <functional>

/**
 * @brief MCP工具服务接口
 * 
 * 职责：
 * - 定义工具服务的公开接口
 * - 提供工具注册和管理功能
 * - 隐藏具体实现细节
 * 
 * 编码规范：
 * - 接口方法使用纯虚函数
 * - { 和 } 要单独一行
 */
class MCPCORE_EXPORT IMCPToolService : public QObject
{
    Q_OBJECT
    
public:
    explicit IMCPToolService(QObject* pParent = nullptr) : QObject(pParent) {}
    
protected:
    /**
     * @brief 析构函数（protected，Service 对象由 Server 管理，不要直接删除）
     */
    virtual ~IMCPToolService() {}
    
public:
    /**
     * @brief 注册工具
     * @param strName 工具名称
     * @param strTitle 工具标题
     * @param strDescription 工具描述
     * @param jsonInputSchema 输入Schema（JSON格式）
     * @param jsonOutputSchema 输出Schema（JSON格式）
     * @param pHandler 处理器对象
     * @param strMethodName 处理方法名
     * @return true表示注册成功，false表示失败
     */
    virtual bool add(const QString& strName,
                     const QString& strTitle,
                     const QString& strDescription,
                     const QJsonObject& jsonInputSchema,
                     const QJsonObject& jsonOutputSchema,
                     QObject* pHandler,
                     const QString& strMethodName) = 0;
    
    /**
     * @brief 注册工具（使用函数）
     * @param strName 工具名称
     * @param strTitle 工具标题
     * @param strDescription 工具描述
     * @param jsonInputSchema 输入Schema（JSON格式）
     * @param jsonOutputSchema 输出Schema（JSON格式）
     * @param execFun 执行函数
     * @return true表示注册成功，false表示失败
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
    virtual bool add(const QString& strName,
                     const QString& strTitle,
                     const QString& strDescription,
                     const QJsonObject& jsonInputSchema,
                     const QJsonObject& jsonOutputSchema,
                     std::function<QJsonObject()> execFun) = 0;
    
    /**
     * @brief 注销工具
     * @param strName 工具名称
     * @return true表示注销成功，false表示失败
     */
    virtual bool remove(const QString& strName) = 0;
    
    /**
     * @brief 获取工具列表
     * @return 工具列表（JSON数组格式）
     */
    virtual QJsonArray list() const = 0;
    
signals:
    /**
     * @brief 工具列表变化信号
     * 当工具注册或注销时发出此信号
     */
    void toolsListChanged();
};

