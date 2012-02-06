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
#include <QTimer>
#include "big_num.h"
#include "lex.h"
#include "aboutdialog.h"


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

    void readHistory();
    bool saveHistory();

    void on_historyList_itemDoubleClicked(QListWidgetItem *item);

    void on_viewHisResButton_clicked();

    void on_clearButton_clicked();

    void disable_on_calculating();
    void enable_after_calculating();

    void on_uiBox_toggled(bool checked);

    void on_listWidget_currentRowChanged(int currentRow);

    void on_aboutButton_clicked();

    void changeWgtPage(int page);
    void nextWgtPage();

private:
    QFutureWatcher<void> watcher;

    QPropertyAnimation *evalAnimation;
    QPropertyAnimation *hisAnimation;

    QTranslator translator;

public:
    Ui::evaluator *ui;
    lex *lx;
    aboutDialog *aD;
};

//Misc
int precedence (char op1);
void do_math(char op, stack<big_num>& number);
#endif // EVALUATOR_H
