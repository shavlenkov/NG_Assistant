#include "../include/assistant.h"
#include "ui_assistant.h"

Assistant::Assistant(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Assistant)
{
    ui->setupUi(this);
    ui->label_2->setText(QString("Hello, %1! How can I help you?").arg(getName()));
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

Assistant::~Assistant()
{
    delete ui;
}
