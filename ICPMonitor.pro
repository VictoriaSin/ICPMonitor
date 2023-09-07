TEMPLATE = subdirs

SUBDIRS += \
    core \      # Ядро приложения
    app \       # Приложение (GUI)
    tests       # Тестирование

CONFIG += ordered c++14

# Если необходимо замерить покрытие кода
contains(CONFIG, code_coverage){
    SUBDIRS -= app  # Убираем компиляцию приложения
    message(Disable App)
}

DISTFILES += README.md ProjectInfo.json
