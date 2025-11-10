/**
 * @file MCPToolOutputSchema.h
 * @brief MCP工具输出Schema辅助类
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QJsonObject>
#include <QString>
#include "MCPCore_global.h.h"

/**
 * @brief MCPToolOutputSchema 辅助类
 *
 * 提供类型安全的输出Schema构建API
 * 
 * 使用示例：
 * @code
 * MCPToolOutputSchema schema;
 * schema.addStringField("message", "消息")
 *       .addNumberField("code", "状态码")
 *       .addBooleanField("success", "是否成功");
 * QJsonObject jsonSchema = schema.toJsonObject();
 * @endcode
 * 
 * 编码规范：
 * - 类成员添加 m_ 前缀
 * - 字符串类型添加 str 前缀
 * - { 和 } 要单独一行
 */
class MCPCORE_EXPORT MCPToolOutputSchema
{
public:
    MCPToolOutputSchema();
    
    MCPToolOutputSchema* withDescription(const QString& strDescription);
    
    // 基本类型字段
    MCPToolOutputSchema* addStringField(const QString& strFieldName, const QString& strDescription);
    MCPToolOutputSchema* addStringFieldWithLength(const QString& strFieldName, const QString& strDescription, int nMinLength, int nMaxLength);
    MCPToolOutputSchema* addStringFieldWithFormat(const QString& strFieldName, const QString& strDescription, const QString& strFormat);
    MCPToolOutputSchema* addStringFieldWithDefault(const QString& strFieldName, const QString& strDescription, const QString& strDefaultValue);
    
    MCPToolOutputSchema* addNumberField(const QString& strFieldName, const QString& strDescription);
    MCPToolOutputSchema* addNumberFieldWithRange(const QString& strFieldName, const QString& strDescription, double dMinimum, double dMaximum);
    MCPToolOutputSchema* addNumberFieldWithDefault(const QString& strFieldName, const QString& strDescription, double dDefaultValue);
    
    MCPToolOutputSchema* addBooleanField(const QString& strFieldName, const QString& strDescription);
    MCPToolOutputSchema* addBooleanFieldWithDefault(const QString& strFieldName, const QString& strDescription, bool bDefaultValue);
    
    MCPToolOutputSchema* addIntegerField(const QString& strFieldName, const QString& strDescription);
    MCPToolOutputSchema* addIntegerFieldWithRange(const QString& strFieldName, const QString& strDescription, int nMinimum, int nMaximum);
    MCPToolOutputSchema* addIntegerFieldWithDefault(const QString& strFieldName, const QString& strDescription, int nDefaultValue);
    
    MCPToolOutputSchema* addNullField(const QString& strFieldName, const QString& strDescription);
    
    // 枚举类型
    MCPToolOutputSchema* addEnumField(const QString& strFieldName, const QString& strDescription, const QStringList& strEnumValues);
    
    // 数组类型
    MCPToolOutputSchema* addStringArrayField(const QString& strFieldName, const QString& strDescription);
    MCPToolOutputSchema* addStringArrayFieldWithSize(const QString& strFieldName, const QString& strDescription, int nMinItems, int nMaxItems);
    MCPToolOutputSchema* addNumberArrayField(const QString& strFieldName, const QString& strDescription);
    MCPToolOutputSchema* addNumberArrayFieldWithSize(const QString& strFieldName, const QString& strDescription, int nMinItems, int nMaxItems);
    MCPToolOutputSchema* addBooleanArrayField(const QString& strFieldName, const QString& strDescription);
    MCPToolOutputSchema* addBooleanArrayFieldWithSize(const QString& strFieldName, const QString& strDescription, int nMinItems, int nMaxItems);
    MCPToolOutputSchema* addArrayField(const QString& strFieldName, const QString& strDescription, const QString& strItemType);
    MCPToolOutputSchema* addArrayFieldWithSize(const QString& strFieldName, const QString& strDescription, const QString& strItemType, int nMinItems, int nMaxItems);
    
    // 对象类型（嵌套）
    MCPToolOutputSchema* addObjectField(const QString& strFieldName, const QString& strDescription, MCPToolOutputSchema* pObjectSchema);
    MCPToolOutputSchema* addObjectArrayField(const QString& strFieldName, const QString& strDescription, MCPToolOutputSchema* pItemSchema);
    
    MCPToolOutputSchema* addRequiredField(const QString& strFieldName);
    
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
    QString m_strDescription;
    QJsonObject m_schema;
    QStringList m_requiredFields;
};

