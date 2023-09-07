#ifndef SCREENWRITER_H
#define SCREENWRITER_H
#include <memory>

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QDir>

#include "global_enums.h"

class BlockDevice;

class ScreenWriter : public QObject
{
    Q_OBJECT

    /*! Для асинхронного запуска скриншота из командной строки */
    QProcess *mProcess {nullptr};

    /*! Таймер для контроля зависания программы */
    QTimer *mKillTimer {nullptr};

    /*! Время, через которое сработает уничтожение процесса */
    const int KillTimeMs {15000};

    /*! Относительный путь хранения скриншотов */
    QString mRelativePath {"./ICPPicture"};

    /*! Максимальное кол-во фотографий в папке */
    uint mMaxScreens = 50;

    /*! Папка хранения скриншотов
        Меняется при смене блочного
        устройства
    */
    QDir *mScreensDir {nullptr};

    /*! Блочное устройство, куда ведётся запись скринов */
    std::shared_ptr<BlockDevice> mBlockDevice;

public:
    explicit ScreenWriter(const QString &relativeScreensPath, uint maxScreens, QObject *parent = nullptr);

    /*! Записываем скриншот из фрейм-буфера Linux (/dev/fb0)
        Будет работать только если видеокарта рисует в него
    */
    void writeScreenFromLinuxFB();

    /*! Записываем скриншот из QPixmap виджета */
    void writeScreenFromWidget(const QPixmap &screen);

    /*! Готов ли писать скриншоты */
    bool isReadyWriteScreen() const;

    /*! Установка блочного устройства */
    void setBlockDevice(const std::shared_ptr<BlockDevice> &blockDev);

    /*! Возвращает блочное устройство */
    std::shared_ptr<BlockDevice> getBlockDevice() const { return mBlockDevice; };

    /*! Установка относительного пути */
    void setRelativePath(const QString &relativePath);

    /*! Возвращает относительный путь */
    QString getRelativePath() const;

    /*! Установка максимального значения скриншотов в папке */
    void setMaxScreens(uint maxScreens);

    /*! Возвращает максимально допустимое число фотографий в папке*/
    uint getMaxScreens() const;

    /*! Генерация имени для скриншота
        format - формат изображения.
    */
    static QString genNameOfScreen(const QString &format);

    /*! Преобразование PPM формата в доступный формат из ConversionPictureFormat::.
        Если конвертация не удалась, возвращается пустая QString().
        Если конвертация успешна, то возвращается путь к сконвертированному файлу.
     */
    static QString conversionPPMtoAny(const QString& ppmPicturePath, ConversionPictureFormat format);

    /*! Конвертирует и перемещает фотографию */
    static bool convertAndMovePicture(const QString &sourcePicturePath, const QString &targetPath, ConversionPictureFormat format);

private:
    /*! Инициализация процесса для создания скриншота
        при помощи программы fbcat
    */
    void initFBCatProcess();

    /*! Удаляет старые скриншоты из текущей папки
        до максимально установленного кол-ва
    */
    bool deleteOldScreens();

    /*! Создание папки для скринов
        Если папка создана или существует,
        то возвращается true. Иначе false.
    */
    bool makeScreensDir();

    /*! Сброс папки скриншотов */
    void resetScreensDir();

    /*! Готова ли папка для скриншотов */
    bool isReadyScreenDir() const;

    /*! Готово ли блочное устройство к взаимодействию */
    bool isReadyBlockDev();

private slots:
    /*! Запись скрина закончена */
    void writeFinished(int exitCode, QProcess::ExitStatus exitStatus);

signals:
    /*! Статус окончания записи скриншота */
    void screenWrittenFinished(bool isWrited);
};

#endif // SCREENWRITER_H
