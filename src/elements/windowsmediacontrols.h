#ifndef WINDOWSMEDIACONTROLS_H
#define WINDOWSMEDIACONTROLS_H

#include <QObject>
#include <QAbstractNativeEventFilter>

class QWindow;
class MusicPlayer;

class WindowsMediaControls : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit WindowsMediaControls(MusicPlayer *player, QObject *parent = nullptr);
    ~WindowsMediaControls() override;

    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

private slots:
    void syncPlaybackStatus();
    void syncDisplayMetadata();
    void handleSmtcButton(int button);

private:
    void shutdownSmtc();
    bool initializeSmtc();
    void installAppCommandFilter();
    void uninstallAppCommandFilter();
    bool handleAppCommand(int command);

    bool m_filterInstalled = false;
    MusicPlayer* m_player = nullptr;
    QWindow* m_sessionWindow = nullptr;

    struct SmtcState;
    SmtcState *m_smtc = nullptr;
};

#endif // WINDOWSMEDIACONTROLS_H
