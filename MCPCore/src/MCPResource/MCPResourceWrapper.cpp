/**
 * @file MCPResourceWrapper.cpp
 * @brief MCP资源包装器实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPResourceWrapper.h"
#include "MCPLog/MCPLog.h"
#include "Utils/MCPMetaObjectHelper.h"
#include <QMetaObject>
#include <QMetaMethod>
#include <QVariant>
#include <QThread>

MCPResourceWrapper::MCPResourceWrapper(const QString& strUri,
                                       QObject* pWrappedObject,
                                       const QMetaMethod& changedSignal,
                                       const QMetaMethod& getMetadata,
                                       const QMetaMethod& getContent,
                                       const QMetaMethod& getAnnotations,
                                       QObject* pParent)
    : MCPResource(strUri, pParent)
    , m_pWrappedObject(pWrappedObject)
    , m_changedSignal(changedSignal)
    , m_getMetadata(getMetadata)
    , m_getContent(getContent)
    , m_getAnnotations(getAnnotations)  // 从create()方法传入，可能无效
{
    // 从包装对象的getMetadata()方法中获取元数据（name、description、mimeType、annotations）
    updatePropertiesFromWrappedObject();
    
    // 初始化包装连接
    initWrapperConnection();
}

MCPResourceWrapper::~MCPResourceWrapper()
{
}

MCPResourceWrapper* MCPResourceWrapper::create(const QString& strUri,
                                                QObject* pWrappedObject,
                                                QObject* pParent)
{
    if (pWrappedObject == nullptr)
    {
        MCP_CORE_LOG_CRITICAL() << "MCPResourceWrapper::create: 包装对象不能为空";
        return nullptr;
    }
    
    if (strUri.isEmpty())
    {
        MCP_CORE_LOG_CRITICAL() << "MCPResourceWrapper::create: URI不能为空";
        return nullptr;
    }
    
    QString strErrorMessage;
    
    // 获取changed信号并验证
    QMetaMethod changedSignal = MCPMetaObjectHelper::getSignal(pWrappedObject, "changed(QString,QString,QString)");
    if (!changedSignal.isValid())
    {
        MCP_CORE_LOG_CRITICAL() << "MCPResourceWrapper::create: 缺少changed(QString,QString,QString)信号";
        return nullptr;
    }
    if (!validateChangedSignal(changedSignal, strErrorMessage))
    {
        MCP_CORE_LOG_CRITICAL() << "MCPResourceWrapper::create: 包装对象不满足最小实现要求:" << strErrorMessage;
        return nullptr;
    }
    
    // 获取getMetadata()方法并验证
    QMetaMethod getMetadataMethod = MCPMetaObjectHelper::getMethod(pWrappedObject, "getMetadata()");
    if (!getMetadataMethod.isValid())
    {
        MCP_CORE_LOG_CRITICAL() << "MCPResourceWrapper::create: 缺少getMetadata()方法";
        return nullptr;
    }
    if (!validateGetMetadata(getMetadataMethod, strErrorMessage))
    {
        MCP_CORE_LOG_CRITICAL() << "MCPResourceWrapper::create: 包装对象不满足最小实现要求:" << strErrorMessage;
        return nullptr;
    }
    
    // 获取getContent()方法并验证
    QMetaMethod getContentMethod = MCPMetaObjectHelper::getMethod(pWrappedObject, "getContent()");
    if (!getContentMethod.isValid())
    {
        MCP_CORE_LOG_CRITICAL() << "MCPResourceWrapper::create: 缺少getContent()方法";
        return nullptr;
    }
    if (!validateGetContent(getContentMethod, strErrorMessage))
    {
        MCP_CORE_LOG_CRITICAL() << "MCPResourceWrapper::create: 包装对象不满足最小实现要求:" << strErrorMessage;
        return nullptr;
    }
    
    // 尝试获取getAnnotations()方法（可选）
    QMetaMethod getAnnotationsMethod;
    QMetaMethod getAnnotationsMethodCandidate = MCPMetaObjectHelper::getMethod(pWrappedObject, "getAnnotations()");
    if (getAnnotationsMethodCandidate.isValid() && 
        getAnnotationsMethodCandidate.returnType() == QMetaType::QJsonObject &&
        getAnnotationsMethodCandidate.parameterCount() == 0)
    {
        getAnnotationsMethod = getAnnotationsMethodCandidate;
        MCP_CORE_LOG_DEBUG() << "MCPResourceWrapper::create: 包装对象支持getAnnotations()方法";
    }
    
    // 所有验证通过，创建对象
    MCPResourceWrapper* pWrapper = new MCPResourceWrapper(strUri, pWrappedObject, 
                                                           changedSignal, 
                                                           getMetadataMethod, 
                                                           getContentMethod,
                                                           getAnnotationsMethod,
                                                           pParent);
    
    return pWrapper;
}

QString MCPResourceWrapper::readContent() const
{
    // 检查包装对象是否已被删除
    if (m_pWrappedObject == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceWrapper: 包装对象已被删除，无法获取内容";
        return QString();
    }
    
    // 通过保存的QMetaMethod调用包装对象的getContent()方法
    // 使用Qt::AutoConnection自动处理线程问题：
    // - 同线程：使用DirectConnection（直接调用）
    // - 跨线程：使用QueuedConnection（异步调用，但这里需要返回值，所以使用BlockingQueuedConnection）
    QString strContent;
    m_getContent.invoke(m_pWrappedObject,
        QThread::currentThread() == m_pWrappedObject->thread() 
        ? Qt::DirectConnection 
        : Qt::BlockingQueuedConnection,
        Q_RETURN_ARG(QString, strContent));
    return strContent;
}

QObject* MCPResourceWrapper::getWrappedObject() const
{
    return m_pWrappedObject;
}

QJsonObject MCPResourceWrapper::getMetadata() const
{
    // 检查包装对象是否已被删除
    if (m_pWrappedObject == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceWrapper: 包装对象已被删除，返回基类元数据";
        return MCPResource::getMetadata();
    }
    
    // 通过保存的QMetaMethod调用包装对象的getMetadata()方法
    // 使用Qt::AutoConnection自动处理线程问题：
    // - 同线程：使用DirectConnection（直接调用）
    // - 跨线程：使用BlockingQueuedConnection（同步调用以获取返回值）
    QJsonObject metadata;
    
    // 检查是否跨线程
    m_getMetadata.invoke(m_pWrappedObject,
        QThread::currentThread() == m_pWrappedObject->thread() 
        ? Qt::DirectConnection 
        : Qt::BlockingQueuedConnection,
        Q_RETURN_ARG(QJsonObject, metadata));
    return metadata;
}

QJsonObject MCPResourceWrapper::getAnnotations() const
{
    // 检查包装对象是否已被删除
    if (m_pWrappedObject == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceWrapper: 包装对象已被删除，返回基类annotations";
        return MCPResource::getAnnotations();
    }
    
    // 如果包装对象实现了getAnnotations()方法，则调用它
    if (m_getAnnotations.isValid())
    {
        QJsonObject annotations;
        m_getAnnotations.invoke(m_pWrappedObject,
            QThread::currentThread() == m_pWrappedObject->thread() 
            ? Qt::DirectConnection 
            : Qt::BlockingQueuedConnection,
            Q_RETURN_ARG(QJsonObject, annotations));
        return annotations;
    }
    
    // 如果包装对象没有实现getAnnotations()方法，返回基类的实现
    return MCPResource::getAnnotations();
}

bool MCPResourceWrapper::validateChangedSignal(const QMetaMethod& changedSignal, QString& strErrorMessage)
{
    if (!changedSignal.isValid())
    {
        strErrorMessage = "changed信号无效";
        return false;
    }
    
    if (changedSignal.parameterCount() != 3)
    {
        strErrorMessage = "changed信号必须有3个QString参数";
        return false;
    }
    
    // 验证参数类型都是QString
    if (changedSignal.parameterType(0) != QMetaType::QString ||
        changedSignal.parameterType(1) != QMetaType::QString ||
        changedSignal.parameterType(2) != QMetaType::QString)
    {
        strErrorMessage = "changed信号的3个参数都必须是QString类型";
        return false;
    }
    
    return true;
}

bool MCPResourceWrapper::validateGetMetadata(const QMetaMethod& getMetadataMethod, QString& strErrorMessage)
{
    if (!getMetadataMethod.isValid())
    {
        strErrorMessage = "getMetadata()方法无效";
        return false;
    }
    
    if (getMetadataMethod.returnType() != QMetaType::QJsonObject)
    {
        strErrorMessage = "getMetadata()方法必须返回QJsonObject类型";
        return false;
    }
    
    if (getMetadataMethod.parameterCount() != 0)
    {
        strErrorMessage = "getMetadata()方法必须无参数";
        return false;
    }
    
    return true;
}

bool MCPResourceWrapper::validateGetContent(const QMetaMethod& getContentMethod, QString& strErrorMessage)
{
    if (!getContentMethod.isValid())
    {
        strErrorMessage = "getContent()方法无效";
        return false;
    }
    
    if (getContentMethod.returnType() != QMetaType::QString)
    {
        strErrorMessage = "getContent()方法必须返回QString类型";
        return false;
    }
    
    if (getContentMethod.parameterCount() != 0)
    {
        strErrorMessage = "getContent()方法必须无参数";
        return false;
    }
    
    return true;
}

void MCPResourceWrapper::onWrappedObjectChanged(const QString& strName, const QString& strDescription, const QString& strMimeType)
{
    // 检查包装对象是否已被删除（虽然理论上不应该发生，但为了安全起见）
    if (m_pWrappedObject == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceWrapper: 包装对象已被删除，忽略changed信号";
        return;
    }
    
    // 更新本地属性（不触发信号，因为这是从包装对象同步数据）
    m_strName = strName;
    m_strDescription = strDescription;
    m_strMimeType = strMimeType;
    
    // 转发changed信号
    emit changed(strName, strDescription, strMimeType);
}

void MCPResourceWrapper::initWrapperConnection()
{
    // 使用QMetaObject::connect连接包装对象的changed信号到onWrappedObjectChanged槽
    // 注意：QObject::connect不能直接使用QMetaMethod，需要使用QMetaObject::connect
    const QMetaObject* pMetaObject = this->metaObject();
    int nSlotIndex = pMetaObject->indexOfSlot("onWrappedObjectChanged(QString,QString,QString)");
    
    if (nSlotIndex >= 0)
    {
        bool bConnected = QMetaObject::connect(m_pWrappedObject, m_changedSignal.methodIndex(),
                                               this, nSlotIndex,
                                               Qt::AutoConnection, nullptr);
        
        if (!bConnected)
        {
            MCP_CORE_LOG_WARNING() << "MCPResourceWrapper: 无法连接包装对象的changed()信号";
        }
        else
        {
            MCP_CORE_LOG_DEBUG() << "MCPResourceWrapper: 已连接包装对象的changed()信号";
        }
    }
    else
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceWrapper: 找不到onWrappedObjectChanged槽";
    }
    
    // 连接包装对象的destroyed信号到onWrappedObjectDestroyed槽
    // 当包装对象被删除时，自动发送资源失效信号
    bool bConnected = QObject::connect(m_pWrappedObject, &QObject::destroyed,
                                      this, &MCPResourceWrapper::onWrappedObjectDestroyed);
    
    if (!bConnected)
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceWrapper: 无法连接包装对象的destroyed()信号";
    }
    else
    {
        MCP_CORE_LOG_DEBUG() << "MCPResourceWrapper: 已连接包装对象的destroyed()信号";
    }
}

void MCPResourceWrapper::onWrappedObjectDestroyed()
{
    // 包装对象已被删除，清空指针并发送失效信号
    MCP_CORE_LOG_WARNING() << "MCPResourceWrapper: 包装对象已被删除，资源失效:" << getUri();
    m_pWrappedObject = nullptr;
    notifyInvalidated();
}

void MCPResourceWrapper::updatePropertiesFromWrappedObject()
{
    // 获取包装对象的元数据
    QJsonObject metadata = getMetadata(); // Calls the overridden getMetadata, which gets from wrapped object
    
    // 更新本地属性（不触发信号，因为这是从包装对象同步数据）
    if (metadata.contains("name"))
    {
        m_strName = metadata["name"].toString();
    }
    
    if (metadata.contains("description"))
    {
        m_strDescription = metadata["description"].toString();
    }
    
    if (metadata.contains("mimeType"))
    {
        m_strMimeType = metadata["mimeType"].toString();
    }
    
    // 提取 annotations（如果存在）
    // 优先从 metadata 中的 annotations 字段获取
    if (metadata.contains("annotations") && metadata["annotations"].isObject())
    {
        QJsonObject annotations = metadata["annotations"].toObject();
        setAnnotations(annotations);
    }
    // 如果 metadata 中没有 annotations，直接调用 getAnnotations() 方法
    // getAnnotations() 会自动处理：如果包装对象实现了就调用它，否则返回基类实现
    else
    {
        QJsonObject annotations = getAnnotations();
        if (!annotations.isEmpty())
        {
            setAnnotations(annotations);
        }
    }
}
