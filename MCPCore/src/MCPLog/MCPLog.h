#ifndef MCPLOG_H
#define MCPLOG_H

#include <QObject>
#include <QLoggingCategory>
#include <QFile>
#include <QTextStream>
#include <QSharedPointer>
#include <QMutex>
#include <QDateTime>

// 日志级别枚举
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Critical = 3,
    Fatal = 4
};

// 日志类别声明
Q_DECLARE_LOGGING_CATEGORY(mcpCore)
Q_DECLARE_LOGGING_CATEGORY(mcpTransport)
Q_DECLARE_LOGGING_CATEGORY(mcpTools)
Q_DECLARE_LOGGING_CATEGORY(mcpSession)
Q_DECLARE_LOGGING_CATEGORY(mcpResource)

/**
 * @brief MCP日志模块类
 * 提供模块级别的日志功能，支持文件输出和QLoggingCategory
 */
class MCPLog : public QObject
{
    Q_OBJECT

public:
    // 获取单例实例
    static MCPLog* instance();
    ~MCPLog();
public:
    // 初始化日志系统
    bool initialize(const QString& strLogFilePath = QString(),
                   LogLevel minLevel = LogLevel::Debug,
                   bool bEnableFileLogging = true);
    // 关闭日志系统
    void shutdown();

    // 设置日志级别
    void setLogLevel(LogLevel level);

    // 设置日志文件路径
    bool setLogFile(const QString& strFilePath);

    // 启用/禁用文件日志
    void setFileLoggingEnabled(bool bEnabled);

private:
    explicit MCPLog(QObject* parent = nullptr);
    // 禁用拷贝构造和赋值
    MCPLog(const MCPLog&) = delete;
    MCPLog& operator=(const MCPLog&) = delete;

    // 格式化日志消息
    QString formatMessage(QtMsgType type, const QMessageLogContext& context, const QString& strMsg);

    // 更新日志过滤规则
    void updateLogFilterRules(LogLevel level);

    // 写入日志到文件
    void writeToFile(const QString& strMessage);

    // 获取日志级别字符串
    QString logLevelToString(LogLevel level) const;
    QString msgTypeToString(QtMsgType type) const;

    // 消息处理器
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& strMsg);

    // 数据成员
    LogLevel m_minLogLevel;
    bool m_bFileLoggingEnabled;
    QSharedPointer<QFile> m_pLogFile;
    QSharedPointer<QTextStream> m_pLogStream;
    QMutex m_fileMutex;
};

// 日志宏定义 - 使用QLoggingCategory
#define MCP_LOG_DEBUG(category) qCDebug(category)
#define MCP_LOG_INFO(category) qCInfo(category)
#define MCP_LOG_WARNING(category) qCWarning(category)
#define MCP_LOG_CRITICAL(category) qCCritical(category)

// 便捷宏 - 通用类别
#define MCP_CORE_LOG_DEBUG() MCP_LOG_DEBUG(mcpCore)
#define MCP_CORE_LOG_INFO() MCP_LOG_INFO(mcpCore)
#define MCP_CORE_LOG_WARNING() MCP_LOG_WARNING(mcpCore)
#define MCP_CORE_LOG_CRITICAL() MCP_LOG_CRITICAL(mcpCore)

// 传输层日志
#define MCP_TRANSPORT_LOG_DEBUG() MCP_LOG_DEBUG(mcpTransport)
#define MCP_TRANSPORT_LOG_INFO() MCP_LOG_INFO(mcpTransport)
#define MCP_TRANSPORT_LOG_WARNING() MCP_LOG_WARNING(mcpTransport)
#define MCP_TRANSPORT_LOG_CRITICAL() MCP_LOG_CRITICAL(mcpTransport)

// 工具日志
#define MCP_TOOLS_LOG_DEBUG() MCP_LOG_DEBUG(mcpTools)
#define MCP_TOOLS_LOG_INFO() MCP_LOG_INFO(mcpTools)
#define MCP_TOOLS_LOG_WARNING() MCP_LOG_WARNING(mcpTools)
#define MCP_TOOLS_LOG_CRITICAL() MCP_LOG_CRITICAL(mcpTools)

// 会话日志
#define MCP_SESSION_LOG_DEBUG() MCP_LOG_DEBUG(mcpSession)
#define MCP_SESSION_LOG_INFO() MCP_LOG_INFO(mcpSession)
#define MCP_SESSION_LOG_WARNING() MCP_LOG_WARNING(mcpSession)
#define MCP_SESSION_LOG_CRITICAL() MCP_LOG_CRITICAL(mcpSession)

// 资源日志
#define MCP_RESOURCE_LOG_DEBUG() MCP_LOG_DEBUG(mcpResource)
#define MCP_RESOURCE_LOG_INFO() MCP_LOG_INFO(mcpResource)
#define MCP_RESOURCE_LOG_WARNING() MCP_LOG_WARNING(mcpResource)
#define MCP_RESOURCE_LOG_CRITICAL() MCP_LOG_CRITICAL(mcpResource)

#endif // MCPLOG_H
