#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <QtGui/QApplication>
#include <QWidget>
#include <QFutureWatcher>
#include <QListWidget>
#include <QProgressDialog>
#include <QPropertyAnimation>
#include <QMenu>
#include <QTranslator>
#include <QWidgetAction>
#include <QtConcurrentRun>
#include <QFile>
#include <QCheckBox>
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

    void on_copyButton_clicked();

    void on_viewStdExp_toggled(bool checked);

    void enable_viewStdExp();

    void on_input_textEdited(const QString &arg1);

    void move2His();
    void move2Eval();

    void readHistory();
    bool saveHistory();

    void on_historyList_itemDoubleClicked(QListWidgetItem *item);

    void on_viewHisResButton_clicked();

    void on_clearButton_clicked();

    void on_historyButton_clicked();

    void changeUI(int state);

private:
    QFutureWatcher<big_num> watcher;
    QProgressDialog *evalProgress;
    QPropertyAnimation *evalAnimation;
    QPropertyAnimation *hisAnimation;
    QMenu *optionMenu;
    QTranslator translator;
    QCheckBox *enUIbox;
    QWidgetAction *enUItext;

public:
    Ui::evaluator *ui;
    lex *lx;
};

//Misc
int precedence (char op1);
void do_math(char op, stack<big_num>& number);
#endif // EVALUATOR_H
