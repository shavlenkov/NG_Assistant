#include "../include/assistant.h"
#include "ui_assistant.h"

Assistant::Assistant(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Assistant)
{
    ui->setupUi(this);
    ui->label_2->setText(QString("Hello, %1! How can I help you?").arg(getName()));
    ui->chat->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    qApp->setStyleSheet(
        "QMessageBox QLabel { background-color: #2b2b2b; color: #61dafb; font-size: 14px; }"
        "QPushButton { background-color: #444; color: #61dafb }"
    );

    m_chatMenu = menuBar()->addMenu("Chat");

    m_createChat = new QAction("Create Chat", this);
    m_saveChat = new QAction("Save Chat", this);
    m_loadChat = new QAction("Load Chat", this);

    m_chatMenu->addAction(m_createChat);
    m_chatMenu->addAction(m_saveChat);
    m_chatMenu->addAction(m_loadChat);

    menuBar()->setStyleSheet(
        "QMenuBar { background-color: #000; color: #61dafb; }"
        "QMenuBar::item { background-color: transparent; color: #61dafb; }"
        "QMenuBar::item:selected { background-color: #444; }"
        "QMenuBar::item:pressed { background-color: #666; }"
    );

    m_chatMenu->setStyleSheet(
        "QMenu { background-color: #000; color: #61dafb; }"
        "QMenu::item { background-color: transparent; color: #61dafb; }"
        "QMenu::item:selected { background-color: #444; }"
        "QMenu::item:pressed { background-color: #666; }"
    );

    m_aiWorker = new AIWorker();
    m_aiThread = new QThread(this);
    m_aiThread->start();
    m_aiWorker->moveToThread(m_aiThread);

    connect(ui->sendMessageButton, &QPushButton::clicked, this, &Assistant::sendUserMessage);
    connect(ui->messageInputField, &QLineEdit::returnPressed, this, &Assistant::sendUserMessage);

    connect(this, &Assistant::sendOllamaRequest, m_aiWorker, &AIWorker::sendOllamaRequest);
    connect(m_aiWorker, &AIWorker::responseReady, this, &Assistant::sendAssistantMessage);
    connect(m_aiWorker, &AIWorker::statusUpdate, ui->statusbar, &QStatusBar::showMessage);
    connect(m_aiWorker, &AIWorker::startTypingAnimation, this, &Assistant::startTypingAnimation);

    connect(m_createChat, &QAction::triggered, this, &Assistant::createChat);
    connect(m_saveChat, &QAction::triggered, this, &Assistant::saveChat);
    connect(m_loadChat, &QAction::triggered, this, &Assistant::loadChat);
}

QString Assistant::getName()
{
    QProcess process;

    process.start("bash", QStringList() << "-c" << "getent passwd $USER | cut -d: -f5 | cut -d, -f1");
    process.waitForFinished();

    QString output = process.readAllStandardOutput().trimmed();

    if(output.isEmpty()) {
        return QString("Anonymous");
    }

    return output;
}

QString Assistant::insertLineBreaks(QString text, int maxLength)
{
    QStringList textParts = {};

    for(int start = 0; start < text.length(); start += maxLength) {
        textParts.append(text.mid(start, maxLength));
    }

    return QString(textParts.join("\n"));
}

void Assistant::sendUserMessage()
{
    QString message = ui->messageInputField->text().trimmed();

    if(!message.isEmpty()) {
        ui->chat->addItem(
            insertLineBreaks(
                QString("[%1] %2: %3").arg(QTime::currentTime().toString("hh:mm"), getName(), message),
                MAX_LENGTH
            )
        );

        ui->messageInputField->clear();

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

    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, [this, message, timer]() {
        ui->chat->item(ui->chat->count() - 1)->setText(
            insertLineBreaks(
                QString("[%1] Assistant: %2").arg(QTime::currentTime().toString("hh:mm"), message.left(messageCharIndex)),
                MAX_LENGTH
            )
        );

        if(messageCharIndex == message.length()) {
            timer->stop();
            delete timer;

            QTimer::singleShot(1000, [this]() {
                ui->statusbar->clearMessage();
                ui->messageInputField->setDisabled(false);
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

    QMessageBox::information(this, "Success", "Chat created successfully!");
}

void Assistant::saveChat()
{
    if(ui->chat->count() == 0) {
        QMessageBox::warning(this, "Warning", "The chat is empty!");

        return;
    }

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

    QJsonDocument chatDoc(chatArray);

    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Save Chat",
        "",
        "JSON Files (*.json)"
    );

    if(fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);

    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Error", "Could not save chat!");

        return;
    }

    file.write(chatDoc.toJson(QJsonDocument::Indented));
    file.close();

    QMessageBox::information(this, "Success", "Chat saved successfully!");
}

void Assistant::loadChat()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Load Chat",
        "",
        "JSON Files (*.json)"
    );

    if(fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Could not open chat file!");

        return;
    }

    QByteArray fileContent = file.readAll();
    file.close();

    QJsonDocument chatDoc = QJsonDocument::fromJson(fileContent);

    if(!chatDoc.isArray()) {
        QMessageBox::critical(this, "Error", "Invalid chat format!");

        return;
    }

    ui->chat->clear();

    QJsonArray chatArray = chatDoc.array();

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
    ui->messageInputField->setDisabled(true);
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

Assistant::~Assistant()
{
    m_aiThread->exit(0);

    delete m_aiWorker;
    delete ui;
}
