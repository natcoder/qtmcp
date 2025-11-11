/**
 * @file MCPPromptService.h
 * @brief MCP提示词服务（内部实现）
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include "IMCPPromptService.h"

class MCPPrompt;
struct MCPPromptConfig;

/**
 * @brief MCP 提示词服务实现类
 * 
 * 职责：
 * - 提示词注册和管理
 * - 提示词列表提供
 * - 提示词内容生成
 */
class MCPPromptService : public IMCPPromptService
{
    Q_OBJECT

public:
    explicit MCPPromptService(QObject* pParent = nullptr);
    virtual ~MCPPromptService();

    // 实现 IMCPPromptService 接口
    bool add(const QString& strName,
             const QString& strDescription,
             const QList<QPair<QString, QPair<QString, bool>>>& arguments,
             std::function<QString(const QMap<QString, QString>&)> generator) override;
    
    bool add(const QString& strName,
             const QString& strDescription,
             const QList<QPair<QString, QPair<QString, bool>>>& arguments,
             const QString& strTemplate) override;
    
    bool remove(const QString& strName) override;
    bool has(const QString& strName) const override;
    QJsonArray list() const override;
    QJsonObject getPrompt(const QString& strName, const QMap<QString, QString>& arguments) override;
    
    bool addFromJson(const QJsonObject& jsonPrompt) override;
    
public:
    // 内部方法（供内部使用）
    bool registerPrompt(MCPPrompt* pPrompt);

signals:
    void promptChanged(const QString& strName);
    /**
     * @brief 提示词列表变化信号
     * 当提示词注册或注销时发出此信号
     */
    void promptsListChanged();

private:
    /**
     * @brief 内部方法：实际执行添加提示词操作
     * @return 成功返回提示词对象指针，失败返回nullptr
     */
    MCPPrompt* doAddImpl(const QString& strName,
                         const QString& strDescription,
                         const QList<QPair<QString, QPair<QString, bool>>>& arguments,
                         std::function<QString(const QMap<QString, QString>&)> generator);
    
    /**
     * @brief 内部方法：实际执行添加提示词操作（使用模板）
     * @return 成功返回提示词对象指针，失败返回nullptr
     */
    MCPPrompt* doAddImpl(const QString& strName,
                         const QString& strDescription,
                         const QList<QPair<QString, QPair<QString, bool>>>& arguments,
                         const QString& strTemplate);
    
    /**
     * @brief 创建并配置提示词对象（公共逻辑）
     * @param strName 提示词名称
     * @param strDescription 提示词描述
     * @param arguments 参数列表
     * @return 创建并配置好的提示词对象指针
     */
    MCPPrompt* createAndConfigurePrompt(const QString& strName,
                                        const QString& strDescription,
                                        const QList<QPair<QString, QPair<QString, bool>>>& arguments);
    
    /**
     * @brief 内部方法：实际执行删除提示词操作
     * @param strName 提示词名称
     * @param bEmitSignal 是否发送信号，默认为true
     */
    bool doRemoveImpl(const QString& strName, bool bEmitSignal = true);
    
    /**
     * @brief 内部方法：实际执行检查提示词是否存在操作
     */
    bool doHasImpl(const QString& strName) const;
    
    /**
     * @brief 内部方法：实际执行获取提示词列表操作
     */
    QJsonArray doListImpl() const;
    
    /**
     * @brief 内部方法：实际执行获取提示词内容操作
     */
    QJsonObject doGetPromptImpl(const QString& strName, const QMap<QString, QString>& arguments);
    
    /**
     * @brief 从配置对象添加提示词（内部方法，供MCPServer使用）
     * @param promptConfig 提示词配置对象
     * @return true表示注册成功，false表示失败
     * 
     * @warning 死锁风险：在服务运行过程中调用此方法添加提示词时，如果调用方与相关对象在同一线程，
     *          且相关对象正在等待某些操作完成，可能导致死锁。建议在服务初始化阶段调用此方法，
     *          避免在运行过程中动态添加提示词。
     */
    bool addFromConfig(const MCPPromptConfig& promptConfig);

private:
    QMap<QString, MCPPrompt*> m_dictPrompts;
private:
	friend class MCPServer;
};

