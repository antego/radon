#ifndef RADON_SCANNER_H
#define RADON_SCANNER_H

#include <QDir>
#include <QObject>

class RadonScanner : public QObject
{
    Q_OBJECT

public:
    RadonScanner(int dTheta, int zScale, std::vector<float> angles, QFileInfoList fileList);
    ~RadonScanner();

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

#endif // RADON_SCANNER_H
