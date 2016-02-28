#ifndef CAPTURER_H
#define CAPTURER_H

#include <opencv2/core/core.hpp>
#include <QObject>

class Capturer : public QObject {
    Q_OBJECT

public:
    Capturer(int camId = 0, int frameWidth = 640, int frameHeight = 480);
    ~Capturer();
    void requestShot(QString pictureName);

public slots:
    void process();

signals:
    void finished();
    void cameraStopped();
    void error(QString caption);
    void shotTaken();

private:
    const int camId, frameWidth, frameHeight;
    QString pictureName;
    bool shotRequested;
    void saveFrame(const cv::Mat& frame);
};

#endif // CAPTURER_H
