#include "../include/assistant.h"

#include <QApplication>
#include <QDebug>

void loadApplicationStyleSheet(QApplication *application)
{
    QFile styleSheetFile(":/css/stylesheet.css");

    if(!styleSheetFile.exists()) {
        qDebug() << "Error: The stylesheet file does not exist!";

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
