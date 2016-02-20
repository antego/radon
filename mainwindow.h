#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <capturer.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void confirmCameraStop();
    void handleError(QString errorCaption);

private slots:
    void handleButton();

private:
    Ui::MainWindow *ui;
    Capturer* capturer;
    QThread* thread;
    bool cameraEnabled;
    void startCamera();
    void stopCamera();
};

#endif // MAINWINDOW_H
