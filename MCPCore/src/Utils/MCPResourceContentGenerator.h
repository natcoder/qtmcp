/**
 * @file MCPResourceContentGenerator.h
 * @brief MCP资源内容生成器（内部实现）
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QString>
#include <QJsonObject>
#include <QJsonArray>

/**
 * @brief MCP资源内容生成器
 * 
 * 职责：
 * - 根据MimeType和文件路径生成MCP协议规定的资源内容
 * - 支持文本和二进制资源的处理
 * - 自动判断内容类型并选择合适的编码方式
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 字符串类型添加 str 前缀
 * - { 和 } 要单独一行
 */
class MCPResourceContentGenerator
{
public:
    /**
     * @brief 根据MimeType和文件路径生成MCP协议规定的资源内容
     * @param strMimeType MIME类型（如 "text/plain", "image/png" 等）
     * @param strFilePath 文件路径
     * @param strUri 资源URI（可选，如果不提供则根据文件路径生成）
     * @return 符合MCP协议的资源内容QJsonObject，包含uri和contents数组
     *         如果文件读取失败，返回空的QJsonObject
     */
    static QJsonObject generateResourceContent(const QString& strMimeType, 
                                                const QString& strFilePath,
                                                const QString& strUri = QString());
    
    /**
     * @brief 判断MIME类型是否为文本类型
     * @param strMimeType MIME类型
     * @return 如果是文本类型返回true，否则返回false
     */
    static bool isTextMimeType(const QString& strMimeType);
    
    /**
     * @brief 读取文件内容为文本
     * @param strFilePath 文件路径
     * @return 文件文本内容，如果读取失败返回空字符串
     */
    static QString readFileAsText(const QString& strFilePath);
    
    /**
     * @brief 读取文件内容并进行Base64编码
     * @param strFilePath 文件路径
     * @return Base64编码后的字符串，如果读取失败返回空字符串
     */
    static QString readFileAsBase64(const QString& strFilePath);
    
    /**
     * @brief 将字节数组进行Base64编码
     * @param data 字节数组
     * @return Base64编码后的字符串
     */
    static QString base64Encode(const QByteArray& data);
    
    /**
     * @brief 根据文件路径生成资源URI
     * @param strFilePath 文件路径
     * @return 资源URI（file://协议）
     */
    static QString generateUriFromFilePath(const QString& strFilePath);

private:
    // 禁止实例化，所有方法都是静态的
    MCPResourceContentGenerator() = delete;
    ~MCPResourceContentGenerator() = delete;
};

