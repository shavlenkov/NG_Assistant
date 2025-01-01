#include "../include/assistant.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Assistant w;
    w.show();
    return a.exec();
}
