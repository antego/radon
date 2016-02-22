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
    void handleCamButton();
    void takePicture();
    void openFolder();

private:
    Ui::MainWindow *ui;
    Capturer* capturer;
    QThread* thread;
    QRegularExpression* regexp;
    bool cameraEnabled;
    void startCamera();
    void stopCamera();
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
