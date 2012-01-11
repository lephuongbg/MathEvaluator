#include <QtGui/QApplication>
#include <QTranslator>
#include "evaluator.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    translator.load("MathEvaluator_vi.qm");
    a.installTranslator(&translator);

    evaluator w;
    w.show();
    
    return a.exec();
}
