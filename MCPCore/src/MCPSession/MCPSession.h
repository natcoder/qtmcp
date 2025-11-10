#pragma once
#include <QObject>
#include <QString>
#include <QUuid>
#include <QDateTime>
#include <QSet>
#include <QList>
#include "MCPPendingNotification.h"
enum class EnumSessionStatus
{
	enConnect,
	enInitializing,
	enInitialized,
};
class MCPSession : public QObject
{
	Q_OBJECT
public:
	explicit MCPSession(QObject* parent = nullptr);
	~MCPSession();
public:
	// 获取会话ID
	QString getSessionId() const;
	EnumSessionStatus getSessionStatus() const;
public:
	//
	quint64 setSseConnectionId(quint64 nConnectId);
	quint64 getSseConnectionId() const;
	//
	QString setProtocolVersion(const QString& strProtocolVersion);
	QString getProtocolVersion() const;
	//
public:
	EnumSessionStatus setStatus(EnumSessionStatus enStatus);
	
	/**
	 * @brief 添加待发送的通知（用于StreamableTransport）
	 * @param notification 待发送的通知对象
	 */
	void addPendingNotification(const MCPPendingNotification& notification);
	
	/**
	 * @brief 添加资源变化通知（便捷方法）
	 * @param strUri 资源URI
	 */
	void addResourceChangedNotification(const QString& strUri);
	
	/**
	 * @brief 添加资源列表变化通知（便捷方法）
	 */
	void addResourcesListChangedNotification();
	
	/**
	 * @brief 添加工具列表变化通知（便捷方法）
	 */
	void addToolsListChangedNotification();
	
	/**
	 * @brief 添加提示词列表变化通知（便捷方法）
	 */
	void addPromptsListChangedNotification();
	
	/**
	 * @brief 获取并清空所有待发送的通知
	 * @return 待发送通知列表
	 */
	QList<MCPPendingNotification> takePendingNotifications();
	
	/**
	 * @brief 检查是否有待发送的通知
	 * @return true表示有待发送的通知
	 */
	bool hasPendingNotifications() const;
	
	/**
	 * @brief 设置传输类型
	 * @param bIsStreamable true表示StreamableTransport，false表示SSE
	 */
	void setTransportType(bool bIsStreamable);
	
	/**
	 * @brief 获取传输类型
	 * @return true表示StreamableTransport，false表示SSE
	 */
	bool isStreamableTransport() const;
	
	/**
	 * @brief 设置连接ID（用于StreamableTransport）
	 * @param nConnectionId 连接ID
	 */
	void setConnectionId(quint64 nConnectionId);
	
	/**
	 * @brief 获取连接ID（用于StreamableTransport）
	 * @return 连接ID
	 */
	quint64 getConnectionId() const;
public:
	quint64 m_nSseConnectId;
	quint64 m_nConnectionId;  // 通用连接ID（用于StreamableTransport）
	//
	QString m_strSessionId;
	//
	EnumSessionStatus m_enStatus;
	//
	QString m_strProtocolVersion;
private:
	QDateTime m_lastTime;
	QList<MCPPendingNotification> m_pendingNotifications;  // 待发送的通知列表（用于StreamableTransport）
	bool m_bIsStreamableTransport;                        // 是否为StreamableTransport
};