#ifndef ASSISTANT_H
#define ASSISTANT_H

#include <QMainWindow>

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
};
#endif // ASSISTANT_H
