#ifndef VERSION_H
#define VERSION_H

/*! Semantic Versioning

    Read https://semver.org
*/
namespace version {

#define VERSION_MAJOR 0 // мажорная версия
#define VERSION_MINOR 5 // минорная версия
#define VERSION_PATCH 0 // версия патча

#ifndef BUILD_INFO
//#define BUILD_INFO // доп. информация о сборке (напр. release,..), может быть задана вручную здесь или при компиляции, если требуется
#endif

//#define VERSION_BUILD - версия (номер) сборки, должна задаваться при компиляции, если требуется

#define M__STRINGIFY(x) #x
#define M_STRINGIFY(x) M__STRINGIFY(x)

#define M_PROJECT_VERSION M_STRINGIFY(VERSION_MAJOR.VERSION_MINOR.VERSION_PATCH)

#ifdef VERSION_BUILD
#ifndef BUILD_INFO
#define PROJECT_VERSION M_PROJECT_VERSION M_STRINGIFY(-VERSION_BUILD)
#else
#define PROJECT_VERSION M_PROJECT_VERSION M_STRINGIFY(-BUILD_INFO.VERSION_BUILD)
#endif
#else
#ifndef BUILD_INFO
#define PROJECT_VERSION M_PROJECT_VERSION
#else
#define PROJECT_VERSION M_PROJECT_VERSION M_STRINGIFY(-BUILD_INFO)
#endif
#endif

/*! Версия программы с версией (номером) сборки и доп. информацией

  Блок <Info> появляется, если задан дефайн BUILD_INFO
  Блок <Build> появляется, если задан дефайн VERSION_BUILD
  Блок <.> появляется, если есть <Info> и <Build>

  Шаблон версии: "Major.Minor.Patch-<Info><.><Build>"
  Пример: "1.0.1-21"
 */
constexpr const char *VERSION_STRING = PROJECT_VERSION;

/*! Версия программы без версии (номера) сборки

  Шаблон версии: "Major.Minor.Patch"
  Пример: "1.0.1"
 */
constexpr const char *VERSION_STRING2 = M_PROJECT_VERSION;
}

#endif // VERSION_H
