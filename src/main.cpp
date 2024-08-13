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
#include "Logger.h"

using SoLive::Logger::Logger;
using SoLive::ProtocolSocketClient::SocketClient;
using SoLive::ProtocolSocketClient::HttpSocketStrategy;
using SoLive::Page::MainWindow;
using SoLive::Config::ConfigFactory;

bool isPingable(const std::string& host)
{
    std::string command = "ping -n 1 " + host;
    int result = system(command.c_str());
    return  result==0;
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
    // ��ʼ����־ϵͳ
    Logger::getInstance().init();

	QApplication app(argc, argv);
    // ������ʽ�ļ�
    applyStyleSheet(app, ":/styles/solive.qss");
    // ����������
    MainWindow mainWindow;
    mainWindow.show();

	// ��ȡ����
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
    // ���ӷ�����
	auto& socketClient = SocketClient::getInstance();
	socketClient.setStrategy(std::make_unique<HttpSocketStrategy>());
	socketClient.connect(uri);

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
