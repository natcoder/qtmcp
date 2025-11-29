/*
 * Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QObject>
#include <QMap>
#include <QList>
#include <QString>
#include <QSharedPointer>
#include <functional>

class MCPContext;
class MCPServerMessage;
class IMCPMiddleware;

/**
 * @brief MCP method router
 *
 * Responsibilities:
 * - Map method names to handler functions
 * - Provide route registration and unregistration APIs
 * - Dispatch incoming requests to the appropriate handler
 * - Centralized error handling
 *
 * Design patterns:
 * - Command pattern for handlers
 * - Uses std::function to register dynamic route handlers
 *
 * Coding rules:
 * - Member variables are prefixed with m_
 * - Pointer types use p prefix
 * - QString-like types use str prefix
 * - Keep braces { and } on their own line
 */
class MCPRouter : public QObject
{
    Q_OBJECT

public:
    // Route handler type
    using RouteHandler = std::function<QSharedPointer<MCPServerMessage>(const QSharedPointer<MCPContext>&)>;

public:
    explicit MCPRouter(QObject* pParent = nullptr);
    virtual ~MCPRouter();

    /**
     * @brief Register a route
     * @param strMethod Method name (e.g. "tools/list", "ping")
     * @param handler The handler function
     */
    void registerRoute(const QString& strMethod, RouteHandler handler);

    /**
     * @brief Unregister a route
     * @param strMethod Method name
     */
    void unregisterRoute(const QString& strMethod);

    /**
     * @brief Dispatch request to the appropriate handler
     * @param strMethod Method name
     * @param pContext Request context
     * @return Response message. If route not found, return an error message
     */
    QSharedPointer<MCPServerMessage> dispatch(const QString& strMethod,
                                               const QSharedPointer<MCPContext>& pContext);

    /**
     * @brief Check whether a route is registered
     * @param strMethod Method name
     * @return true if registered
     */
    bool hasRoute(const QString& strMethod) const;

    /**
     * @brief Get all registered route method names
     * @return list of method names
     */
    QStringList getRegisteredRoutes() const;

    /**
     * @brief Add middleware
     * @param pMiddleware Middleware object pointer
     *
     * Notes:
     * - Middlewares run in the order they are added
     * - Earlier-added middleware executes earlier
     */
    void use(QSharedPointer<IMCPMiddleware> pMiddleware);

    /**
     * @brief Clear all middlewares
     */
    void clearMiddlewares();

    /**
     * @brief Get middleware count
     * @return number of middlewares
     */
    int getMiddlewareCount() const;

private:
    // route map: method name -> handler
    QMap<QString, RouteHandler> m_dictRoutes;

    // middleware list (executed in add order)
    QList<QSharedPointer<IMCPMiddleware>> m_listMiddlewares;
};
