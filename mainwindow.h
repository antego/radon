#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "capturer.h"
#include "scannerradon.h"
#include "scanneriradon.h"

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
    void doIRadon();
    void handleRadonFinish();

private:
    Ui::MainWindow *ui;
    Capturer* capturer;
    QThread* capThread;
    ScannerRadon* scannerRadon;
    ScannerIRadon* scannerIRadon;
    QThread* scanThread;
    QRegularExpression* regexp;
    QDir* currentDir;
    QFileInfoList fileList;
    bool cameraEnabled;
    bool closeRequested;
    void startCamera();
    void stopCamera();
    void closeEvent(QCloseEvent *event);
    void reloadFolder();
};

#endif // MAINWINDOW_H
