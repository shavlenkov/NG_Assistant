#include "../include/util.h"

QPair<QString, QString> Util::executeLinuxCommand(QString command, QStringList args)
{
    QProcess process;

    process.start(command, args);
    process.waitForFinished();

    QString output = process.readAllStandardOutput().trimmed();
    QString error = process.readAllStandardError().trimmed();

    return qMakePair(output, error);
}

QString Util::getName()
{
    QPair<QString, QString> result = executeLinuxCommand("bash", QStringList() << "-c" << "getent passwd $USER | cut -d: -f5 | cut -d, -f1");

    if(QString(result.first).isEmpty() || !QString(result.second).isEmpty()) {
        return QString("Anonymous");
    }

    return result.first;
}

QString Util::insertLineBreaks(QString text, int maxLength)
{
    QStringList formattedTextParts = {};

    for(const QString &part : text.split("\n")) {
        for(int start = 0; start < part.length(); start += maxLength) {
            formattedTextParts.append(part.mid(start, maxLength));
        }
    }

    return QString(formattedTextParts.join("\n"));
}

QStringList Util::getPathsFromListWidget(QListWidget *pathListWidget)
{
    QStringList paths = {};

    for(int counter = 0; counter < pathListWidget->count(); counter++) {
        paths.append(pathListWidget->item(counter)->text().replace(QDir::homePath(), "~"));
    }

    return paths;
}

bool Util::isLinuxCommand(QString command)
{
    return !QString(executeLinuxCommand("which", QStringList() << command.split(" ")[0]).first).isEmpty();
}

bool Util::isValidChatFormat(QJsonDocument chatDoc)
{
    if(!chatDoc.isObject() ||
       !chatDoc.object().contains("chat") ||
       !chatDoc.object().contains("context") ||
       chatDoc.object()["chat"].toArray().isEmpty() ||
       chatDoc.object()["context"].toArray().isEmpty())
    {
        return false;
    }

    for(const QJsonValue &value : chatDoc.object()["chat"].toArray()) {
        if(!value.isObject() ||
           !value.toObject().contains("message") ||
           !value.toObject().contains("name") ||
           !value.toObject().contains("time") ||
           !value.toObject()["message"].isString() ||
           !value.toObject()["name"].isString() ||
           !value.toObject()["time"].isString())
        {
            return false;
        }
    }

    for(const QJsonValue &value : chatDoc.object()["context"].toArray()) {
        if(value.isDouble() && value.toDouble() != value.toInt()) {
            return false;
        }

        if(value.isString()) {
            return false;
        }
    }

    return true;
}
