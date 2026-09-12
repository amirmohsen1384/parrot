#ifndef WINDOWSMEDIACONTROLS_H
#define WINDOWSMEDIACONTROLS_H

#include <QAbstractNativeEventFilter>
#include <QObject>

class MusicPlayer;
class QWindow;

// Windows-only bridge for headset / keyboard / system media keys.
// Uses System Media Transport Controls (SMTC) plus WM_APPCOMMAND.
class WindowsMediaControls : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit WindowsMediaControls(MusicPlayer *player, QObject *parent = nullptr);
    ~WindowsMediaControls() override;

    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

private slots:
    void handleSmtcButton(int button);
    void syncPlaybackStatus();
    void syncDisplayMetadata();

private:
    void installAppCommandFilter();
    void uninstallAppCommandFilter();
    bool initializeSmtc();
    void shutdownSmtc();
    bool handleAppCommand(int command);

    MusicPlayer *m_player = nullptr;
    QWindow *m_sessionWindow = nullptr;
    bool m_filterInstalled = false;

    struct SmtcState;
    SmtcState *m_smtc = nullptr;
};

#endif // WINDOWSMEDIACONTROLS_H
