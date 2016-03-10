#ifndef SCANNER_H
#define SCANNER_H

#include <QDir>
#include <QObject>

class Scanner : public QObject
{
    Q_OBJECT

public:
    enum shaftOrientation {VERTICAL, HORIZONTAL};
    Scanner(int dK, int dRho, QFileInfoList fileList, std::vector<float> angles, shaftOrientation shaft);
    ~Scanner();

signals:
    void finished();
    void error(QString caption);

public slots:
    void scan();

public:
    shaftOrientation shaft;
    int dK;
    int dRho;
    QFileInfoList fileList;
    std::vector<float> angles;

};

#endif // SCANNER_H
