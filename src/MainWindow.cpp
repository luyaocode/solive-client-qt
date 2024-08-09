#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "HomePage.h"
#include "LiveViewerPage.h"

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
	}

	void MainWindow::handleSwitchPage(Page page)
	{
		if (stackedWidget->currentIndex() != int(page))
		{
			stackedWidget->setCurrentIndex(int(page));
		}
	}
}