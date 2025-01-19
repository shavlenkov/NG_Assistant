#include "../include/aiworker.h"

AIWorker::AIWorker(QObject *parent)
    : QObject{parent}
{
    m_manager = new QNetworkAccessManager(this);
}

void AIWorker::sendOllamaRequest(QString prompt)
{
    QSettings settings("../../.env", QSettings::IniFormat);

    QNetworkRequest request(QUrl(settings.value("OLLAMA_URL").toString()));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json = {};

    json["model"] = "llama3.2";
    json["stream"] = false;

    if(m_context.empty()) {
        json["prompt"] = "You are the Linux assistant. "
                         "If the text is about creating, deleting, renaming, or moving a folder or a file, just reply with a Linux command (without extra text, "
                         "all paths must begin with \"~/\", example: "
                         "creating a folder: mkdir ~/folder, mkdir ~/Desktop/folder "
                         "crearing a file: touch ~/file.txt, touch ~/Desktop/file.txt "
                         "deleting a folder: rm -Rf ~/folder, rm -Rf ~/Desktop/folder "
                         "deleting a file: rm -f ~/file.txt, rm -f ~/Desktop/file.txt "
                         "renaming a folder: mv ~/old_folder ~/new_folder, mv ~/Desktop/old_folder ~/Desktop/new_folder "
                         "renaming a file: mv ~/old_file.txt ~/new_file.txt, mv ~/Desktop/old_file.txt ~/Desktop/new_file.txt "
                         "moving a folder: mv ~/folder ~/Desktop/, mv ~/Desktop/folder ~/ "
                         "moving a file: mv ~/file.txt ~/Desktop/, mv ~/Desktop/file.txt ~/), "
                         "and if it is about something else, just reply to the message.";
    } else {
        json["prompt"] = prompt;
        json["context"] = m_context;
    }

    QNetworkReply *reply = m_manager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply, prompt]() {
        if(m_context.empty()) {
            m_context = QJsonDocument::fromJson(reply->readAll()).object().value("context").toArray();

            sendOllamaRequest(prompt);
        } else {
            receivedResponse(reply);
        }

        reply->deleteLater();
    });

    emit statusUpdate("Thinking...");

    if(!m_animationStarted) {
        emit startTypingAnimation();

        m_animationStarted = true;
    }
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

    m_animationStarted = false;
}
