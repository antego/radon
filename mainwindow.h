#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <worker.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void handleStopButton();
    void handleStartButton();

private:
    Ui::MainWindow *ui;
    Worker* worker;
    QThread* thread;
};

#endif // MAINWINDOW_H
