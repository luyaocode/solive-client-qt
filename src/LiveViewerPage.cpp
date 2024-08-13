#include "LiveViewerPage.h"
#include "ui_LiveViewerPage.h"
#include <QVideoWidget>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QToolTip>
#include <QMessageBox>
#include <QKeyEvent>
#include <QAudioFormat>
#include <QAudioOutput>
#include "AudioPlayer.h"
#include "LiveViewerPageDef.h"
#include "LiveClient.h"
#include "VideoRenderer.h"
#include "Logger.h"

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
	}

	LiveViewerPage::~LiveViewerPage()
	{
		delete _ui;
	}

	void LiveViewerPage::setupUi()
	{
		_ui->setupUi(this);
		_ui->room_id_ledit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[A-Za-z0-9]{8}$"), _ui->room_id_ledit));
		_ui->room_id_ledit->installEventFilter(new RoomIdLineEditFilter(_ui->room_id_ledit));

		_videoRendererPtr.reset(new SoLive::Ctrl::VideoRenderer(this));
		_videoRendererPtr->setFixedSize(1200,900);

		QHBoxLayout* layout = new QHBoxLayout();
		layout->addWidget(_videoRendererPtr.get());
		_ui->videoRendererWidget->setLayout(layout);

	}

	void LiveViewerPage::setupConnection()
	{
		connect(_ui->back_btn, &QPushButton::clicked, [this]()
			{
				backHome(Page::Home);
			}
		);
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
					if (text == currRoom())
					{
						QMessageBox::warning(_ui->room_id_ledit, "警告", "您已在此直播间内");
					}
					else
					{
						enterRoom(text);
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
				QString text = _ui->room_id_ledit->text();
				leaveRoom();
				setCurrRoom();
			}
		);


		auto& liveClient=LiveClient::LiveClient::getInstance();
		connect(&liveClient, SIGNAL(onRemoteVideoTrackReceived(webrtc::VideoTrackInterface*)), this, SLOT(onVideoTrackReady(webrtc::VideoTrackInterface*)));
		connect(&liveClient, SIGNAL(onRemoteAudioTrackReceived(webrtc::AudioTrackInterface*)), this, SLOT(onAudioTrackReady(webrtc::AudioTrackInterface*)));
		connect(&liveClient, SIGNAL(roomConnected(const QString&)), this, SLOT(onRoomConnected(const QString&)));

	}

	void LiveViewerPage::setCurrRoom(const QString& roomId)
	{
		_currRoom = roomId;
	}

	void LiveViewerPage::onVideoTrackReady(webrtc::VideoTrackInterface* videoTrack)
	{
		videoTrack->AddOrUpdateSink(_videoRendererPtr.get(), rtc::VideoSinkWants());
	}

	void LiveViewerPage::onAudioTrackReady(webrtc::AudioTrackInterface* audioTrack)
	{
		LOG(Info,"audio track got.")
		QAudioFormat format;
		format.setSampleRate(48000); // Sample rate of the audio data
		format.setChannelCount(1);   // Number of audio channels
		format.setSampleSize(16);    // Sample size in bits
		format.setCodec("audio/pcm"); // Codec used for the audio data
		format.setByteOrder(QAudioFormat::LittleEndian);
		format.setSampleType(QAudioFormat::SignedInt);

		QAudioOutput* audioOutput = new QAudioOutput(format);
		audioOutput->setVolume(1.0);

		AudioPlayer* audioPlayer = new AudioPlayer(/*audioOutput*/);

		audioTrack->AddSink(audioPlayer);

		// Start playback
		auto ioDevice = audioOutput->start();
	}

	void LiveViewerPage::onRoomConnected(const QString& roomId)
	{
		setCurrRoom(roomId);
	}
}

#include "LiveViewerPage.moc"
