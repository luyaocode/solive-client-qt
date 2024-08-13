#ifndef LiVEVIEWERPAGE_H
#define LiVEVIEWERPAGE_H

#include <QWidget>
#include <QLabel>
#include <QString>
#include "PageDef.h"
#include "LiveClient.h"

namespace Ui
{
	class LiveViewerPage;
}

namespace SoLive::Ctrl
{
	class VideoRenderer;
}

namespace SoLive::Page
{
	class LiveViewerPage : public QWidget
	{
		Q_OBJECT

	public:
		explicit LiveViewerPage(QWidget* parent = nullptr);
		virtual ~LiveViewerPage();
	Q_SIGNALS:
		void backHome(Page page);
		void enterRoom(const QString& roomId);
		void leaveRoom(const QString& roomId="");

	private:
		Ui::LiveViewerPage* _ui;
		QScopedPointer<SoLive::Ctrl::VideoRenderer> _videoRendererPtr;
		QString _currRoom;
	private:
		void setupUi();
		void setupConnection();
		void setCurrRoom(const QString& roomId="");
		inline const QString& currRoom() const { return _currRoom; }
	public Q_SLOTS:
		void onVideoTrackReady(webrtc::VideoTrackInterface* videoTrack);
		void onAudioTrackReady(webrtc::AudioTrackInterface* audioTrack);
		void onRoomConnected(const QString& roomId);
	};
}

#endif // LiVEVIEWERPAGE_H