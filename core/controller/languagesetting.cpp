#include "languagesetting.h"
#include "global_functions.h"

#include <QCoreApplication>
#include "QDebug"

LanguageSetting::LanguageSetting()
{

}

bool LanguageSetting::registrateLangFile(QLocale::Language lang, const QString &langFilePath)
{
    // Если указанный файл не корректный
    if (!isGoodFile(langFilePath)) {
        return false;
    }

    // Если такого языка в приложении ещё не было
    if (!mLangFiles.contains(lang)) {
        mLangFiles.insert(lang, std::make_shared<QList<std::shared_ptr<TransItem>>>());
    }

    // Если файл перевода уже есть
    if (isContainsLangFile(langFilePath)) {
        return false;
    }

    // Добавляем в список файл с переводом
    const auto langList = mLangFiles.value(lang);
    langList->append(std::make_shared<TransItem>(langFilePath));

    return true;
}

bool LanguageSetting::setAppLanguage(QLocale::Language lang)
{
    // Если язык и так установлен
    if (lang == mCurrentLanguage) {
        return true;
    }

    // Если данного языка нет в системе
    if (!isContainsLanguage(lang)) {
        return false;
    }

    // Если текущий язык не русский
    if (mCurrentLanguage != QLocale::Language::Russian) {
        // Удаляем текущие трансляторы приложения
        for(const auto &el : qAsConst(*mLangFiles.value(mCurrentLanguage).get())){
            el->removeTranslator();
        }
    }

    // Если устанавливаемый язык не русский
    if (lang != QLocale::Language::Russian) {
        // Устанавливаем новые трансляторы приложения
        for(const auto &el : qAsConst(*mLangFiles.value(lang).get())){
            el->setTranslator();
        }
    }

    // Запоминаем текущий язык
    mCurrentLanguage = lang;
    return true;
}

bool LanguageSetting::isContainsLanguage(QLocale::Language lang) const
{
    return mLangFiles.contains(lang);
}

QLocale::Language LanguageSetting::getCurrentLanguage() const
{
    return mCurrentLanguage;
}

QList<QLocale::Language> LanguageSetting::getSupportedLanguages() const
{
    QList<QLocale::Language> mLangList;

    // Обход хэша
    auto it = mLangFiles.constBegin();
    while (it != mLangFiles.constEnd()) {
        mLangList.append(it.key());
        ++it;
    }

    return mLangList;
}

bool LanguageSetting::isContainsLangFile(const QString &langFilePath) const
{
    // Обход языков
    auto itHash = mLangFiles.constBegin();
    while (itHash != mLangFiles.constEnd()) {
        // Для русского языка скип
        if (itHash.key() == QLocale::Language::Russian) {
            ++itHash;
            continue;
        }

        // Берём список с трансляторами
        const auto TransItemList = itHash.value();

        // Обход списков с трансляторами
        auto itTransItemList = TransItemList->constBegin();
        while (itTransItemList != TransItemList->constEnd()) {
            if (itTransItemList->get()->getFilePath() == langFilePath) {
                return true;
            }
            ++itTransItemList;
        }

        ++itHash;
    }

    return false;
}

LanguageSetting::TransItem::TransItem(const QString &transFile)
{
    filePath = transFile;
    translator.load(filePath);
}

void LanguageSetting::TransItem::setTranslator()
{
    qApp->installTranslator(&translator);
}

void LanguageSetting::TransItem::removeTranslator()
{
    qApp->removeTranslator(&translator);
}

QString LanguageSetting::TransItem::getFilePath() const
{
    return filePath;
}
