##设置库名称
set(LIBRARY_TARGET_NAME json-schema-validator)

set(THIRDPARTY_DIR "${PROJECT_SOURCE_DIR}/3rdparty")
##查找所有头文件
file(GLOB_RECURSE  ${THIRDPARTY_DIR}_HEADER_FILES
    LIST_DIRECTORIES False
    "${THIRDPARTY_DIR}/${LIBRARY_TARGET_NAME}/*.h"
)
##设置VS筛选器，头文件分文件夹
source_group(
    TREE "${THIRDPARTY_DIR}/${LIBRARY_TARGET_NAME}"
    PREFIX "Header Files"
    FILES ${${LIBRARY_TARGET_NAME}_HEADER_FILES}
)


##查找所有源文件
file(GLOB_RECURSE  ${LIBRARY_TARGET_NAME}_SRC_FILES
    LIST_DIRECTORIES False
    "${THIRDPARTY_DIR}/${LIBRARY_TARGET_NAME}/*.cpp"
    "${THIRDPARTY_DIR}/${LIBRARY_TARGET_NAME}/*.hpp"
)
##设置VS筛选器，源码分文件夹
source_group(
    TREE "${THIRDPARTY_DIR}/${LIBRARY_TARGET_NAME}"
    PREFIX "Source Files"
    FILES ${${LIBRARY_TARGET_NAME}_SRC_FILES}
)

##添加目标
add_library(${LIBRARY_TARGET_NAME} STATIC
#add_library(${LIBRARY_TARGET_NAME} SHARED
    ${${LIBRARY_TARGET_NAME}_HEADER_FILES}
    ${${LIBRARY_TARGET_NAME}_SRC_FILES}
    ${${LIBRARY_TARGET_NAME}_QM_FILE}
)

##添加别名
add_library(${PROJECT_NAME}::${LIBRARY_TARGET_NAME} ALIAS ${LIBRARY_TARGET_NAME})

##指定工程文件分组
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES FOLDER "3rdparty")
##添加宏配置
target_compile_definitions(${LIBRARY_TARGET_NAME} PRIVATE UNICODE)

##配置构建/使用时的头文件路径
target_include_directories(
    ${LIBRARY_TARGET_NAME}
    PUBLIC
    "$<BUILD_INTERFACE:${THIRDPARTY_DIR}/${LIBRARY_TARGET_NAME}/>"
    PRIVATE
    "$<BUILD_INTERFACE:${THIRDPARTY_DIR}/${LIBRARY_TARGET_NAME}/src/>"
    "$<BUILD_INTERFACE:${THIRDPARTY_DIR}/>"
)

find_package(Qt5 COMPONENTS Core Network Concurrent REQUIRED)
target_link_libraries(${LIBRARY_TARGET_NAME}
    PRIVATE Qt5::Core Qt5::Network Qt5::Concurrent
)

##以下警告视为错误
##禁用httplib lambda警告
target_compile_options(${LIBRARY_TARGET_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/wd2888>)

##打开qt特性配置
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES AUTOMOC ON)
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES AUTOUIC ON)
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES AUTORCC ON)

##使用预编译头
#target_precompile_headers(${LIBRARY_TARGET_NAME} PRIVATE ${THIRDPARTY_DIR}/QtJsonSchema/stdafx.h)

##使用UNICUDE字符集
add_definitions(-DUNICODE -D_UNICODE)
##启用C++异常
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES COMPILE_FLAGS "/EHa")

##生成前事件

##链接前事件

##生成后事件

##重建目录
install(DIRECTORY 
    ${${LIBRARY_TARGET_NAME}_SOURCE_DIR}/
    DESTINATION include/${LIBRARY_TARGET_NAME}
    FILES_MATCHING PATTERN "*.h*"
)

##安装构建结果
install(TARGETS ${LIBRARY_TARGET_NAME}
    EXPORT ${PROJECT_NAME}
    RUNTIME DESTINATION bin/${PROJECT_PLATFORM}/$<IF:$<CONFIG:DEBUG>,Debug,Release>
    ARCHIVE DESTINATION lib/${PROJECT_PLATFORM}/$<IF:$<CONFIG:DEBUG>,Debug,Release>
    LIBRARY DESTINATION lib/${PROJECT_PLATFORM}/$<IF:$<CONFIG:DEBUG>,Debug,Release>
)
##安装pdb(可选)
if (MSVC AND BUILD_SHARED_LIBS)
install(FILES $<TARGET_PDB_FILE:${LIBRARY_TARGET_NAME}> DESTINATION bin/${PROJECT_PLATFORM}/$<IF:$<CONFIG:DEBUG>,Debug,Release> OPTIONAL)
endif()
