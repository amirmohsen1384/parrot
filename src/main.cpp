#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QTime>
#include <QVBoxLayout>
#include <QWidget>

#include "elements/musicplayer.h"
#include "repository/databasemanager.h"

namespace
{
    QString formatMs(qint64 ms)
    {
        if (ms < 0)
            ms = 0;
        return QTime(0, 0).addMSecs(static_cast<int>(ms % (24 * 60 * 60 * 1000))).toString(QStringLiteral("mm:ss"));
    }

    QString playbackStateText(QMediaPlayer::PlaybackState state)
    {
        switch (state) {
        case QMediaPlayer::PlayingState:
            return QStringLiteral("Playing");
        case QMediaPlayer::PausedState:
            return QStringLiteral("Paused");
        case QMediaPlayer::StoppedState:
            return QStringLiteral("Stopped");
        }
        return QStringLiteral("Unknown");
    }

    class PlaybackMockWindow : public QWidget
    {
    public:
        explicit PlaybackMockWindow(MusicPlayer *player, QWidget *parent = nullptr)
            : QWidget(parent)
            , m_player(player)
        {
            setWindowTitle(QStringLiteral("Parrot — Playback / Headset Mock"));
            resize(520, 320);

            m_songLabel = new QLabel(QStringLiteral("No song loaded"));
            m_songLabel->setWordWrap(true);
            m_stateLabel = new QLabel(playbackStateText(m_player->playbackState()));
            m_positionLabel = new QLabel(formatMs(0));
            m_durationLabel = new QLabel(formatMs(0));
            m_volumeLabel = new QLabel(QStringLiteral("100%"));
            m_eventLabel = new QLabel(QStringLiteral("Waiting for headset / media-key events…"));
            m_eventLabel->setWordWrap(true);
            m_errorLabel = new QLabel;
            m_errorLabel->setStyleSheet(QStringLiteral("color: #b00020;"));
            m_errorLabel->setWordWrap(true);

            m_positionSlider = new QSlider(Qt::Horizontal);
            m_positionSlider->setRange(0, 0);
            m_volumeSlider = new QSlider(Qt::Horizontal);
            m_volumeSlider->setRange(0, 100);
            m_volumeSlider->setValue(100);

            auto *openButton = new QPushButton(QStringLiteral("Open audio file…"));
            auto *playButton = new QPushButton(QStringLiteral("Play"));
            auto *pauseButton = new QPushButton(QStringLiteral("Pause"));
            auto *stopButton = new QPushButton(QStringLiteral("Stop"));
            auto *backButton = new QPushButton(QStringLiteral("−5s"));
            auto *forwardButton = new QPushButton(QStringLiteral("+5s"));

            auto *controls = new QHBoxLayout;
            controls->addWidget(openButton);
            controls->addWidget(playButton);
            controls->addWidget(pauseButton);
            controls->addWidget(stopButton);
            controls->addWidget(backButton);
            controls->addWidget(forwardButton);

            auto *form = new QFormLayout;
            form->addRow(QStringLiteral("Song"), m_songLabel);
            form->addRow(QStringLiteral("State"), m_stateLabel);
            form->addRow(QStringLiteral("Position"), m_positionLabel);
            form->addRow(QStringLiteral("Duration"), m_durationLabel);
            form->addRow(QStringLiteral("Seek"), m_positionSlider);
            form->addRow(QStringLiteral("Volume"), m_volumeSlider);
            form->addRow(QStringLiteral("Volume %"), m_volumeLabel);
            form->addRow(QStringLiteral("Last event"), m_eventLabel);
            form->addRow(QStringLiteral("Error"), m_errorLabel);

            auto *hint = new QLabel(
                QStringLiteral("Tip: keep this window open, start playback, then use your headset "
                               "play/pause/stop/next/previous and volume controls. "
                               "You can also pass a file path as the first CLI argument."));
            hint->setWordWrap(true);

            auto *layout = new QVBoxLayout(this);
            layout->addLayout(controls);
            layout->addLayout(form);
            layout->addWidget(hint);
            layout->addStretch();

            connect(openButton, &QPushButton::clicked, this, &PlaybackMockWindow::openSong);
            connect(playButton, &QPushButton::clicked, this, [this]() { m_player->play(); });
            connect(pauseButton, &QPushButton::clicked, m_player, &MusicPlayer::pause);
            connect(stopButton, &QPushButton::clicked, m_player, &MusicPlayer::stop);
            connect(backButton, &QPushButton::clicked, this, [this]() {
                m_player->seekBackward(5000);
                noteEvent(QStringLiteral("UI seek backward 5s"));
            });
            connect(forwardButton, &QPushButton::clicked, this, [this]() {
                m_player->seekForward(5000);
                noteEvent(QStringLiteral("UI seek forward 5s"));
            });

            connect(m_positionSlider, &QSlider::sliderPressed, this, [this]() {
                m_seekDragging = true;
            });
            connect(m_positionSlider, &QSlider::sliderReleased, this, [this]() {
                m_seekDragging = false;
                m_player->setPosition(m_positionSlider->value());
            });

            connect(m_volumeSlider, &QSlider::valueChanged, this, [this](int value) {
                if (m_updatingVolumeUi)
                    return;
                m_player->setVolume(value / 100.0f);
            });

            connect(m_player, &MusicPlayer::currentSongChanged, this, [this](const Song &song) {
                const QString title = !song.data.name.isEmpty()
                                          ? song.data.name
                                          : song.data.fileName.toLocalFile();
                m_songLabel->setText(title.isEmpty() ? QStringLiteral("No song loaded") : title);
            });
            connect(m_player, &MusicPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state) {
                m_stateLabel->setText(playbackStateText(state));
                noteEvent(QStringLiteral("playbackStateChanged → %1").arg(playbackStateText(state)));
            });
            connect(m_player, &MusicPlayer::positionChanged, this, [this](qint64 positionMs) {
                m_positionLabel->setText(formatMs(positionMs));
                if (!m_seekDragging) {
                    m_positionSlider->blockSignals(true);
                    m_positionSlider->setValue(static_cast<int>(positionMs));
                    m_positionSlider->blockSignals(false);
                }
            });
            connect(m_player, &MusicPlayer::durationChanged, this, [this](qint64 durationMs) {
                m_durationLabel->setText(formatMs(durationMs));
                m_positionSlider->setRange(0, static_cast<int>(qMax(0LL, durationMs)));
            });
            connect(m_player, &MusicPlayer::volumeChanged, this, [this](float volume) {
                m_updatingVolumeUi = true;
                const int percent = qRound(volume * 100.0f);
                m_volumeSlider->setValue(percent);
                m_volumeLabel->setText(QStringLiteral("%1%").arg(percent));
                m_updatingVolumeUi = false;
            });
            connect(m_player, &MusicPlayer::mutedChanged, this, [this](bool muted) {
                noteEvent(muted ? QStringLiteral("muted") : QStringLiteral("unmuted"));
            });
            connect(m_player, &MusicPlayer::playbackFinished, this, [this]() {
                noteEvent(QStringLiteral("playbackFinished"));
            });
            connect(m_player, &MusicPlayer::errorOccurred, this, [this](QMediaPlayer::Error, const QString &errorString) {
                m_errorLabel->setText(errorString);
                noteEvent(QStringLiteral("error: %1").arg(errorString));
            });
            connect(m_player, &MusicPlayer::nextRequested, this, [this]() {
                noteEvent(QStringLiteral("headset/media nextRequested"));
            });
            connect(m_player, &MusicPlayer::previousRequested, this, [this]() {
                noteEvent(QStringLiteral("headset/media previousRequested"));
            });
            connect(m_player, &MusicPlayer::audioOutputDeviceChanged, this, [this](const QAudioDevice &device) {
                noteEvent(QStringLiteral("audio device → %1").arg(device.description()));
            });
        }

