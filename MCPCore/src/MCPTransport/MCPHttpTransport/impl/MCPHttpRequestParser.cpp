/**
 * @file MCPHttpRequestParser.cpp
 * @brief MCP HTTP请求解析器实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPHttpRequestParser.h"
#include "MCPLog.h"
#include "llhttp.h"
#include <QUrl>
#include <QSharedPointer>
MCPHttpRequestParser::MCPHttpRequestParser(QObject* parent)
    : QObject(parent)
{
    m_pParser = new llhttp_t();
    m_pSettings = new llhttp_settings_t();
    resetParser();
}

MCPHttpRequestParser::~MCPHttpRequestParser()
{
}

bool MCPHttpRequestParser::appendData(const QByteArray& data)
{
	m_byteRawData.append(data);
	if (data.size() == 0)
    {
        return true;
    }

    // 执行解析
    auto xError = llhttp_execute(m_pParser, data.constData(), data.size());
    if (xError == HPE_OK)
    {
        return true;
    }
    MCP_TRANSPORT_LOG_WARNING()<<"llhttp解析错误:"<<llhttp_errno_name(xError)<<" " << llhttp_get_error_reason(m_pParser);
    QByteArray newData = QByteArray(llhttp_get_error_pos(m_pParser));
    //解析错误，重置
    resetParser();
	if (newData.size() == 0)
	{
		return false;
	}
    return appendData(newData);
}


// 静态回调函数实现
int MCPHttpRequestParser::onMessageBegin(llhttp_t* parser)
{
    MCPHttpRequestParser* pInstance = static_cast<MCPHttpRequestParser*>(parser->data);
    pInstance->m_pRequestData = QSharedPointer<MCPHttpRequestData>::create();
    return 0;
}

int MCPHttpRequestParser::onUrl(llhttp_t* parser, const char* data, size_t length)
{
    MCPHttpRequestParser* pInstance = static_cast<MCPHttpRequestParser*>(parser->data);
	QString urlPart = QString::fromUtf8(data, length);
    pInstance->m_pRequestData->m_strUrl.append(urlPart);
    return 0;
}

int MCPHttpRequestParser::onHeaderField(llhttp_t* parser, const char* data, size_t length)
{
    MCPHttpRequestParser* pInstance = static_cast<MCPHttpRequestParser*>(parser->data);
    pInstance->m_pRequestData->m_lstHeaderNames << QString::fromUtf8(data, length);
    return 0;
}

int MCPHttpRequestParser::onHeaderValue(llhttp_t* parser, const char* data, size_t length)
{
    MCPHttpRequestParser* pInstance = static_cast<MCPHttpRequestParser*>(parser->data);
    pInstance->m_pRequestData->m_dictHeaders[pInstance->m_pRequestData->m_lstHeaderNames.back().toLower()].append(QString::fromUtf8(data, length));
    return 0;
}

int MCPHttpRequestParser::onHeadersComplete(llhttp_t* parser)
{
    MCPHttpRequestParser* pInstance = static_cast<MCPHttpRequestParser*>(parser->data);
    auto pRequestData = pInstance->m_pRequestData;
	// 解析HTTP方法
    pRequestData->m_strMethod = QString::fromUtf8(llhttp_method_name(static_cast<llhttp_method_t>(parser->method)));
	// 解析HTTP版本
    pRequestData->m_strHttpVersion = QString("HTTP/%1.%2").arg(parser->http_major).arg(parser->http_minor);
    // 保存最后一个头部
    // 解析URL
    // 分离路径和查询字符串
	pRequestData->m_strPath = pRequestData->m_strUrl;
	int queryPos = pRequestData->m_strUrl.indexOf('?');
    if (queryPos != -1)
    {
		// 解析查询参数
		pRequestData->m_strPath = pRequestData->m_strUrl.left(queryPos);
        auto strQueryString = pRequestData->m_strUrl.mid(queryPos + 1);
        for (const QString& strPair : strQueryString.split('&'))
        {
            int nEqualPos = strPair.indexOf('=');
            if (nEqualPos != -1)
            {
                QString strKey = strPair.left(nEqualPos);
                QString strValue = strPair.mid(nEqualPos + 1);
                pRequestData->m_dictQueryParams[strKey] = QUrl::fromPercentEncoding(strValue.toUtf8());
            }
        }
    }
    return 0;
}

int MCPHttpRequestParser::onBody(llhttp_t* parser, const char* data, size_t length)
{
    MCPHttpRequestParser* pInstance = static_cast<MCPHttpRequestParser*>(parser->data);
    pInstance->m_pRequestData->m_byteBody.append(data, length);
    return 0;
}

int MCPHttpRequestParser::onMessageComplete(llhttp_t* parser)
{
    MCPHttpRequestParser* pInstance = static_cast<MCPHttpRequestParser*>(parser->data);
    MCP_TRANSPORT_LOG_INFO() << "收到HTTP请求，方法:" << pInstance->m_pRequestData->getMethod()
        << ", URL:" << pInstance->m_pRequestData->getUrl()
        << ", 大小:" << pInstance->m_byteRawData.size();

    // 记录详细的HTTP请求内容
    MCP_TRANSPORT_LOG_DEBUG().noquote() << "HTTP请求详情:\n" << pInstance->m_byteRawData;

    emit pInstance->httpRequestReceived(pInstance->m_byteRawData, pInstance->m_pRequestData);
    return 0;
}

bool MCPHttpRequestParser::resetParser()
{
    //
	llhttp_settings_init(m_pSettings);
	// 设置回调函数
	m_pSettings->on_message_begin = &MCPHttpRequestParser::onMessageBegin;
	m_pSettings->on_url = &MCPHttpRequestParser::onUrl;
	m_pSettings->on_header_field = &MCPHttpRequestParser::onHeaderField;
	m_pSettings->on_header_value = &MCPHttpRequestParser::onHeaderValue;
	m_pSettings->on_headers_complete = &MCPHttpRequestParser::onHeadersComplete;
	m_pSettings->on_body = &MCPHttpRequestParser::onBody;
	m_pSettings->on_message_complete = &MCPHttpRequestParser::onMessageComplete;

	// 
    llhttp_init(m_pParser, HTTP_REQUEST, m_pSettings);
	m_pParser->data = this;
	m_byteRawData = QByteArray();
    m_pRequestData = QSharedPointer<MCPHttpRequestData>::create();
    return true;
}
