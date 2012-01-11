#include "evaluator.h"
#include "ui_evaluator.h"
#include "lex.h"
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QProgressDialog>


/********************************************
 *             GUI DEFINITION               *
 ********************************************/
evaluator::evaluator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::evaluator)
{
    ui->setupUi(this);
    ui->retranslateUi(this);

    lx = new lex;

    evalProgress = new QProgressDialog(this, Qt::WindowTitleHint);
    evalProgress->setWindowIcon(QIcon(":/Tutorial.ico"));
    evalProgress->setLabelText(tr("Calculating..."));
    evalProgress->setMinimum(0);
    evalProgress->setMaximum(0);
    evalProgress->setCancelButton(0);
    evalProgress->setFixedSize(evalProgress->size());
    connect(&watcher, SIGNAL(finished()), evalProgress, SLOT(close()));
}

evaluator::~evaluator()
{
    delete ui;
    delete lx;
    delete evalProgress;
}

void evaluator::on_evaluateButton_clicked()
{
    QString expression = ui->input->text();

    //Connect error info to output
    connect(lx,SIGNAL(error(QString)),ui->output,SLOT(setText(QString)));

    //Put the expression into evaluation class
    lx->set(expression.toStdString());

    //Show progress for long operation




    //Move evaluating process to another thread
    QFuture<big_num> future = QtConcurrent::run(lx, &lex::evaluate);
    watcher.setFuture(future);
    evalProgress->exec();


    //Enable view standard expression after done calculation
    ui->viewStdExp->setEnabled(true);
    if (ui->viewStdExp->isChecked())
        ui->input->setText(lx->standardized());
    //Get the result
    big_num result = future.result();

    //Display the result
    if (!(result.sign == 0))
    {
        QString out = QString::fromStdString(result.number);
        if (result.sign == -1)
            out.insert(0,'-');
        ui->output->setPlainText(out);
    } else {
        //Add more error info
        QString currentText = ui->output->toHtml();
        currentText.append(tr("<span style='color:red'><b>Calculation error!!!<b></span>"));
        ui->output->setText(currentText);
    }
}

void evaluator::on_input_returnPressed()
{
    on_evaluateButton_clicked();
}

void evaluator::on_pushButton_clicked()
{
    ui->output->selectAll();
    ui->output->copy();
    ui->output->moveCursor(QTextCursor::Start);
}

void evaluator::on_viewStdExp_toggled(bool checked)
{
    if (checked && lx->standardized() != "")
        ui->input->setText(lx->standardized());
    else
        ui->input->setText(QString::fromStdString(lx->input));
}

void evaluator::enable_viewStdExp()
{
    ui->viewStdExp->setEnabled(true);
}

void evaluator::on_input_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1);
    ui->viewStdExp->setDisabled(true);
}
