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
    static QPair<QString, QString> executeLinuxCommand(const QString &command, const QStringList &args);
    static QString getName();
    static QString insertLineBreaks(const QString &text, int maxLength);
    static QStringList getPathsFromListWidget(QListWidget *pathListWidget);
    static bool isLinuxCommand(const QString &command);
    static bool isValidChatFormat(const QJsonDocument &chatDoc);
};

#endif // UTIL_H
