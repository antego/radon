#ifndef INVERSE_RADON_SCANNER_H
#define INVERSE_RADON_SCANNER_H

#include <QDir>
#include <QObject>

class InverseRadonScanner : public QObject
{
    Q_OBJECT

public:
    enum shaftOrientation {VERTICAL, HORIZONTAL};
    InverseRadonScanner(int dK,
                        int dRho,
                        QFileInfoList fileList,
                        std::vector<float> angles,
                        shaftOrientation shaft);
    ~InverseRadonScanner();

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

#endif // INVERSE_RADON_SCANNER_H
