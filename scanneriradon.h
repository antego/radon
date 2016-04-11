#ifndef SCANNERIRADON_H
#define SCANNERIRADON_H

#include <QDir>
#include <QObject>

class ScannerIRadon : public QObject
{
    Q_OBJECT

public:
    enum shaftOrientation {VERTICAL, HORIZONTAL};
    ScannerIRadon(int dK, int dRho, QFileInfoList fileList, std::vector<float> angles, shaftOrientation shaft);
    ~ScannerIRadon();

signals:
    void finished();
    void error(QString caption);
    void setStepsCount(int count);
    void setCurrentCount(int count);

public slots:
    void scan();

public:
    shaftOrientation shaft;
    int dK;
    int dRho;
    QFileInfoList fileList;
    std::vector<float> angles;

};

#endif // SCANNERIRADON_H
