#ifndef ASSISTANT_H
#define ASSISTANT_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui {
class Assistant;
}
QT_END_NAMESPACE

class Assistant : public QMainWindow
{
    Q_OBJECT

public:
    Assistant(QWidget *parent = nullptr);
    ~Assistant();

private:
    Ui::Assistant *ui;

    QString getName();
};
#endif // ASSISTANT_H
