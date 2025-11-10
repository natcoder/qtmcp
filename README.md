# MCP Server 核心库说明文档

## 项目概述

MCP Server 是一个基于 Qt/C++ 实现的 **Model Context Protocol (MCP)** 服务器框架。该框架提供了完整的 MCP 协议实现，支持通过 HTTP 传输层与 MCP 客户端进行通信，为 AI 应用提供工具、资源和提示词服务。

### 核心特性

- ✅ **完整的 MCP 协议实现**：支持 MCP 协议规范（2025-06-18版本）
- ✅ **HTTP 传输层**：基于 HTTP/1.1 协议实现，支持高并发连接
- ✅ **三大核心服务**：工具服务（Tools）、资源服务（Resources）、提示词服务（Prompts）
- ✅ **配置文件驱动**：支持 JSON 配置文件自动加载和启动
- ✅ **灵活的工具注册**：支持函数式和对象方法两种注册方式
- ✅ **中间件支持**：提供中间件机制，支持请求预处理
- ✅ **会话管理**：完整的会话生命周期管理
- ✅ **订阅通知**：支持资源、工具、提示词的变更通知

## 架构设计

### 整体架构

```
┌─────────────────────────────────────────────────────────┐
│                    应用程序层                            │
│  (MCPAutoServer / 自定义服务器实现)                      │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│                   服务器接口层                           │
│              (IMCPServer / MCPServer)                   │
└────────────────────┬────────────────────────────────────┘
                     │
        ┌────────────┼────────────┐
        │            │            │
┌───────▼──────┐ ┌──▼──────┐ ┌──▼──────────┐
│  工具服务     │ │资源服务  │ │ 提示词服务   │
│ ToolService  │ │Resource │ │ Prompt      │
│              │ │Service  │ │ Service     │
└───────┬──────┘ └──┬──────┘ └──┬──────────┘
        │            │            │
        └────────────┼────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│                   路由与调度层                           │
│  (MCPRouter / MCPRequestDispatcher / MCPContext)        │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│                   消息处理层                             │
│  (MCPMessage / MCPServerMessage / MCPMessageSender)     │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│                   传输层                                 │
│  (MCPHttpTransport / IMCPTransport)                      │
└─────────────────────────────────────────────────────────┘
```

### 核心模块说明

#### 1. 服务器层（Server Layer）

**接口定义**：`IMCPServer`
- 提供统一的服务器接口
- 管理服务器生命周期（启动、停止、运行状态）
- 提供三大服务的访问接口

**实现类**：`MCPServer`
- 服务器核心实现
- 协调各个组件的初始化
- 管理传输层和会话服务

**自动启动**：`MCPAutoServer`
- 基于配置文件自动启动服务器
- 自动检测并加载 `MCPServerConfig` 目录下的配置
- 自动绑定工具处理器

#### 2. 服务层（Service Layer）

##### 工具服务（Tool Service）

**接口**：`IMCPToolService`
**实现**：`MCPToolService`

功能：
- 工具注册和管理
- 工具调用执行
- 工具列表查询
- 工具变更通知

支持两种注册方式：
1. **函数式注册**：使用 `std::function` 注册工具处理函数
2. **对象方法注册**：绑定到 QObject 的槽函数

##### 资源服务（Resource Service）

**接口**：`IMCPResourceService`
**实现**：`MCPResourceService`

功能：
- 资源注册和管理
- 资源内容读取
- 资源列表查询
- 资源变更通知

支持两种资源类型：
1. **文件资源**：从文件系统加载
2. **内容资源**：通过函数动态生成

##### 提示词服务（Prompt Service）

**接口**：`IMCPPromptService`
**实现**：`MCPPromptService`

功能：
- 提示词注册和管理
- 提示词模板渲染（支持 `{{变量名}}` 占位符）
- 提示词列表查询
- 提示词变更通知

#### 3. 路由层（Routing Layer）

**MCPRouter**：方法路由器
- 将 MCP 方法名映射到处理函数
- 支持中间件机制
- 统一错误处理

**MCPRequestDispatcher**：请求分发器
- 解析客户端请求
- 路由到对应的处理器
- 生成响应消息

**MCPContext**：请求上下文
- 封装请求信息
- 提供会话信息访问
- 传递请求参数

#### 4. 消息层（Message Layer）

**MCPMessage**：基础消息类
- 封装 MCP 协议消息格式
- 支持请求、响应、通知三种消息类型

**MCPServerMessage**：服务器消息
- 服务器端消息封装
- 支持错误响应生成

**MCPMessageSender**：消息发送器
- 统一的消息发送接口
- 处理消息序列化
- 管理连接状态

#### 5. 传输层（Transport Layer）

**MCPHttpTransport**：HTTP 传输实现
- 基于 Qt 的 QTcpServer 实现
- 支持 HTTP/1.1 协议
- 线程池管理连接
- 支持高并发请求处理

**MCPHttpConnection**：HTTP 连接管理
- 管理单个 HTTP 连接
- 解析 HTTP 请求
- 构建 HTTP 响应

#### 6. 配置层（Config Layer）

**IMCPServerConfig**：配置接口
**MCPServerConfig**：配置实现

