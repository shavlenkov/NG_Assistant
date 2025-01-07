#include "../include/aiworker.h"

AIWorker::AIWorker(QObject *parent)
    : QObject{parent}
{
    m_manager = new QNetworkAccessManager(this);

    connect(m_manager, &QNetworkAccessManager::finished, this, &AIWorker::receivedResponse);
}

void AIWorker::sendOllamaRequest(QString prompt)
{
    QNetworkRequest request(QUrl("http://127.0.0.1:11434/api/generate"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json = {};

    json["model"] = "llama3.2";
    json["prompt"] = prompt;
    json["stream"] = false;

    m_manager->post(request, QJsonDocument(json).toJson());

    emit statusUpdate("Thinking...");
    emit startTypingAnimation();
}

void AIWorker::receivedResponse(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError) {
        emit responseReady(QJsonDocument::fromJson(reply->readAll()).object().value("response").toString());
        emit statusUpdate("Done!");
    } else {
        qDebug() << "Error: " << reply->errorString();
        emit statusUpdate(reply->errorString());
    }
}
