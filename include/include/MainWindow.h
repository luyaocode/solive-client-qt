#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "PageDef.h"


namespace Ui
{
	class MainWindow;
}
namespace SoLive::Page
{
	class MainWindow : public QMainWindow
	{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget* parent = nullptr);
		virtual ~MainWindow();
	private:
		Ui::MainWindow* _ui;
		QStackedWidget* stackedWidget;
		QWidget* homePage;
		QWidget* liveViewerPage;
	private:
		void setupUi();
		void setupConnection();
	private Q_SLOTS:
		void handleSwitchPage(Page page);
	};
}
#endif // MAINWINDOW_H

