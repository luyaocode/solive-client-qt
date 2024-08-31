#include "stdafx.h"
#include "ISocketClientObserver.h"
#include "LiveViewerPage.h"
#include "ui_LiveViewerPage.h"
#include "AudioPlayer.h"
#include "LiveViewerPageDef.h"
#include "LiveClient.h"
#include "VideoRenderer.h"
#include "AudioPlayer.h"
#include "MediaManager.h"
#include "UtilDef.h"
#include "ConnectionState.h"
#include "SocketClient.h"
#include "MessageManager.h"
#include "PeerConnectionFactory.h"

namespace SoLive::Page
{
	class RoomIdLineEditFilter : public QObject
	{
		Q_OBJECT
	public:
		RoomIdLineEditFilter(QObject* parent = nullptr) : QObject(parent) {}
	protected:
		bool eventFilter(QObject* obj, QEvent* event) override;
	};

	bool RoomIdLineEditFilter::eventFilter(QObject* obj, QEvent* event)
	{
		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
			if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
			{
				QLineEdit* lineEdit = qobject_cast<QLineEdit*>(obj);
				if (lineEdit)
				{
					QString text = lineEdit->text();
					if (text.length() == ROOM_ID_LEN && text.contains(QRegExp("^[a-zA-Z0-9]{8}$")))
					{
						//QMessageBox::information(lineEdit, "通知", "Input is valid!");
					}
					else
					{
						QMessageBox::warning(lineEdit, "警告", "无效的直播间号");
					}
				}
				return true;
			}
		}
		return QObject::eventFilter(obj, event);
	}

	LiveViewerPage::LiveViewerPage(QWidget* parent) :
		QWidget(parent),
		_ui(new Ui::LiveViewerPage)
	{
		setupUi();
		setupConnection();
		auto& socketClient = ProtocolSocketClient::SocketClient::getInstance();
		socketClient.addObserver(std::shared_ptr<ISocketClientObserver>(this));
	}

	LiveViewerPage::~LiveViewerPage()
	{
		delete _ui;
	}

	void LiveViewerPage::hideUi()
	{
		setVisibleRecursively(_ui->gridLayout,false);
	}

	void LiveViewerPage::showUi()
	{
		setVisibleRecursively(_ui->gridLayout, true);
	}

	void LiveViewerPage::setVisibleRecursively(QLayout* layout,bool show)
	{
		if (!layout) return;

		for (int i = 0; i < layout->count(); ++i)
		{
			QLayoutItem* item = layout->itemAt(i);
			if (item)
			{
				QWidget* widget = item->widget();
				if (widget&&widget!=_ui->videoRendererWidget)
				{
					widget->setVisible(show);
				}
				else
				{
					QLayout* subLayout = item->layout();
					if (subLayout)
					{
						setVisibleRecursively(subLayout,show);
					}
				}
			}
		}
	}

	void LiveViewerPage::setupUi()
	{
		_ui->setupUi(this);
		_ui->room_id_ledit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[A-Za-z0-9]{8}$"), _ui->room_id_ledit));
		_ui->room_id_ledit->installEventFilter(new RoomIdLineEditFilter(_ui->room_id_ledit));
		_ui->leave_room_btn->setEnabled(false);
		_ui->record_btn->setEnabled(false);
		_ui->record_combo->setEnabled(false);
		_ui->screen_shot_btn->setEnabled(false);

		auto& mediaManager = SoLive::Util::MediaManager::instance();
		_audioPlayerPtr = mediaManager.audioPlayerPtr();
		_videoRendererPtr = mediaManager.videoRendererPtr();
		_videoRendererPtr->setParent(this);
		_videoRendererPtr->setFixedSize(VIDEO_WIDTH_ORIGIN, VIDEO_WIDTH_ORIGIN/ ASPECT_RATIO);

		QHBoxLayout* layout = new QHBoxLayout();
		layout->addWidget(_videoRendererPtr.get());
		_ui->videoRendererWidget->setLayout(layout);

		_ui->record_combo->addItems(QStringList{ SoLive::Util::VIDEO_AND_AUDIO,
			SoLive::Util::ONLY_VIDEO,SoLive::Util::ONLY_AUDIO });
		_ui->record_combo->setCurrentIndex(0);
	}

	void LiveViewerPage::setupConnection()
	{
		connect(_ui->room_id_ledit, &QLineEdit::textChanged, [this](const QString& text)
			{
				auto lineEdit = _ui->room_id_ledit;
				QRegularExpression regex("^[A-Za-z0-9]{8}$");
				if (text.length() != ROOM_ID_LEN)
				{
					QToolTip::showText(lineEdit->mapToGlobal(QPoint(0, 0)), WARNING_INPUT_LESS_NUM);
				}
				else if (!regex.match(text).hasMatch())
				{
					QToolTip::showText(lineEdit->mapToGlobal(QPoint(0, 0)), WARNING_INPUT_INVALID);
				}
				else
				{
					QToolTip::hideText();
				}
			}
		);

		connect(_ui->enter_room_btn, &QPushButton::clicked, [this]()
			{
				QString text = _ui->room_id_ledit->text();
				if (text.length() == 8 && text.contains(QRegExp("^[a-zA-Z0-9]{8}$")))
				{
					if (currRoom()== text)
					{
						QMessageBox::warning(_ui->room_id_ledit, "警告", "您已在此直播间内");
					}
					else
					{
						enterRoom(text,currRoom());
					}
				}
				else
				{
					QMessageBox::warning(_ui->room_id_ledit, "警告", "无效的直播间号");
				}
			}
		);

		connect(_ui->leave_room_btn, &QPushButton::clicked, [this]()
			{
				if (currRoom().isEmpty())
				{
					return;
				}
				leaveRoom(currRoom()); //Signal
				clearPlayer();
			}
		);

		connect(_ui->record_btn, &QPushButton::clicked, [this]()
			{
				auto& mediaManager = SoLive::Util::MediaManager::instance();
				//auto isPlaying = _audioPlayerPtr->isPlaying();
				//auto isRendering = _videoRendererPtr->isRendering();
				if (currRoom().isEmpty()) return;

				if (mediaManager.isRecording())
				{
					mediaManager.stopRecord();
					_ui->record_btn->setText(BTN_RECORD);
				}
				else
				{
					mediaManager.startRecord(static_cast<SoLive::Util::RecordMode>(_ui->record_combo->currentIndex()));
					_ui->record_btn->setText(BTN_STOP_RECORD);
				}
			});

		connect(_ui->screen_shot_btn, &QPushButton::clicked, [this]()
			{
				auto& mediaManager = SoLive::Util::MediaManager::instance();
				mediaManager.screenShot();
			});


		auto& liveClient=LiveClient::LiveClient::getInstance();
		connect(&liveClient, SIGNAL(onRemoteVideoTrackReceived(webrtc::VideoTrackInterface*)), this, SLOT(onVideoTrackReady(webrtc::VideoTrackInterface*)));
		connect(&liveClient, SIGNAL(onRemoteAudioTrackReceived(webrtc::AudioTrackInterface*)), this, SLOT(onAudioTrackReady(webrtc::AudioTrackInterface*)));
		connect(&liveClient, SIGNAL(roomConnected(const QString&)), this, SLOT(onRoomConnected(const QString&)));
		connect(&liveClient, SIGNAL(roomConnected(const QString&)), _videoRendererPtr.get(), SLOT(onRoomConnected(const QString&)));
		connect(this, SIGNAL(currRoomChanged(const QString&)), this, SLOT(onCurrRoomChanged(const QString&)));
		auto& mediaMgr = SoLive::Util::MediaManager::instance();
		connect(this, SIGNAL(currRoomChanged(const QString&)), &mediaMgr, SLOT(onCurrRoomChanged(const QString&)));
		connect(&mediaMgr, SIGNAL(sendEvent(const Event&)), this, SLOT(onEvent(const Event&)));
		
		// 视频渲染组件槽函数连接
		connect(this, SIGNAL(currRoomChanged(const QString&)), _videoRendererPtr.get(), SLOT(onCurrRoomChanged(const QString&)));
	}

	void LiveViewerPage::setCurrRoom(const QString& room)
	{
		if (_currRoom != room)
		{
			_currRoom = room;
			currRoomChanged(_currRoom);
		}
	}

	void LiveViewerPage::onVideoTrackReady(webrtc::VideoTrackInterface* videoTrack)
	{
		_videoRendererPtr->start();
		videoTrack->AddOrUpdateSink(_videoRendererPtr.get(), rtc::VideoSinkWants());
		_videoTrack = videoTrack;
	}

	void LiveViewerPage::onAudioTrackReady(webrtc::AudioTrackInterface* audioTrack)
	{
		_audioPlayerPtr->start();
		audioTrack->AddSink(_audioPlayerPtr.get());
		_audioTrack = audioTrack;
	}

	void LiveViewerPage::onRoomConnected(const QString& room)
	{
		MSG_PUSH(std::string("已进入直播间 ") + room.toUtf8().constData())
		setCurrRoom(room);
	}

	void LiveViewerPage::clearPlayer()
	{
		auto videoTrack = dynamic_cast<webrtc::VideoTrackInterface*>(_videoTrack);
		videoTrack->RemoveSink(_videoRendererPtr.get());
		_videoRendererPtr->clear();

		auto audioTrack = dynamic_cast<webrtc::AudioTrackInterface*>(_audioTrack);
		audioTrack->RemoveSink(_audioPlayerPtr.get());
		_audioPlayerPtr->stop();

		_videoTrack = nullptr;
		_audioTrack = nullptr;
		setCurrRoom();
	}

	void LiveViewerPage::onClearWidget()
	{
		clearPlayer();
	}

	void LiveViewerPage::onCurrRoomChanged(const QString& room)
	{
		if (room.isEmpty())
		{
			_ui->leave_room_btn->setEnabled(false);
			_ui->record_btn->setEnabled(false);
			_ui->record_combo->setEnabled(false);
			_ui->screen_shot_btn->setEnabled(false);
		}
		else
		{
			_ui->leave_room_btn->setEnabled(true);
			_ui->record_btn->setEnabled(true);
			_ui->record_combo->setEnabled(true);
			_ui->screen_shot_btn->setEnabled(true);
		}
	}

	void LiveViewerPage::onEvent(const Event& e)
	{
		switch (e.type)
		{
		case EventType::PlayStatus:
			if (_videoTrack)
			{
				_videoTrack->set_enabled(e.play);
			}
			if (_audioTrack)
			{
				_audioTrack->set_enabled(e.play);
			}
			break;
		case EventType::Muted:
			if (_audioTrack)
			{
				_audioTrack->set_enabled(!e.muted);
			}
			break;
		case EventType::Volume:
		{
			auto signalingThread = SoLive::PeerConnection::PeerConnectionFactory::getInstance().signalingThread();
			double vol = e.volume / 100;
			signalingThread->PostTask([this,vol]()
				{
					auto audioTrack = dynamic_cast<webrtc::AudioTrackInterface*>(_audioTrack);
					if (audioTrack)
					{
						auto source = audioTrack->GetSource();
						source->SetVolume(vol);
					}
				});
			break;
		}
		case EventType::Reconnect:
		{
			auto& liveClient=LiveClient::LiveClient::getInstance();
			liveClient.reconnect();
			break;
		}
		default:
			break;
		}
	}

	void LiveViewerPage::onConnectionStateChanged(SoLive::ProtocolSocketClient::ConnectionState newState)
	{
		if (QThread::currentThread() != this->thread())
		{
			QMetaObject::invokeMethod(this, [this, newState]()
				{
					this->onConnectionStateChanged(newState);
				}, Qt::QueuedConnection);
			return;
		}
		switch (newState)
		{
		case ProtocolSocketClient::ConnectionState::Connected:
			setEnabled(true);
			break;
		case ProtocolSocketClient::ConnectionState::Disconnected:
			setEnabled(false);
			break;
		case ProtocolSocketClient::ConnectionState::Reconnecting:
			setEnabled(false);
			break;
		default:
			break;
		}
	}
}

#include "LiveViewerPage.moc"