功能：
- 服务器配置管理（端口、名称、版本等）
- 从目录加载配置（支持 Tools、Resources、Prompts 子目录）
- 保存配置到目录

配置文件结构：
```
MCPServerConfig/
├── ServerConfig.json      # 主配置文件
├── Tools/                 # 工具配置目录
│   ├── calculator.json
│   └── ...
├── Resources/             # 资源配置目录
│   └── ...
└── Prompts/               # 提示词配置目录
    └── ...
```

## 快速开始

### 1. 基本使用

#### 方式一：使用自动启动（推荐）

```cpp
#include <QCoreApplication>
#include "IMCPServer.h"
#include "MyExampleHandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // 创建工具处理器（必须创建，MCPAutoServer 会通过 objectName 找到它）
    MyExampleHandler* pHandler = new MyExampleHandler(qApp);
    pHandler->setObjectName("MyExampleHandler");
    
    // 自动启动服务器（从 MCPServerConfig 目录加载配置）
    StartAutoMCPServer();
    
    return app.exec();
}
```

#### 方式二：手动创建和配置

```cpp
#include <QCoreApplication>
#include "IMCPServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // 创建服务器实例
    auto pServer = IMCPServer::createServer();
    
    // 配置服务器
    auto pConfig = pServer->getConfig();
    pConfig->setPort(8888);
    pConfig->setServerName("MyServer");
    
    // 注册工具
    auto pToolService = pServer->getToolService();
    QJsonObject inputSchema = {
        {"type", "object"},
        {"properties", QJsonObject{
            {"name", QJsonObject{{"type", "string"}}}
        }}
    };
    QJsonObject outputSchema = {
        {"type", "object"},
        {"properties", QJsonObject{
            {"result", QJsonObject{{"type", "string"}}}
        }}
    };
    
    pToolService->add("greet", "Greet Tool", "A greeting tool",
        inputSchema, outputSchema,
        []() -> QJsonObject {
            QJsonObject result;
            result["content"] = QJsonArray{QJsonObject{{"type", "text"}, {"text", "Hello!"}}};
            return result;
        });
    
    // 启动服务器
    pServer->start();
    
    return app.exec();
}
```

### 2. 创建工具处理器

工具处理器是一个继承自 `QObject` 的类，包含处理工具调用的槽函数：

```cpp
// MyExampleHandler.h
#pragma once
#include <QObject>
#include <QJsonObject>

class MyExampleHandler : public QObject
{
    Q_OBJECT
public:
    explicit MyExampleHandler(QObject* parent = nullptr);
    
public slots:
    // 工具处理方法：参数类型必须与 inputSchema 匹配
    QJsonObject calculateOperation(double a, double b, const QString& operation);
    
    // 返回值必须是 QJsonObject，符合 outputSchema
};
```

```cpp
// MyExampleHandler.cpp
#include "MyExampleHandler.h"

MyExampleHandler::MyExampleHandler(QObject* parent)
    : QObject(parent)
{
    // 设置属性，让 MCPAutoServer 能够找到这个处理器
    setProperty("MPCServerHandlerName", "MyExampleHandler");
}

QJsonObject MyExampleHandler::calculateOperation(double a, double b, const QString& operation)
{
    QJsonObject result;
    double value = 0;
    
    if (operation == "add") {
        value = a + b;
    } else if (operation == "subtract") {
        value = a - b;
    } else if (operation == "multiply") {
        value = a * b;
    } else if (operation == "divide") {
        value = b != 0 ? a / b : 0;
    }
    
    result["operands"] = QJsonArray{a, b};
    result["operation"] = operation;
    result["result"] = value;
    result["success"] = true;
    result["timestamp"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    
    return result;
}
```

### 3. 配置文件示例

#### ServerConfig.json

```json
{
    "port": 5555,
    "serverInfo": {
        "name": "MyServer",
        "title": "My MCP Server",
        "version": "1.0.0"
    },
    "instructions": "这是一个示例 MCP 服务器"
}
```

#### Tools/calculator.json

```json
{
    "name": "calculator",
    "title": "计算器",
    "description": "支持基本数学运算的计算器工具",
    "execHandler": "MyExampleHandler",
    "execMethod": "calculateOperation",
    "inputSchema": {
        "type": "object",
        "properties": {
            "a": {
                "type": "number",
                "description": "第一个数字"
            },
            "b": {
                "type": "number",
                "description": "第二个数字"
            },
            "operation": {
                "type": "string",
                "description": "操作类型",
                "enum": ["add", "subtract", "multiply", "divide"]
            }
        },
        "required": ["a", "b", "operation"]
    },
    "outputSchema": {
        "type": "object",
        "properties": {
            "result": {
                "type": "number",
                "description": "计算结果"
            },
            "success": {
                "type": "boolean",
                "description": "操作是否成功"
            }
        }
    }
}
```

## 配置文件详解

### 配置文件目录结构

MCP Server 使用目录结构来组织配置文件，默认配置目录为 `MCPServerConfig`：

