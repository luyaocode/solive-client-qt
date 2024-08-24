cmake_minimum_required(VERSION 3.12.0)

#set PROJECT_NAME and version
project(SoLive)
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/utf-8)
endif()
if (MSVC)
    # WebRTC采用的是/MT，mediasoupclient也需要采用/MT
    #set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

# 设置 CMake 选项以使用默认的 C++ 标准库
set(WEBRTC_USE_CUSTOM_LIBCXX OFF CACHE BOOL "Use custom libc++")
set(WEBRTC_USE_CUSTOM_LIBCXX_FOR_HOST OFF CACHE BOOL "Use custom libc++ for host")
# 定义宏以避免与 WebRTC 的信号槽机制冲突，意味着不能使用signal(s)，slot(s)和emit，但是可以使用Q_SLOT和Q_SIGNAL
add_definitions(-DQT_DEPRECATED_WARNINGS -DQT_NO_KEYWORDS)
# 取消定义windows.h中的宏min和max，防止与limits.h中的min和max冲突
add_definitions(-DNOMINMAX)

# 设置 Windows SDK 路径
set(CMAKE_PREFIX_PATH "C:/Program Files (x86)/Windows Kits/10")
set(CMAKE_LIBRARY_PATH "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.22621.0/um/x64")
#"C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x64")

#set path for third_party
set(CMAKE_PREFIX_PATH
    "D:/devs/Qt/5.14.2/msvc2017_64"
    "D:/repos/vcpkg/installed/x64-windows-static/share"
    "D:/repos/vcpkg/installed/x64-windows/share"
    CACHE STRING "Directories to search for packages"
)

# 设置CMake工具链文件
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/vcpkg_toolchain.cmake"
    CACHE STRING "Vcpkg toolchain file")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui Test)
find_package(Qt5 COMPONENTS Multimedia MultimediaWidgets REQUIRED)

# vcpkg安装包
find_package(absl REQUIRED CONFIG)
# 使用vcpkg安装无法指定静态多线程运行时库和_ITERATOR_DEBUG_LEVEL，采用CMake编译静态库就好了
find_package(spdlog REQUIRED CONFIG)

#find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system beast asio)
set(BOOST_LIBRARIES Boost::beast Boost::asio Boost::system)

find_package(OpenSSL REQUIRED)

# 添加头文件
set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include/include)
file(GLOB_RECURSE HEADERS_DIRECTORY LIST_DIRECTORIES true
    "${INCLUDE_PATH}/*"
)
list(APPEND HEADERS_DIRECTORY
    ${INCLUDE_PATH}
    ${CMAKE_SOURCE_DIR}/include/libmediasoupclient
    ${CMAKE_SOURCE_DIR}/include/libwebrtc
    ${CMAKE_SOURCE_DIR}/include/json-3.11.2
    ${CMAKE_SOURCE_DIR}/include/json-3.11.2/nlohmann
    ${CMAKE_SOURCE_DIR}/include/sio-client
    "D:/repos/vcpkg/installed/x64-windows/include"
    "D:/repos/vcpkg/installed/x64-windows-static/include"
)

# 设置UI文件的搜索路径
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/ui)

# 生成目标
file(GLOB_RECURSE project_headers
    "${INCLUDE_PATH}/*.h"
    "${INCLUDE_PATH}/*.hpp"
)

file(GLOB_RECURSE project_sources
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

    # 添加qrc文件
file(GLOB qrc_files
    "${CMAKE_SOURCE_DIR}/resources/*.qrc"
)
qt5_add_resources(QRC_FILES ${qrc_files})
SOURCE_GROUP("Resource Files" FILES ${qrc_files})

add_executable(${PROJECT_NAME} 
    ${project_headers} 
    ${project_sources} 
    ${ui_headers} 
    ${QRC_FILES}
)

# 包含头文件
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${HEADERS_DIRECTORY}
    ${CMAKE_CURRENT_BINARY_DIR}/SoLive_autogen/include
    ${FFMPEG_INCLUDE_DIRS}
)

# 定义编译宏
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    WEBRTC_WIN
	PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}"
)

# 使用预编译头文件
target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)

# 定义调试级别
if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _ITERATOR_DEBUG_LEVEL=0)
else()
    # target_compile_definitions(${PROJECT_NAME} PRIVATE _ITERATOR_DEBUG_LEVEL=0)
endif()

# 设置库目录
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib)
set(FFMPEG_LIBRARY_DIRS "D:/repos/vcpkg/installed/x64-windows-static/lib")
target_link_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_LIBRARY_DIRS})


# 链接三方库、win32
set(FFMPEG_LIBRARIES avcodec avformat avutil swscale swresample avfilter avdevice libx264)
set(QT_LIBS Qt5::Core
	Qt5::Gui
	Qt5::Widgets
	Qt5::Multimedia
	Qt5::MultimediaWidgets
    Qt5::Test
)
set(WIN32_LIBS
    iphlpapi
    Ole32
    Secur32
    strmiids
    quartz
    mfuuid
    msdmo
    wmcodecdspuuid
    winmm
    dxguid
    dmoguids
    ws2_32
    bcrypt
    mfplat
    mf
    mfreadwrite
)
# 查找库

target_link_libraries(${PROJECT_NAME} PRIVATE
    #${BOOST_LIBRARIES}
    #${SSL_LIBS}
    ${FFMPEG_LIBRARIES}
    ${BOOST_LIBRARIES}
    ${QT_LIBS}
    ${WIN32_LIBS}
    #sioclient
    sioclient_tls
    OpenSSL::SSL
    OpenSSL::Crypto
    webrtc
    sdptransform
    mediasoupclient
)
# vcpkg包
target_link_libraries(${PROJECT_NAME} PRIVATE 
    absl::base absl::strings absl::optional
    spdlog::spdlog
)


