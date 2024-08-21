#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "PageDef.h"
#include "ISocketClientObserver.h"
#include "ThreadPool.h"


namespace Ui
{
	class MainWindow;
	class StatusBarWidget;
}

namespace SoLive::Page
{
	class MainWindow : public QMainWindow, public ProtocolSocketClient::ISocketClientObserver
	{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget* parent = nullptr);
		virtual ~MainWindow();
	private:
		Ui::MainWindow* _ui;
		Ui::StatusBarWidget* _statusBarWidget;
		QStackedWidget* stackedWidget;
		QWidget* homePage;
		QWidget* liveViewerPage;
		std::shared_ptr<ProtocolSocketClient::ISocketClientObserver> _observerPtr;
		QTimer		_messageTimer;
		QLabel*		_msgLabel;
	private:
		void setupUi();
		void setupMenuBar();
		void setupToolBar();
		void setupStatusBar();
		void setupConnection();
		void subscribeSocketClient();
		void unsubscribeSocketClient();
		void startMessageHandlingThread();
		void handleMessages();
	private Q_SLOTS:
		void handleSwitchPage(Page page);
		void onBackActionTriggered();
	public:
		virtual void onConnectionStateChanged(ProtocolSocketClient::ConnectionState newState) override;
		virtual void onOnlinePersonNumberChanged(int num) override;
	};
}
#endif // MAINWINDOW_H

