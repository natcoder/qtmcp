#pragma once
#include <QObject>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QMap>
#include <memory.h>
#include "MCPHttpRequestData.h"
// 前向声明http_parser结构体

class MCPServerRequest;
class MCPHttpRequestData;
struct llhttp__internal_s;
using llhttp_t = llhttp__internal_s;
struct llhttp_settings_s;
using llhttp_settings_t = llhttp_settings_s;
class MCPHttpRequestParser : public QObject
{
    Q_OBJECT

    // 解析状态枚举
public:
    explicit MCPHttpRequestParser(QObject* parent = nullptr);
    ~MCPHttpRequestParser();
signals:
	void httpRequestReceived(QByteArray data, QSharedPointer<MCPHttpRequestData> pRequestData);
public:
    bool appendData(const QByteArray& data);
private:
    // HTTP解析器回调函数
    static int onMessageBegin(llhttp_t* parser);
    static int onUrl(llhttp_t* parser, const char* data, size_t length);
    static int onHeaderField(llhttp_t* parser, const char* data, size_t length);
    static int onHeaderValue(llhttp_t* parser, const char* data, size_t length);
    static int onHeadersComplete(llhttp_t* parser);
    static int onBody(llhttp_t* parser, const char* data, size_t length);
    static int onMessageComplete(llhttp_t* parser);
private:
    bool resetParser();
private:
    QByteArray m_byteRawData;
    QSharedPointer<MCPHttpRequestData> m_pRequestData;
private:

    llhttp_t* m_pParser;
    llhttp_settings_t* m_pSettings;
};
