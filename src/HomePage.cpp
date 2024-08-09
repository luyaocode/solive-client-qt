#include "HomePage.h"
#include "ui_HomePage.h"
#include <iostream>
#include <memory>
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
		subscribeSocketClient();
	}

	HomePage::~HomePage()
	{
		delete _ui;
		unsubscribeSocketClient();
	}

	void HomePage::setupUi()
	{
		_ui->setupUi(this);
		conn_state_label = _ui->conn_state_label;
		online_num_label = _ui->online_num_label;
		live_stream_btn = _ui->live_stream_btn;
	}

	void HomePage::setupConnection()
	{
		connect(_ui->live_stream_btn, &QPushButton::clicked, [this]()
			{
				switchPage(Page::LiveViewer);
			});
	}

	void HomePage::subscribeSocketClient()
	{
		auto& socketClient = ProtocolSocketClient::SocketClient::getInstance();
		_observerPtr = std::shared_ptr<ISocketClientObserver>(this);
		socketClient.addObserver(_observerPtr);
	}

	void HomePage::unsubscribeSocketClient()
	{
		auto& socketClient = ProtocolSocketClient::SocketClient::getInstance();
		if (_observerPtr)
		{
			socketClient.removeObserver(_observerPtr);
		}
	}

	void HomePage::onConnectionStateChanged(ProtocolSocketClient::ConnectionState newState)
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
		conn_state_label->setText(text);
	}

	void HomePage::onOnlinePersonNumberChanged(int num)
	{
		online_num_label->setText(QString::number(num));
	}
}