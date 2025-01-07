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

public slots:
    void sendAssistantMessage(QString message);
    void startTypingAnimation();

signals:
    void sendOllamaRequest(QString prompt);

private:
    Ui::Assistant *ui;

    QMenu *m_chatMenu;
    QAction *m_createChat;
    QAction *m_saveChat;
    QAction *m_loadChat;

    QThread *m_aiThread;
    AIWorker *m_aiWorker;

    QList<QTimer*> m_timers;

    const int MAX_LENGTH = 59;

    int dotCounter = 0;

    QString getName();
    QString insertLineBreaks(QString text, int maxLength);
};
#endif // ASSISTANT_H
