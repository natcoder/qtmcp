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
#include "MCPLog.h"
#include "Utils/MCPInvokeHelper.h"
#include <QSet>
#include <QFile>
#include <QFileInfo>
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
        return doAddImpl(strUri, strName, strDescription, strMimeType, contentProvider);
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
        return doAddImpl(strUri, strName, strDescription, strFilePath, strMimeType);
    });
}

bool MCPResourceService::registerResource(const QString& strUri, MCPResource* pResource)
{
    if (m_dictResources.contains(strUri))
    {
        MCP_CORE_LOG_WARNING() << "MCPResourceService: 资源已存在:" << strUri;
        return false;
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

bool MCPResourceService::doAddImpl(const QString& strUri,
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
    bool bSuccess = registerResource(strUri, pResource);
    
    // 如果注册失败，删除Resource对象避免内存泄漏
    if (!bSuccess)
    {
        pResource->deleteLater();
    }
    
    return bSuccess;
}

bool MCPResourceService::doAddImpl(const QString& strUri,
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
    bool bSuccess = registerResource(strUri, pResource);
    
    // 如果注册失败，删除Resource对象避免内存泄漏
    if (!bSuccess)
    {
        pResource->deleteLater();
    }
    
    return bSuccess;
}

bool MCPResourceService::doRemoveImpl(const QString& strUri)
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
    emit resourceDeleted(strUri);  // 通知订阅者资源已删除（订阅机制）
    emit resourcesListChanged();    // 通知所有客户端（广播通知）
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
