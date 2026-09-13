#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QObject>
#include <QAudioDevice>
#include <QMediaPlayer>

#include "domain.h"

class QAudioOutput;
class QMediaDevices;
class WindowsMediaControls;

class MusicPlayer : public QObject
{
    Q_OBJECT
public:
    explicit MusicPlayer(QObject *parent = nullptr);

    bool hasSong() const;
    bool hasSource() const;
    Song currentSong() const;

    QMediaPlayer::PlaybackState playbackState() const;
    QMediaPlayer::MediaStatus mediaStatus() const;
    QMediaPlayer::Error error() const;
    QString errorString() const;

    qint64 position() const;
    qint64 duration() const;

    float volume() const;
    bool isMuted() const;
    QAudioDevice audioOutputDevice() const;

public slots:
    void setSong(const Song &song);
    void clear();

    void play();
    void play(const Song &song);
    void pause();
    void resume();
    void stop();
    void togglePlayPause();

    void setPosition(qint64 positionMs);
    void seekForward(qint64 offsetMs);
    void seekBackward(qint64 offsetMs);

    void setVolume(float volume);
    void increaseVolume(float step = 0.05f);
    void decreaseVolume(float step = 0.05f);
    void setMuted(bool muted);
    void toggleMute();

    void requestNext();
    void requestPrevious();

signals:
    void currentSongChanged(const Song &song);
    void playbackStateChanged(QMediaPlayer::PlaybackState state);
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void positionChanged(qint64 positionMs);
    void durationChanged(qint64 durationMs);
    void volumeChanged(float volume);
    void mutedChanged(bool muted);
    void audioOutputDeviceChanged(const QAudioDevice &device);
    void playbackFinished();
    void errorOccurred(QMediaPlayer::Error error, const QString &errorString);
    void nextRequested();
    void previousRequested();

private slots:
    void onAudioOutputsChanged();
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onErrorOccurred(QMediaPlayer::Error error, const QString &errorString);
    void onAudioOutputDeviceChanged();

private:
    void applySource(const QUrl &source);
    void syncAudioOutputToDefault(bool force);
    qint64 clampedPosition(qint64 positionMs) const;
    bool isCurrentDeviceAvailable() const;

    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
    QMediaDevices *m_mediaDevices = nullptr;
#ifdef Q_OS_WIN
    WindowsMediaControls *m_windowsMediaControls = nullptr;
#endif
    Song m_currentSong;
    bool m_followDefaultOutput = true;
};

#endif // MUSICPLAYER_H
