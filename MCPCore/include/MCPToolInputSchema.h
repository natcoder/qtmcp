/**
 * @file MCPToolInputSchema.h
 * @brief MCP工具输入Schema辅助类
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include "MCPCore_global.h.h"

/**
 * @brief MCPToolInputSchema 辅助类
 *
 * 提供类型安全的输入Schema构建API，简化Schema定义
 * 
 * 使用场景：
 * - 简单Schema：直接使用JSON对象
 * - 复杂Schema：使用此辅助类（链式调用更清晰）
 * 
 * 使用示例：
 * @code
 * MCPToolInputSchema schema;
 * schema.addStringField("name", "用户名", true)
 *       .addNumberField("age", "年龄", true)
 *       .addEnumField("gender", "性别", QStringList() << "male" << "female", true);
 * QJsonObject jsonSchema = schema.toJsonObject();
 * @endcode
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 字符串类型添加 str 前缀
 * - 数值类型添加 n 或 d 前缀
 * - { 和 } 要单独一行
 */
class MCPCORE_EXPORT MCPToolInputSchema
{
public:
    MCPToolInputSchema();
    
    // 基本类型字段
    MCPToolInputSchema* addStringField(const QString& strFieldName, const QString& strDescription, bool bRequired = true);
    MCPToolInputSchema* addStringFieldWithLength(const QString& strFieldName, const QString& strDescription, int nMinLength, int nMaxLength, bool bRequired = true);
    MCPToolInputSchema* addStringFieldWithFormat(const QString& strFieldName, const QString& strDescription, const QString& strFormat, bool bRequired = true);
    MCPToolInputSchema* addStringFieldWithDefault(const QString& strFieldName, const QString& strDescription, const QString& strDefaultValue, bool bRequired = false);
    
    MCPToolInputSchema* addNumberField(const QString& strFieldName, const QString& strDescription, bool bRequired = true);
    MCPToolInputSchema* addNumberFieldWithRange(const QString& strFieldName, const QString& strDescription, double dMinimum, double dMaximum, bool bRequired = true);
    MCPToolInputSchema* addNumberFieldWithDefault(const QString& strFieldName, const QString& strDescription, double dDefaultValue, bool bRequired = false);
    
    MCPToolInputSchema* addBooleanField(const QString& strFieldName, const QString& strDescription, bool bRequired = true);
    MCPToolInputSchema* addBooleanFieldWithDefault(const QString& strFieldName, const QString& strDescription, bool bDefaultValue, bool bRequired = false);
    
    MCPToolInputSchema* addIntegerField(const QString& strFieldName, const QString& strDescription, bool bRequired = true);
    MCPToolInputSchema* addIntegerFieldWithRange(const QString& strFieldName, const QString& strDescription, int nMinimum, int nMaximum, bool bRequired = true);
    MCPToolInputSchema* addIntegerFieldWithDefault(const QString& strFieldName, const QString& strDescription, int nDefaultValue, bool bRequired = false);
    
    // 枚举类型
    MCPToolInputSchema* addEnumField(const QString& strFieldName, const QString& strDescription, const QStringList& strEnumValues, bool bRequired = true);
    
    // 数组类型
    MCPToolInputSchema* addStringArrayField(const QString& strFieldName, const QString& strDescription, bool bRequired = true);
    MCPToolInputSchema* addStringArrayFieldWithSize(const QString& strFieldName, const QString& strDescription, int nMinItems, int nMaxItems, bool bRequired = true);
    MCPToolInputSchema* addNumberArrayField(const QString& strFieldName, const QString& strDescription, bool bRequired = true);
    MCPToolInputSchema* addNumberArrayFieldWithSize(const QString& strFieldName, const QString& strDescription, int nMinItems, int nMaxItems, bool bRequired = true);
    MCPToolInputSchema* addBooleanArrayField(const QString& strFieldName, const QString& strDescription, bool bRequired = true);
    MCPToolInputSchema* addBooleanArrayFieldWithSize(const QString& strFieldName, const QString& strDescription, int nMinItems, int nMaxItems, bool bRequired = true);
    MCPToolInputSchema* addArrayField(const QString& strFieldName, const QString& strDescription, const QString& strItemType, bool bRequired = true);
    MCPToolInputSchema* addArrayFieldWithSize(const QString& strFieldName, const QString& strDescription, const QString& strItemType, int nMinItems, int nMaxItems, bool bRequired = true);
    
    // 对象类型（嵌套）
    MCPToolInputSchema* addObjectField(const QString& strFieldName, const QString& strDescription, MCPToolInputSchema* pObjectSchema, bool bRequired = true);
    MCPToolInputSchema* addObjectArrayField(const QString& strFieldName, const QString& strDescription, MCPToolInputSchema* pItemSchema, bool bRequired = true);
    
    /**
     * @brief 转换为JSON对象
     * @return JSON Schema对象
     */
    QJsonObject toJsonObject() const;
    
    /**
     * @brief 转换为字符串
     * @return JSON格式字符串
     */
    QString toString() const;

private:
    // 辅助方法：创建字段Schema
    QJsonObject createFieldSchema(const QString& strType, const QString& strDescription) const;
    QJsonObject createFieldSchemaWithDefault(const QString& strType, const QString& strDescription, const QJsonValue& defaultValue) const;
    QJsonObject createNumberFieldSchemaWithRange(const QString& strDescription, double dMinimum, double dMaximum) const;
    QJsonObject createIntegerFieldSchemaWithRange(const QString& strDescription, int nMinimum, int nMaximum) const;
    QJsonObject createStringFieldSchemaWithLength(const QString& strDescription, int nMinLength, int nMaxLength) const;
    QJsonObject createArrayFieldSchema(const QString& strItemType, const QString& strDescription) const;
    QJsonObject createArrayFieldSchemaWithSize(const QString& strItemType, const QString& strDescription, int nMinItems, int nMaxItems) const;

private:
    QJsonObject m_schema;
    QStringList m_requiredFields;
};

