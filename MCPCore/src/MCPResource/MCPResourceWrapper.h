/**
 * @file MCPResourceWrapper.h
 * @brief MCP资源包装器（将QObject包装为MCPResource）
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include "MCPResource.h"
#include <QMetaObject>
#include <QMetaMethod>

/**
 * @brief MCP资源包装器
 * 
 * 职责：
 * - 将任意QObject对象包装为MCPResource
 * - 通过Qt Meta机制动态调用包装对象的方法和信号
 * - 在构造时验证包装对象是否满足最小实现要求
 * - 提供友好的错误处理和验证反馈
 * 
 * 包装对象需要提供以下接口：
 * - QJsonObject getMetadata() const;  // slot方法，获取资源元数据
 * - QString getContent() const;        // slot方法，获取资源内容
 * - void changed(const QString&, const QString&, const QString&);  // 信号，通知资源变化
 * 
 * 使用场景：
 * - 当已有QObject对象实现了资源接口，需要适配为MCPResource时
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 指针类型添加 p 前缀
 * - { 和 } 要单独一行
 */
class MCPResourceWrapper : public MCPResource
{
    Q_OBJECT
    
public:
    virtual ~MCPResourceWrapper();
    
    /**
     * @brief 静态创建函数，创建一个MCPResourceWrapper对象
     * @param strUri 资源URI
     * @param pWrappedObject 要包装的QObject对象指针
     * @param pParent 父对象
     * @return 创建的MCPResourceWrapper对象指针，如果pWrappedObject为空或不满足最小要求则返回nullptr
     * 
     * 此函数会自动获取QObject的最小接口元数据（getMetadata、getContent方法，changed信号）
     * 并进行参数验证，验证通过后调用私有构造函数创建对象
     * 
     * 使用示例：
     * @code
     * QObject* pMyObject = new MyObject();
     * MCPResourceWrapper* pWrapper = MCPResourceWrapper::create("uri://my-resource", pMyObject);
     * if (pWrapper)
     * {
     *     // 使用包装器（create()已保证对象合法）
     * }
     * @endcode
     */
    static MCPResourceWrapper* create(const QString& strUri,
                                      QObject* pWrappedObject,
                                      QObject* pParent = nullptr);
    
public:
    /**
     * @brief 读取资源内容（实现基类纯虚函数）
     * @return 资源内容字符串
     * 
     * 通过Qt Meta机制调用包装对象的getContent()方法
     */
    QString readContent() const override;
    
    /**
     * @brief 获取资源元数据（重写基类方法）
     * @return 资源元数据对象
     * 
     * 通过Qt Meta机制调用包装对象的getMetadata()方法
     */
    QJsonObject getMetadata() const override;
    
    /**
     * @brief 获取资源注解（Annotations）（重写基类方法）
     * @return 资源注解对象，包含 audience、priority、lastModified 等字段
     * 
     * 如果包装对象实现了 getAnnotations() 方法，则调用它
     * 否则返回基类的实现（可能为空对象）
     */
    QJsonObject getAnnotations() const override;
    
    /**
     * @brief 获取包装对象指针
     * @return 包装对象指针
     */
    QObject* getWrappedObject() const;
    
private:
    /**
     * @brief 验证changed信号
     * @param changedSignal 已获取的changed信号QMetaMethod
     * @param strErrorMessage 输出参数，验证失败时的错误信息
     * @return 如果验证通过返回true，否则返回false
     */
    static bool validateChangedSignal(const QMetaMethod& changedSignal, QString& strErrorMessage);
    
    /**
     * @brief 验证getMetadata方法
     * @param getMetadataMethod 已获取的getMetadata方法QMetaMethod
     * @param strErrorMessage 输出参数，验证失败时的错误信息
     * @return 如果验证通过返回true，否则返回false
     */
    static bool validateGetMetadata(const QMetaMethod& getMetadataMethod, QString& strErrorMessage);
    
    /**
     * @brief 验证getContent方法
     * @param getContentMethod 已获取的getContent方法QMetaMethod
     * @param strErrorMessage 输出参数，验证失败时的错误信息
     * @return 如果验证通过返回true，否则返回false
     */
    static bool validateGetContent(const QMetaMethod& getContentMethod, QString& strErrorMessage);
    
    /**
     * @brief 私有构造函数
     * @param strUri 资源URI
     * @param pWrappedObject 要包装的QObject对象指针（必须不为空）
     * @param changedSignal changed信号的QMetaMethod（必须有效）
     * @param getMetadata getMetadata方法的QMetaMethod（必须有效）
     * @param getContent getContent方法的QMetaMethod（必须有效）
     * @param getAnnotations getAnnotations方法的QMetaMethod（可选，可能无效）
     * @param pParent 父对象
     * 
     * 注意：
     * - 此构造函数为私有，只能通过create()静态函数调用
     * - 构造函数不进行参数验证，假设所有参数都已通过create()验证
     * - 会从pWrappedObject的getMetadata()方法中获取元数据（name、description、mimeType、annotations）
     */
    explicit MCPResourceWrapper(const QString& strUri,
                                QObject* pWrappedObject,
                                const QMetaMethod& changedSignal,
                                const QMetaMethod& getMetadata,
                                const QMetaMethod& getContent,
                                const QMetaMethod& getAnnotations,
                                QObject* pParent = nullptr);
    
    /**
     * @brief 初始化包装连接
     * 连接包装对象的changed信号到onWrappedObjectChanged槽
     * 连接包装对象的destroyed信号到onWrappedObjectDestroyed槽
     */
    void initWrapperConnection();
    
    /**
     * @brief 从包装对象获取元数据并更新本地属性
     * 从包装对象的getMetadata()返回的JSON中提取name、description、mimeType
     * 如果包装对象实现了getAnnotations()方法，也会提取annotations并设置到基类成员变量中
     */
    void updatePropertiesFromWrappedObject();
    
private slots:
    /**
     * @brief 处理包装对象的changed信号
     * 当包装对象发出changed信号时，转发为MCPResource的changed信号
     */
    void onWrappedObjectChanged(const QString& strName, const QString& strDescription, const QString& strMimeType);
    
    /**
     * @brief 处理包装对象的destroyed信号
     * 当包装对象被删除时，发送资源失效信号
     */
    void onWrappedObjectDestroyed();
    
private:
    QObject* m_pWrappedObject;        // 包装的QObject对象指针
    QMetaMethod m_changedSignal;     // changed信号的QMetaMethod
    QMetaMethod m_getMetadata;       // getMetadata方法的QMetaMethod
    QMetaMethod m_getContent;        // getContent方法的QMetaMethod
    QMetaMethod m_getAnnotations;    // getAnnotations方法的QMetaMethod（可选，可能无效）
};
