#pragma once
/**
 * @brief 示例处理器，展示如何使用 MCPServerHandler
 */
#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
class MyExampleHandler : public QObject
{
    Q_OBJECT
public:
    explicit MyExampleHandler(QObject* parent = nullptr);

public slots:
	QJsonObject calculateOperation(double a, double b, const QString& operation);
};
