#include "../include/assistant.h"
#include "ui_assistant.h"

Assistant::Assistant(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Assistant)
{
    ui->setupUi(this);
}

Assistant::~Assistant()
{
    delete ui;
}
