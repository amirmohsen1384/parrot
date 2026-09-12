#include "windowsmediacontrols.h"
#include "musicplayer.h"

#include <QCoreApplication>
#include <QDebug>
#include <QWindow>

#include <windows.h>
#include <unknwn.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.h>
#include <systemmediatransportcontrolsinterop.h>

using winrt::Windows::Media::MediaPlaybackStatus;
using winrt::Windows::Media::MediaPlaybackType;
using winrt::Windows::Media::SystemMediaTransportControls;
using winrt::Windows::Media::SystemMediaTransportControlsButton;
using winrt::Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs;

namespace
{
    constexpr qint64 SeekStepMs = 5000;
    constexpr float VolumeStep = 0.05f;
}

struct WindowsMediaControls::SmtcState
{
    SystemMediaTransportControls controls{nullptr};
    SystemMediaTransportControls::ButtonPressed_revoker buttonRevoker;
};

WindowsMediaControls::WindowsMediaControls(MusicPlayer *player, QObject *parent)
    : QObject(parent)
    , m_player(player)
{
    Q_ASSERT(m_player != nullptr);

    try {
        winrt::init_apartment(winrt::apartment_type::single_threaded);
    } catch (const winrt::hresult_error &) {
        // QApplication already initializes COM/STA in typical Qt setups.
    } catch (...) {
    }

    installAppCommandFilter();

    if (!initializeSmtc()) {
        qWarning() << "WindowsMediaControls: SMTC initialization failed;"
                    << "WM_APPCOMMAND handling remains active.";
    }

    connect(m_player, &MusicPlayer::playbackStateChanged,
            this, &WindowsMediaControls::syncPlaybackStatus);
    connect(m_player, &MusicPlayer::currentSongChanged,
            this, &WindowsMediaControls::syncDisplayMetadata);
    connect(m_player, &MusicPlayer::mediaStatusChanged,
            this, &WindowsMediaControls::syncDisplayMetadata);

    syncPlaybackStatus();
    syncDisplayMetadata();
}

WindowsMediaControls::~WindowsMediaControls()
{
    shutdownSmtc();
    uninstallAppCommandFilter();
}

void WindowsMediaControls::installAppCommandFilter()
{
    if (m_filterInstalled)
        return;
    if (QCoreApplication *app = QCoreApplication::instance()) {
        app->installNativeEventFilter(this);
        m_filterInstalled = true;
    }
}

void WindowsMediaControls::uninstallAppCommandFilter()
{
    if (!m_filterInstalled)
        return;
    if (QCoreApplication *app = QCoreApplication::instance())
        app->removeNativeEventFilter(this);
    m_filterInstalled = false;
}

bool WindowsMediaControls::initializeSmtc()
{
    m_sessionWindow = new QWindow();
    m_sessionWindow->setObjectName(QStringLiteral("ParrotMediaSessionWindow"));
    m_sessionWindow->setFlags(Qt::FramelessWindowHint | Qt::Tool);
    m_sessionWindow->resize(1, 1);
    m_sessionWindow->setPosition(-10000, -10000);
    m_sessionWindow->create();

    const HWND hwnd = reinterpret_cast<HWND>(m_sessionWindow->winId());
    if (!hwnd) {
        qWarning() << "WindowsMediaControls: failed to obtain session HWND.";
        delete m_sessionWindow;
        m_sessionWindow = nullptr;
        return false;
    }

    try {
        auto interop = winrt::get_activation_factory<SystemMediaTransportControls,
                                                     ISystemMediaTransportControlsInterop>();

        SystemMediaTransportControls controls{nullptr};
        winrt::check_hresult(interop->GetForWindow(
            hwnd,
            winrt::guid_of<SystemMediaTransportControls>(),
            winrt::put_abi(controls)));

        controls.IsEnabled(true);
        controls.IsPlayEnabled(true);
        controls.IsPauseEnabled(true);
        controls.IsStopEnabled(true);
        controls.IsNextEnabled(true);
        controls.IsPreviousEnabled(true);
        controls.IsFastForwardEnabled(true);
        controls.IsRewindEnabled(true);
        controls.PlaybackStatus(MediaPlaybackStatus::Closed);

        m_smtc = new SmtcState();
        m_smtc->controls = controls;
        m_smtc->buttonRevoker = controls.ButtonPressed(
            winrt::auto_revoke,
            [this](SystemMediaTransportControls const &,
                   SystemMediaTransportControlsButtonPressedEventArgs const &args) {
                const int button = static_cast<int>(args.Button());
                QMetaObject::invokeMethod(
                    this,
                    [this, button]() { handleSmtcButton(button); },
                    Qt::QueuedConnection);
            });

        return true;
    } catch (const winrt::hresult_error &error) {
        qWarning() << "WindowsMediaControls: SMTC GetForWindow failed:"
                    << QString::number(static_cast<int32_t>(error.code()), 16);
    } catch (...) {
        qWarning() << "WindowsMediaControls: unexpected SMTC initialization failure.";
    }

    delete m_sessionWindow;
    m_sessionWindow = nullptr;
    return false;
}

