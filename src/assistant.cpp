#include "../include/assistant.h"
#include "ui_assistant.h"

Assistant::Assistant(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Assistant)
{
    ui->setupUi(this);
    ui->label_2->setText(QString("Hello, %1! How can I help you?").arg(getName()));

    connect(ui->sendMessageButton, &QPushButton::clicked, this, &Assistant::sendMessage);
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

void Assistant::sendMessage()
{
    QString message = ui->messageInputField->toPlainText().trimmed();

    if(!message.isEmpty()) {
        ui->chat->addItem(QTime::currentTime().toString("[hh:mm] ") + getName() + ": " + message);

        ui->messageInputField->clear();
    }
}

Assistant::~Assistant()
{
    delete ui;
}
