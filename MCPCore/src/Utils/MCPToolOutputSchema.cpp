/**
 * @file MCPToolOutputSchema.cpp
 * @brief MCP工具输出模式实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPToolOutputSchema.h"
#include <QJsonArray>
#include <QJsonDocument>
MCPToolOutputSchema::MCPToolOutputSchema()
{
	m_schema.insert("type", "object");
	m_schema.insert("properties", QJsonObject());
}

MCPToolOutputSchema* MCPToolOutputSchema::withDescription(const QString& strDescription)
{
    m_strDescription = strDescription;
    return this;
}

// 基本类型字段
MCPToolOutputSchema* MCPToolOutputSchema::addStringField(const QString& strFieldName, const QString& strDescription)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createFieldSchema("string", strDescription));
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addStringFieldWithLength(const QString& strFieldName, const QString& strDescription, int nMinLength, int nMaxLength)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createStringFieldSchemaWithLength(strDescription, nMinLength, nMaxLength));
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addStringFieldWithFormat(const QString& strFieldName, const QString& strDescription, const QString& format)
{
    QJsonObject properties = m_schema.value("properties").toObject();

    QJsonObject fieldSchema;
    fieldSchema.insert("type", "string");
    fieldSchema.insert("description", strDescription);
    fieldSchema.insert("format", format);

    properties.insert(strFieldName, fieldSchema);
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addStringFieldWithDefault(const QString& strFieldName, const QString& strDescription, const QString& defaultValue)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createFieldSchemaWithDefault("string", strDescription, defaultValue));
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addNumberField(const QString& strFieldName, const QString& strDescription)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createFieldSchema("number", strDescription));
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addNumberFieldWithRange(const QString& strFieldName, const QString& strDescription, double minimum, double maximum)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createNumberFieldSchemaWithRange(strDescription, minimum, maximum));
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addNumberFieldWithDefault(const QString& strFieldName, const QString& strDescription, double defaultValue)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createFieldSchemaWithDefault("number", strDescription, defaultValue));
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addBooleanField(const QString& strFieldName, const QString& strDescription)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createFieldSchema("boolean", strDescription));
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addBooleanFieldWithDefault(const QString& strFieldName, const QString& strDescription, bool defaultValue)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createFieldSchemaWithDefault("boolean", strDescription, defaultValue));
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addIntegerField(const QString& strFieldName, const QString& strDescription)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createFieldSchema("integer", strDescription));
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addIntegerFieldWithRange(const QString& strFieldName, const QString& strDescription, int nMinimum, int nMaximum)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createIntegerFieldSchemaWithRange(strDescription, nMinimum, nMaximum));
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addIntegerFieldWithDefault(const QString& strFieldName, const QString& strDescription, int nDefaultValue)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createFieldSchemaWithDefault("integer", strDescription, nDefaultValue));
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addNullField(const QString& strFieldName, const QString& strDescription)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createFieldSchema("null", strDescription));
    m_schema.insert("properties", properties);
    return this;
}

// 枚举类型字段
MCPToolOutputSchema* MCPToolOutputSchema::addEnumField(const QString& strFieldName, const QString& strDescription, const QStringList& enumValues)
{
    QJsonObject properties = m_schema.value("properties").toObject();

    QJsonObject fieldSchema;
    fieldSchema.insert("type", "string");
    fieldSchema.insert("description", strDescription);

    QJsonArray enumArray;
    for (const QString& value : enumValues) {
        enumArray.append(value);
    }
    fieldSchema.insert("enum", enumArray);

    properties.insert(strFieldName, fieldSchema);
    m_schema.insert("properties", properties);
    return this;
}

// 数组类型字段
MCPToolOutputSchema* MCPToolOutputSchema::addStringArrayField(const QString& strFieldName, const QString& strDescription)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createArrayFieldSchema("string", strDescription));
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addStringArrayFieldWithSize(const QString& strFieldName, const QString& strDescription, int nMinItems, int nMaxItems)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createArrayFieldSchemaWithSize("string", strDescription, nMinItems, nMaxItems));
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addNumberArrayField(const QString& strFieldName, const QString& strDescription)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createArrayFieldSchema("number", strDescription));
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addNumberArrayFieldWithSize(const QString& strFieldName, const QString& strDescription, int nMinItems, int nMaxItems)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createArrayFieldSchemaWithSize("number", strDescription, nMinItems, nMaxItems));
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addBooleanArrayField(const QString& strFieldName, const QString& strDescription)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createArrayFieldSchema("boolean", strDescription));
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addBooleanArrayFieldWithSize(const QString& strFieldName, const QString& strDescription, int nMinItems, int nMaxItems)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createArrayFieldSchemaWithSize("boolean", strDescription, nMinItems, nMaxItems));
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addArrayField(const QString& strFieldName, const QString& strDescription, const QString& itemType)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createArrayFieldSchema(itemType, strDescription));
    m_schema.insert("properties", properties);
    return this;
}

MCPToolOutputSchema* MCPToolOutputSchema::addArrayFieldWithSize(const QString& strFieldName, const QString& strDescription, const QString& strItemType, int nMinItems, int nMaxItems)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createArrayFieldSchemaWithSize(strItemType, strDescription, nMinItems, nMaxItems));
    m_schema.insert("properties", properties);
    return this;
}

// 对象类型字段（嵌套）
MCPToolOutputSchema* MCPToolOutputSchema::addObjectField(const QString& strFieldName, const QString& strDescription, MCPToolOutputSchema* pObjectSchema)
{
    QJsonObject properties = m_schema.value("properties").toObject();

    QJsonObject fieldSchema;
    fieldSchema.insert("type", "object");
    fieldSchema.insert("description", strDescription);
    if (pObjectSchema)
    {
        fieldSchema.insert("properties", pObjectSchema->toJsonObject().value("properties"));
    }
    else
    {
        fieldSchema.insert("properties", QJsonObject());
    }

    properties.insert(strFieldName, fieldSchema);
    m_schema.insert("properties", properties);
    return this;
}

// 对象数组类型字段
MCPToolOutputSchema* MCPToolOutputSchema::addObjectArrayField(const QString& strFieldName, const QString& strDescription, MCPToolOutputSchema* pItemSchema)
{
    QJsonObject properties = m_schema.value("properties").toObject();

    QJsonObject fieldSchema;
    fieldSchema.insert("type", "array");
    fieldSchema.insert("description", strDescription);

    if (pItemSchema)
    {
        fieldSchema.insert("items", pItemSchema->toJsonObject());
    }
    else
    {
        fieldSchema.insert("items", QJsonObject
        {
            {"type", "object"},
            {"properties", QJsonObject()}
        });
    }

    properties.insert(strFieldName, fieldSchema);
    m_schema.insert("properties", properties);
    return this;
}

// 必填字段管理
MCPToolOutputSchema* MCPToolOutputSchema::addRequiredField(const QString& strFieldName)
{
    if (!m_requiredFields.contains(strFieldName))
    {
        m_requiredFields.append(strFieldName);
    }
    return this;
}

// 获取最终的QJsonObject
QJsonObject MCPToolOutputSchema::toJsonObject() const
{
    QJsonObject result = m_schema;
	result.insert("description", m_strDescription);
	if (!m_requiredFields.isEmpty())
    {
        QJsonArray requiredArray;
        for (const QString& field : m_requiredFields)
        {
            requiredArray.append(field);
        }
        result.insert("required", requiredArray);
    }
    return result;
}


QJsonObject MCPToolOutputSchema::createFieldSchema(const QString& type, const QString& strDescription) const
{
    QJsonObject schema;
    schema.insert("type", type);
    schema.insert("description", strDescription);
    return schema;
}

QJsonObject MCPToolOutputSchema::createFieldSchemaWithDefault(const QString& type, const QString& strDescription, const QJsonValue& defaultValue) const
{
    QJsonObject schema = createFieldSchema(type, strDescription);
    schema.insert("default", defaultValue);
    return schema;
}

QJsonObject MCPToolOutputSchema::createNumberFieldSchemaWithRange(const QString& strDescription, double minimum, double maximum) const
{
    QJsonObject schema;
    schema.insert("type", "number");
    schema.insert("description", strDescription);
    schema.insert("minimum", minimum);
    schema.insert("maximum", maximum);
    return schema;
}

QJsonObject MCPToolOutputSchema::createIntegerFieldSchemaWithRange(const QString& strDescription, int nMinimum, int nMaximum) const
{
    QJsonObject schema;
    schema.insert("type", "integer");
    schema.insert("description", strDescription);
    schema.insert("minimum", nMinimum);
    schema.insert("maximum", nMaximum);
    return schema;
}

QJsonObject MCPToolOutputSchema::createStringFieldSchemaWithLength(const QString& strDescription, int nMinLength, int nMaxLength) const
{
    QJsonObject schema;
    schema.insert("type", "string");
    schema.insert("description", strDescription);
    if (nMinLength >= 0)
    {
        schema.insert("minLength", nMinLength);
    }
    if (nMaxLength >= 0)
    {
        schema.insert("maxLength", nMaxLength);
    }
    return schema;
}

QJsonObject MCPToolOutputSchema::createArrayFieldSchema(const QString& strItemType, const QString& strDescription) const
{
    QJsonObject schema;
    schema.insert("type", "array");
    schema.insert("description", strDescription);
    schema.insert("items", QJsonObject
    {
        {"type", strItemType}
    });
    return schema;
}

QJsonObject MCPToolOutputSchema::createArrayFieldSchemaWithSize(const QString& strItemType, const QString& strDescription, int nMinItems, int nMaxItems) const
{
    QJsonObject schema;
    schema.insert("type", "array");
    schema.insert("description", strDescription);
    schema.insert("items", QJsonObject
    {
        {"type", strItemType}
    });
    if (nMinItems >= 0)
    {
        schema.insert("minItems", nMinItems);
    }
    if (nMaxItems >= 0)
    {
        schema.insert("maxItems", nMaxItems);
    }
    return schema;
}
