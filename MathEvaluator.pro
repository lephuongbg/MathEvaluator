#-------------------------------------------------
#
# Project created by QtCreator 2012-01-06T23:08:57
#
#-------------------------------------------------

QT       += core gui

TARGET = MathEvaluator
TEMPLATE = app


SOURCES += main.cpp\
        evaluator.cpp \
    oplus.cpp \
    omultiply.cpp \
    ominus.cpp \
    odivide.cpp \
    big_num.cpp \
    misc.cpp \
    lex.cpp

HEADERS  += evaluator.h \
    big_num.h \
    lex.h

FORMS    += evaluator.ui

RESOURCES += \
    icon.qrc

TRANSLATIONS = ME_vi.ts

CODECFORTR = UTF-8

CODECFORSRC = UTF-8
