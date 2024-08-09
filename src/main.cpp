#include <QApplication>
#include <iostream>
#include <string>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "MainWindow.h"
#include "libmediasoupclient/mediasoupclient.hpp"
#include "SocketClient.h"
#include "HttpSocketStrategy.h"
#include "ConfigFactory.h"

using SoLive::ProtocolSocketClient::SocketClient;
using SoLive::ProtocolSocketClient::HttpSocketStrategy;
using SoLive::Page::MainWindow;
using SoLive::Config::ConfigFactory;

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
	QApplication app(argc, argv);
    // 加载样式文件
    applyStyleSheet(app, ":/styles/solive.qss");

	MainWindow mainWindow;
	mainWindow.show();
	std::cout << "in func main" << std::endl;

	// 获取配置
    std::string uri;
    try
    {
        std::string configPath = std::string(PROJECT_ROOT_DIR) + "/config/" + "config.json";
        auto configStrategy = ConfigFactory::createConfigStrategy("json", configPath);
        uri = configStrategy->getUri("dev");
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    // 连接服务器
	auto& socketClient = SocketClient::getInstance();
	socketClient.setStrategy(std::make_unique<HttpSocketStrategy>());
	socketClient.connect(uri);



	return app.exec();
}
