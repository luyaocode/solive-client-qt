#include "stdafx.h"
#include "MainWindow.h"
#include "SocketClient.h"
#include "HttpSocketStrategy.h"
#include "HttpsSocketStrategy.h"
#include "ConfigManager.h"
#include "MediaManager.h"

using SoLive::Logger::Logger;
using SoLive::ProtocolSocketClient::SocketClient;
using SoLive::ProtocolSocketClient::HttpSocketStrategy;
using SoLive::ProtocolSocketClient::HttpsSocketStrategy;
using SoLive::Page::MainWindow;
using SoLive::Config::ConfigManager;
using SoLive::Util::MediaManager;

bool isPingable(const std::string& host)
{
    std::string command = "ping -n 1 " + host;
    int result = system(command.c_str());
    return  result==0;
}

bool checkFFmpeg()
{
    const char* inputVideo = "D:/IDM/cartoon.mp4";
    AVFormatContext* fmt_ctx = nullptr;
    if (avformat_open_input(&fmt_ctx, inputVideo, nullptr, nullptr) == 0)
    {
        avformat_close_input(&fmt_ctx);
    }
    else
    {
        return false;
    }
    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec)
    {
        throw std::runtime_error("Codec not found");
    }
    return true;
}

void applyStyleSheet(QApplication& app, const QString& filePath)
{
    QFile file(filePath);
    if (!file.exists())
    {
        qDebug() << "File does not exist:" << filePath;
        return;
    }
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        app.setStyleSheet(styleSheet);
        file.close();
    }
    else
    {
        qDebug() << "Failed to open file:" << filePath;
    }
}

int main(int argc, char* argv[])
{
    // 环境检测
    if (!checkFFmpeg())
    {
        std::cout << "FFmpeg failed." << std::endl;
    }
    // 加载全局配置
    std::string configPath = std::string(PROJECT_ROOT_DIR) + "/config/" + "config.json";
    auto& configManager = ConfigManager::instance();
    configManager.loadConfig("json", configPath);

    // 初始化日志
    Logger::getInstance().init();

    // 加载样式文件
    QApplication app(argc, argv);
    applyStyleSheet(app, ":/styles/solive.qss");
    QFontDatabase fontDatabase;
    QStringList fontFamilies = fontDatabase.families();
    if (fontFamilies.empty())
    {
        throw std::runtime_error("No fonts available on the system.");
    }
    QFont globalFont(fontFamilies[0], 12);
    app.setFont(globalFont);

    // 获取配置
    std::string uri;
    try
    {
        uri = configManager.getUri("dev");
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    auto& socketClient = SocketClient::getInstance();
    socketClient.setStrategy(std::make_unique<HttpSocketStrategy>());

    // 加载主界面
    MainWindow mainWindow;
    mainWindow.show();
    // 连接服务器
    socketClient.connect(uri);

    // 初始化媒体管理器
    MediaManager::instance().init();

    std::string host = "stun.l.google.com";
    if (isPingable(host))
    {
        std::cout << host << " is reachable." << std::endl;
    }
    else
    {
        std::cout << host << " is not reachable." << std::endl;
    }

	return app.exec();
}
