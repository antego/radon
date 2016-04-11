#ifndef SCANNERRADON_H
#define SCANNERRADON_H

#include <QDir>
#include <QObject>

class ScannerRadon : public QObject
{
    Q_OBJECT

public:
    ScannerRadon(int dTheta, int zScale, std::vector<float> angles, QFileInfoList fileList);
    ~ScannerRadon();

signals:
    void finished();
    void error(QString caption);
    void setStepsCount(int count);
    void setCurrentCount(int count);

public slots:
    void scan();

public:
    int dTheta;
    int zScale;
    QFileInfoList fileList;
    std::vector<float> angles;
};

#endif // SCANNERRADON_H
