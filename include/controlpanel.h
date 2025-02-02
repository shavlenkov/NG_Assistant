#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>

#include "util.h"

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
    void pathsSelected(const QStringList &paths);

private:
    Ui::ControlPanel *ui;
};

#endif // CONTROLPANEL_H
