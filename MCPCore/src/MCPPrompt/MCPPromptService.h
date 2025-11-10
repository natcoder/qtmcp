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
     */
    bool doAddImpl(const QString& strName,
                   const QString& strDescription,
                   const QList<QPair<QString, QPair<QString, bool>>>& arguments,
                   std::function<QString(const QMap<QString, QString>&)> generator);
    
    /**
     * @brief 内部方法：实际执行添加提示词操作（使用模板）
     */
    bool doAddImpl(const QString& strName,
                   const QString& strDescription,
                   const QList<QPair<QString, QPair<QString, bool>>>& arguments,
                   const QString& strTemplate);
    
    /**
     * @brief 内部方法：实际执行删除提示词操作
     */
    bool doRemoveImpl(const QString& strName);
    
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

private:
    QMap<QString, MCPPrompt*> m_dictPrompts;
};

