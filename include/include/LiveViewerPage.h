#ifndef LiVEVIEWERPAGE_H
#define LiVEVIEWERPAGE_H

#include <QWidget>
#include <QLabel>
#include <PageDef.h>
#include <LiveClient.h>

namespace Ui
{
	class LiveViewerPage;
}
namespace SoLive::Page
{
	class LiveViewerPage : public QWidget
	{
		Q_OBJECT

	public:
		explicit LiveViewerPage(QWidget* parent = nullptr);
		virtual ~LiveViewerPage();
	Q_SIGNAL
		void backHome(Page page);

	private:
		Ui::LiveViewerPage* _ui;
	private:
		void setupUi();
		void setupConnection();
	};
}

#endif // LiVEVIEWERPAGE_H