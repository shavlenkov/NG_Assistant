#ifndef AIWORKER_H
#define AIWORKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QSettings>

class AIWorker : public QObject
{
    Q_OBJECT
public:
    explicit AIWorker(QObject *parent = nullptr);

private slots:
    void receivedResponse(QNetworkReply *reply);

public slots:
    void sendOllamaRequest(QString prompt);

signals:
    void responseReady(QString text);
    void statusUpdate(QString text, int timeout = 0);
    void startTypingAnimation();

private:
    QNetworkAccessManager *m_manager;
    QJsonArray m_context;

    bool m_animationStarted = false;
};

#endif // AIWORKER_H
