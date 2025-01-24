#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QFileDialog>
#include <QDir>
#include <QListWidgetItem>

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
    void removeFolder();

signals:
    void pathsSelected(QStringList paths);

private:
    Ui::ControlPanel *ui;
};

#endif // CONTROLPANEL_H
