/**
 * @file MCPHelper.h
 * @brief MCP统一辅助工具类
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include "MCPCore_global.h.h"
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <functional>

/**
 * @brief MCP统一辅助工具类
 * 
 * 职责：
 * - 提供线程安全的方法调用
 * - 提供反射调用辅助
 * - 封装内部实现细节
 * 
 * 编码规范：
 * - 静态方法，无需实例化
 * - 参数命名遵循项目规范
 * - { 和 } 要单独一行
 */
class MCPCORE_EXPORT MCPHelper
{
public:
    /**
     * @brief 同步调用目标对象的方法（线程安全）
     * @param pTargetObj 目标对象
     * @param fun 要执行的函数
     * @return true表示成功，false表示失败
     */
    static void syncInvoke(QObject* pTargetObj, const std::function<void()>& fun);
    
    /**
     * @brief 异步调用目标对象的方法（线程安全）
     * @param pTargetObj 目标对象
     * @param fun 要执行的函数
     * @return true表示成功，false表示失败
     */
    static void asyncInvoke(QObject* pTargetObj, const std::function<void()>& fun);
    
    /**
     * @brief 同步调用对象的方法（通过反射）
     * @param pHandler 处理器对象
     * @param strMethodName 方法名
     * @param lstArguments 参数列表
     * @return 方法返回值
     */
    static QVariant callMethod(QObject* pHandler, const QString& strMethodName, const QVariantList& lstArguments);
    
    /**
     * @brief 同步调用对象的方法（通过反射，使用Map参数）
     * @param pHandler 处理器对象
     * @param strMethodName 方法名
     * @param dictArguments 参数字典
     * @return 方法返回值
     */
    static QVariant callMethod(QObject* pHandler, const QString& strMethodName, const QVariantMap& dictArguments);
    
    /**
     * @brief 设置线程名称
     * @param dwThreadID 线程ID
     * @param strThreadName 线程名称
     * @return true表示成功，false表示失败
     */
    static void setThreadName(unsigned long dwThreadID, const QString& strThreadName);
    
    /**
     * @brief 设置当前线程名称
     * @param strThreadName 线程名称
     * @return true表示成功，false表示失败
     */
    static void setCurrentThreadName(const QString& strThreadName);
};

