#include "../include/assistant.h"
#include "ui_assistant.h"

Assistant::Assistant(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Assistant)
    , controlPanel(nullptr)
{
    ui->setupUi(this);
    ui->label_2->setText(QString("Hello, %1! How can I help you?").arg(Util::getName()));
    ui->chat->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_chatMenu = menuBar()->addMenu("Chat");

    m_createChat = new QAction("Create Chat", this);
    m_saveChat = new QAction("Save Chat", this);
    m_loadChat = new QAction("Load Chat", this);

    m_chatMenu->addAction(m_createChat);
    m_chatMenu->addAction(m_saveChat);
    m_chatMenu->addAction(m_loadChat);

    m_infoMenu = menuBar()->addMenu("Info");

    m_aboutProject = new QAction("About Project", this);

    m_infoMenu->addAction(m_aboutProject);

    m_aiWorker = new AIWorker();
    m_aiThread = new QThread(this);
    m_aiThread->start();
    m_aiWorker->moveToThread(m_aiThread);

    connect(ui->sendUserMessageButton, &QPushButton::clicked, this, &Assistant::sendUserMessage);
    connect(ui->userMessageInputField, &QLineEdit::returnPressed, this, &Assistant::sendUserMessage);

    connect(this, &Assistant::sendOllamaRequest, m_aiWorker, &AIWorker::sendOllamaRequest);
    connect(m_aiWorker, &AIWorker::responseReady, this, &Assistant::sendAssistantMessage);
    connect(m_aiWorker, &AIWorker::statusUpdate, ui->statusbar, &QStatusBar::showMessage);
    connect(m_aiWorker, &AIWorker::startTypingAnimation, this, &Assistant::startTypingAnimation);

    connect(m_createChat, &QAction::triggered, this, &Assistant::createChat);
    connect(m_saveChat, &QAction::triggered, this, &Assistant::saveChat);
    connect(m_loadChat, &QAction::triggered, this, &Assistant::loadChat);

    connect(m_aboutProject, &QAction::triggered, this, &Assistant::showAboutProjectDialog);

    connect(ui->openControlPanelButton, &QPushButton::clicked, this, &Assistant::openControlPanel);
}

void Assistant::sendUserMessage()
{
    QString message = ui->userMessageInputField->text().trimmed();

    if(!message.isEmpty()) {
        ui->chat->addItem(
            Util::insertLineBreaks(
                QString("[%1] %2: %3").arg(QTime::currentTime().toString("hh:mm"), Util::getName(), message),
                MAX_LENGTH
            )
        );

        ui->userMessageInputField->clear();

        emit sendOllamaRequest(message);
    }
}

void Assistant::sendAssistantMessage(QString message)
{
    for(QTimer *timer : m_timers) {
        timer->stop();

        delete timer;
    }

    m_timers.clear();

    if(Util::isLinuxCommand(message)) {
        bool containsPath = false;

        if(!m_paths.empty()) {
            for(const QString &path : m_paths) {
                if(message.contains(path)) {
                    containsPath = true;
                    break;
                }
            }
        }

        if(!containsPath) {
            QPair<QString, QString> result = Util::executeLinuxCommand("bash", QStringList() << "-c" << message);

            if(QString(result.second).isEmpty()) {
                message = "Action completed successfully!";

                if(!QString(result.first).isEmpty()) {
                    message.append(QString("\n%1").arg(result.first));
                }
            } else {
                message = "Error!";
            }
        } else {
            message = "Forbidden!";
        }
    }

    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, [this, message, timer]() {
        ui->chat->item(ui->chat->count() - 1)->setText(
            Util::insertLineBreaks(
                QString("[%1] Assistant: %2").arg(QTime::currentTime().toString("hh:mm"), message.left(messageCharIndex)),
                MAX_LENGTH
            )
        );

        if(messageCharIndex == message.length()) {
            timer->stop();

            delete timer;

            QTimer::singleShot(1000, [this]() {
                ui->statusbar->clearMessage();
                ui->userMessageInputField->setDisabled(false);
            });

            messageCharIndex = 1;
        }

        messageCharIndex++;
    });

    timer->start(50);
}

void Assistant::createChat()
{
    ui->chat->clear();

    m_aiWorker->setContext(QJsonArray());

    QMessageBox::information(this, "Success", "Chat created successfully!");
}

