/**
 * @file MCPHandlerResolver.h
 * @brief MCP Handler解析器
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QString>
#include <QMap>

/**
 * @brief MCP Handler解析器
 * 
 * 职责：
 * - 从应用程序中查找Handler对象
 * - 构建Handler名称到对象的映射表
 * - 支持多种Handler标识方式
 * 
 * 使用示例：
 * @code
 * QMap<QString, QObject*> handlers = MCPHandlerResolver::resolveHandlers();
 * QObject* pHandler = handlers.value("MyHandler");
 * @endcode
 * 
 * 编码规范：
 * - 静态工具类
 * - 字符串类型添加 str 前缀
 * - 指针类型添加 p 前缀
 * - { 和 } 要单独一行
 */
class MCPHandlerResolver
{
public:
    /**
     * @brief 从应用程序中查找所有Handler对象
     * @param pSearchRoot 搜索根对象，默认为qApp
     * @return Handler名称到对象的映射表
     * 
     * 支持的标识方式：
     * 1. 对象的objectName
     * 2. 对象的"MPCServerHandlerName"属性（用于工具Handler）
     * 3. 对象的"MCPResourceHandlerName"属性（用于资源Handler）
     */
    static QMap<QString, QObject*> resolveHandlers(QObject* pSearchRoot = nullptr);
    
    /**
     * @brief 从应用程序中查找所有资源Handler对象
     * @param pSearchRoot 搜索根对象，默认为qApp
     * @return 资源Handler名称到对象的映射表
     * 
     * 支持的标识方式：
     * 1. 对象的objectName
     * 2. 对象的"MCPResourceHandlerName"属性
     */
    static QMap<QString, QObject*> resolveResourceHandlers(QObject* pSearchRoot = nullptr);
    
    /**
     * @brief 查找指定名称的Handler
     * @param strHandlerName Handler名称
     * @param pSearchRoot 搜索根对象，默认为qApp
     * @return Handler对象指针，未找到返回nullptr
     */
    static QObject* findHandler(const QString& strHandlerName, QObject* pSearchRoot = nullptr);
};