```
MCPServerConfig/
├── ServerConfig.json      # 主配置文件（必需）
├── Tools/                 # 工具配置目录（可选）
│   ├── calculator.json
│   ├── my_tool.json
│   └── ...
├── Resources/             # 资源配置目录（可选）
│   ├── config_file.json
│   ├── wrapper_example.json
│   └── ...
└── Prompts/               # 提示词配置目录（可选）
    ├── code_review.json
    ├── generate_api_doc.json
    └── ...
```

### 1. 主配置文件（ServerConfig.json）

主配置文件定义了服务器的基本信息和运行参数。

#### 字段说明

| 字段 | 类型 | 必需 | 说明 |
|------|------|------|------|
| `port` | number | 是 | 服务器监听端口号（1-65535） |
| `serverInfo` | object | 是 | 服务器信息对象 |
| `serverInfo.name` | string | 是 | 服务器名称（标识符，建议使用小写字母和连字符） |
| `serverInfo.title` | string | 是 | 服务器显示标题 |
| `serverInfo.version` | string | 是 | 服务器版本号（遵循语义化版本规范） |
| `instructions` | string | 否 | 服务器使用说明（可选，用于向客户端描述服务器功能） |

#### 完整示例

```json
{
    "port": 5555,
    "serverInfo": {
        "name": "mcp-x-server",
        "title": "MCP X Server Example",
        "version": "1.0.0"
    },
    "instructions": "这是一个示例 MCP 服务器，提供了计算器工具、配置资源访问和代码审查提示词等功能。"
}
```

### 2. 工具配置文件（Tools/*.json）

工具配置文件定义了服务器提供的工具（Tools），每个工具对应一个 JSON 文件。

#### 字段说明

| 字段 | 类型 | 必需 | 说明 |
|------|------|------|------|
| `name` | string | 是 | 工具名称（唯一标识符，建议使用小写字母和下划线） |
| `title` | string | 是 | 工具显示标题 |
| `description` | string | 是 | 工具功能描述 |
| `execHandler` | string | 是 | 处理器对象名称（必须与代码中 QObject 的 `objectName` 或 `MPCServerHandlerName` 属性匹配） |
| `execMethod` | string | 是 | 处理方法名（处理器类中的 `public slots` 方法名） |
| `inputSchema` | object | 是 | 输入参数 JSON Schema（定义工具输入参数的结构和类型） |
| `outputSchema` | object | 是 | 输出结果 JSON Schema（定义工具返回值的结构和类型） |
| `annotations` | object | 否 | 工具注解（可选，用于扩展元数据） |
| `annotations.audience` | array | 否 | 目标受众（如 `["user", "assistant"]`） |
| `annotations.priority` | number | 否 | 优先级（0.0-1.0） |
| `annotations.lastModified` | string | 否 | 最后修改时间（ISO 8601 格式） |

#### inputSchema 和 outputSchema

