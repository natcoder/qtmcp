/**
 * @file HttpRequestData.cpp
 * @brief HTTP请求数据实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPHttpRequestData.h"
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

QString MCPHttpRequestData::getMethod() const
{
    return m_strMethod;
}

QString MCPHttpRequestData::getUrl() const
{
    return m_strUrl;
}

QString MCPHttpRequestData::getPath() const
{
    return m_strPath;
}

QString MCPHttpRequestData::getHttpVersion() const
{
    return m_strHttpVersion;
}

QString MCPHttpRequestData::getHeader(const QString& key) const
{
    return m_dictHeaders.value(key.toLower(), QString());
}

QString MCPHttpRequestData::getQueryParameter(const QString& key) const
{
    return m_dictQueryParams.value(key, QString());
}

QByteArray MCPHttpRequestData::getBody() const
{
    return m_byteBody;
}

QByteArray MCPHttpRequestData::rebuildRawRequestData() const
{
    QString result;

    // 构建请求路径（包含查询参数）
    QString requestPath = m_strPath;
    if (!m_dictQueryParams.isEmpty())
    {
        requestPath += "?";
        QStringList paramList;
        for (auto it = m_dictQueryParams.begin(); it != m_dictQueryParams.end(); ++it)
        {
            // 使用QUrl进行正确的URL编码
            QByteArray encodedKey = QUrl::toPercentEncoding(it.key());
            QByteArray encodedValue = QUrl::toPercentEncoding(it.value());
            paramList.append(QString::fromUtf8(encodedKey) + "=" + QString::fromUtf8(encodedValue));
        }
        requestPath += paramList.join("&");
    }

    // 构建请求行：METHOD PATH HTTP/VERSION\r\n
    result += m_strMethod + " " + requestPath + " " + m_strHttpVersion + "\r\n";

    // 添加所有请求头
    for (auto it = m_dictHeaders.begin(); it != m_dictHeaders.end(); ++it)
    {
        result += it.key() + ": " + it.value() + "\r\n";
    }

    // 添加空行分隔头和体
    result += "\r\n";

    // 添加请求体（如果有的话）
    if (!m_byteBody.isEmpty())
    {
        result += QString::fromUtf8(m_byteBody);
    }

    return result.toUtf8();
}