#include "../include/controlpanel.h"
#include "ui_controlpanel.h"

ControlPanel::ControlPanel(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ControlPanel)
{
    ui->setupUi(this);

    connect(ui->selectFolderButton, &QPushButton::clicked, this, &ControlPanel::selectFolder);
    connect(ui->removeFolderButton, &QPushButton::clicked, this, &ControlPanel::removeFolder);
}

void ControlPanel::selectFolder()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, "Select Folder");

    if(!folderPath.isEmpty() && ui->paths->findItems(folderPath, Qt::MatchExactly).isEmpty()) {
        if(folderPath == "/" || folderPath == "/home") {
            return;
        }

        ui->paths->addItem(folderPath);
    }

    QStringList paths = {};

    for(int counter = 0; counter < ui->paths->count(); counter++) {
        paths.append(ui->paths->item(counter)->text().replace(QDir::homePath(), "~"));
    }

    emit pathsSelected(paths);
}

void ControlPanel::removeFolder()
{
    QListWidgetItem *selectedPath = ui->paths->currentItem();

    if(!selectedPath) {
        return;
    }

    delete selectedPath;

    QStringList paths = {};

    for(int counter = 0; counter < ui->paths->count(); counter++) {
        paths.append(ui->paths->item(counter)->text().replace(QDir::homePath(), "~"));
    }

    emit pathsSelected(paths);
}

ControlPanel::~ControlPanel()
{
    delete ui;
}
