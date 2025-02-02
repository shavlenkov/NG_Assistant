#ifndef AIWORKER_H
#define AIWORKER_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
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

    QJsonArray getContext() const;
    void setContext(const QJsonArray &context);

public slots:
    void sendOllamaRequest(const QString &prompt);

signals:
    void responseReady(const QString &text);
    void statusUpdate(const QString &text, int timeout = 0);
    void startTypingAnimation();

private:
    QNetworkAccessManager *m_manager;
    QJsonArray m_context = {};

    bool m_animationStarted = false;
};

#endif // AIWORKER_H