这两个字段遵循 [JSON Schema](https://json-schema.org/) 规范，用于定义工具的参数和返回值结构。

**inputSchema 示例**：
```json
{
    "type": "object",
    "properties": {
        "a": {
            "type": "number",
            "description": "第一个数字"
        },
        "b": {
            "type": "number",
            "description": "第二个数字"
        },
        "operation": {
            "type": "string",
            "description": "操作类型",
            "enum": ["add", "subtract", "multiply", "divide"]
        }
    },
    "required": ["a", "b", "operation"]
}
```

**outputSchema 示例**：
```json
{
    "type": "object",
    "description": "计算器操作结果",
    "properties": {
        "operands": {
            "type": "array",
            "description": "参与运算的操作数",
            "items": {
                "type": "number"
            }
        },
        "operation": {
            "type": "string",
            "description": "执行的操作"
        },
        "result": {
            "type": "number",
            "description": "计算结果"
        },
        "success": {
            "type": "boolean",
            "description": "操作是否成功"
        },
        "timestamp": {
            "type": "string",
            "description": "操作时间戳"
        }
    },
    "required": ["operands", "operation", "result", "success", "timestamp"]
}
```

#### 工具处理器方法签名要求

工具处理方法必须满足以下要求：

1. **方法必须是 `public slots`**：使用 Qt 的元对象系统
2. **参数类型匹配**：方法参数的类型必须与 `inputSchema` 中定义的类型匹配
   - JSON Schema `number` → C++ `double` 或 `int`
   - JSON Schema `string` → C++ `QString`
   - JSON Schema `boolean` → C++ `bool`
   - JSON Schema `array` → C++ `QJsonArray`
   - JSON Schema `object` → C++ `QJsonObject`
3. **返回值类型**：必须返回 `QJsonObject`，且结构符合 `outputSchema`

**示例方法签名**：
```cpp
// 对应上面的 inputSchema
QJsonObject calculateOperation(double a, double b, const QString& operation);
```

#### 完整示例

```json
{
    "name": "calculator",
    "title": "计算器",
    "description": "支持基本数学运算的计算器工具（加、减、乘、除）",
    "execHandler": "MyExampleHandler",
    "execMethod": "calculateOperation",
    "inputSchema": {
        "type": "object",
        "properties": {
            "a": {
                "type": "number",
                "description": "第一个数字"
            },
            "b": {
                "type": "number",
                "description": "第二个数字"
            },
            "operation": {
                "type": "string",
                "description": "操作类型",
                "enum": ["add", "subtract", "multiply", "divide"]
            }
        },
        "required": ["a", "b", "operation"]
    },
    "outputSchema": {
        "type": "object",
        "description": "计算器操作结果",
        "properties": {
            "operands": {
                "type": "array",
                "items": {"type": "number"}
            },
            "operation": {"type": "string"},
            "result": {"type": "number"},
            "success": {"type": "boolean"},
            "timestamp": {"type": "string"}
        },
        "required": ["operands", "operation", "result", "success", "timestamp"]
    },
    "annotations": {
        "audience": ["user", "assistant"],
        "priority": 0.8,
        "lastModified": "2025-01-12T15:00:58Z"
    }
}
```

### 3. 资源配置文件（Resources/*.json）

资源配置文件定义了服务器提供的资源（Resources），支持三种资源类型：文件资源、包装资源和内容资源。

#### 字段说明

| 字段 | 类型 | 必需 | 说明 |
|------|------|------|------|
| `uri` | string | 是 | 资源 URI（唯一标识符，建议使用 `file://`、`wrapper://` 或自定义协议前缀） |
| `name` | string | 是 | 资源显示名称 |
| `description` | string | 是 | 资源描述 |
| `mimeType` | string | 是 | MIME 类型（如 `text/plain`、`application/json`、`image/png` 等） |
| `type` | string | 是 | 资源类型：`"file"`（文件资源）、`"wrapper"`（包装资源）、`"content"`（内容资源，默认） |
| `filePath` | string | 条件 | 文件路径（`type` 为 `"file"` 时必需，相对于应用程序目录或绝对路径） |
| `handlerName` | string | 条件 | Handler 名称（`type` 为 `"wrapper"` 时必需，必须与代码中 QObject 的 `objectName` 或 `MCPResourceHandlerName` 属性匹配） |
| `content` | string | 条件 | 静态内容（`type` 为 `"content"` 时可选，直接提供资源内容） |
| `annotations` | object | 否 | 资源注解（可选） |

#### 资源类型详解

##### 文件资源（type: "file"）

从文件系统加载资源内容，适用于静态文件。

```json
{
    "uri": "file://config",
    "name": "配置文件",
    "description": "服务器配置信息",
    "mimeType": "text/plain",
    "type": "file",
    "filePath": "MCPServerConfig/Resources/config.txt"
}
```

**注意事项**：
- `filePath` 可以是相对路径（相对于应用程序目录）或绝对路径
- 文件必须存在且可读
- 文件内容会在资源注册时加载到内存

##### 包装资源（type: "wrapper"）

通过 `MCPResourceWrapper` 包装 QObject 对象，支持动态内容生成。

```json
{
    "uri": "wrapper://example",
    "name": "包装资源示例",
    "description": "通过MCPResourceWrapper包装QObject对象，每5秒自动更新内容",
    "mimeType": "application/json",
    "type": "wrapper",
    "handlerName": "MyResourceHandler"
}
```

**注意事项**：
- `handlerName` 必须与代码中 QObject 的 `objectName` 或 `MCPResourceHandlerName` 属性匹配
- Handler 对象必须实现资源内容生成方法
- 支持动态内容更新，适合需要实时数据的场景

##### 内容资源（type: "content"）

直接在配置文件中提供资源内容，适用于小型静态内容。

```json
{
    "uri": "content://welcome",
    "name": "欢迎信息",
    "description": "服务器欢迎信息",
    "mimeType": "text/plain",
    "type": "content",
    "content": "欢迎使用 MCP Server！"
}
```

**注意事项**：
- `content` 字段可选，如果不提供，资源内容为空
- 适合小型文本内容，不适合大型文件

#### 完整示例

**文件资源示例**：
```json
{
    "uri": "file://config",
    "name": "配置文件",
    "description": "服务器配置信息",
    "mimeType": "text/plain",
    "type": "file",
    "filePath": "MCPServerConfig/Resources/config.txt",
    "annotations": {
        "audience": ["user"],
        "priority": 0.5
    }
}
```

**包装资源示例**：
```json
{
    "uri": "wrapper://system-info",
    "name": "系统信息",
    "description": "动态生成的系统信息",
    "mimeType": "application/json",
    "type": "wrapper",
    "handlerName": "MyResourceHandler"
}
```

### 4. 提示词配置文件（Prompts/*.json）

提示词配置文件定义了服务器提供的提示词模板（Prompts），支持参数化模板。

#### 字段说明

| 字段 | 类型 | 必需 | 说明 |
|------|------|------|------|
| `name` | string | 是 | 提示词名称（唯一标识符） |
| `description` | string | 是 | 提示词功能描述 |
| `arguments` | array | 是 | 参数列表（定义提示词模板的参数） |
| `arguments[].name` | string | 是 | 参数名称（模板中的变量名） |
| `arguments[].description` | string | 是 | 参数描述 |
| `arguments[].required` | boolean | 是 | 是否必需参数 |
| `template` | string | 是 | 提示词模板字符串（支持 `{{变量名}}` 格式的占位符） |

#### 模板语法

提示词模板支持使用 `{{变量名}}` 格式的占位符，在生成提示词时会被实际参数值替换。

**示例模板**：
```
请审查以下{{language}}代码，并给出改进建议：

{{code}}

请从以下方面进行审查：
1. 代码质量和可读性
2. 性能优化建议
3. 潜在的bug
4. 最佳实践
```

#### 完整示例

**代码审查提示词**：
```json
{
    "name": "code_review",
    "description": "对代码进行审查并给出改进建议",
    "arguments": [
        {
            "name": "code",
            "description": "要审查的代码",
            "required": true
        },
        {
            "name": "language",
            "description": "编程语言",
            "required": false
        }
    ],
    "template": "请审查以下{{language}}代码，并给出改进建议：\n\n{{code}}\n\n请从以下方面进行审查：\n1. 代码质量和可读性\n2. 性能优化建议\n3. 潜在的bug\n4. 最佳实践"
}
```

**API 文档生成提示词**：
```json
{
    "name": "generate_api_doc",
    "description": "为函数生成API文档",
    "arguments": [
        {
            "name": "function_name",
            "description": "函数名",
            "required": true
        },
        {
            "name": "parameters",
            "description": "参数列表",
            "required": false
        }
    ],
    "template": "请为函数 {{function_name}} 生成详细的API文档。\n\n函数名：{{function_name}}\n参数：{{parameters}}\n\n请包含：\n1. 功能描述\n2. 参数说明\n3. 返回值说明\n4. 使用示例"
}
```

### 配置文件加载机制

#### 自动加载

使用 `MCPAutoServer` 或 `StartAutoMCPServer()` 时，系统会自动：

1. **检测配置目录**：在应用程序目录下查找 `MCPServerConfig` 目录
2. **加载主配置**：读取 `ServerConfig.json`
3. **加载工具配置**：扫描 `Tools/` 目录下的所有 `.json` 文件
4. **加载资源配置**：扫描 `Resources/` 目录下的所有 `.json` 文件
5. **加载提示词配置**：扫描 `Prompts/` 目录下的所有 `.json` 文件
6. **绑定处理器**：根据 `execHandler` 和 `handlerName` 查找对应的 QObject 对象

#### 手动加载

也可以通过代码手动加载配置：

```cpp
auto pServer = IMCPServer::createServer();
auto pConfig = pServer->getConfig();

// 从目录加载配置
if (pConfig->loadFromDirectory("MCPServerConfig"))
{
    // 配置加载成功
    pServer->start();
}
```

#### 配置验证

配置文件加载时会进行基本验证：

- **JSON 格式验证**：确保文件是有效的 JSON
- **必需字段检查**：检查必需字段是否存在
- **类型验证**：验证字段类型是否符合要求
- **处理器绑定验证**：检查处理器对象是否存在

如果验证失败，会输出错误日志，但不会阻止服务器启动（其他有效配置仍会加载）。

### 配置文件最佳实践

1. **命名规范**：
   - 文件名使用小写字母、数字和下划线（如 `my_tool.json`）
   - 工具名称、资源 URI、提示词名称建议使用小写字母和下划线

2. **文件组织**：
   - 每个工具/资源/提示词使用独立的 JSON 文件
   - 文件名与配置中的 `name` 字段保持一致（便于管理）

3. **Schema 设计**：
   - 为所有参数提供清晰的 `description`
   - 使用 `enum` 限制字符串参数的取值范围
   - 明确标记 `required` 字段

4. **版本管理**：
   - 将配置文件纳入版本控制系统
   - 使用 `annotations.lastModified` 记录修改时间

5. **错误处理**：
   - 在工具处理方法中处理可能的错误情况
   - 返回符合 `outputSchema` 的错误信息

## 关键概念详解

### ⚠️ 重要：处理器对象查找机制

MCP Server 使用 **对象查找机制** 来绑定配置文件中的工具和资源到实际的处理器对象。理解这个机制对于正确使用框架至关重要。

#### 1. qApp - Qt 应用程序对象

**`qApp`** 是 Qt 框架提供的全局指针，指向 `QCoreApplication` 的实例（或 `QApplication`，如果使用 GUI 应用）。

**作用**：
- 作为 Qt 对象树的根节点
- `MCPHandlerResolver` 默认从 `qApp` 开始搜索所有子对象
- 确保处理器对象在应用程序生命周期内存在

**使用方式**：
```cpp
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // qApp 是全局指针，指向 app 对象
    // 创建处理器对象时，将 qApp 作为父对象
    MyExampleHandler* pHandler = new MyExampleHandler(qApp);
    // 或者使用 &app
    // MyExampleHandler* pHandler = new MyExampleHandler(&app);
    
    return app.exec();
}
```

**关键点**：
- ✅ **必须**将处理器对象设置为 `qApp`（或 `QCoreApplication` 实例）的子对象
- ✅ 这样 `MCPHandlerResolver` 才能通过 `qApp->findChildren<QObject*>()` 找到它们
- ❌ 如果处理器对象没有父对象，或者父对象不是 `qApp`，将无法被找到

#### 2. MPCServerHandlerName - 工具处理器属性

**`MPCServerHandlerName`** 是一个自定义的 Qt 属性，用于标识工具处理器对象。

**作用**：
- 在配置文件的 `execHandler` 字段中指定处理器名称
- `MCPHandlerResolver` 通过此属性查找对应的处理器对象
- 允许一个对象同时通过 `objectName` 和此属性被识别

**使用方式**：

**方式一：在构造函数中设置属性**
```cpp
// MyExampleHandler.cpp
MyExampleHandler::MyExampleHandler(QObject* parent)
    : QObject(parent)
{
    // 设置 MPCServerHandlerName 属性
    setProperty("MPCServerHandlerName", "MyExampleHandler");
}
```

**方式二：在 main 函数中设置**
```cpp
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    MyExampleHandler* pHandler = new MyExampleHandler(qApp);
    // 设置属性
    pHandler->setProperty("MPCServerHandlerName", "MyExampleHandler");
    // 同时设置 objectName（推荐，作为备用标识）
    pHandler->setObjectName("MyExampleHandler");
    
    StartAutoMCPServer();
    return app.exec();
}
```

**配置文件中的对应关系**：
```json
{
    "name": "calculator",
    "execHandler": "MyExampleHandler",  // ← 必须与属性值匹配
    "execMethod": "calculateOperation",
    ...
}
```

**查找优先级**：
`MCPHandlerResolver` 按以下顺序查找处理器：
1. 首先检查 `objectName` 是否匹配
2. 然后检查 `MPCServerHandlerName` 属性是否匹配
3. 如果找到匹配的对象，返回该对象

**最佳实践**：
- ✅ **同时设置** `MPCServerHandlerName` 属性和 `objectName`，确保双重保障
- ✅ 属性值应与配置文件中的 `execHandler` 完全一致（区分大小写）
- ✅ 属性值应使用有意义的名称，便于识别和管理

#### 3. MCPResourceHandlerName - 资源处理器属性

**`MCPResourceHandlerName`** 是一个自定义的 Qt 属性，用于标识资源处理器对象（用于包装资源类型）。

**作用**：
- 在配置文件的 `handlerName` 字段中指定处理器名称
- `MCPHandlerResolver` 通过此属性查找对应的资源处理器对象
- 专门用于 `type: "wrapper"` 类型的资源

**使用方式**：

**在构造函数中设置属性**
```cpp
// MyResourceHandler.cpp
MyResourceHandler::MyResourceHandler(QObject* pParent)
    : QObject(pParent)
{
    // 设置 MCPResourceHandlerName 属性
    setProperty("MCPResourceHandlerName", "MyResourceHandler");
    
    // 同时设置 objectName（推荐）
    setObjectName("MyResourceHandler");
}
```

**在 main 函数中设置**
```cpp
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    MyResourceHandler* pResourceHandler = new MyResourceHandler(qApp);
    // 设置属性
    pResourceHandler->setProperty("MCPResourceHandlerName", "MyResourceHandler");
    // 同时设置 objectName
    pResourceHandler->setObjectName("MyResourceHandler");
    
    StartAutoMCPServer();
    return app.exec();
}
```

**配置文件中的对应关系**：
```json
{
    "uri": "wrapper://example",
    "type": "wrapper",
    "handlerName": "MyResourceHandler",  // ← 必须与属性值匹配
    ...
}
```

**资源处理器的特殊要求**：
资源处理器对象必须实现以下接口：
- `getMetadata()` 方法：返回资源的元数据（`QJsonObject`）
- `getContent()` 方法：返回资源内容（`QString`）
- `changed()` 信号：当资源内容变化时发出

**示例实现**：
```cpp
class MyResourceHandler : public QObject
{
    Q_OBJECT
public:
    explicit MyResourceHandler(QObject* pParent = nullptr)
        : QObject(pParent)
    {
        setProperty("MCPResourceHandlerName", "MyResourceHandler");
        setObjectName("MyResourceHandler");
    }
    
public slots:
    QJsonObject getMetadata() const
    {
        QJsonObject metadata;
        metadata["name"] = "示例资源";
        metadata["description"] = "资源描述";
        metadata["mimeType"] = "application/json";
        return metadata;
    }
    
    QString getContent() const
    {
        return "{\"message\":\"Hello\"}";
    }
    
signals:
    void changed(const QString& name, const QString& description, const QString& mimeType);
};
```

#### 查找机制总结

`MCPHandlerResolver` 的查找流程：

```
1. 从 qApp 开始搜索所有子对象
   ↓
2. 对每个对象检查：
   ├─ objectName 是否匹配？
   ├─ MPCServerHandlerName 属性是否匹配？（工具处理器）
   └─ MCPResourceHandlerName 属性是否匹配？（资源处理器）
   ↓
3. 找到匹配的对象，返回映射表
```

**完整示例**：

```cpp
#include <QCoreApplication>
#include "MyExampleHandler.h"
#include "MyResourceHandler.h"
#include "IMCPServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // ============ 工具处理器 ============
    // 方式1：在构造函数中已设置属性
    MyExampleHandler* pHandler = new MyExampleHandler(qApp);
    // 方式2：同时设置 objectName（推荐）
    pHandler->setObjectName("MyExampleHandler");
    
    // ============ 资源处理器 ============
    // 方式1：在构造函数中已设置属性
    MyResourceHandler* pResourceHandler = new MyResourceHandler(qApp);
    // 方式2：同时设置 objectName（推荐）
    pResourceHandler->setObjectName("MyResourceHandler");
    
    // ============ 自动启动服务器 ============
    // StartAutoMCPServer() 会：
    // 1. 从 MCPServerConfig 目录加载配置
    // 2. 通过 MCPHandlerResolver 查找处理器对象
    // 3. 将配置文件中的 execHandler/handlerName 与找到的对象绑定
    StartAutoMCPServer();
    
    return app.exec();
}
```

#### 常见错误和解决方案

**错误1：处理器对象找不到**
```
错误信息：找不到处理器 "MyExampleHandler"
```

**原因**：
- 处理器对象没有设置为 `qApp` 的子对象
- 属性值或 `objectName` 与配置文件不匹配
- 处理器对象在服务器启动后才创建

**解决方案**：
```cpp
// ✅ 正确：在 StartAutoMCPServer() 之前创建
MyExampleHandler* pHandler = new MyExampleHandler(qApp);
pHandler->setObjectName("MyExampleHandler");
StartAutoMCPServer();

// ❌ 错误：在 StartAutoMCPServer() 之后创建
StartAutoMCPServer();
MyExampleHandler* pHandler = new MyExampleHandler(qApp);  // 太晚了！
```

**错误2：属性值不匹配**
```
配置文件：execHandler: "MyExampleHandler"
代码中：setProperty("MPCServerHandlerName", "MyHandler")  // 不匹配！
```

**解决方案**：
确保属性值与配置文件完全一致（区分大小写）：
```cpp
// ✅ 正确
setProperty("MPCServerHandlerName", "MyExampleHandler");

// ❌ 错误
setProperty("MPCServerHandlerName", "MyHandler");
setProperty("MPCServerHandlerName", "myexamplehandler");  // 大小写不匹配
```

**错误3：忘记设置父对象**
```cpp
// ❌ 错误：没有父对象，无法被找到
MyExampleHandler* pHandler = new MyExampleHandler();

// ✅ 正确：设置 qApp 为父对象
MyExampleHandler* pHandler = new MyExampleHandler(qApp);
```

#### 快速检查清单

在创建处理器对象时，确保：

- [ ] 处理器对象继承自 `QObject`
- [ ] 使用 `qApp`（或 `QCoreApplication` 实例）作为父对象
- [ ] 设置了 `MPCServerHandlerName` 属性（工具处理器）或 `MCPResourceHandlerName` 属性（资源处理器）
- [ ] 设置了 `objectName`（推荐，作为备用标识）
- [ ] 属性值与配置文件中的 `execHandler` 或 `handlerName` 完全匹配
- [ ] 在 `StartAutoMCPServer()` **之前**创建处理器对象
- [ ] 工具处理方法必须是 `public slots`
- [ ] 资源处理器实现了 `getMetadata()`、`getContent()` 方法和 `changed()` 信号

## API 参考

### IMCPServer 接口

#### 创建和销毁

```cpp
// 创建服务器实例
static IMCPServer* createServer();

// 销毁服务器实例（推荐使用此方法）
static void destroyServer(IMCPServer* pServer);
```

#### 服务器控制

```cpp
// 启动服务器
virtual bool start() = 0;

// 停止服务器
virtual void stop() = 0;

// 检查运行状态
virtual bool isRunning() = 0;
```

#### 服务访问

```cpp
// 获取配置对象
virtual IMCPServerConfig* getConfig() = 0;

// 获取工具服务
virtual IMCPToolService* getToolService() = 0;

// 获取资源服务
virtual IMCPResourceService* getResourceService() = 0;

// 获取提示词服务
virtual IMCPPromptService* getPromptService() = 0;
```

### IMCPToolService 接口

#### 注册工具（函数式）

```cpp
bool add(const QString& strName,
         const QString& strTitle,
         const QString& strDescription,
         const QJsonObject& jsonInputSchema,
         const QJsonObject& jsonOutputSchema,
         std::function<QJsonObject()> execFun);
```

#### 注册工具（对象方法）

```cpp
bool add(const QString& strName,
         const QString& strTitle,
         const QString& strDescription,
         const QJsonObject& jsonInputSchema,
         const QJsonObject& jsonOutputSchema,
         QObject* pHandler,
         const QString& strMethodName);
```

#### 其他方法

```cpp
// 注销工具
bool remove(const QString& strName);

// 获取工具列表
QJsonArray list() const;
```

### IMCPResourceService 接口

#### 注册资源（文件）

```cpp
bool add(const QString& strUri,
         const QString& strName,
         const QString& strDescription,
         const QString& strFilePath,
         const QString& strMimeType = QString());
```

#### 注册资源（内容提供函数）

```cpp
bool add(const QString& strUri,
         const QString& strName,
         const QString& strDescription,
         const QString& strMimeType,
         std::function<QString()> contentProvider);
```

#### 其他方法

```cpp
// 注销资源
bool remove(const QString& strUri);

// 检查资源是否存在
bool has(const QString& strUri) const;

// 获取资源列表
QJsonArray list(const QString& strUriPrefix = QString()) const;

// 读取资源内容
QJsonObject readResource(const QString& strUri);
```

### IMCPPromptService 接口

#### 注册提示词（模板）

```cpp
bool add(const QString& strName,
         const QString& strDescription,
         const QList<QPair<QString, QPair<QString, bool>>>& arguments,
         const QString& strTemplate);
```

#### 注册提示词（生成函数）

```cpp
bool add(const QString& strName,
         const QString& strDescription,
         const QList<QPair<QString, QPair<QString, bool>>>& arguments,
         std::function<QString(const QMap<QString, QString>&)> generator);
```

#### 其他方法

```cpp
// 注销提示词
bool remove(const QString& strName);

// 检查提示词是否存在
bool has(const QString& strName) const;

// 获取提示词列表
QJsonArray list() const;

// 获取提示词内容
QJsonObject getPrompt(const QString& strName, 
                      const QMap<QString, QString>& arguments);
```

## 设计模式与最佳实践

### 设计模式

1. **工厂模式**：`IMCPServer::createServer()` 用于创建服务器实例
2. **策略模式**：传输层接口 `IMCPTransport` 支持多种传输实现
3. **观察者模式**：使用 Qt 信号槽机制实现服务变更通知
4. **命令模式**：路由系统使用 `std::function` 封装处理函数
5. **中间件模式**：支持请求预处理和响应后处理

### 编码规范

- **命名规范**：
  - 类成员变量：`m_` 前缀（如 `m_pServer`）
  - 指针变量：`p` 前缀（如 `pServer`）
  - 数值变量：`n` 前缀（如 `nPort`）
  - 字符串变量：`str` 前缀（如 `strName`）
  
- **代码格式**：
  - 大括号 `{` 和 `}` 单独一行
  - 使用 Qt 的编码风格

### 最佳实践

1. **工具处理器设计**：
   - 继承自 `QObject`，使用 Qt 的元对象系统
   - 使用 `public slots` 定义处理方法
   - 方法参数类型必须与 `inputSchema` 匹配
   - 返回值必须是 `QJsonObject`，符合 `outputSchema`

2. **错误处理**：
   - 工具方法应返回包含错误信息的 JSON 对象
   - 使用 `MCPError` 类生成标准错误响应

3. **资源管理**：
   - 使用 `IMCPServer::destroyServer()` 销毁服务器实例
   - Service 对象由 Server 管理，不要直接删除

4. **线程安全**：
   - 服务器在独立线程中运行
   - 工具处理器方法可能在不同线程中调用，注意线程安全

## 依赖项

### 必需依赖

- **Qt 5.12+**：核心框架依赖
  - QtCore
  - QtNetwork
  
### 第三方库

- **nlohmann/json**：JSON 处理（已包含在 3rdparty 目录）
- **json-schema-validator**：JSON Schema 验证（已包含在 3rdparty 目录）
- **llhttp**：HTTP 解析（已包含在 3rdparty 目录）

## 构建说明

### CMake 配置

项目使用 CMake 构建系统，主要配置文件：

- `CMake/CMakeLists.txt`：主 CMake 文件
- `CMake/MCPCore/CMakeLists.txt`：MCPCore 库配置
- `CMake/Examples/MCPXServer/CMakeLists.txt`：示例服务器配置

### 构建步骤

```bash
# 创建构建目录
mkdir build && cd build

# 配置 CMake
cmake ../CMake

# 编译
cmake --build .

# 运行示例
./bin/Debug/MCPXServer
```

## 示例项目

完整的示例项目位于 `Examples/MCPXServer` 目录，包含：

- 工具处理器示例（`MyExampleHandler`）
- 资源处理器示例（`MyResourceHandler`）
- 配置文件示例（`MCPServerConfig`）

## 协议支持

当前实现支持 **MCP 协议规范（2025-06-18版本）**，包括：

- ✅ 初始化握手（Initialize）
- ✅ 工具调用（Tools）
- ✅ 资源访问（Resources）
- ✅ 提示词服务（Prompts）
- ✅ 订阅通知（Subscriptions）
- ✅ HTTP 传输层

## 扩展开发

### 添加自定义传输层

1. 实现 `IMCPTransport` 接口
2. 在 `MCPServer` 中注册传输层实现

### 添加中间件

1. 实现 `IMCPMiddleware` 接口
2. 在 `MCPRouter` 中注册中间件

### 添加自定义消息类型

1. 扩展 `MCPMessageType` 枚举
2. 实现对应的消息处理逻辑

## 故障排查

### 常见问题

1. **服务器启动失败**
   - 检查端口是否被占用
   - 检查配置文件格式是否正确

2. **工具调用失败**
   - 检查工具处理器是否正确注册
   - 检查 `objectName` 是否与配置文件中的 `execHandler` 匹配
   - 检查方法签名是否与 `inputSchema` 匹配

3. **资源读取失败**
   - 检查资源 URI 是否正确
   - 检查文件路径是否存在
   - 检查文件权限

## 许可证

Copyright (c) 2025 zhangheng. All rights reserved.

## 贡献

欢迎提交 Issue 和 Pull Request。

## 更新日志

### v1.0.0 (2025-01-01)
- 初始版本发布
- 支持 MCP 协议核心功能
- 提供 HTTP 传输层实现
- 支持工具、资源、提示词三大服务

