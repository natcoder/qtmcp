/**
 * @file MCPResource.h
 * @brief MCP资源基类（使用Qt Meta机制）
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

/**
 * @brief MCP资源基类
 * 
 * 职责：
 * - 定义资源的元数据（URI、名称、描述、MIME类型等）
 * - 使用Qt Meta机制（Q_PROPERTY）提供属性系统
 * - 提供资源变化的信号通知
 * - 定义资源内容的读取接口（纯虚函数）
 * 
 * 重要说明：
 * - URI是资源的唯一标识符，创建后不可修改
 * - 资源的身份由URI决定，元数据变化不影响资源身份
 * - 元数据（name、description、mimeType）可以修改，修改时会发出变化信号
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 字符串类型添加 str 前缀
 * - { 和 } 要单独一行
 */
class MCPResource : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uri READ getUri CONSTANT)  // URI不可变，使用CONSTANT
    Q_PROPERTY(QString name READ getName WRITE setName)
    Q_PROPERTY(QString description READ getDescription WRITE setDescription)
    Q_PROPERTY(QString mimeType READ getMimeType WRITE setMimeType)
    
public:
    explicit MCPResource(const QString& strUri, 
                         QObject* pParent = nullptr);
    virtual ~MCPResource();
signals:
    /**
     * @brief 资源变化信号
     * 当资源元数据或内容发生变化时发出此信号
     * @param strName 资源名称
     * @param strDescription 资源描述
     * @param strMimeType MIME类型
     * 
     * 注意：元数据变化（name、description、mimeType）或内容变化时都会发出此信号
     */
    void changed(const QString& strName, const QString& strDescription, const QString& strMimeType);
    
    /**
     * @brief 资源失效信号
     * 当资源失效（如文件被删除、资源不可用等）时发出此信号
     * 
     * 注意：资源失效后，资源对象可能仍然存在，但已无法正常使用
     * 订阅者应该取消订阅或处理失效情况
     */
    void invalidated();
public:
    // ============ 属性访问器（Q_PROPERTY） ============
    
    /**
     * @brief 获取资源URI
     * @return 资源URI
     * 
     * 注意：URI是资源的唯一标识符，创建后不可修改
     */
    QString getUri() const;
    
    /**
     * @brief 获取资源名称
     * @return 资源名称
     */
    QString getName() const;
    
    /**
     * @brief 设置资源名称
     * @param strName 资源名称
     * 
     * 注意：修改名称不会改变资源身份（URI不变），但会发出 changed 信号
     */
    void setName(const QString& strName);
    
    /**
     * @brief 获取资源描述
     * @return 资源描述
     */
    QString getDescription() const;
    
    /**
     * @brief 设置资源描述
     * @param strDescription 资源描述
     * 
     * 注意：修改描述不会改变资源身份（URI不变），但会发出 changed 信号
     */
    void setDescription(const QString& strDescription);
    
    /**
     * @brief 获取MIME类型
     * @return MIME类型
     */
    QString getMimeType() const;
    
    /**
     * @brief 设置MIME类型
     * @param strMimeType MIME类型
     * 
     * 注意：修改MIME类型不会改变资源身份（URI不变），但会发出 changed 信号
     */
    void setMimeType(const QString& strMimeType);
    
    /**
     * @brief 设置资源注解（Annotations）
     * @param annotations 注解对象
     * 
     * 根据 MCP 协议规范，annotations 包含：
     * - audience: 数组，有效值为 "user" 和 "assistant"
     * - priority: 0.0 到 1.0 的数字，表示重要性
     * - lastModified: ISO 8601 格式的时间戳
     */
    void setAnnotations(const QJsonObject& annotations);
    
    /**
     * @brief 获取目标受众（audience）
     * @return 受众数组，包含 "user" 和/或 "assistant"
     */
    QJsonArray getAudience() const;
    
    /**
     * @brief 设置目标受众（audience）
     * @param audience 受众数组，有效值为 "user" 和 "assistant"
     */
    void setAudience(const QJsonArray& audience);
    
    /**
     * @brief 获取优先级（priority）
     * @return 优先级值，范围 0.0 到 1.0
     */
    double getPriority() const;
    
    /**
     * @brief 设置优先级（priority）
     * @param priority 优先级值，范围 0.0 到 1.0（1.0 表示最重要，0.0 表示最不重要）
     */
    void setPriority(double priority);
    
    /**
     * @brief 获取最后修改时间（lastModified）
     * @return ISO 8601 格式的时间戳字符串
     */
    QString getLastModified() const;
    
    /**
     * @brief 设置最后修改时间（lastModified）
     * @param lastModified ISO 8601 格式的时间戳字符串（例如："2025-01-12T15:00:58Z"）
     */
    void setLastModified(const QString& lastModified);
    
    /**
     * @brief 更新最后修改时间为当前时间
     */
    void updateLastModified();
    
    /**
     * @brief 通知资源变化
     * 手动触发资源变化信号，用于内容变化时通知订阅者
     */
    void notifyChanged();
    
    /**
     * @brief 通知资源失效
     * 手动触发资源失效信号，用于资源不可用时通知订阅者
     * 
     * 使用场景：
     * - 资源文件被删除
     * - 资源提供者不可用
     * - 资源权限被撤销
     * - 其他导致资源无法使用的情况
     */
    void notifyInvalidated();
    
public slots:
    // ============ 资源元数据和内容接口（Qt Meta机制） ============
    
    /**
     * @brief 获取资源元数据（JSON格式）
     * @return 资源元数据对象
     * 
     * 作为slot，可以通过QMetaObject::invokeMethod调用
     * 子类可以重写此方法以提供自定义的元数据获取逻辑
     */
    virtual QJsonObject getMetadata() const;
    
    /**
     * @brief 获取资源注解（Annotations）
     * @return 资源注解对象，包含 audience、priority、lastModified 等字段
     * 
     * 根据 MCP 协议规范，annotations 是可选的，用于提供客户端如何使用或显示资源的提示
     * 作为slot，可以通过QMetaObject::invokeMethod调用
     */
    virtual QJsonObject getAnnotations() const;
    
    /**
     * @brief 获取资源内容
     * @return 资源内容字符串
     * 
     * 这是获取资源内容的公共接口，内部调用 readContent()
     * 对于文本资源，返回文本内容；对于二进制资源，返回Base64编码的内容
     * 
     * 作为slot，可以通过QMetaObject::invokeMethod调用
     */
    QString getContent() const;
    
    
public:
    // ============ 资源内容读取接口（纯虚函数，由子类实现） ============
    
    /**
     * @brief 读取资源内容（纯虚函数，由子类实现）
     * @return 资源内容字符串
     * 
     * 注意：子类必须实现此方法，提供具体的资源内容读取逻辑
     * 此方法应该是 const 的，因为读取内容不应该修改对象状态
     * 
     * 注意：此方法不是slot，因为它是纯虚函数，需要通过 getContent() slot 来调用
     */
    virtual QString readContent() const = 0;    
protected:
    QString m_strUri;
    QString m_strName;
    QString m_strDescription;
    QString m_strMimeType;
    
    // 资源注解（Annotations），根据 MCP 协议规范
    QJsonArray m_audience;        // 目标受众数组，有效值为 "user" 和 "assistant"
    double m_priority;             // 优先级，范围 0.0 到 1.0
    QString m_strLastModified;     // 最后修改时间，ISO 8601 格式
};
