#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QPushButton>
#include "PageDef.h"

namespace Ui
{
	class HomePage;
}

namespace SoLive::Page
{
	class HomePage : public QWidget
	{
		Q_OBJECT

	public:
		explicit HomePage(QWidget* parent = nullptr);
		virtual ~HomePage();
	Q_SIGNAL
		void switchPage(Page page);
	private:
		Ui::HomePage* _ui;
		QPushButton* live_stream_btn;
	private:
		void setupUi();
		void setupConnection();
	};
}
#endif // HOMEPAGE_H

