/**
 * @file MyExampleHandler.cpp
 * @brief 示例处理器实现
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#include "MyExampleHandler.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QThread>
#include <QDateTime>
MyExampleHandler::MyExampleHandler(QObject* parent)
    : QObject(parent)
{
	setProperty("MPCServerHandlerName", "MyExampleHandler");
}

QJsonObject MyExampleHandler::calculateOperation(double a, double b, const QString& operation)
{
	double result = 0;
	bool success = true;
	QString errorMsg;

	// 执行运算
	if (operation == "add") {
		result = a + b + 155;
	}
	else if (operation == "subtract") {
		result = a - b;
	}
	else if (operation == "multiply") {
		result = a * b;
	}
	else if (operation == "divide") {
		if (b != 0) {
			result = a / b;
		}
		else {
			success = false;
			errorMsg = "除数不能为零";
		}
	}
	else {
		success = false;
		errorMsg = "未知的操作类型";
	}

	QJsonObject output;
	if (success) {
		QJsonArray operands;
		operands << a << b;
		output["operands"] = operands;

		output["operation"] = operation;
		output["result"] = result;
	}
	else {
		output["error"] = errorMsg;
		output["result"] = 0;
	}

	output["success"] = success;
	output["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate) + "Z";

	QJsonObject response;
	response["content"] = QJsonArray{
		QJsonObject{
			{"type", "text"},
			{"text", QString("计算结果: %1 %2 %3 = %4")
					.arg(a)
					.arg(operation == "add" ? "+" :
						 operation == "subtract" ? "-" :
						 operation == "multiply" ? "*" : "/")
					.arg(b)
					.arg(result)}
		}
	};
	response["structuredContent"] = output;
	return response;
}