#ifndef ASSISTANT_H
#define ASSISTANT_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QTime>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QFile>
#include <QFileDialog>
#include <QByteArray>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QThread>
#include <QStatusBar>
#include <QList>
#include <QTimer>

#include "aiworker.h"
#include "controlpanel.h"
#include "util.h"
#include "config.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Assistant;
}
QT_END_NAMESPACE

class Assistant : public QMainWindow
{
    Q_OBJECT

public:
    Assistant(QWidget *parent = nullptr);
    ~Assistant();

private slots:
    void sendUserMessage();

    void createChat();
    void saveChat();
    void loadChat();

    void openControlPanel();

    void showAboutProjectDialog();

public slots:
    void sendAssistantMessage(QString message);
    void startTypingAnimation();

signals:
    void sendOllamaRequest(QString prompt);

private:
    Ui::Assistant *ui;
    ControlPanel *controlPanel;

    QMenu *m_chatMenu;
    QAction *m_createChat;
    QAction *m_saveChat;
    QAction *m_loadChat;

    QMenu *m_infoMenu;
    QAction *m_aboutProject;

    QThread *m_aiThread;
    AIWorker *m_aiWorker;

    QList<QTimer*> m_timers;
    QStringList m_paths;

    const int MAX_LENGTH = 63;

    int dotCounter = 0;
    int messageCharIndex = 1;
};
#endif // ASSISTANT_H