void Assistant::saveChat()
{
    if(ui->chat->count() == 0) {
        QMessageBox::warning(this, "Warning", "The chat is empty!");

        return;
    }

    QJsonObject rootObject = {};

    rootObject["context"] = m_aiWorker->getContext();

    QJsonArray chatArray = {};

    for(int counter = 0; counter < ui->chat->count(); counter++) {
        QJsonObject messageObject = {};

        QString message = ui->chat->item(counter)->text();

        QRegularExpression regex(R"(\[(\d{2}:\d{2})\]\s+(\w+):\s+([\s\S]+))");
        QRegularExpressionMatch match = regex.match(message);

        if(match.hasMatch()) {
            messageObject["time"] = match.captured(1);
            messageObject["name"] = match.captured(2);
            messageObject["message"] = match.captured(3);
        }

        chatArray.append(messageObject);
    }

    rootObject["chat"] = chatArray;

    QJsonDocument chatDoc(rootObject);

    QString fileName = QFileDialog::getSaveFileName(
        nullptr,
        "Save Chat",
        "",
        "JSON Files (*.json)"
    );

    if(fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);

    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Error", "Couldn't save the chat!");

        return;
    }

    file.write(chatDoc.toJson(QJsonDocument::Indented));
    file.close();

    QMessageBox::information(this, "Success", "Chat saved successfully!");
}

void Assistant::loadChat()
{
    QString fileName = QFileDialog::getOpenFileName(
        nullptr,
        "Load Chat",
        "",
        "JSON Files (*.json)"
    );

    if(fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Couldn't open the chat file!");

        return;
    }

    QByteArray fileContent = file.readAll();
    file.close();

    QJsonDocument chatDoc = QJsonDocument::fromJson(fileContent);

    if(!Util::isValidChatFormat(chatDoc)) {
        QMessageBox::critical(this, "Error", "Invalid chat format!");

        return;
    }

    QJsonObject rootObject = chatDoc.object();

    QJsonArray contextArray = rootObject.value("context").toArray();
    QJsonArray chatArray = rootObject.value("chat").toArray();

    m_aiWorker->setContext(contextArray);

    ui->chat->clear();

    for(const QJsonValue &value : chatArray) {
        QString time = value.toObject().value("time").toString();
        QString name = value.toObject().value("name").toString();
        QString message = value.toObject().value("message").toString();

        if(!time.isEmpty() && !name.isEmpty() && !message.isEmpty()) {
            ui->chat->addItem(QString("[%1] %2: %3").arg(time, name, message));
        }
    }

    QMessageBox::information(this, "Success", "Chat loaded successfully!");
}

void Assistant::startTypingAnimation()
{
    ui->userMessageInputField->setDisabled(true);
    ui->chat->addItem(QString("[%1] Assistant: ").arg(QTime::currentTime().toString("hh:mm")));

    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, [this]() {
        if(dotCounter <= 3) {
            ui->chat->item(ui->chat->count() - 1)->setText(
                QString("[%1] Assistant: %2").arg(QTime::currentTime().toString("hh:mm"), QString(dotCounter, '.'))
            );

            dotCounter++;
        } else {
            ui->chat->item(ui->chat->count() - 1)->setText(
                QString("[%1] Assistant: ").arg(QTime::currentTime().toString("hh:mm"))
            );

            dotCounter = 0;
        }
    });

    m_timers.append(timer);

    timer->start(200);
}

void Assistant::openControlPanel()
{
    if(!controlPanel) {
        controlPanel = new ControlPanel(this);

        connect(controlPanel, &ControlPanel::pathsSelected, this, [this](const QStringList &paths) {
            m_paths = paths;
        });
    }

    controlPanel->show();
}

void Assistant::showAboutProjectDialog()
{
    QMessageBox aboutProjectDialog;

    aboutProjectDialog.setWindowTitle("About Project");
    aboutProjectDialog.setText(
        QString(
            "<b>Project Name</b>: %1<br/>"
            "<b>Project Description</b>: %2<br/>"
            "<b>Operating System</b>: %3<br/>"
            "<b>Technologies Used</b>: %4<br/>"
            "<b>Author</b>: <a href=\"%6\" style=\"color: #61dafb;\">%5</a><br/>"
            "<b>GitHub Repository</b>: <a href=\"%7\" style=\"color: #61dafb;\">%7</a>"
            "<br/><br/>"
            "&copy; %8 %5. All rights reserved."
        ).arg(
            PROJECT_NAME,
            PROJECT_DESCRIPTION,
            OPERATING_SYSTEM,
            TECHNOLOGIES_USED,
            AUTHOR_FULL_NAME,
            AUTHOR_GITHUB_URL,
            GITHUB_REPO_URL,
            COPYRIGHT_YEAR
        )
    );

    aboutProjectDialog.exec();
}

Assistant::~Assistant()
{
    m_aiThread->exit(0);

    delete m_aiWorker;
    delete ui;
    delete controlPanel;
}
