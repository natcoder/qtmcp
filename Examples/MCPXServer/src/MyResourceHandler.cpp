/**
 * @file MyResourceHandler.cpp
 * @brief 资源Handler示例类实现
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MyResourceHandler.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <QVariant>
MyResourceHandler::MyResourceHandler(QObject* pParent)
    : QObject(pParent)
    , m_strName("示例资源")
    , m_strDescription("这是一个用于验证MCPResourceWrapper的示例资源")
    , m_strMimeType("application/json")
    , m_strContent("{\"message\":\"Hello, MCP Resource!\",\"timestamp\":\"\"}")
    , m_pTimer(nullptr)
    , m_nUpdateCount(0)
{
    // 设置MCPResourceHandlerName属性，以便MCPHandlerResolver能找到此对象
    setProperty("MCPResourceHandlerName", "MyResourceHandler");
    
    // 设置objectName作为备用标识
    setObjectName("MyResourceHandler");
    
    // 创建定时器，每5秒更新一次资源内容（用于测试资源变化通知）
    m_pTimer = new QTimer(this);
    connect(m_pTimer, &QTimer::timeout, this, &MyResourceHandler::onTimerTimeout);
    m_pTimer->start(5000); // 5秒触发一次
}

MyResourceHandler::~MyResourceHandler()
{
    if (m_pTimer)
    {
        m_pTimer->stop();
    }
}

QJsonObject MyResourceHandler::getMetadata() const
{
    QJsonObject metadata;
    metadata["name"] = m_strName;
    metadata["description"] = m_strDescription;
    metadata["mimeType"] = m_strMimeType;
    return metadata;
}

QString MyResourceHandler::getContent() const
{
    // 返回当前资源内容
    return m_strContent;
}

void MyResourceHandler::updateContent(const QString& strNewContent)
{
    if (m_strContent != strNewContent)
    {
        m_strContent = strNewContent;
        // 发出changed信号通知资源内容变化
        emit changed(m_strName, m_strDescription, m_strMimeType);
    }
}

void MyResourceHandler::updateName(const QString& strNewName)
{
    if (m_strName != strNewName)
    {
        m_strName = strNewName;
        // 发出changed信号通知资源元数据变化
        emit changed(m_strName, m_strDescription, m_strMimeType);
    }
}

void MyResourceHandler::onTimerTimeout()
{
    // 模拟资源内容变化
    m_nUpdateCount++;
    
    QJsonObject contentObj;
    contentObj["message"] = "Hello, MCP Resource!";
    contentObj["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    contentObj["updateCount"] = m_nUpdateCount;
    
    QJsonDocument doc(contentObj);
    QString strNewContent = doc.toJson(QJsonDocument::Compact);
    
    // 更新内容并发出changed信号
    updateContent(strNewContent);
}

