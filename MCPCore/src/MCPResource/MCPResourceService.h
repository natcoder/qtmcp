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
struct MCPResourceConfig;

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
    
    bool addFromJson(const QJsonObject& jsonResource, QObject* pSearchRoot = nullptr) override;
    
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
     * @return 成功返回资源对象指针，失败返回nullptr
     */
    MCPResource* doAddImpl(const QString& strUri,
                           const QString& strName,
                           const QString& strDescription,
                           const QString& strMimeType,
                           std::function<QString()> contentProvider);
    
    /**
     * @brief 内部方法：实际执行添加资源操作（从文件路径）
     * @return 成功返回资源对象指针，失败返回nullptr
     */
    MCPResource* doAddImpl(const QString& strUri,
                           const QString& strName,
                           const QString& strDescription,
                           const QString& strFilePath,
                           const QString& strMimeType);
    
    /**
     * @brief 内部方法：实际执行删除资源操作
     * @param strUri 资源URI
     * @param bEmitSignal 是否发送信号，默认为true
     */
    bool doRemoveImpl(const QString& strUri, bool bEmitSignal = true);
    
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
    
    /**
     * @brief 从配置添加文件资源
     * @param resourceConfig 资源配置对象
     * @return true表示注册成功，false表示失败
     */
    bool addFileResourceFromConfig(const MCPResourceConfig& resourceConfig);
    
    /**
     * @brief 从配置添加包装资源
     * @param resourceConfig 资源配置对象
     * @param dictHandlers Handler名称到对象的映射表，如果为空则从qApp搜索
     * @return true表示注册成功，false表示失败
     */
    bool addWrapperResourceFromConfig(const MCPResourceConfig& resourceConfig, const QMap<QString, QObject*>& dictHandlers = QMap<QString, QObject*>());
    
    /**
     * @brief 从配置添加内容资源
     * @param resourceConfig 资源配置对象
     * @return true表示注册成功，false表示失败
     */
    bool addContentResourceFromConfig(const MCPResourceConfig& resourceConfig);
    
    /**
     * @brief 如果配置中包含annotations，则应用到资源
     * @param pResource 资源对象指针
     * @param annotations 注解对象
     * @return true表示成功，false表示失败
     */
    bool applyAnnotationsIfNeeded(MCPResource* pResource, const QJsonObject& annotations);
    
    /**
     * @brief 从配置对象添加资源（内部方法，供MCPServer使用）
     * @param resourceConfig 资源配置对象
     * @param dictHandlers Handler名称到对象的映射表，如果为空则从qApp搜索
     * @return true表示注册成功，false表示失败
     * 
     * @warning 死锁风险：在服务运行过程中调用此方法添加资源时，如果添加的是wrapper类型资源，
     *          且调用方与包装对象（pWrappedObject）在同一线程，可能导致死锁。
     *          这是因为创建MCPResourceWrapper时会在构造函数中调用updatePropertiesFromWrappedObject()，
     *          进而调用包装对象的getMetadata()方法，如果此时包装对象正在等待某些操作完成，
     *          就会发生死锁。建议在服务初始化阶段调用此方法，避免在运行过程中动态添加wrapper类型资源。
     */
    bool addFromConfig(const MCPResourceConfig& resourceConfig, const QMap<QString, QObject*>& dictHandlers = QMap<QString, QObject*>());

private:
    QMap<QString, MCPResource*> m_dictResources;
    
    // 订阅管理（基于sessionId）
    QMap<QString, QSet<QString>> m_subscriptions;  // URI -> 会话ID集合
    QMap<QString, QSet<QString>> m_sessionSubscriptions;  // 会话ID -> URI集合
private:
    friend class MCPServer;
};


