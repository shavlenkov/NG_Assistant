#include "../include/assistant.h"

#include <QApplication>
#include <QFile>
#include <QDebug>

void loadApplicationStyleSheet(QApplication *application)
{
    QFile styleSheetFile(":/qss/stylesheet.qss");

    if(!styleSheetFile.exists()) {
        qDebug() << "Error: The stylesheet file doesn't exist!";

        return;
    }

    if(!styleSheetFile.open(QFile::ReadOnly)) {
        qDebug() << "Error: Failed to open the stylesheet file!";

        return;
    }

    application->setStyleSheet(styleSheetFile.readAll());
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    loadApplicationStyleSheet(&a);

    Assistant w;
    w.show();

    return a.exec();
}
