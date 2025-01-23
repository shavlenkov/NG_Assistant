#include "../include/aiworker.h"

AIWorker::AIWorker(QObject *parent)
    : QObject{parent}
{
    m_manager = new QNetworkAccessManager(this);
}

QJsonArray AIWorker::getContext()
{
    return m_context;
}

void AIWorker::setContext(QJsonArray context)
{
    m_context = context;
}

void AIWorker::sendOllamaRequest(QString prompt)
{
    QSettings settings(".env", QSettings::IniFormat);

    QNetworkRequest request(QUrl(settings.value("OLLAMA_URL").toString()));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json = {};

    json["model"] = "llama3.2";
    json["stream"] = false;

    if(m_context.empty()) {
        json["prompt"] = "You are the Linux assistant. "
                         "If the text is about creating, deleting, renaming, or moving a folder or a file, writing text to a file, reading text from a file, just reply with a Linux command (without extra text, "
                         "all paths must begin with \"~/\", example: "
                         "creating a folder: mkdir ~/folder, mkdir ~/Desktop/folder "
                         "crearing a file: touch ~/file.txt, touch ~/Desktop/file.txt "
                         "deleting a folder: rm -Rf ~/folder, rm -Rf ~/Desktop/folder "
                         "deleting a file: rm -f ~/file.txt, rm -f ~/Desktop/file.txt "
                         "renaming a folder: mv ~/old_folder ~/new_folder, mv ~/Desktop/old_folder ~/Desktop/new_folder "
                         "renaming a file: mv ~/old_file.txt ~/new_file.txt, mv ~/Desktop/old_file.txt ~/Desktop/new_file.txt "
                         "moving a folder: mv ~/folder ~/Desktop/, mv ~/Desktop/folder ~/ "
                         "moving a file: mv ~/file.txt ~/Desktop/, mv ~/Desktop/file.txt ~/ "
                         "writing text to a file: echo \"text\" > ~/file.txt, echo \"text\" > ~/Desktop/file.txt "
                         "reading text from a file: cat ~/file.txt, cat ~/Desktop/file.txt), "
                         "and if the text is about something else (example: What is C++? What color is the sky? How high is Everest? Write the text of Lorem Ipsum), "
                         "just reply to the message in text without a Linux command.";
    } else {
        json["prompt"] = prompt;
        json["context"] = m_context;
    }

    QNetworkReply *reply = m_manager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply, prompt]() {
        if(reply->error() == QNetworkReply::NoError) {
            QJsonDocument responseDoc = QJsonDocument::fromJson(reply->readAll());

            if(!responseDoc.isObject()) {
                emit statusUpdate("Invalid JSON response");
                reply->deleteLater();

                return;
            }

            QJsonObject responseObject = responseDoc.object();

            if(!responseObject.contains("response") || !responseObject.contains("context")) {
                emit statusUpdate("The JSON response doesn't contain the expected fields");
                reply->deleteLater();

                return;
            }

            if(m_context.isEmpty()) {
                m_context = responseObject.value("context").toArray();

                sendOllamaRequest(prompt);
            } else {
                m_context = responseObject.value("context").toArray();

                emit responseReady(responseObject.value("response").toString());
                emit statusUpdate("Done!");
            }
        } else {
            qDebug() << "Error: " << reply->errorString();
            emit statusUpdate(reply->errorString());
        }

        m_animationStarted = false;

        reply->deleteLater();
    });

    emit statusUpdate("Thinking...");

    if(!m_animationStarted) {
        m_animationStarted = true;

        emit startTypingAnimation();
    }
}
