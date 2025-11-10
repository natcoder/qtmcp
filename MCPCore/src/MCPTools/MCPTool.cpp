/**
 * @file MCPTool.cpp
 * @brief MCP工具实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPTool.h"
#include "MCPLog/MCPLog.h"
#include "Utils/MCPMethodHelper.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include "nlohmann/json.hpp"
#include "nlohmann/json-schema.hpp"
MCPTool::MCPTool(const QString& strName, QObject* pParent)
    : QObject(pParent)
    , m_strName(strName)
    , m_pExecHandler(nullptr)
    , m_execFun(nullptr)
    , m_audience(QJsonArray())
    , m_priority(0.5)  // 默认优先级为 0.5
    , m_strLastModified("")
{
    m_strExecMethodName = m_strName;
	m_strTitle = QString("Tool: %1").arg(m_strName);
	m_strDescription = QString("Tool: %1").arg(m_strName);
	
	// 初始化默认的JSON Schema
	m_jsonInputSchema = QJsonObject
	{
	    {"type", "object"},
	    {"properties", QJsonObject{}},
	    {"required", QJsonArray{}}
	};
	
	m_jsonOutputSchema = QJsonObject
	{
	    {"type", "object"},
	    {"properties", QJsonObject{}},
	    {"required", QJsonArray{}}
	};
}

MCPTool::~MCPTool()
{
}

MCPTool* MCPTool::withTitle(const QString& strTitle)
{
    m_strTitle = strTitle;
    return this;
}

MCPTool* MCPTool::withDescription(const QString& strDescription)
{
    m_strDescription = strDescription;
    return this;
}

MCPTool* MCPTool::withInputSchema(const QJsonObject& jsonInputSchema)
{
    m_jsonInputSchema = jsonInputSchema;
    return this;
}

MCPTool* MCPTool::withOutputSchema(const QJsonObject& jsonOutputSchema)
{
    m_jsonOutputSchema = jsonOutputSchema;
    return this;
}

MCPTool* MCPTool::withAnnotations(const QJsonObject& annotations)
{
    if (annotations.contains("audience"))
    {
        QJsonValue audienceValue = annotations["audience"];
        if (audienceValue.isArray())
        {
            m_audience = audienceValue.toArray();
        }
    }
    
    if (annotations.contains("priority"))
    {
        QJsonValue priorityValue = annotations["priority"];
        if (priorityValue.isDouble())
        {
            double priority = priorityValue.toDouble();
            // 限制优先级范围在 0.0 到 1.0 之间
            if (priority < 0.0)
            {
                m_priority = 0.0;
            }
            else if (priority > 1.0)
            {
                m_priority = 1.0;
            }
            else
            {
                m_priority = priority;
            }
        }
    }
    
    if (annotations.contains("lastModified"))
    {
        m_strLastModified = annotations["lastModified"].toString();
    }
    
    return this;
}

QJsonObject MCPTool::getAnnotations() const
{
    QJsonObject annotations;
    
    if (!m_audience.isEmpty())
    {
        annotations["audience"] = m_audience;
    }
    
    if (m_priority >= 0.0 && m_priority <= 1.0)
    {
        annotations["priority"] = m_priority;
    }
    
    if (!m_strLastModified.isEmpty())
    {
        annotations["lastModified"] = m_strLastModified;
    }
    
    return annotations;
}

MCPTool* MCPTool::withAudience(const QJsonArray& audience)
{
    m_audience = audience;
    return this;
}

MCPTool* MCPTool::withPriority(double priority)
{
    // 限制优先级范围在 0.0 到 1.0 之间
    if (priority < 0.0)
    {
        m_priority = 0.0;
    }
    else if (priority > 1.0)
    {
        m_priority = 1.0;
    }
    else
    {
        m_priority = priority;
    }
    return this;
}

MCPTool* MCPTool::withLastModified(const QString& lastModified)
{
    m_strLastModified = lastModified;
    return this;
}

MCPTool* MCPTool::updateLastModified()
{
    // 使用 ISO 8601 格式：YYYY-MM-DDTHH:mm:ssZ
    QDateTime now = QDateTime::currentDateTimeUtc();
    m_strLastModified = now.toString(Qt::ISODate);
    return this;
}

void MCPTool::onHandlerDestroyed()
{
	MCP_TOOLS_LOG_WARNING() << "MCPTool: Handler已销毁，工具将被注销:" << m_strName;
	emit handlerDestroyed(m_strName);
};

MCPTool* MCPTool::withExecHandler(QObject* pExecHandler, const QString& strMethodName)
{
    if (m_pExecHandler = pExecHandler)
    {
		initSchemaValidator();
		m_strExecMethodName = strMethodName.isEmpty() ? m_strExecMethodName : strMethodName;
		// 监听Handler的销毁信号，当Handler被销毁时通知ToolService
		QObject::connect(pExecHandler, &QObject::destroyed, this, &MCPTool::onHandlerDestroyed);
    }
    
    return this;
}

MCPTool* MCPTool::withExecFun(std::function<QJsonObject()> execFun)
{
    m_execFun = execFun;
	initSchemaValidator();
    return this;
}


void MCPTool::initSchemaValidator()
{
	static auto _initValidator = [](QObject* pThis, const QJsonObject& schemaObject, const char* szValidatorName)
		{
			QScopedPointer< nlohmann::json_schema::json_validator> pValidator(new nlohmann::json_schema::json_validator());
			try
			{
				auto schemaData = QJsonDocument(schemaObject).toJson();
				pValidator->set_root_schema(nlohmann::json::parse(schemaData.constData()));
				pThis->setProperty(szValidatorName, QVariant::fromValue<void*>(pValidator.take()));
			}
			catch (const std::exception& e)
			{
				MCP_TOOLS_LOG_WARNING() << szValidatorName << " 初始化错误: " << e.what();
			}
		};
	
	// 直接使用JSON Schema
	_initValidator(this, m_jsonInputSchema, "InputValidator");
	_initValidator(this, m_jsonOutputSchema, "OutputValidator");
}

bool MCPTool::validateInput(const QJsonObject& inputObject)
{
	if (auto pValidator = (nlohmann::json_schema::json_validator*)property("InputValidator").value<void*>())
	{
		try
		{
			auto json = pValidator->validate(nlohmann::json::parse(QJsonDocument(inputObject).toJson().constData()));
			return true;
		}
		catch (const std::exception& e)
		{
			MCP_TOOLS_LOG_WARNING() << "输入验证失败: " << e.what();
			return false;
		}
	}
	return false;
}


bool MCPTool::validateOutput(const QJsonObject& outputObject)
{
	if (!outputObject.contains("content"))
	{
		MCP_TOOLS_LOG_WARNING() << "输出必须包含'content'字段";
		return false;
	}
	if (!outputObject["content"].isArray())
	{
		MCP_TOOLS_LOG_WARNING() << "输出'content'字段必须是数组";
		return false;
	}
	if (!outputObject.contains("structuredContent"))
	{
		MCP_TOOLS_LOG_WARNING() << "输出必须包含'structuredContent'字段";
		return false;
	}
	if (auto pValidator = (nlohmann::json_schema::json_validator*)property("OutputValidator").value<void*>())
	{
		try
		{
			pValidator->validate(nlohmann::json::parse(QJsonDocument(outputObject.value("structuredContent").toObject()).toJson().constData()));
			return true;
		}
		catch (const std::exception& e)
		{
			MCP_TOOLS_LOG_WARNING() << "输出验证失败: " << e.what();
			return false;
		}
	}
	return true;
}

QString MCPTool::getName() const
{
    return m_strName;
}


QJsonObject MCPTool::execute(const QJsonObject& jsonCallArguments)
{
	validateInput(jsonCallArguments);
	QJsonObject jsonObject =
		(m_pExecHandler != nullptr)
		? MCPMethodHelper::syncCallMethod(m_pExecHandler, m_strExecMethodName, jsonCallArguments.toVariantMap()).toJsonObject()
		: (m_execFun != nullptr) ? m_execFun()
		: QJsonObject();
	validateOutput(jsonObject);
	//
	return jsonObject;
}


QJsonObject MCPTool::getSchema() const
{
    QJsonObject json;
    json["name"] = m_strName;
	json["title"] = m_strTitle;
	json["description"] = m_strDescription;
    json["inputSchema"] = m_jsonInputSchema;
    json["outputSchema"] = m_jsonOutputSchema;
    
    // 添加 annotations（如果存在）
    QJsonObject annotations = getAnnotations();
    if (!annotations.isEmpty())
    {
        json["annotations"] = annotations;
    }
    
    return json;
}

QString MCPTool::toString() const
{
    QJsonDocument doc(getSchema());
    return QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
}