        void loadPath(const QString &path)
        {
            const QFileInfo info(path);
            if (!info.exists() || !info.isFile()) {
                m_errorLabel->setText(QStringLiteral("File not found: %1").arg(path));
                return;
            }

            Song song;
            song.data.name = info.completeBaseName();
            song.data.fileName = QUrl::fromLocalFile(info.absoluteFilePath());
            m_errorLabel->clear();
            m_player->play(song);
            noteEvent(QStringLiteral("loaded + play: %1").arg(info.absoluteFilePath()));
        }

    private:
        void openSong()
        {
            const QString path = QFileDialog::getOpenFileName(
                this,
                QStringLiteral("Open audio file"),
                QString(),
                QStringLiteral("Audio (*.mp3 *.wav *.flac *.ogg *.m4a *.aac);;All files (*.*)"));
            if (!path.isEmpty())
                loadPath(path);
        }

        void noteEvent(const QString &text)
        {
            m_eventLabel->setText(text);
        }

        MusicPlayer *m_player = nullptr;
        QLabel *m_songLabel = nullptr;
        QLabel *m_stateLabel = nullptr;
        QLabel *m_positionLabel = nullptr;
        QLabel *m_durationLabel = nullptr;
        QLabel *m_volumeLabel = nullptr;
        QLabel *m_eventLabel = nullptr;
        QLabel *m_errorLabel = nullptr;
        QSlider *m_positionSlider = nullptr;
        QSlider *m_volumeSlider = nullptr;
        bool m_seekDragging = false;
        bool m_updatingVolumeUi = false;
    };
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    DatabaseManager::instance().initialize();

    MusicPlayer player;
    PlaybackMockWindow window(&player);
    window.show();

    if (argc > 1)
        window.loadPath(QString::fromLocal8Bit(argv[1]));

    return app.exec();
}
