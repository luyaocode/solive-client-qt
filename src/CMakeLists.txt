cmake_minimum_required(VERSION 3.12.0)

#set PROJECT_NAME and version
project(SoLive)
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置 CMake 选项以使用默认的 C++ 标准库
set(WEBRTC_USE_CUSTOM_LIBCXX OFF CACHE BOOL "Use custom libc++")
set(WEBRTC_USE_CUSTOM_LIBCXX_FOR_HOST OFF CACHE BOOL "Use custom libc++ for host")
# 定义宏以避免与 WebRTC 的信号槽机制冲突
add_definitions(-DQT_DEPRECATED_WARNINGS -DQT_NO_KEYWORDS)
# 取消定义windows.h中的宏min和max，防止与limits.h中的min和max冲突
add_definitions(-DNOMINMAX)

#set path for QT
set(CMAKE_PREFIX_PATH
    "D:/devs/Qt/5.14.2/msvc2017_64"
    "D:/repos/vcpkg/installed/x64-windows/share"
    CACHE STRING "Directories to search for packages")

# 设置CMake工具链文件
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/vcpkg_toolchain.cmake"
    CACHE STRING "Vcpkg toolchain file")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)

# 查找 nlohmann/json 包
find_package(nlohmann_json CONFIG REQUIRED)

find_package(absl CONFIG REQUIRED)

# 设置头文件路径
set(HEADERS_FILE 
	${CMAKE_SOURCE_DIR}/include/libmediasoupclient
	${CMAKE_SOURCE_DIR}/include/libwebrtc
	"D:/repos/vcpkg/installed/x64-windows/include/nlohmann")

set(project_ui
	MainWindow.ui)

set(project_headers
	MainWindow.h)

set(project_sources
	main.cpp MainWindow.cpp)

# 添加源文件
add_executable(${PROJECT_NAME} ${project_headers} ${project_ui} ${project_sources})

# 定义编译宏
target_compile_definitions(${PROJECT_NAME} PRIVATE WEBRTC_WIN)

# 添加头文件
target_include_directories(${PROJECT_NAME} PRIVATE 
	${HEADERS_FILE})

# Use the widgets module from Qt 5
target_link_libraries(${PROJECT_NAME}
	PUBLIC
	Qt5::Core
	Qt5::Gui
	Qt5::Widgets)
# 链接 nlohmann/json 库
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} PRIVATE absl::base absl::strings absl::optional)
