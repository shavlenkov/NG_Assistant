#include "../include/controlpanel.h"
#include "ui_controlpanel.h"

ControlPanel::ControlPanel(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ControlPanel)
{
    ui->setupUi(this);

    connect(ui->selectFolderButton, &QPushButton::clicked, this, &ControlPanel::selectFolder);
}

QString ControlPanel::getUsername()
{
    QProcess process;

    process.start("bash", QStringList() << "-c" << "whoami");
    process.waitForFinished();

    QString output = process.readAllStandardOutput().trimmed();

    if(output.isEmpty()) {
        return QString("anonymous");
    }

    return output;
}

void ControlPanel::selectFolder()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, "Select Folder");

    if(!folderPath.isEmpty() && ui->paths->findItems(folderPath, Qt::MatchExactly).isEmpty()) {
        ui->paths->addItem(folderPath);
    }

    QStringList paths = {};

    for(int counter = 0; counter < ui->paths->count(); counter++) {
        paths.append(ui->paths->item(counter)->text().replace(QString("/home/%1").arg(getUsername()), "~"));
    }

    emit pathsSelected(paths);
}

ControlPanel::~ControlPanel()
{
    delete ui;
}
