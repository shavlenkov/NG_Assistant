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

    emit pathsSelected(Util::getPathsFromListWidget(ui->paths));
}

void ControlPanel::removeFolder()
{
    QListWidgetItem *selectedPath = ui->paths->currentItem();

    if(!selectedPath) {
        return;
    }

    delete selectedPath;

    emit pathsSelected(Util::getPathsFromListWidget(ui->paths));
}

ControlPanel::~ControlPanel()
{
    delete ui;
}
