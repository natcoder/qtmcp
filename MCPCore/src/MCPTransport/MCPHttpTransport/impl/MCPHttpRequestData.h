#pragma once
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QMap>
#include <QSharedPointer> 
class MCPHttpRequestData
{
public:
    QString getMethod() const;
    QString getUrl() const;
    QString getPath() const;
    QString getHttpVersion() const;
    QString getHeader(const QString& key) const;
    QString getQueryParameter(const QString& key) const;
    QByteArray getBody() const;
    QByteArray rebuildRawRequestData() const;
protected:
    // 重置请求
    QString m_strMethod;
    QString m_strUrl;
    QString m_strPath;
    QString m_strHttpVersion;
    QStringList m_lstHeaderNames;
    QMap<QString, QString> m_dictHeaders;
    QMap<QString, QString> m_dictQueryParams;
    QByteArray m_byteBody;
private:
    friend class MCPHttpRequestParser;
};
Q_DECLARE_METATYPE(MCPHttpRequestData*)
Q_DECLARE_METATYPE(QSharedPointer<MCPHttpRequestData>)
