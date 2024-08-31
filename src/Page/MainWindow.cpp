#include "stdafx.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "HomePage.h"
#include "LiveViewerPage.h"
#include "LiveViewerPageDef.h"
#include "SocketClient.h"
#include "ui_StatusBar.h"
#include "RecordSettingDialog.h"
#include "MessageManager.h"
#include "ConfigManager.h"
using namespace SoLive::Config;

constexpr double AspectRatio = 1024.0 / 768;

namespace SoLive::Page
{
	MainWindow::MainWindow(QWidget* parent) :
		QMainWindow(parent),
		_ui(new Ui::MainWindow)
	{
		setupUi();
		setupConnection();
		subscribeSocketClient();
		startMessageHandlingThread();
	}

	MainWindow::~MainWindow()
	{
		unsubscribeSocketClient();
		if (_ui)
		{
			delete _ui;
			_ui = nullptr;
		}
	}

	void MainWindow::setupUi()
	{
		_ui->setupUi(this);
		setupMenuBar();
		setupToolBar();
		setupStatusBar();

		stackedWidget = new QStackedWidget(this);
		setCentralWidget(stackedWidget);
		homePage = new HomePage(this);
		stackedWidget->addWidget(homePage);
		liveViewerPage = new LiveViewerPage(this);
		stackedWidget->addWidget(liveViewerPage);
		stackedWidget->setCurrentWidget(homePage);
		showMaximized();
	}

