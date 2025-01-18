#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QFileDialog>

namespace Ui {
class ControlPanel;
}

class ControlPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit ControlPanel(QWidget *parent = nullptr);
    ~ControlPanel();

private slots:
    void selectFolder();

signals:
    void pathsSelected(QStringList paths);

private:
    Ui::ControlPanel *ui;

    QString getUsername();
};

#endif // CONTROLPANEL_H
