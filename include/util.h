#ifndef UTIL_H
#define UTIL_H

#include <QPair>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QListWidget>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>

class Util
{
public:
    static QPair<QString, QString> executeLinuxCommand(QString command, QStringList args);
    static QString getName();
    static QString insertLineBreaks(QString text, int maxLength);
    static QStringList getPathsFromListWidget(QListWidget *pathListWidget);
    static bool isLinuxCommand(QString command);
    static bool isValidChatFormat(QJsonDocument chatDoc);
};

#endif // UTIL_H