	void MainWindow::setupMenuBar()
	{
		auto menuBar = this->menuBar();
		QMenu* fileMenu = menuBar->addMenu(tr("文件"));
		QAction* openLogAction = new QAction(tr("打开日志目录"), this);
		connect(openLogAction, &QAction::triggered, this, [this]()
			{
				auto& cfgMgr = ConfigManager::instance();
				auto strLogPath = std::any_cast<std::string>(cfgMgr.getValue(CONFIG_LOG_PATH));
				QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromUtf8(strLogPath.c_str())));
			});
		fileMenu->addAction(openLogAction);
		QAction* openRecordAction = new QAction(tr("打开录制目录"), this);
		connect(openRecordAction, &QAction::triggered, this, [this]()
			{
				auto& cfgMgr = ConfigManager::instance();
				auto strRecordPath = std::any_cast<std::string>(cfgMgr.getValue(CONFIG_RECORD_SAVE_PATH));
				QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromUtf8(strRecordPath.c_str())));
			});
		fileMenu->addAction(openRecordAction);

		QMenu* settingMenu = menuBar->addMenu(tr("设置"));
		QAction* recordSettingAction = new QAction(tr("录制选项"), this);
		connect(recordSettingAction, &QAction::triggered, this, [this]()
			{
				SoLive::Dlg::RecordSettingDialog dlg(this);
				dlg.exec();
			});

		settingMenu->addAction(recordSettingAction);
		settingMenu->addSeparator();
		QAction* userSetting = new QAction(tr("用户"), this);
		settingMenu->addAction(userSetting);
	}

	void MainWindow::setupToolBar()
	{
		QToolBar* toolBar = new QToolBar(tr("MainToolbar"), this);
		addToolBar(toolBar);
		QAction* backAction = new QAction(tr("返回"), this);
		toolBar->addAction(backAction);

		connect(backAction, &QAction::triggered, this, &MainWindow::onBackActionTriggered);
	}

	void MainWindow::setupStatusBar()
	{
		auto statusBar = this->statusBar();

		QWidget* statusBarContainer = new QWidget(this);
		auto* layout = new QHBoxLayout(statusBarContainer);
		layout->setMargin(0);
		layout->setSpacing(0);
		_statusBarWidget = new Ui::StatusBarWidget;
		_statusBarWidget->setupUi(statusBarContainer);
		layout->addWidget(_statusBarWidget->label_4);
		layout->addWidget(_statusBarWidget->conn_state_label);
		layout->addItem(_statusBarWidget->horizontalSpacer_2);
		layout->addWidget(_statusBarWidget->label_2);
		layout->addWidget(_statusBarWidget->online_num_label);
		QSpacerItem* leftSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		layout->addItem(leftSpacer);
		_msgLabel = new QLabel(statusBarContainer);
		_msgLabel->setObjectName("msgLabel");
		_msgLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		_msgLabel->setAlignment(Qt::AlignLeft| Qt::AlignVCenter);
		layout->addWidget(_msgLabel);
		layout->addItem(_statusBarWidget->horizontalSpacer);
		statusBarContainer->setLayout(layout);
		statusBar->addWidget(statusBarContainer);

	}

	void MainWindow::setupConnection()
	{
		connect(homePage, SIGNAL(switchPage(Page)), this, SLOT(handleSwitchPage(Page)));
		connect(liveViewerPage, SIGNAL(backHome(Page)), this, SLOT(handleSwitchPage(Page)));

		auto& liveClient = SoLive::LiveClient::LiveClient::getInstance();
		connect(liveViewerPage, SIGNAL(enterRoom(const QString&, const QString&)), &liveClient, SLOT(handleEnterRoom(const QString&, const QString&)));
		connect(liveViewerPage, SIGNAL(leaveRoom(const QString&)), &liveClient, SLOT(handleLeaveRoom(const QString&)));
		connect(&liveClient, SIGNAL(sigClearWidget()), liveViewerPage, SLOT(onClearWidget()));

	}

	void MainWindow::startMessageHandlingThread()
	{
		connect(&_messageTimer, &QTimer::timeout, this, [this]()
			{
				_msgLabel->clear();
			});
		_messageTimer.setInterval(3000);
		_messageTimer.setSingleShot(true);

		auto& thrPool = SoLive::Util::ThreadPool::instance();
		thrPool.enqueue([this]() { handleMessages(); });
	}

	void MainWindow::handleMessages()
	{
		while (true)
		{
			auto& messageMgr = SoLive::Util::MessageManager::instance();
			std::string msg;
			if (messageMgr.pop(msg))
			{
				QMetaObject::invokeMethod(this, [this, msg]()
					{
						_msgLabel->setText(QString::fromUtf8(msg.c_str()));
						LOG(Trace, msg)
						_messageTimer.start();
					});
			}
			//QThread::msleep(3000);
		}
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

	void MainWindow::subscribeSocketClient()
	{
		auto& socketClient = ProtocolSocketClient::SocketClient::getInstance();
		_observerPtr = std::shared_ptr<ISocketClientObserver>(this);
		socketClient.addObserver(_observerPtr);
	}

	void MainWindow::unsubscribeSocketClient()
	{
		auto& socketClient = ProtocolSocketClient::SocketClient::getInstance();
		if (_observerPtr)
		{
			socketClient.removeObserver(_observerPtr);
		}
	}

	void MainWindow::onConnectionStateChanged(ProtocolSocketClient::ConnectionState newState)
	{
		QString text = "";
		switch (newState)
		{
		case ProtocolSocketClient::ConnectionState::Connected:
			text = ProtocolSocketClient::STR_CONNECTED;
			break;
		case ProtocolSocketClient::ConnectionState::Disconnected:
			text = ProtocolSocketClient::STR_UNCONNECTED;
			break;
		case ProtocolSocketClient::ConnectionState::Reconnecting:
			text = ProtocolSocketClient::STR_CONNECTING;
			break;
		default:
			text = ProtocolSocketClient::STR_UNCONNECTED;
			break;
		}
		_statusBarWidget->conn_state_label->setText(text);
	}

	void MainWindow::onOnlinePersonNumberChanged(int num)
	{
		_statusBarWidget->online_num_label->setText(QString::number(num));
	}

	void MainWindow::onBackActionTriggered()
	{
		handleSwitchPage(Page::Home);
	}
}