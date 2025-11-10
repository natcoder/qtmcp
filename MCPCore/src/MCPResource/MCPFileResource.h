/**
 * @file MCPFileResource.h
 * @brief MCP文件资源类（继承自MCPContentResource）
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include "MCPContentResource.h"

/**
 * @brief MCP文件资源类
 * 
 * 职责：
 * - 实现基于文件路径的资源内容读取
 * - 支持文本和二进制文件
 * - 自动推断MIME类型
 * 
 * 注意：
 * - 继承自MCPContentResource，复用链式调用接口（withName、withDescription、withMimeType）
 * - 通过contentProvider实现文件读取逻辑
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 字符串类型添加 str 前缀
 * - { 和 } 要单独一行
 */
class MCPFileResource : public MCPContentResource
{
    Q_OBJECT
    
public:
    /**
     * @brief 构造函数
     * @param strUri 资源URI
     * @param strFilePath 文件路径
     * @param strName 资源名称（可选，如果不提供则使用文件名）
     * @param pParent 父对象
     */
    explicit MCPFileResource(const QString& strUri,
                             const QString& strFilePath,
                             const QString& strName = QString(),
                             QObject* pParent = nullptr);
    
    virtual ~MCPFileResource();
    
    /**
     * @brief 获取文件路径
     * @return 文件路径
     */
    QString getFilePath() const;
    
private:
    /**
     * @brief 根据文件扩展名推断MIME类型
     */
    void inferMimeType();
    
    /**
     * @brief 创建文件内容提供函数
     * @return 内容提供函数
     */
    std::function<QString()> createFileContentProvider() const;
    
private:
    QString m_strFilePath;  // 文件路径
};

