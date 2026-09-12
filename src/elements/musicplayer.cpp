#include "musicplayer.h"

#include <QAudioOutput>
#include <QDebug>
#include <QFileInfo>
#include <QMediaDevices>

#ifdef Q_OS_WIN
#include "windowsmediacontrols.h"
#endif

namespace
{
    constexpr float MinVolume = 0.0f;
    constexpr float MaxVolume = 1.0f;
}

MusicPlayer::MusicPlayer(QObject *parent) : QObject(parent)
{
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_mediaDevices = new QMediaDevices(this);

    m_audioOutput->setDevice(QMediaDevices::defaultAudioOutput());
    m_audioOutput->setVolume(1.0f);
    m_player->setAudioOutput(m_audioOutput);

    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &MusicPlayer::playbackStateChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &MusicPlayer::onMediaStatusChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &MusicPlayer::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &MusicPlayer::durationChanged);
    connect(m_player, &QMediaPlayer::errorOccurred, this, &MusicPlayer::onErrorOccurred);
    connect(m_audioOutput, &QAudioOutput::volumeChanged, this, &MusicPlayer::volumeChanged);
    connect(m_audioOutput, &QAudioOutput::mutedChanged, this, &MusicPlayer::mutedChanged);
    connect(m_audioOutput, &QAudioOutput::deviceChanged, this, &MusicPlayer::onAudioOutputDeviceChanged);
    connect(m_mediaDevices, &QMediaDevices::audioOutputsChanged, this, &MusicPlayer::onAudioOutputsChanged);

#ifdef Q_OS_WIN
    m_windowsMediaControls = new WindowsMediaControls(this, this);
#endif
}

Song MusicPlayer::currentSong() const
{
    return m_currentSong;
}

bool MusicPlayer::hasSong() const
{
    return m_currentSong.id != INVALID_ID || !m_currentSong.data.name.isEmpty() || !m_currentSong.data.fileName.isEmpty();
}

bool MusicPlayer::hasSource() const
{
    return !m_player->source().isEmpty();
}

QMediaPlayer::PlaybackState MusicPlayer::playbackState() const
{
    return m_player->playbackState();
}

QMediaPlayer::MediaStatus MusicPlayer::mediaStatus() const
{
    return m_player->mediaStatus();
}

QMediaPlayer::Error MusicPlayer::error() const
{
    return m_player->error();
}

QString MusicPlayer::errorString() const
{
    return m_player->errorString();
}

qint64 MusicPlayer::position() const
{
    return m_player->position();
}

qint64 MusicPlayer::duration() const
{
    return m_player->duration();
}

float MusicPlayer::volume() const
{
    return m_audioOutput->volume();
}

bool MusicPlayer::isMuted() const
{
    return m_audioOutput->isMuted();
}

QAudioDevice MusicPlayer::audioOutputDevice() const
{
    return m_audioOutput->device();
}

void MusicPlayer::setSong(const Song &song)
{
    const bool songChanged = m_currentSong.id != song.id
                             || m_currentSong.data.fileName != song.data.fileName
                             || m_currentSong.data.name != song.data.name;
    m_currentSong = song;
    applySource(song.data.fileName);
    if (songChanged)
        emit currentSongChanged(m_currentSong);
}

void MusicPlayer::clear()
{
    m_player->stop();
    m_player->setSource(QUrl());
    if (hasSong() || !m_currentSong.data.fileName.isEmpty()) {
        m_currentSong = Song {};
        emit currentSongChanged(m_currentSong);
    }
}

void MusicPlayer::play()
{
    if (!hasSource()) {
        qWarning() << "MusicPlayer::play: no media source loaded.";
        return;
    }
    m_player->play();
}

void MusicPlayer::play(const Song &song)
{
    setSong(song);
    play();
}

void MusicPlayer::pause()
{
    if (m_player->playbackState() != QMediaPlayer::PlayingState)
        return;
    m_player->pause();
}

