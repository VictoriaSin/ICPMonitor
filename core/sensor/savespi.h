#ifndef SAVESPI_H
#define SAVESPI_H

#include <QObject>
#include <QFile>
#include <QThread>

class SaveSPI : public QThread
{
    Q_OBJECT

    QFile SPIData;
public:
    SaveSPI();
protected:
    void run();
public slots:
    void fillFile();
};

#endif // SAVESPI_H
