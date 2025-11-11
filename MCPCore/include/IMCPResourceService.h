/**
 * @file IMCPResourceService.h
 * @brief MCP资源服务接口
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include "MCPCore_global.h.h"
#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <functional>

/**
 * @brief MCP资源服务接口
 * 
 * 职责：
 * - 定义资源服务的公开接口
 * - 提供资源注册和管理功能
 * - 隐藏具体实现细节
 * 
 * 编码规范：
 * - 接口方法使用纯虚函数
 * - { 和 } 要单独一行
 * 
 * @note 死锁风险说明：在服务运行过程中调用addFromJson等方法添加wrapper类型资源时，
 *       如果调用方与包装对象在同一线程，可能导致死锁。此问题目前暂时不处理，
 *       建议在服务初始化阶段完成资源添加操作。
 */
class MCPCORE_EXPORT IMCPResourceService : public QObject
{
    Q_OBJECT
    
public:
    explicit IMCPResourceService(QObject* pParent = nullptr) : QObject(pParent) {}
    
protected:
    /**
     * @brief 析构函数（protected，Service 对象由 Server 管理，不要直接删除）
     */
    virtual ~IMCPResourceService() {}
    
public:
    /**
     * @brief 注册资源
     * @param strUri 资源URI
     * @param strName 资源名称
     * @param strDescription 资源描述
     * @param strMimeType MIME类型
     * @param contentProvider 内容提供函数
     * @return true表示注册成功，false表示失败
     */
    virtual bool add(const QString& strUri,
                     const QString& strName,
                     const QString& strDescription,
                     const QString& strMimeType,
                     std::function<QString()> contentProvider) = 0;
    
    /**
     * @brief 注册资源（从文件路径）
     * @param strUri 资源URI
     * @param strName 资源名称
     * @param strDescription 资源描述
     * @param strFilePath 文件路径
     * @param strMimeType MIME类型（可选，如果不提供则根据文件扩展名推断）
     * @return true表示注册成功，false表示失败
     * 
     * 使用示例：
     * @code
     * auto pResourceService = pServer->getResourceService();
     * // 方式1：自动推断MIME类型
     * pResourceService->add("file:///path/to/file.txt", "My File", "A text file", "/path/to/file.txt");
     * 
     * // 方式2：指定MIME类型
     * pResourceService->add("file:///path/to/image.png", "My Image", "An image", "/path/to/image.png", "image/png");
     * @endcode
     */
    virtual bool add(const QString& strUri,
                     const QString& strName,
                     const QString& strDescription,
                     const QString& strFilePath,
                     const QString& strMimeType = QString()) = 0;
    
    /**
     * @brief 注销资源
     * @param strUri 资源URI
     * @return true表示注销成功，false表示失败
     */
    virtual bool remove(const QString& strUri) = 0;
    
    /**
     * @brief 检查资源是否存在
     * @param strUri 资源URI
     * @return true表示存在，false表示不存在
     */
    virtual bool has(const QString& strUri) const = 0;
    
    /**
     * @brief 获取资源列表
     * @param strUriPrefix URI前缀（可选，用于过滤）
     * @return 资源列表（JSON数组格式）
     */
    virtual QJsonArray list(const QString& strUriPrefix = QString()) const = 0;
    
    /**
     * @brief 读取资源内容
     * @param strUri 资源URI
     * @return 资源内容（JSON对象格式）
     */
    virtual QJsonObject readResource(const QString& strUri) = 0;
    
    /**
     * @brief 从JSON对象添加资源
     * @param jsonResource JSON对象，包含资源的配置信息
     * @param pSearchRoot 搜索Handler的根对象，默认为nullptr（使用qApp）
     * @return true表示注册成功，false表示失败
     * 
     * JSON对象格式：
     * {
     *   "uri": "资源URI",
     *   "name": "资源名称",
     *   "description": "资源描述",
     *   "mimeType": "MIME类型（可选，默认text/plain）",
     *   "type": "资源类型：file/wrapper/content（可选，默认content）",
     *   "filePath": "文件路径（file类型必需）",
     *   "content": "静态内容（content类型必需）",
     *   "handlerName": "Handler名称（wrapper类型必需）",
     *   "annotations": { ... }（可选）
     * }
     * 
     * 使用示例：
     * @code
     * QJsonObject json;
     * json["uri"] = "file:///path/to/file";
     * json["name"] = "My File";
     * json["type"] = "file";
     * json["filePath"] = "/path/to/file.txt";
     * pResourceService->addFromJson(json);
     * @endcode
     */
    virtual bool addFromJson(const QJsonObject& jsonResource, QObject* pSearchRoot = nullptr) = 0;
    
signals:
    /**
     * @brief 资源列表变化信号
     * 当资源注册或注销时发出此信号
     */
    void resourcesListChanged();
};

