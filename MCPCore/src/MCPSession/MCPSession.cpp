/**
 * @file MCPSession.cpp
 * @brief MCP会话实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MCPSession.h"
#include "MCPPendingNotification.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
MCPSession::MCPSession(QObject* parent)
	: QObject(parent)
	, m_nSseConnectId(0)
	, m_nConnectionId(0)
	, m_enStatus(EnumSessionStatus::enConnect)
	, m_bIsStreamableTransport(false)
{
	m_strSessionId = QUuid::createUuid().toString().remove('{').remove('}');
}

MCPSession::~MCPSession()
{
}

QString MCPSession::getSessionId() const
{
	return m_strSessionId;
}

EnumSessionStatus MCPSession::getSessionStatus() const
{
	return m_enStatus;
}

quint64 MCPSession::setSseConnectionId(quint64 nConnectId)
{
	auto nPrevId = m_nSseConnectId;
	m_nSseConnectId = nConnectId;
	return nPrevId;
}

quint64 MCPSession::getSseConnectionId() const
{
	return m_nSseConnectId;
}

QString MCPSession::setProtocolVersion(const QString& strProtocolVersion)
{
	auto strPrevVersion = m_strProtocolVersion;
	m_strProtocolVersion = strProtocolVersion;
	return strPrevVersion;
}

QString MCPSession::getProtocolVersion() const
{
	return m_strProtocolVersion;
}

EnumSessionStatus MCPSession::setStatus(EnumSessionStatus enStatus)
{
	auto enPrevStatus = m_enStatus;
	m_enStatus = enStatus;
	return enPrevStatus;
}

void MCPSession::addPendingNotification(const MCPPendingNotification& notification)
{
	// 检查是否已存在相同的通知（避免重复）
	for (const auto& existing : m_pendingNotifications)
	{
		if (existing == notification)
		{
			return;  // 已存在，不重复添加
		}
	}
	m_pendingNotifications.append(notification);
}

void MCPSession::addResourceChangedNotification(const QString& strUri)
{
	addPendingNotification(MCPPendingNotification(MCPPendingNotificationType::ResourceChanged, strUri));
}

void MCPSession::addResourcesListChangedNotification()
{
	addPendingNotification(MCPPendingNotification(MCPPendingNotificationType::ResourcesListChanged));
}

void MCPSession::addToolsListChangedNotification()
{
	addPendingNotification(MCPPendingNotification(MCPPendingNotificationType::ToolsListChanged));
}

void MCPSession::addPromptsListChangedNotification()
{
	addPendingNotification(MCPPendingNotification(MCPPendingNotificationType::PromptsListChanged));
}

QList<MCPPendingNotification> MCPSession::takePendingNotifications()
{
	QList<MCPPendingNotification> notifications = m_pendingNotifications;
	m_pendingNotifications.clear();
	return notifications;
}

bool MCPSession::hasPendingNotifications() const
{
	return !m_pendingNotifications.isEmpty();
}

void MCPSession::setTransportType(bool bIsStreamable)
{
	m_bIsStreamableTransport = bIsStreamable;
}

bool MCPSession::isStreamableTransport() const
{
	return m_bIsStreamableTransport;
}

void MCPSession::setConnectionId(quint64 nConnectionId)
{
	m_nConnectionId = nConnectionId;
}

quint64 MCPSession::getConnectionId() const
{
	return m_nConnectionId;
}

