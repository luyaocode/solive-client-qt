#include "LiveViewerPage.h"
#include "ui_LiveViewerPage.h"
#include <QVideoWidget>
namespace SoLive::Page
{
	LiveViewerPage::LiveViewerPage(QWidget* parent) :
		QWidget(parent),
		_ui(new Ui::LiveViewerPage)
	{
		setupUi();
		setupConnection();
		auto& liveClient=SoLive::LiveClient::LiveClient::getInstance();
	}

	LiveViewerPage::~LiveViewerPage()
	{
		delete _ui;
	}

	void LiveViewerPage::setupUi()
	{
		_ui->setupUi(this);
	}

	void LiveViewerPage::setupConnection()
	{
		connect(_ui->back_btn, &QPushButton::clicked, [this]()
			{
				backHome(Page::Home);
			});
	}
}