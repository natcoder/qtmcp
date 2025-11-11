/**
 * @file MCPResourceService.cpp
 * @brief MCP资源服务实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPResourceService.h"
#include "MCPResource.h"
#include "MCPContentResource.h"
#include "MCPFileResource.h"
#include "MCPResourceWrapper.h"
#include "MCPLog.h"
#include "Utils/MCPInvokeHelper.h"
#include "MCPConfig/MCPResourcesConfig.h"
#include "Utils/MCPHandlerResolver.h"
#include <QSet>
#include "Utils/MCPResourceContentGenerator.h"

MCPResourceService::MCPResourceService(QObject* pParent)
    : IMCPResourceService(pParent)
{

}

MCPResourceService::~MCPResourceService()
{
    // 清理所有资源
    for (MCPResource* pResource : m_dictResources)
    {
        if (pResource)
        {
            pResource->deleteLater();
        }
    }
    m_dictResources.clear();
    m_subscriptions.clear();
    m_sessionSubscriptions.clear();
}

bool MCPResourceService::add(const QString& strUri,
                              const QString& strName,
                              const QString& strDescription,
                              const QString& strMimeType,
                              std::function<QString()> contentProvider)
{
    return MCPInvokeHelper::syncInvokeReturn(this, [this, strUri, strName, strDescription, strMimeType, contentProvider]()
    {
        return doAddImpl(strUri, strName, strDescription, strMimeType, contentProvider) != nullptr;
    });
}

bool MCPResourceService::add(const QString& strUri,
                              const QString& strName,
                              const QString& strDescription,
                              const QString& strFilePath,
                              const QString& strMimeType)
{
    return MCPInvokeHelper::syncInvokeReturn(this, [this, strUri, strName, strDescription, strFilePath, strMimeType]()
    {
        return doAddImpl(strUri, strName, strDescription, strFilePath, strMimeType) != nullptr;
    });
}

bool MCPResourceService::registerResource(const QString& strUri, MCPResource* pResource)
{
    // 如果已存在，先删除旧的（覆盖），不发送信号，因为后面注册新对象时会发送
    if (m_dictResources.contains(strUri))
    {
        MCP_CORE_LOG_INFO() << "MCPResourceService: 资源已存在，覆盖旧资源:" << strUri;
        doRemoveImpl(strUri, false);
    }
    
    m_dictResources[strUri] = pResource;
    MCP_CORE_LOG_INFO() << "MCPResourceService: 资源已注册:" << strUri;
    
    // 连接资源的 changed 信号到 resourceContentChanged(QString) 信号
    // 这样当资源的元数据（name、description、mimeType）或内容变化时，会通知订阅者
    QObject::connect(pResource, &MCPResource::changed, this, [this, strUri](const QString&, const QString&, const QString&)
    {
        emit resourceContentChanged(strUri);
    });
    
    // 连接资源的 invalidated 信号到 resourceDeleted(QString) 信号
    // 这样当资源失效时，会通知订阅者资源已删除
    QObject::connect(pResource, &MCPResource::invalidated, this, [this, strUri]()
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceService: 资源失效:" << strUri;
        emit resourceDeleted(strUri);  // 通知订阅者资源已删除
    });
    
    emit resourceContentChanged(strUri);  // 通知订阅者（订阅机制）- 资源注册
    emit resourcesListChanged();    // 通知所有客户端（广播通知）
    return true;
}

bool MCPResourceService::remove(const QString& strUri)
{
    return MCPInvokeHelper::syncInvokeReturn(this, [this, strUri]()
    {
        return doRemoveImpl(strUri);
    });
}

bool MCPResourceService::has(const QString& strUri) const
{
    return MCPInvokeHelper::syncInvokeReturn(const_cast<MCPResourceService*>(this), [this, strUri]()
    {
        return doHasImpl(strUri);
    });
}

QJsonArray MCPResourceService::list(const QString& strUriPrefix) const
{
    QJsonArray arrResult;
    MCPInvokeHelper::syncInvoke(const_cast<MCPResourceService*>(this), [this, &arrResult, strUriPrefix]()
    {
        arrResult = doListImpl(strUriPrefix);
    });
    return arrResult;
}

QJsonObject MCPResourceService::readResource(const QString& strUri)
{
    QJsonObject objResult;
    MCPInvokeHelper::syncInvoke(this, [this, &objResult, strUri]()
    {
        objResult = doReadResourceImpl(strUri);
    });
    return objResult;
}

bool MCPResourceService::addFromJson(const QJsonObject& jsonResource, QObject* pSearchRoot)
{
    return MCPInvokeHelper::syncInvokeReturn(this, [this, jsonResource, pSearchRoot]()
    {
        // 将JSON对象转换为MCPResourceConfig
        MCPResourceConfig resourceConfig = MCPResourceConfig::fromJson(jsonResource);
        
        // 如果提供了pSearchRoot，先解析handlers；否则传递空map，让addFromConfig从qApp搜索
        QMap<QString, QObject*> dictHandlers;
        if (pSearchRoot)
        {
            dictHandlers = MCPHandlerResolver::resolveHandlers(pSearchRoot);
        }
        
        return addFromConfig(resourceConfig, dictHandlers);
    });
}

bool MCPResourceService::addFromConfig(const MCPResourceConfig& resourceConfig, const QMap<QString, QObject*>& dictHandlers)
{
    // 根据资源类型调用对应的处理方法
    if (resourceConfig.strType == "file")
    {
        return addFileResourceFromConfig(resourceConfig);
    }
    else if (resourceConfig.strType == "wrapper")
    {
        return addWrapperResourceFromConfig(resourceConfig, dictHandlers);
    }        
    return addContentResourceFromConfig(resourceConfig);
}

bool MCPResourceService::addFileResourceFromConfig(const MCPResourceConfig& resourceConfig)
{
    // 验证文件路径
    if (resourceConfig.strFilePath.isEmpty())
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceService: 文件资源配置无效（缺少filePath）:" 
                               << resourceConfig.strUri;
        return false;
    }
    
    // 使用文件路径方式（支持自动MIME类型推断和二进制文件）
    MCPResource* pResource = doAddImpl(
        resourceConfig.strUri,
        resourceConfig.strName,
        resourceConfig.strDescription,
        resourceConfig.strFilePath,
        resourceConfig.strMimeType.isEmpty() ? QString() : resourceConfig.strMimeType);
    
    // 应用annotations（如果存在）
    if (pResource != nullptr)
    {
        applyAnnotationsIfNeeded(pResource, resourceConfig.annotations);
    }
    
    return pResource != nullptr;
}

bool MCPResourceService::addWrapperResourceFromConfig(const MCPResourceConfig& resourceConfig, const QMap<QString, QObject*>& dictHandlers)
{
    // 验证Handler名称
    if (resourceConfig.strHandlerName.isEmpty())
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceService: 包装资源配置无效（缺少handlerName）:" 
                               << resourceConfig.strUri;
        return false;
    }
    
    // 从dictHandlers映射表中查找Handler，如果dictHandlers为空则从qApp搜索
    QObject* pHandler = nullptr;
    if (!dictHandlers.isEmpty())
    {
        pHandler = dictHandlers.value(resourceConfig.strHandlerName, nullptr);
    }
    else
    {
        pHandler = MCPHandlerResolver::findHandler(resourceConfig.strHandlerName, nullptr);
    }
    
    if (pHandler == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceService: 资源Handler未找到:" << resourceConfig.strHandlerName
                               << ", 资源URI:" << resourceConfig.strUri;
        return false;
    }
    
    // 创建MCPResourceWrapper
    MCPResourceWrapper* pWrapper = MCPResourceWrapper::create(
        resourceConfig.strUri,
        pHandler,
        this);
    
    if (pWrapper == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceService: 创建资源包装器失败:" << resourceConfig.strUri
                               << ", Handler:" << resourceConfig.strHandlerName;
        return false;
    }
    
    // 设置资源元数据
    pWrapper->setName(resourceConfig.strName);
    pWrapper->setDescription(resourceConfig.strDescription);
    if (!resourceConfig.strMimeType.isEmpty())
    {
        pWrapper->setMimeType(resourceConfig.strMimeType);
    }
    
    // 注册资源
    bool bSuccess = registerResource(resourceConfig.strUri, pWrapper);
    
    // 应用annotations（如果存在）
    if (bSuccess)
    {
        applyAnnotationsIfNeeded(pWrapper, resourceConfig.annotations);
    }
    
    return bSuccess;
}

bool MCPResourceService::addContentResourceFromConfig(const MCPResourceConfig& resourceConfig)
{
    // 验证内容
    if (resourceConfig.strContent.isEmpty())
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceService: 内容资源配置无效（缺少content）:" 
                               << resourceConfig.strUri;
        return false;
    }
    
    // 使用静态内容方式
    QString strContent = resourceConfig.strContent;
    std::function<QString()> contentProvider = [strContent]() { return strContent; };
    
    MCPResource* pResource = doAddImpl(
        resourceConfig.strUri,
        resourceConfig.strName,
        resourceConfig.strDescription,
        resourceConfig.strMimeType.isEmpty() ? "text/plain" : resourceConfig.strMimeType,
        contentProvider);
    
    // 应用annotations（如果存在）
    if (pResource != nullptr)
    {
        applyAnnotationsIfNeeded(pResource, resourceConfig.annotations);
    }
    
    return pResource != nullptr;
}

bool MCPResourceService::applyAnnotationsIfNeeded(MCPResource* pResource, const QJsonObject& annotations)
{
    if (annotations.isEmpty())
    {
        return true;  // 没有annotations，直接返回成功
    }
    
    if (pResource == nullptr)
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceService: 无法应用annotations，资源对象为空";
        return false;
    }
    
    pResource->setAnnotations(annotations);
    return true;
}

MCPResource* MCPResourceService::doAddImpl(const QString& strUri,
                                           const QString& strName,
                                           const QString& strDescription,
                                           const QString& strMimeType,
                                           std::function<QString()> contentProvider)
{
    // 创建资源对象（父对象设为this）
    MCPContentResource* pResource = new MCPContentResource(strUri, this);
    pResource->withName(strName)
        ->withDescription(strDescription)
        ->withMimeType(strMimeType)
        ->withContentProvider(contentProvider);
    
    // 注册到服务
    if (!registerResource(strUri, pResource))
    {
        // 如果注册失败，删除Resource对象避免内存泄漏
        pResource->deleteLater();
        return nullptr;
    }
    
    return pResource;
}

MCPResource* MCPResourceService::doAddImpl(const QString& strUri,
                                           const QString& strName,
                                           const QString& strDescription,
                                           const QString& strFilePath,
                                           const QString& strMimeType)
{
    // 创建文件资源对象（父对象设为this）
    MCPFileResource* pResource = new MCPFileResource(strUri, strFilePath, strName, this);
    pResource->withDescription(strDescription);
    
    // 如果提供了MIME类型，设置它（否则MCPFileResource会自动推断）
    if (!strMimeType.isEmpty())
    {
        pResource->withMimeType(strMimeType);
    }
    
    // 注册到服务
    if (!registerResource(strUri, pResource))
    {
        // 如果注册失败，删除Resource对象避免内存泄漏
        pResource->deleteLater();
        return nullptr;
    }
    
    return pResource;
}

bool MCPResourceService::doRemoveImpl(const QString& strUri, bool bEmitSignal)
{
    if (!m_dictResources.contains(strUri))
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceService: 资源不存在:" << strUri;
        return false;
    }
    
    MCPResource* pResource = m_dictResources.take(strUri);
    if (pResource)
    {
        pResource->deleteLater();
    }
    
    MCP_CORE_LOG_INFO() << "MCPResourceService: 资源已注销:" << strUri;
    if (bEmitSignal)
    {
        emit resourceDeleted(strUri);  // 通知订阅者资源已删除（订阅机制）
        emit resourcesListChanged();    // 通知所有客户端（广播通知）
    }
    return true;
}

bool MCPResourceService::doHasImpl(const QString& strUri) const
{
    return m_dictResources.contains(strUri);
}

QJsonArray MCPResourceService::doListImpl(const QString& strUriPrefix) const
{
    QJsonArray arrResources;
    
    for (auto it = m_dictResources.constBegin(); it != m_dictResources.constEnd(); ++it)
    {
        const QString& strUri = it.key();
        MCPResource* pResource = it.value();
        
        // 如果指定了前缀，则过滤
        if (!strUriPrefix.isEmpty() && !strUri.startsWith(strUriPrefix))
        {
            continue;
        }
        
        // 构建资源元数据（使用 getMetadata() 方法，自动包含 annotations）
        QJsonObject metadata = pResource->getMetadata();
        metadata["uri"] = strUri;  // URI 需要单独添加，因为 getMetadata() 不包含 URI
        
        arrResources.append(metadata);
    }
    
    return arrResources;
}

QJsonObject MCPResourceService::doReadResourceImpl(const QString& strUri)
{
    if (!m_dictResources.contains(strUri))
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceService: 尝试读取不存在的资源:" << strUri;
        return QJsonObject();
    }
    
    MCPResource* pResource = m_dictResources[strUri];
    QString strContent = pResource->readContent();
    QString strMimeType = pResource->getMimeType();
    
    // 根据MCP协议规范，资源响应格式：
    // {
    //   "contents": [
    //     {
    //       "uri": "...",
    //       "mimeType": "...",
    //       "text": "..." 或 "blob": "..."
    //     }
    //   ]
    // }
    QJsonObject result;
    QJsonArray contents;
    QJsonObject contentObj;
    
    contentObj["uri"] = strUri;
    
    // 如果MIME类型不为空，添加到content对象中
    if (!strMimeType.isEmpty())
    {
        contentObj["mimeType"] = strMimeType;
    }
    
    // 根据MIME类型决定使用text还是blob字段
    if (MCPResourceContentGenerator::isTextMimeType(strMimeType))
    {
        // 文本类型，使用text字段
        contentObj["text"] = strContent;
    }
    else
    {
        // 二进制类型，使用blob字段（Base64编码）
        // 注意：readContent()对于二进制资源已经返回Base64编码的字符串
        contentObj["blob"] = strContent;
    }
    
    contents.append(contentObj);
    result["contents"] = contents;
    
    return result;
}

bool MCPResourceService::subscribe(const QString& strUri, const QString& strSessionId)
{
    if (strUri.isEmpty())
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceService: 订阅失败，URI为空";
        return false;
    }
    
    if (strSessionId.isEmpty())
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceService: 订阅失败，会话ID为空";
        return false;
    }
    
    // 检查是否已经订阅
    if (m_subscriptions.contains(strUri))
    {
        QSet<QString>& sessionIds = m_subscriptions[strUri];
        if (sessionIds.contains(strSessionId))
        {
            MCP_CORE_LOG_DEBUG() << "MCPResourceService: 会话" << strSessionId 
                                << "已订阅URI:" << strUri;
            return true; // 已经订阅，返回成功
        }
    }
    
    // 添加到订阅映射
    m_subscriptions[strUri].insert(strSessionId);
    m_sessionSubscriptions[strSessionId].insert(strUri);
    
    MCP_CORE_LOG_INFO() << "MCPResourceService: 会话" << strSessionId 
                       << "已订阅URI:" << strUri;
    return true;
}

bool MCPResourceService::unsubscribe(const QString& strUri, const QString& strSessionId)
{
    if (strUri.isEmpty())
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceService: 取消订阅失败，URI为空";
        return false;
    }
    
    if (strSessionId.isEmpty())
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceService: 取消订阅失败，会话ID为空";
        return false;
    }
    
    // 从URI订阅映射中移除
    if (m_subscriptions.contains(strUri))
    {
        QSet<QString>& sessionIds = m_subscriptions[strUri];
        if (sessionIds.contains(strSessionId))
        {
            sessionIds.remove(strSessionId);
            if (sessionIds.isEmpty())
            {
                // 如果没有订阅者了，移除该URI的映射
                m_subscriptions.remove(strUri);
            }
        }
        else
        {
            MCP_CORE_LOG_DEBUG() << "MCPResourceService: 会话" << strSessionId 
                                << "未订阅URI:" << strUri;
            return false;
        }
    }
    else
    {
        MCP_CORE_LOG_DEBUG() << "MCPResourceService: URI不存在订阅:" << strUri;
        return false;
    }
    
    // 从会话订阅映射中移除
    if (m_sessionSubscriptions.contains(strSessionId))
    {
        QSet<QString>& uris = m_sessionSubscriptions[strSessionId];
        uris.remove(strUri);
        if (uris.isEmpty())
        {
            // 如果该会话没有订阅了，移除该会话的映射
            m_sessionSubscriptions.remove(strSessionId);
        }
    }
    
    MCP_CORE_LOG_INFO() << "MCPResourceService: 会话" << strSessionId 
                       << "已取消订阅URI:" << strUri;
    return true;
}

void MCPResourceService::unsubscribeAll(const QString& strSessionId)
{
    if (strSessionId.isEmpty())
    {
        MCP_CORE_LOG_DEBUG() << "MCPResourceService: 会话ID为空";
        return;
    }
    
    if (!m_sessionSubscriptions.contains(strSessionId))
    {
        MCP_CORE_LOG_DEBUG() << "MCPResourceService: 会话" << strSessionId 
                            << "没有订阅记录";
        return;
    }
    
    // 获取该会话的所有订阅URI
    QSet<QString> uris = m_sessionSubscriptions[strSessionId];
    
    // 遍历所有URI，从订阅映射中移除该会话
    for (const QString& strUri : uris)
    {
        if (m_subscriptions.contains(strUri))
        {
            QSet<QString>& sessionIds = m_subscriptions[strUri];
            sessionIds.remove(strSessionId);
            
            if (sessionIds.isEmpty())
            {
                // 如果没有订阅者了，移除该URI的映射
                m_subscriptions.remove(strUri);
            }
        }
    }
    
    // 移除会话的订阅映射
    m_sessionSubscriptions.remove(strSessionId);
    
    MCP_CORE_LOG_INFO() << "MCPResourceService: 会话" << strSessionId 
                       << "的所有订阅已取消，共" << uris.size() << "个订阅";
}

QSet<QString> MCPResourceService::getSubscribedSessionIds(const QString& strUri) const
{
    if (strUri.isEmpty())
    {
        return QSet<QString>();
    }
    
    if (!m_subscriptions.contains(strUri))
    {
        return QSet<QString>();
    }
    
    return m_subscriptions[strUri];
}

MCPResource* MCPResourceService::getResource(const QString& strUri) const
{
    if (strUri.isEmpty())
    {
        return nullptr;
    }
    
    if (!m_dictResources.contains(strUri))
    {
        return nullptr;
    }
    
    return m_dictResources[strUri];
}