void WindowsMediaControls::shutdownSmtc()
{
    if (m_smtc) {
        try {
            m_smtc->buttonRevoker.revoke();
            if (m_smtc->controls) {
                m_smtc->controls.IsEnabled(false);
                m_smtc->controls.PlaybackStatus(MediaPlaybackStatus::Closed);
            }
        } catch (...) {
        }
        delete m_smtc;
        m_smtc = nullptr;
    }

    delete m_sessionWindow;
    m_sessionWindow = nullptr;
}

bool WindowsMediaControls::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    if (eventType != "windows_generic_MSG" && eventType != "windows_dispatcher_MSG")
        return false;

    const MSG *msg = static_cast<MSG *>(message);
    if (!msg || msg->message != WM_APPCOMMAND)
        return false;

    const int command = GET_APPCOMMAND_LPARAM(msg->lParam);
    if (!handleAppCommand(command))
        return false;

    if (result)
        *result = TRUE;
    return true;
}

bool WindowsMediaControls::handleAppCommand(int command)
{
    if (!m_player)
        return false;

    switch (command) {
    case APPCOMMAND_MEDIA_PLAY:
        m_player->play();
        return true;
    case APPCOMMAND_MEDIA_PAUSE:
        m_player->pause();
        return true;
    case APPCOMMAND_MEDIA_PLAY_PAUSE:
        m_player->togglePlayPause();
        return true;
    case APPCOMMAND_MEDIA_STOP:
        m_player->stop();
        return true;
    case APPCOMMAND_MEDIA_NEXTTRACK:
        m_player->requestNext();
        return true;
    case APPCOMMAND_MEDIA_PREVIOUSTRACK:
        m_player->requestPrevious();
        return true;
    case APPCOMMAND_MEDIA_FAST_FORWARD:
        m_player->seekForward(SeekStepMs);
        return true;
    case APPCOMMAND_MEDIA_REWIND:
        m_player->seekBackward(SeekStepMs);
        return true;
    case APPCOMMAND_VOLUME_UP:
        m_player->increaseVolume(VolumeStep);
        return true;
    case APPCOMMAND_VOLUME_DOWN:
        m_player->decreaseVolume(VolumeStep);
        return true;
    case APPCOMMAND_VOLUME_MUTE:
        m_player->toggleMute();
        return true;
    default:
        return false;
    }
}

void WindowsMediaControls::handleSmtcButton(int button)
{
    if (!m_player)
        return;

    switch (static_cast<SystemMediaTransportControlsButton>(button)) {
    case SystemMediaTransportControlsButton::Play:
        m_player->play();
        break;
    case SystemMediaTransportControlsButton::Pause:
        m_player->pause();
        break;
    case SystemMediaTransportControlsButton::Stop:
        m_player->stop();
        break;
    case SystemMediaTransportControlsButton::Next:
        m_player->requestNext();
        break;
    case SystemMediaTransportControlsButton::Previous:
        m_player->requestPrevious();
        break;
    case SystemMediaTransportControlsButton::FastForward:
        m_player->seekForward(SeekStepMs);
        break;
    case SystemMediaTransportControlsButton::Rewind:
        m_player->seekBackward(SeekStepMs);
        break;
    default:
        break;
    }
}

void WindowsMediaControls::syncPlaybackStatus()
{
    if (!m_smtc || !m_smtc->controls || !m_player)
        return;

    try {
        switch (m_player->playbackState()) {
        case QMediaPlayer::PlayingState:
            m_smtc->controls.PlaybackStatus(MediaPlaybackStatus::Playing);
            break;
        case QMediaPlayer::PausedState:
            m_smtc->controls.PlaybackStatus(MediaPlaybackStatus::Paused);
            break;
        case QMediaPlayer::StoppedState:
            m_smtc->controls.PlaybackStatus(m_player->hasSource()
                                                ? MediaPlaybackStatus::Stopped
                                                : MediaPlaybackStatus::Closed);
            break;
        }
    } catch (const winrt::hresult_error &error) {
        qWarning() << "WindowsMediaControls: failed to sync playback status:"
                    << QString::number(static_cast<int32_t>(error.code()), 16);
    }
}

void WindowsMediaControls::syncDisplayMetadata()
{
    if (!m_smtc || !m_smtc->controls || !m_player)
        return;

    try {
        auto updater = m_smtc->controls.DisplayUpdater();
        updater.Type(MediaPlaybackType::Music);

        const Song song = m_player->currentSong();
        const QString title = !song.data.name.isEmpty()
                                  ? song.data.name
                                  : song.data.fileName.fileName();
        updater.MusicProperties().Title(winrt::to_hstring(title.toStdString()));
        updater.MusicProperties().Artist(winrt::hstring{});
        updater.MusicProperties().AlbumTitle(winrt::hstring{});
        updater.Update();
    } catch (const winrt::hresult_error &error) {
        qWarning() << "WindowsMediaControls: failed to sync display metadata:"
                    << QString::number(static_cast<int32_t>(error.code()), 16);
    }
}
