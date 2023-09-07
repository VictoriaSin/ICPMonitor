TEMPLATE = subdirs

CONFIG += ordered

# Если необходимо замерить покрытие кода
contains(CONFIG, code_coverage){
    CONFIG += gunit_stable
    message(Code Coverage Make)
}

# Если необходимо тестирование
contains(CONFIG, gunit_alpha|gunit_beta|gunit_stable){
    SUBDIRS += GUnitTests
    message(GUnitTests Enabled)
}
