#include "MainWindow.h"
#include <QString>
#include <QJsonObject>
#include "ui_MainWindow.h"
#include "HomePage.h"
#include "LiveViewerPage.h"
#include "LiveViewerPageDef.h"
#include "SocketClient.h"

constexpr double AspectRatio = 1024.0 / 768;

namespace SoLive::Page
{
	MainWindow::MainWindow(QWidget* parent) :
		QMainWindow(parent),
		_ui(new Ui::MainWindow)
	{
		setupUi();
		setupConnection();
	}

	MainWindow::~MainWindow()
	{
		delete _ui;
	}

	void MainWindow::setupUi()
	{
		_ui->setupUi(this);

		stackedWidget = new QStackedWidget(this);
		setCentralWidget(stackedWidget);
		homePage = new HomePage(this);
		stackedWidget->addWidget(homePage);
		liveViewerPage = new LiveViewerPage(this);
		stackedWidget->addWidget(liveViewerPage);
		stackedWidget->setCurrentWidget(homePage);
	}

	void MainWindow::setupConnection()
	{
		connect(homePage, SIGNAL(switchPage(Page)), this, SLOT(handleSwitchPage(Page)));
		connect(liveViewerPage, SIGNAL(backHome(Page)), this, SLOT(handleSwitchPage(Page)));
		connect(liveViewerPage, SIGNAL(enterRoom(const QString&)), this, SLOT(handleEnterRoom(const QString&)));
		connect(liveViewerPage, SIGNAL(leaveRoom(const QString&)), this, SLOT(handleLeaveRoom(const QString&)));
	}

	void MainWindow::handleSwitchPage(Page page)
	{
		if (stackedWidget->currentIndex() != int(page))
		{
			stackedWidget->setCurrentIndex(int(page));
		}
		if (Page::LiveViewer == page)
		{
			SoLive::LiveClient::LiveClient::getInstance();
		}
	}

	void MainWindow::handleEnterRoom(const QString& roomId)
	{
		auto& socketClient=SoLive::ProtocolSocketClient::SocketClient::getInstance();
		auto jsonObj = std::make_unique<QJsonObject>();
		(*jsonObj)["isLive"] = true;
		(*jsonObj)["id"] = roomId;
		socketClient.emit(EVENT_ENTER_ROOM,std::move(jsonObj));
	}
	void MainWindow::handleLeaveRoom(const QString& roomId)
	{
	}
}