void MusicPlayer::resume()
{
    if (m_player->playbackState() != QMediaPlayer::PausedState)
        return;
    m_player->play();
}

void MusicPlayer::stop()
{
    if (m_player->playbackState() == QMediaPlayer::StoppedState
        && m_player->position() == 0) {
        return;
    }
    m_player->stop();
}

void MusicPlayer::togglePlayPause()
{
    if (m_player->playbackState() == QMediaPlayer::PlayingState)
        pause();
    else
        play();
}

void MusicPlayer::setPosition(qint64 positionMs)
{
    if (!hasSource())
        return;
    m_player->setPosition(clampedPosition(positionMs));
}

void MusicPlayer::seekForward(qint64 offsetMs)
{
    if (offsetMs < 0)
        offsetMs = -offsetMs;
    setPosition(position() + offsetMs);
}

void MusicPlayer::seekBackward(qint64 offsetMs)
{
    if (offsetMs < 0)
        offsetMs = -offsetMs;
    setPosition(position() - offsetMs);
}

void MusicPlayer::setVolume(float volume)
{
    m_audioOutput->setVolume(qBound(MinVolume, volume, MaxVolume));
}

void MusicPlayer::increaseVolume(float step)
{
    setVolume(volume() + qAbs(step));
}

void MusicPlayer::decreaseVolume(float step)
{
    setVolume(volume() - qAbs(step));
}

void MusicPlayer::setMuted(bool muted)
{
    m_audioOutput->setMuted(muted);
}

void MusicPlayer::toggleMute()
{
    setMuted(!isMuted());
}

void MusicPlayer::requestNext()
{
    emit nextRequested();
}

void MusicPlayer::requestPrevious()
{
    emit previousRequested();
}

void MusicPlayer::onAudioOutputsChanged()
{
    if (m_followDefaultOutput || !isCurrentDeviceAvailable())
        syncAudioOutputToDefault(true);
}

void MusicPlayer::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    emit mediaStatusChanged(status);
    if (status == QMediaPlayer::EndOfMedia)
        emit playbackFinished();
}

void MusicPlayer::onErrorOccurred(QMediaPlayer::Error error, const QString &errorString)
{
    emit errorOccurred(error, errorString);
}

void MusicPlayer::onAudioOutputDeviceChanged()
{
    emit audioOutputDeviceChanged(m_audioOutput->device());
}

void MusicPlayer::applySource(const QUrl &source)
{
    if (source.isEmpty()) {
        m_player->stop();
        m_player->setSource(QUrl());
        return;
    }

    if (source.isLocalFile()) {
        const QFileInfo info(source.toLocalFile());
        if (!info.exists() || !info.isFile()) {
            m_player->stop();
            m_player->setSource(QUrl());
            emit errorOccurred(QMediaPlayer::ResourceError,
                               QStringLiteral("Audio file does not exist: %1")
                                   .arg(source.toLocalFile()));
            return;
        }
    }

    if (m_player->source() == source) {
        m_player->setPosition(0);
        return;
    }

    m_player->stop();
    m_player->setSource(source);
}

void MusicPlayer::syncAudioOutputToDefault(bool force)
{
    const QAudioDevice defaultDevice = QMediaDevices::defaultAudioOutput();
    if (!force && m_audioOutput->device() == defaultDevice)
        return;
    m_audioOutput->setDevice(defaultDevice);
    m_followDefaultOutput = true;
}

qint64 MusicPlayer::clampedPosition(qint64 positionMs) const
{
    positionMs = qMax(0LL, positionMs);
    const qint64 total = m_player->duration();
    if (total > 0)
        positionMs = qMin(positionMs, total);
    return positionMs;
}

bool MusicPlayer::isCurrentDeviceAvailable() const
{
    const QAudioDevice current = m_audioOutput->device();
    if (current.isNull())
        return false;

    const QList<QAudioDevice> outputs = QMediaDevices::audioOutputs();
    for (const QAudioDevice &device : outputs) {
        if (device.id() == current.id())
            return true;
    }
    return false;
}
