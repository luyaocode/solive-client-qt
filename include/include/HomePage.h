#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "ISocketClientObserver.h"
#include "PageDef.h"

namespace Ui
{
	class HomePage;
}

namespace SoLive::Page
{
	class HomePage : public QWidget,
		public ProtocolSocketClient::ISocketClientObserver
	{
		Q_OBJECT

	public:
		explicit HomePage(QWidget* parent = nullptr);
		virtual ~HomePage();
	Q_SIGNAL
		void switchPage(Page page);
	private:
		Ui::HomePage* _ui;
		QWidget* liveViewerPage;
		QLabel* conn_state_label;
		QLabel* online_num_label;
		QPushButton* live_stream_btn;
		std::shared_ptr<ProtocolSocketClient::ISocketClientObserver> _observerPtr;
	private:
		void setupUi();
		void setupConnection();
		void subscribeSocketClient();
		void unsubscribeSocketClient();
		virtual void onConnectionStateChanged(ProtocolSocketClient::ConnectionState newState) override;
		virtual void onOnlinePersonNumberChanged(int num) override;
	};
}
#endif // HOMEPAGE_H

