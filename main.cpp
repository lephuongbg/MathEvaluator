#include <QtGui/QApplication>
#include <QTranslator>
#include "evaluator.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    evaluator w;
    w.setFixedSize(w.size());
    w.show();
    
    return a.exec();
}
