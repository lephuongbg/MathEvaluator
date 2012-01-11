#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <QWidget>
#include <QFutureWatcher>
#include <QProgressDialog>
#include <QPushButton>
#include "big_num.h"
#include "lex.h"


namespace Ui {
class evaluator;
}

class evaluator : public QWidget
{
    Q_OBJECT

public:
    explicit evaluator(QWidget *parent = 0);
    ~evaluator();

private slots:
    void on_evaluateButton_clicked();

    void on_input_returnPressed();

    void on_pushButton_clicked();

    void on_viewStdExp_toggled(bool checked);

    void enable_viewStdExp();

    void on_input_textEdited(const QString &arg1);

private:
    QFutureWatcher<big_num> watcher;
    QProgressDialog *evalProgress;

public:
    Ui::evaluator *ui;
    lex *lx;
};

//Misc
int precedence (char op1);
void do_math(char op, stack<big_num>& number);
#endif // EVALUATOR_H
