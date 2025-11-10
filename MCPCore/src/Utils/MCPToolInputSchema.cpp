/**
 * @file MCPToolInputSchema.cpp
 * @brief MCP工具输入模式实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPToolInputSchema.h"
#include <QJsonArray>
#include <QJsonDocument>

MCPToolInputSchema::MCPToolInputSchema()
{
    // 直接在构造函数中初始化基本结构
    m_schema.insert("type", "object");
    m_schema.insert("properties", QJsonObject());
}


// 基本类型字段
MCPToolInputSchema* MCPToolInputSchema::addStringField(const QString& strFieldName, const QString& strDescription, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createFieldSchema("string", strDescription));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

MCPToolInputSchema* MCPToolInputSchema::addStringFieldWithLength(const QString& strFieldName, const QString& strDescription, int nMinLength, int nMaxLength, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createStringFieldSchemaWithLength(strDescription, nMinLength, nMaxLength));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

MCPToolInputSchema* MCPToolInputSchema::addStringFieldWithFormat(const QString& strFieldName, const QString& strDescription, const QString& strFormat, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();

    QJsonObject fieldSchema;
    fieldSchema.insert("type", "string");
    fieldSchema.insert("description", strDescription);
    fieldSchema.insert("format", strFormat);

    properties.insert(strFieldName, fieldSchema);
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }

    return this;
}

MCPToolInputSchema* MCPToolInputSchema::addStringFieldWithDefault(const QString& strFieldName, const QString& strDescription, const QString& strDefaultValue, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createFieldSchemaWithDefault("string", strDescription, strDefaultValue));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

MCPToolInputSchema* MCPToolInputSchema::addNumberField(const QString& strFieldName, const QString& strDescription, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createFieldSchema("number", strDescription));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

MCPToolInputSchema* MCPToolInputSchema::addNumberFieldWithRange(const QString& strFieldName, const QString& strDescription, double dMinimum, double dMaximum, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createNumberFieldSchemaWithRange(strDescription, dMinimum, dMaximum));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

MCPToolInputSchema* MCPToolInputSchema::addNumberFieldWithDefault(const QString& strFieldName, const QString& strDescription, double dDefaultValue, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createFieldSchemaWithDefault("number", strDescription, dDefaultValue));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

MCPToolInputSchema* MCPToolInputSchema::addBooleanField(const QString& strFieldName, const QString& strDescription, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createFieldSchema("boolean", strDescription));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

MCPToolInputSchema* MCPToolInputSchema::addBooleanFieldWithDefault(const QString& strFieldName, const QString& strDescription, bool bDefaultValue, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createFieldSchemaWithDefault("boolean", strDescription, bDefaultValue));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

MCPToolInputSchema* MCPToolInputSchema::addIntegerField(const QString& strFieldName, const QString& strDescription, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createFieldSchema("integer", strDescription));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

MCPToolInputSchema* MCPToolInputSchema::addIntegerFieldWithRange(const QString& strFieldName, const QString& strDescription, int nMinimum, int nMaximum, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createIntegerFieldSchemaWithRange(strDescription, nMinimum, nMaximum));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

MCPToolInputSchema* MCPToolInputSchema::addIntegerFieldWithDefault(const QString& strFieldName, const QString& strDescription, int nDefaultValue, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createFieldSchemaWithDefault("integer", strDescription, nDefaultValue));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

// 枚举类型字段
MCPToolInputSchema* MCPToolInputSchema::addEnumField(const QString& strFieldName, const QString& strDescription, const QStringList& strEnumValues, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();

    QJsonObject fieldSchema;
    fieldSchema.insert("type", "string");
    fieldSchema.insert("description", strDescription);

    QJsonArray enumArray;
    for (const QString& strValue : strEnumValues)
    {
        enumArray.append(strValue);
    }
    fieldSchema.insert("enum", enumArray);

    properties.insert(strFieldName, fieldSchema);
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

// 数组类型字段
MCPToolInputSchema* MCPToolInputSchema::addStringArrayField(const QString& strFieldName, const QString& strDescription, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createArrayFieldSchema("string", strDescription));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

MCPToolInputSchema* MCPToolInputSchema::addStringArrayFieldWithSize(const QString& strFieldName, const QString& strDescription, int nMinItems, int nMaxItems, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createArrayFieldSchemaWithSize("string", strDescription, nMinItems, nMaxItems));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

MCPToolInputSchema* MCPToolInputSchema::addNumberArrayField(const QString& strFieldName, const QString& strDescription, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createArrayFieldSchema("number", strDescription));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

MCPToolInputSchema* MCPToolInputSchema::addNumberArrayFieldWithSize(const QString& strFieldName, const QString& strDescription, int nMinItems, int nMaxItems, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createArrayFieldSchemaWithSize("number", strDescription, nMinItems, nMaxItems));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

MCPToolInputSchema* MCPToolInputSchema::addBooleanArrayField(const QString& strFieldName, const QString& strDescription, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createArrayFieldSchema("boolean", strDescription));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

MCPToolInputSchema* MCPToolInputSchema::addBooleanArrayFieldWithSize(const QString& strFieldName, const QString& strDescription, int nMinItems, int nMaxItems, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createArrayFieldSchemaWithSize("boolean", strDescription, nMinItems, nMaxItems));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

MCPToolInputSchema* MCPToolInputSchema::addArrayField(const QString& strFieldName, const QString& strDescription, const QString& strItemType, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createArrayFieldSchema(strItemType, strDescription));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

MCPToolInputSchema* MCPToolInputSchema::addArrayFieldWithSize(const QString& strFieldName, const QString& strDescription, const QString& strItemType, int nMinItems, int nMaxItems, bool bRequired)
{
    QJsonObject properties = m_schema.value("properties").toObject();
    properties.insert(strFieldName, createArrayFieldSchemaWithSize(strItemType, strDescription, nMinItems, nMaxItems));
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

// 对象类型字段（嵌套）
MCPToolInputSchema* MCPToolInputSchema::addObjectField(const QString& strFieldName, const QString& strDescription, MCPToolInputSchema* pObjectSchema, bool bRequired)
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

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

// 对象数组类型字段
MCPToolInputSchema* MCPToolInputSchema::addObjectArrayField(const QString& strFieldName, const QString& strDescription, MCPToolInputSchema* pItemSchema, bool bRequired)
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
        fieldSchema.insert("items", QJsonObject{{"type", "object"}, {"properties", QJsonObject()}});
    }

    properties.insert(strFieldName, fieldSchema);
    m_schema.insert("properties", properties);

    if (bRequired)
    {
        if (!m_requiredFields.contains(strFieldName))
        {
            m_requiredFields.append(strFieldName);
        }
    }
    return this;
}

// 获取最终的QJsonObject
QJsonObject MCPToolInputSchema::toJsonObject() const
{
    QJsonObject result = m_schema;
    if (!m_requiredFields.isEmpty())
    {
        QJsonArray requiredArray;
        for (const QString& strField : m_requiredFields)
        {
            requiredArray.append(strField);
        }
        result.insert("required", requiredArray);
    }
    return result;
}

QString MCPToolInputSchema::toString() const
{
    QJsonObject jsonObj = toJsonObject();
    QJsonDocument doc(jsonObj);
    QByteArray jsonBytes = doc.toJson(QJsonDocument::Indented);

    if (jsonBytes.isEmpty())
    {
        return "{}";
    }

    QString result = QString::fromUtf8(jsonBytes);
    if (result.isEmpty())
    {
        return "{}";
    }

    return result;
}

// 辅助方法
QJsonObject MCPToolInputSchema::createFieldSchema(const QString& strType, const QString& strDescription) const
{
    QJsonObject schema;
    schema.insert("type", strType);
    schema.insert("description", strDescription);
    return schema;
}

QJsonObject MCPToolInputSchema::createFieldSchemaWithDefault(const QString& strType, const QString& strDescription, const QJsonValue& defaultValue) const
{
    QJsonObject schema = createFieldSchema(strType, strDescription);
    schema.insert("default", defaultValue);
    return schema;
}

QJsonObject MCPToolInputSchema::createNumberFieldSchemaWithRange(const QString& strDescription, double dMinimum, double dMaximum) const
{
    QJsonObject schema;
    schema.insert("type", "number");
    schema.insert("description", strDescription);
    schema.insert("minimum", dMinimum);
    schema.insert("maximum", dMaximum);
    return schema;
}

QJsonObject MCPToolInputSchema::createIntegerFieldSchemaWithRange(const QString& strDescription, int nMinimum, int nMaximum) const
{
    QJsonObject schema;
    schema.insert("type", "integer");
    schema.insert("description", strDescription);
    schema.insert("minimum", nMinimum);
    schema.insert("maximum", nMaximum);
    return schema;
}

QJsonObject MCPToolInputSchema::createStringFieldSchemaWithLength(const QString& strDescription, int nMinLength, int nMaxLength) const
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

QJsonObject MCPToolInputSchema::createArrayFieldSchema(const QString& strItemType, const QString& strDescription) const
{
    QJsonObject schema;
    schema.insert("type", "array");
    schema.insert("description", strDescription);
    schema.insert("items", QJsonObject{{"type", strItemType}});
    return schema;
}

QJsonObject MCPToolInputSchema::createArrayFieldSchemaWithSize(const QString& strItemType, const QString& strDescription, int nMinItems, int nMaxItems) const
{
    QJsonObject schema;
    schema.insert("type", "array");
    schema.insert("description", strDescription);
    schema.insert("items", QJsonObject{{"type", strItemType}});
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