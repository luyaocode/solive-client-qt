#include "stdafx.h"
#include "HomePage.h"
#include "ui_HomePage.h"
#include "SocketClient.h"
#include "PageDef.h"

namespace SoLive::Page
{
	HomePage::HomePage(QWidget* parent) :
		QWidget(parent),
		_ui(new Ui::HomePage)
	{
		setupUi();
		setupConnection();
	}

	HomePage::~HomePage()
	{
		delete _ui;
	}

	void HomePage::setupUi()
	{
		_ui->setupUi(this);
	}

	void HomePage::setupConnection()
	{
		connect(_ui->live_stream_btn, &QPushButton::clicked, [this]()
			{
				switchPage(Page::LiveViewer);
			});
	}
}