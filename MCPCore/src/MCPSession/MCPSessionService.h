#pragma once
#include <QObject>
#include <QMap>
#include <QList>
#include <QDateTime>
#include <QString>
#include "MCPMessage.h"
#include "MCPSession.h"
#include "MCPClientMessage.h"

/**
 * @brief MCP 会话服务
 * 
 * 职责：
 * - Session创建和验证
 * - Session状态维护
 * - 过期清理
 */
class MCPSessionService : public QObject
{
    Q_OBJECT

public:
    explicit MCPSessionService(QObject* pParent = nullptr);
    virtual ~MCPSessionService();
public:
    void removeSessionBySSEConnectId(quint64 nConnectionId);
public:
    QSharedPointer<MCPSession> getSession(quint64 nConnectionId, const QSharedPointer<MCPClientMessage> pClientMessage);
    
    /**
     * @brief 获取所有活跃的连接ID
     * @return 连接ID列表
     */
    QList<quint64> getAllActiveConnectionIds() const;
    
    /**
     * @brief 根据会话ID获取会话
     * @param strSessionId 会话ID
     * @return 会话指针，如果不存在则返回nullptr
     */
    QSharedPointer<MCPSession> getSessionBySessionId(const QString& strSessionId) const;
    
    /**
     * @brief 根据连接ID获取会话（用于StreamableTransport）
     * @param nConnectionId 连接ID
     * @return 会话指针，如果不存在则返回nullptr
     */
    QSharedPointer<MCPSession> getSessionByConnectionId(quint64 nConnectionId) const;
    
    /**
     * @brief 获取所有会话（包括SSE和StreamableTransport）
     * @return 会话列表
     */
    QList<QSharedPointer<MCPSession>> getAllSessions() const;

private:
    QMap<QString, QSharedPointer<MCPSession>> m_dictSessions;
};
