##设置库名称
set(LIBRARY_TARGET_NAME MCPXServer)

##查找所有头文件
file(GLOB_RECURSE  ${LIBRARY_TARGET_NAME}_HEADER_FILES
    LIST_DIRECTORIES False
    "${PROJECT_SOURCE_DIR}/Examples/MCPXServer/include/*.h"
    "${PROJECT_SOURCE_DIR}/Examples/MCPXServer/src/*.h"
    "${PROJECT_SOURCE_DIR}/Examples/MCPXServer/*.h"
)
message("PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}")
##设置VS筛选器，头文件分文件夹
source_group(
    TREE "${PROJECT_SOURCE_DIR}/Examples/MCPXServer"
    PREFIX "Header Files"
    FILES ${${LIBRARY_TARGET_NAME}_HEADER_FILES}
)

##查找所有源文件
file(GLOB_RECURSE  ${LIBRARY_TARGET_NAME}_SRC_FILES
    LIST_DIRECTORIES False
    "${PROJECT_SOURCE_DIR}/Examples/MCPXServer/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/Examples/MCPXServer/*.cpp"
)
##设置VS筛选器，源码分文件夹
source_group(
    TREE "${PROJECT_SOURCE_DIR}/Examples/MCPXServer"
    PREFIX "Source Files"
    FILES ${${LIBRARY_TARGET_NAME}_SRC_FILES}
)

add_executable(${LIBRARY_TARGET_NAME} WIN32
    ${${LIBRARY_TARGET_NAME}_HEADER_FILES}
    ${${LIBRARY_TARGET_NAME}_SRC_FILES}
)

##指定工程文件分组
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES FOLDER "Example")

##添加宏配置
target_compile_definitions(${LIBRARY_TARGET_NAME} PRIVATE UNICODE)
##配置构建/使用时的头文件路径
target_include_directories(
    ${LIBRARY_TARGET_NAME}
    PUBLIC
    "$<INSTALL_INTERFACE:include>"
    "$<INSTALL_INTERFACE:include/${LIBRARY_TARGET_NAME}>"
)
add_dependencies(${LIBRARY_TARGET_NAME} MCPCore)


find_package(Qt5 COMPONENTS Core Network REQUIRED)
target_link_libraries(${LIBRARY_TARGET_NAME}
    PRIVATE Qt5::Core Qt5::Network
    PRIVATE ${PROJECT_NAME}::MCPCore
)

##以下警告视为错误
##窗口子系统
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /we4150 /we4172 /we4700 /we4715")
set(RUN_PATH "${QT_QMAKE_EXECUTABLE}/../")
set(RUN_PATH "PATH=%PATH%" ${RUN_PATH})
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES VS_DEBUGGER_ENVIRONMENT "${RUN_PATH}")

##打开qt特性配置
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES AUTOMOC ON)
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES AUTOUIC ON)
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES AUTORCC ON)
##使用UNICUDE字符集


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
if (MSVC)
install(FILES $<TARGET_PDB_FILE:${LIBRARY_TARGET_NAME}> DESTINATION bin/${PROJECT_PLATFORM}/$<IF:$<CONFIG:DEBUG>,Debug,Release> OPTIONAL)
endif()
