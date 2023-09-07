#ifndef LANGUAGESETTING_H
#define LANGUAGESETTING_H

#include <QLocale>
#include <QTranslator>
#include <memory>

/*! Настройки языка приложения */
class LanguageSetting
{
    /*! Транслятор для одного файла */
    struct TransItem{
        QTranslator translator;
        QString filePath;

        TransItem(const QString &transFile);
        void setTranslator();
        void removeTranslator();
        QString getFilePath() const;
    };

    /*! Контейнер с файлами перевода (Русский язык по умолчанию)
        key - язык;
        value - список с файлами перевода.
    */
    QHash<QLocale::Language,
        std::shared_ptr<
            QList<std::shared_ptr<
                TransItem>>>> mLangFiles {{std::make_pair(QLocale::Language::Russian, nullptr)}};

    /*! Текущий язык приложения */
    QLocale::Language mCurrentLanguage {QLocale::Language::Russian};

public:
    LanguageSetting();

    /*! Регистрация файла перевода lang формате *.qm */
    bool registrateLangFile(QLocale::Language lang, const QString &langFilePath);

    /*! Установка языка приложения */
    bool setAppLanguage(QLocale::Language lang);

    /*! Есть ли такой язык в системе */
    bool isContainsLanguage(QLocale::Language lang) const;

    /*! Возвращает текущий установленный язык */
    QLocale::Language getCurrentLanguage() const;

    /*! Список доступных языков */
    QList<QLocale::Language> getSupportedLanguages() const;

    /*! Есть ли данный файл перевода */
    bool isContainsLangFile(const QString &langFilePath) const;
};

#endif // LANGUAGESETTING_H
