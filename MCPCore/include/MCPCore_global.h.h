/**
 * @file MCPCore_global.h.h
 * @brief MCPCore库的全局定义和导出宏
 * @author zhangheng
 * @date 2025-01-01
 * @copyright Copyright (c) 2025 zhangheng. All rights reserved.
 */

#pragma once
#include <QtCore/qglobal.h>

#ifdef MCPXServer_LIB
	# define MCPCORE_EXPORT Q_DECL_EXPORT
	//# define MCPCORE_EXPORT 
#else
	//# define MCPCORE_EXPORT 
	# define MCPCORE_EXPORT Q_DECL_IMPORT
#endif