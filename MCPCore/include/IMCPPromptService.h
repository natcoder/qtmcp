/**
 * @file IMCPPromptService.h
 * @brief MCP提示词服务接口
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
#include <QList>
#include <QPair>
#include <QMap>
#include <functional>

/**
 * @brief MCP提示词服务接口
 * 
 * 职责：
 * - 定义提示词服务的公开接口
 * - 提供提示词注册和管理功能
 * - 隐藏具体实现细节
 * 
 * 编码规范：
 * - 接口方法使用纯虚函数
 * - { 和 } 要单独一行
 * 
 * @note 死锁风险说明：在服务运行过程中调用addFromJson等方法添加提示词时，
 *       如果调用方与相关对象在同一线程，可能导致死锁。此问题目前暂时不处理，
 *       建议在服务初始化阶段完成提示词添加操作。
 */
class MCPCORE_EXPORT IMCPPromptService : public QObject
{
    Q_OBJECT
    
public:
    explicit IMCPPromptService(QObject* pParent = nullptr) : QObject(pParent) {}
    
protected:
    /**
     * @brief 析构函数（protected，Service 对象由 Server 管理，不要直接删除）
     */
    virtual ~IMCPPromptService() {}
    
public:
    /**
     * @brief 注册提示词
     * @param strName 提示词名称
     * @param strDescription 提示词描述
     * @param arguments 参数列表（名称、(描述、是否必需)）
     * @param generator 提示词生成函数
     * @return true表示注册成功，false表示失败
     */
    virtual bool add(const QString& strName,
                     const QString& strDescription,
                     const QList<QPair<QString, QPair<QString, bool>>>& arguments,
                     std::function<QString(const QMap<QString, QString>&)> generator) = 0;
    
    /**
     * @brief 注册提示词（使用模板）
     * @param strName 提示词名称
     * @param strDescription 提示词描述
     * @param arguments 参数列表（名称、(描述、是否必需)）
     * @param strTemplate 提示词模板字符串，支持 {{变量名}} 格式的占位符
     * @return true表示注册成功，false表示失败
     * 
     * 使用示例：
     * @code
     * auto pPromptService = pServer->getPromptService();
     * QList<QPair<QString, QPair<QString, bool>>> args;
     * args.append(qMakePair("name", qMakePair("用户名", true)));
     * pPromptService->add("greeting", "问候提示词", args, "Hello {{name}}, welcome!");
     * @endcode
     */
    virtual bool add(const QString& strName,
                     const QString& strDescription,
                     const QList<QPair<QString, QPair<QString, bool>>>& arguments,
                     const QString& strTemplate) = 0;
    
    /**
     * @brief 注销提示词
     * @param strName 提示词名称
     * @return true表示注销成功，false表示失败
     */
    virtual bool remove(const QString& strName) = 0;
    
    /**
     * @brief 检查提示词是否存在
     * @param strName 提示词名称
     * @return true表示存在，false表示不存在
     */
    virtual bool has(const QString& strName) const = 0;
    
    /**
     * @brief 获取提示词列表
     * @return 提示词列表（JSON数组格式）
     */
    virtual QJsonArray list() const = 0;
    
    /**
     * @brief 获取提示词内容
     * @param strName 提示词名称
     * @param arguments 参数字典
     * @return 提示词内容（JSON对象格式）
     */
    virtual QJsonObject getPrompt(const QString& strName, const QMap<QString, QString>& arguments) = 0;
    
    /**
     * @brief 从JSON对象添加提示词
     * @param jsonPrompt JSON对象，包含提示词的配置信息
     * @return true表示注册成功，false表示失败
     * 
     * JSON对象格式：
     * {
     *   "name": "提示词名称",
     *   "description": "提示词描述",
     *   "template": "提示词模板字符串，支持 {{变量名}} 格式的占位符",
     *   "arguments": [
     *     {
     *       "name": "参数名",
     *       "description": "参数描述",
     *       "required": true/false
     *     },
     *     ...
     *   ]
     * }
     * 
     * 使用示例：
     * @code
     * QJsonObject json;
     * json["name"] = "greeting";
     * json["description"] = "问候提示词";
     * json["template"] = "Hello {{name}}, welcome!";
     * QJsonArray args;
     * QJsonObject arg;
     * arg["name"] = "name";
     * arg["description"] = "用户名";
     * arg["required"] = true;
     * args.append(arg);
     * json["arguments"] = args;
     * pPromptService->addFromJson(json);
     * @endcode
     */
    virtual bool addFromJson(const QJsonObject& jsonPrompt) = 0;
    
signals:
    /**
     * @brief 提示词列表变化信号
     * 当提示词注册或注销时发出此信号
     */
    void promptsListChanged();
};

