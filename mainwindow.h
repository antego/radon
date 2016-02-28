#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "capturer.h"

#include <QMainWindow>
#include <QDir>

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
    void handleCamButton();
    void takePicture();
    void openFolder();
    void closeIfNeeded();
    void ackShot();
    void doRadon();

private:
    Ui::MainWindow *ui;
    Capturer* capturer;
    QThread* thread;
    QRegularExpression* regexp;
    QDir* currentDir;
    bool cameraEnabled;
    bool closeRequested;
    void startCamera();
    void stopCamera();
    void closeEvent(QCloseEvent *event);
    void reloadFolder();
};

#endif // MAINWINDOW_H
