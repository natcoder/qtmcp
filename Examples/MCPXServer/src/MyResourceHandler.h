/**
 * @file MyResourceHandler.h
 * @brief 资源Handler示例类（用于验证MCPResourceWrapper）
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QTimer>

/**
 * @brief 资源Handler示例类
 * 
 * 职责：
 * - 实现MCPResource的最小接口要求
 * - 提供getMetadata()、getContent()方法和changed()信号
 * - 用于验证MCPResourceWrapper的功能
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 字符串类型添加 str 前缀
 * - { 和 } 要单独一行
 */
class MyResourceHandler : public QObject
{
    Q_OBJECT
    
public:
    explicit MyResourceHandler(QObject* pParent = nullptr);
    virtual ~MyResourceHandler();
    
public slots:
    /**
     * @brief 获取资源元数据
     * @return 资源元数据对象
     */
    QJsonObject getMetadata() const;
    
    /**
     * @brief 获取资源内容
     * @return 资源内容字符串
     */
    QString getContent() const;
    
    /**
     * @brief 更新资源内容（用于测试）
     * @param strNewContent 新的资源内容
     */
    void updateContent(const QString& strNewContent);
    
    /**
     * @brief 更新资源名称（用于测试）
     * @param strNewName 新的资源名称
     */
    void updateName(const QString& strNewName);
    
signals:
    /**
     * @brief 资源变化信号
     * @param strName 资源名称
     * @param strDescription 资源描述
     * @param strMimeType MIME类型
     */
    void changed(const QString& strName, const QString& strDescription, const QString& strMimeType);
    
private slots:
    /**
     * @brief 定时器超时，模拟资源内容变化
     */
    void onTimerTimeout();
    
private:
    QString m_strName;           // 资源名称
    QString m_strDescription;    // 资源描述
    QString m_strMimeType;       // MIME类型
    QString m_strContent;        // 资源内容
    QTimer* m_pTimer;            // 定时器（用于模拟资源变化）
    int m_nUpdateCount;          // 更新计数
};

