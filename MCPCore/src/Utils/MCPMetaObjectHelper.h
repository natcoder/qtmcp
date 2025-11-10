/**
 * @file MCPMetaObjectHelper.h
 * @brief MCP元对象助手类（提供QObject元对象操作的通用工具）
 * @author zhangheng
 * @date 2025-01-09
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
#include <QString>
#include <QJsonObject>

/**
 * @brief MCP元对象助手类
 * 
 * 职责：
 * - 提供QObject元对象的通用操作工具
 * - 查找和获取信号、方法、属性等元对象信息
 * - 通过Qt Meta机制调用对象的方法
 * - 提供统一的错误处理和日志记录
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 指针类型添加 p 前缀
 * - 字符串类型添加 str 前缀
 * - { 和 } 要单独一行
 */
class MCPMetaObjectHelper
{
public:
    /**
     * @brief 获取指定QObject的信号
     * @param pObject QObject对象指针
     * @param strSignalSignature 信号签名（如 "changed()"）
     * @return 信号的QMetaMethod，如果不存在或对象为空则返回无效的QMetaMethod
     * 
     * 使用示例：
     * @code
     * QObject* pMyObject = new MyObject();
     * QMetaMethod changedSignal = MCPMetaObjectHelper::getSignal(pMyObject, "changed()");
     * if (changedSignal.isValid())
     * {
     *     QObject::connect(pMyObject, changedSignal, receiver, slot);
     * }
     * @endcode
     */
    static QMetaMethod getSignal(QObject* pObject, const QString& strSignalSignature);
    
    /**
     * @brief 获取指定QObject的方法（slot或method）
     * @param pObject QObject对象指针
     * @param strMethodSignature 方法签名（如 "getContent()"）
     * @return 方法的QMetaMethod，如果不存在或对象为空则返回无效的QMetaMethod
     * 
     * 使用示例：
     * @code
     * QObject* pMyObject = new MyObject();
     * QMetaMethod getContentMethod = MCPMetaObjectHelper::getMethod(pMyObject, "getContent()");
     * if (getContentMethod.isValid())
     * {
     *     // 使用方法
     * }
     * @endcode
     */
    static QMetaMethod getMethod(QObject* pObject, const QString& strMethodSignature);
    
    /**
     * @brief 检查QObject是否有指定的信号
     * @param pObject QObject对象指针
     * @param strSignalSignature 信号签名（如 "changed()"）
     * @return 如果存在返回true，否则返回false
     */
    static bool hasSignal(QObject* pObject, const QString& strSignalSignature);
    
    /**
     * @brief 检查QObject是否有指定的方法（slot或method）
     * @param pObject QObject对象指针
     * @param strMethodSignature 方法签名（如 "getContent()"）
     * @return 如果存在返回true，否则返回false
     */
    static bool hasMethod(QObject* pObject, const QString& strMethodSignature);
    
    /**
     * @brief 检查QObject是否有指定名称和返回类型的slot方法
     * @param pObject QObject对象指针
     * @param strMethodName 方法名（不含参数，如 "getContent"）
     * @param nReturnType 返回类型（QMetaType类型）
     * @return 如果存在返回true，否则返回false
     */
    static bool hasSlot(QObject* pObject, const QString& strMethodName, int nReturnType);
    
    /**
     * @brief 通过Qt Meta机制调用对象的方法（无返回值）
     * @param pObject QObject对象指针
     * @param strMethodName 方法名
     * @return 如果调用成功返回true，否则返回false
     */
    static bool invokeMethod(QObject* pObject, const QString& strMethodName);
    
    /**
     * @brief 通过Qt Meta机制调用对象的方法（返回QString）
     * @param pObject QObject对象指针
     * @param strMethodName 方法名
     * @param strResult 输出参数，方法返回值
     * @return 如果调用成功返回true，否则返回false
     */
    static bool invokeMethod(QObject* pObject, const QString& strMethodName, QString& strResult);
    
    /**
     * @brief 通过Qt Meta机制调用对象的方法（返回QJsonObject）
     * @param pObject QObject对象指针
     * @param strMethodName 方法名
     * @param objResult 输出参数，方法返回值
     * @return 如果调用成功返回true，否则返回false
     */
    static bool invokeMethod(QObject* pObject, const QString& strMethodName, QJsonObject& objResult);
    
    /**
     * @brief 获取QObject的元对象
     * @param pObject QObject对象指针
     * @return 元对象指针，如果对象为空则返回nullptr
     */
    static const QMetaObject* getMetaObject(QObject* pObject);

private:
    // 禁止实例化，所有方法都是静态的
    MCPMetaObjectHelper() = delete;
    ~MCPMetaObjectHelper() = delete;
};

