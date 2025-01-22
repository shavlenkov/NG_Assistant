#ifndef UTIL_H
#define UTIL_H

#include <QPair>
#include <QString>
#include <QStringList>
#include <QProcess>

class Util
{
public:
    static QPair<QString, QString> executeLinuxCommand(QString command, QStringList args);
    static QString getName();
    static QString insertLineBreaks(QString text, int maxLength);
    static bool isLinuxCommand(QString command);
};

#endif // UTIL_H
