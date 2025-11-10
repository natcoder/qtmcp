/**
 * @file MCPResourceService.h
 * @brief MCP资源服务（内部实现）
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
#include "IMCPResourceService.h"

class MCPResource;

/**
 * @brief MCP 资源服务实现类
 * 
 * 职责：
 * - 资源注册和管理
 * - 资源读取操作
 * - 资源列表提供
 * - 资源订阅管理（只有资源支持订阅）
 */
class MCPResourceService : public IMCPResourceService
{
    Q_OBJECT

public:
    explicit MCPResourceService(QObject* pParent = nullptr);
    virtual ~MCPResourceService();

    // 实现 IMCPResourceService 接口
    bool add(const QString& strUri,
             const QString& strName,
             const QString& strDescription,
             const QString& strMimeType,
             std::function<QString()> contentProvider) override;
    
    bool add(const QString& strUri,
             const QString& strName,
             const QString& strDescription,
             const QString& strFilePath,
             const QString& strMimeType = QString()) override;
    
    bool remove(const QString& strUri) override;
    bool has(const QString& strUri) const override;
    QJsonArray list(const QString& strUriPrefix = QString()) const override;
    QJsonObject readResource(const QString& strUri) override;
    
public:
    // 内部方法（供内部使用）
    bool registerResource(const QString& strUri, MCPResource* pResource);
    
    /**
     * @brief 订阅资源变化
     * @param strUri 资源URI
     * @param strSessionId 会话ID
     * @return 是否订阅成功
     */
    bool subscribe(const QString& strUri, const QString& strSessionId);
    
    /**
     * @brief 取消订阅资源变化
     * @param strUri 资源URI
     * @param strSessionId 会话ID
     * @return 是否取消订阅成功
     */
    bool unsubscribe(const QString& strUri, const QString& strSessionId);
    
    /**
     * @brief 取消会话的所有订阅
     * @param strSessionId 会话ID
     */
    void unsubscribeAll(const QString& strSessionId);
    
    /**
     * @brief 获取订阅指定URI的所有会话ID
     * @param strUri 资源URI
     * @return 订阅该URI的会话ID集合
     */
    QSet<QString> getSubscribedSessionIds(const QString& strUri) const;
    
    /**
     * @brief 获取资源对象（内部方法，供内部使用）
     * @param strUri 资源URI
     * @return 资源对象指针，如果不存在返回nullptr
     */
    MCPResource* getResource(const QString& strUri) const;

signals:
    /**
     * @brief 资源内容变化信号（用于订阅机制）
     * 当以下情况发生时发出此信号：
     * - 资源注册时
     * - 资源元数据变化时（name、description、mimeType）
     * - 资源内容变化时（通过 notifyChanged()）
     * 
     * 由ServerHandler处理订阅通知，向订阅该URI的客户端发送通知
     */
    void resourceContentChanged(const QString& strUri);
    
    /**
     * @brief 资源删除信号（用于订阅机制）
     * 当以下情况发生时发出此信号：
     * - 资源注销时
     * - 资源失效时（通过 notifyInvalidated()）
     * 
     * 由ServerHandler处理订阅通知，向订阅该URI的客户端发送删除通知
     */
    void resourceDeleted(const QString& strUri);
    
    /**
     * @brief 资源列表变化信号（用于广播通知）
     * 当资源注册或注销时发出此信号，向所有客户端广播
     * 
     * 注意：元数据变化不会触发此信号，只有资源的添加和删除会触发
     */
    void resourcesListChanged();

private:
    /**
     * @brief 内部方法：实际执行添加资源操作
     */
    bool doAddImpl(const QString& strUri,
                   const QString& strName,
                   const QString& strDescription,
                   const QString& strMimeType,
                   std::function<QString()> contentProvider);
    
    /**
     * @brief 内部方法：实际执行添加资源操作（从文件路径）
     */
    bool doAddImpl(const QString& strUri,
                   const QString& strName,
                   const QString& strDescription,
                   const QString& strFilePath,
                   const QString& strMimeType);
    
    /**
     * @brief 内部方法：实际执行删除资源操作
     */
    bool doRemoveImpl(const QString& strUri);
    
    /**
     * @brief 内部方法：实际执行检查资源是否存在操作
     */
    bool doHasImpl(const QString& strUri) const;
    
    /**
     * @brief 内部方法：实际执行获取资源列表操作
     */
    QJsonArray doListImpl(const QString& strUriPrefix) const;
    
    /**
     * @brief 内部方法：实际执行读取资源内容操作
     */
    QJsonObject doReadResourceImpl(const QString& strUri);

private:
    QMap<QString, MCPResource*> m_dictResources;
    
    // 订阅管理（基于sessionId）
    QMap<QString, QSet<QString>> m_subscriptions;  // URI -> 会话ID集合
    QMap<QString, QSet<QString>> m_sessionSubscriptions;  // 会话ID -> URI集合
};


