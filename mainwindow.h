#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "capturer.h"
#include "radon_scanner.h"
#include "inverse_radon_scanner.h"

#include <QMainWindow>
#include <QDir>
#include <QtCore>

#include "ui_mainwindow.h"


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
    void doInverseRadon();
    void handleRadonFinish();
    void generateTestImages();

private:
    Ui_MainWindow ui;
    QScopedPointer<QRegularExpression> regexp;
    QScopedPointer<QDir> currentDir;
    QFileInfoList fileList;
    bool cameraEnabled;
    bool closeRequested;

    Capturer* capturer;
    QThread* capThread;
    RadonScanner* radonScanner;
    InverseRadonScanner* inverseRadonScanner;
    QThread* scanThread;

    void startCamera();
    void stopCamera();
    void closeEvent(QCloseEvent *event);
    void reloadFolder();
};

#endif // MAINWINDOW_H
