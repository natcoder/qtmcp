/**
 * @file MCPTool.h
 * @brief MCP工具类（内部实现）
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QJsonObject>
#include <QString>
#include <QJsonArray>
#include <QDateTime>
#include <functional>

/**
 * @brief MCP工具类
 * 
 * 职责：
 * - 定义工具的元数据
 * - 执行工具调用
 * - 验证输入输出
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 字符串类型添加 str 前缀
 * - { 和 } 要单独一行
 */
class MCPTool : public QObject
{
	Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param strName 工具名称
     * @param pParent 父对象（通常为MCPToolService）
     */
    explicit MCPTool(const QString& strName, QObject* pParent = nullptr);
    ~MCPTool();
    
public:
    MCPTool* withTitle(const QString& strTitle);
    MCPTool* withDescription(const QString& strDescription);
    MCPTool* withInputSchema(const QJsonObject& jsonInputSchema);
    MCPTool* withOutputSchema(const QJsonObject& jsonOutputSchema);
    
    /**
     * @brief 设置工具注解（Annotations）
     * @param annotations 注解对象
     * 
     * 根据 MCP 协议规范，annotations 包含：
     * - audience: 数组，有效值为 "user" 和 "assistant"
     * - priority: 0.0 到 1.0 的数字，表示重要性
     * - lastModified: ISO 8601 格式的时间戳
     */
    MCPTool* withAnnotations(const QJsonObject& annotations);
    
    /**
     * @brief 获取工具注解（Annotations）
     * @return 注解对象
     */
    QJsonObject getAnnotations() const;
    
    /**
     * @brief 设置目标受众（audience）
     * @param audience 受众数组，有效值为 "user" 和 "assistant"
     */
    MCPTool* withAudience(const QJsonArray& audience);
    
    /**
     * @brief 设置优先级（priority）
     * @param priority 优先级值，范围 0.0 到 1.0（1.0 表示最重要，0.0 表示最不重要）
     */
    MCPTool* withPriority(double priority);
    
    /**
     * @brief 设置最后修改时间（lastModified）
     * @param lastModified ISO 8601 格式的时间戳字符串（例如："2025-01-12T15:00:58Z"）
     */
    MCPTool* withLastModified(const QString& lastModified);
    
    /**
     * @brief 更新最后修改时间为当前时间
     */
    MCPTool* updateLastModified();

public:
    QString getName() const;
    QJsonObject execute(const QJsonObject& jsonCallArguments);
    QJsonObject getSchema() const;
    QString toString() const;
    
signals:
    /**
     * @brief Handler被销毁信号
     * @param strToolName 工具名称
     * 
     * 当绑定的Handler对象被销毁时发出此信号，
     * MCPToolService会监听此信号并自动注销该Tool
     */
    void handlerDestroyed(const QString& strToolName);
private slots:
	void onHandlerDestroyed();
private:
	MCPTool* withExecHandler(QObject* pExecHandler, const QString& strMethodName = QString());
	MCPTool* withExecFun(std::function<QJsonObject()> execFun);
	
private:
	void initSchemaValidator();
	bool validateInput(const QJsonObject& inputObject);
	bool validateOutput(const QJsonObject& outputObject);
	
private:
	QString m_strName;
	QString m_strTitle;
	QString m_strDescription;
	QJsonObject m_jsonInputSchema;
	QJsonObject m_jsonOutputSchema;
	
	// 工具注解（Annotations），根据 MCP 协议规范，可选
	QJsonArray m_audience;        // 目标受众数组，有效值为 "user" 和 "assistant"
	double m_priority;             // 优先级，范围 0.0 到 1.0
	QString m_strLastModified;     // 最后修改时间，ISO 8601 格式
	
	QObject* m_pExecHandler;
	QString m_strExecMethodName;
	std::function<QJsonObject()> m_execFun;
	
private:
	friend class MCPToolService;
	friend class MCPAutoServer;
};

