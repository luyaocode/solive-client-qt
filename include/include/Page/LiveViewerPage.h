#ifndef LiVEVIEWERPAGE_H
#define LiVEVIEWERPAGE_H

#include <QWidget>
#include <QLabel>
#include <QString>
#include <mutex>
#include "PageDef.h"
#include "LiveClient.h"
#include "ISocketClientObserver.h"
#include "UtilDef.h"
using namespace SoLive::Util;

namespace Ui
{
	class LiveViewerPage;
}

namespace SoLive::Ctrl
{
	class VideoRenderer;
	class AudioPlayer;
}

namespace SoLive::Page
{
	class LiveViewerPage : public QWidget,
		public SoLive::ProtocolSocketClient::ISocketClientObserver
	{
		Q_OBJECT
			Q_PROPERTY(QString currRoom READ currRoom WRITE setCurrRoom NOTIFY currRoomChanged)


	public:
		explicit LiveViewerPage(QWidget* parent = nullptr);
		virtual ~LiveViewerPage();
		void hideUi();
		void showUi();
	Q_SIGNALS:
		void enterRoom(const QString& newRoom, const QString& oldRoom);
		void leaveRoom(const QString& roomId = "");
		void currRoomChanged(const QString& room);

	private:
		Ui::LiveViewerPage* _ui;
		QSharedPointer<SoLive::Ctrl::VideoRenderer> _videoRendererPtr;
		QSharedPointer<SoLive::Ctrl::AudioPlayer> _audioPlayerPtr;
		webrtc::MediaStreamTrackInterface* _audioTrack{nullptr};
		webrtc::MediaStreamTrackInterface* _videoTrack{nullptr};
		QString _currRoom;
	private:
		void setupUi();
		void setupConnection();
		const QString currRoom() const { return _currRoom; }
		void setCurrRoom(const QString& room = "");
		void clearPlayer();
		void setVisibleRecursively(QLayout* layout,bool show);
	public Q_SLOTS:
		void onVideoTrackReady(webrtc::VideoTrackInterface* videoTrack);
		void onAudioTrackReady(webrtc::AudioTrackInterface* audioTrack);
		void onRoomConnected(const QString& room);
		void onClearWidget();
		void onCurrRoomChanged(const QString& room);
		void onEvent(const Event& e);
	public:
		void onConnectionStateChanged(SoLive::ProtocolSocketClient::ConnectionState newState) override;
	};
}

#endif // LiVEVIEWERPAGE_H