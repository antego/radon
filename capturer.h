#ifndef WORKER_H
#define WORKER_H

#include <QObject>

class Capturer : public QObject {
    Q_OBJECT

public:
    Capturer(int camId = 0, int frameWidth = 640, int frameHeight = 480);
    ~Capturer();

public slots:
    void process();

signals:
    void finished();
    void cameraStopped();
    void error(QString caption);

public:

private:
    const int camId, frameWidth, frameHeight;
};

#endif // WORKER_H
