#include "evaluator.h"
#include "ui_evaluator.h"

/********************************************
 *             GUI DEFINITION               *
 ********************************************/
evaluator::evaluator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::evaluator)
{
    ui->setupUi(this);
    translator.load("ME_vi");
    QCoreApplication::installTranslator(&translator);
    ui->retranslateUi(this);

    lx = new lex;

    //Calculating progress bar
    ui->evalProgressBar->hide();
    ui->evalProgressBar->setMinimum(0);
    ui->evalProgressBar->setMaximum(0);
    connect(&watcher,SIGNAL(finished()),ui->evalProgressBar,SLOT(hide()));

    //Disable some elements upon calculating
    connect(&watcher,SIGNAL(started()),this,SLOT(disable_on_calculating()));

    //Reenable some elements after done calculating
    connect(&watcher,SIGNAL(finished()),this,SLOT(enable_after_calculating()));
    connect(lx,SIGNAL(success()),this,SLOT(enable_viewStdExp()));

    //Animation
    evalAnimation = new QPropertyAnimation(ui->evalWidget,"geometry");
    hisAnimation = new QPropertyAnimation(ui->historyWidget,"geometry");

    //Read history when start
    readHistory();

    //Connect evaluation class info to the program
    connect(lx,SIGNAL(error(QString)),ui->output,SLOT(setText(QString)));
    connect(lx,SIGNAL(result_str(QString)),ui->output,SLOT(setText(QString)));

    //Save successfully evaluated expression
    connect(lx,SIGNAL(success()),this,SLOT(saveHistory()));

    //Add option menu
    optionMenu = new QMenu;
    enUIbox = new QCheckBox(tr("English interface"),optionMenu);
    enUItext = new QWidgetAction(optionMenu);
    enUItext->setDefaultWidget(enUIbox);
    optionMenu->addAction(enUItext);
    connect(enUIbox,SIGNAL(stateChanged(int)),this,SLOT(changeUI(int)));
    ui->optionButton->setMenu(optionMenu);
}

evaluator::~evaluator()
{
    delete ui;
    delete lx;
    delete evalAnimation;
    delete hisAnimation;
    delete optionMenu;
}

/********************************************
 *         ACTIVATING ELEMENTS              *
 ********************************************/

void evaluator::on_evaluateButton_clicked()
{
    QString expression = ui->input->text();
    if (expression == "")
        return;

    //Put the expression into evaluation class
    lx->set(expression.toStdString());

    //Excute evaluation process in another thread
    QFuture<void> future = QtConcurrent::run(lx, &lex::excute);
    watcher.setFuture(future);

    //Show progress
    ui->evalProgressBar->show();
}

void evaluator::on_input_returnPressed()
{
    on_evaluateButton_clicked();
}

void evaluator::on_copyButton_clicked()
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

void evaluator::on_input_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1);
    ui->viewStdExp->setDisabled(true);
}

void evaluator::on_historyList_itemDoubleClicked(QListWidgetItem *item)
{
    ui->input->setText(item->text());
    on_evaluateButton_clicked();
    move2Eval();
}

void evaluator::on_viewHisResButton_clicked()
{
    ui->input->setText(ui->historyList->currentItem()->text());
    on_evaluateButton_clicked();
}

void evaluator::on_clearButton_clicked()
{
    QFile file("history");
    file.resize(0);
    readHistory();
}

void evaluator::on_historyButton_clicked()
{
    if (lx->standardized() == "")
        ui->historyList->setCurrentRow(0);
    else
    {
        QList<QListWidgetItem*> list = ui->historyList->findItems(lx->standardized(), Qt::MatchExactly);
        ui->historyList->setCurrentItem(list.at(0));
    }
}

/********************************************
 *               ANIMATION                  *
 ********************************************/

void evaluator::move2His()
{
    //Set up animation for history widget
    hisAnimation->setDuration(200);
    hisAnimation->setStartValue(QRect(451,0,451,352));
    hisAnimation->setEndValue(QRect(0,0,451,352));
    hisAnimation->start();

    //Setup animation for evaluation widget
    evalAnimation->setDuration(200);
    evalAnimation->setStartValue(QRect(0,0,451,352));
    evalAnimation->setEndValue(QRect(-451,0,451,352));
    evalAnimation->start();
}

void evaluator::move2Eval()
{
    //Set up animation for history widget
    hisAnimation->setDuration(200);
    hisAnimation->setStartValue(QRect(0,0,451,352));
    hisAnimation->setEndValue(QRect(451,0,451,352));
    hisAnimation->start();

    //Setup animation for evaluation widget
    evalAnimation->setDuration(200);
    evalAnimation->setStartValue(QRect(-451,0,451,352));
    evalAnimation->setEndValue(QRect(0,0,451,352));
    evalAnimation->start();
}

/********************************************
 *             READ/SAVE HISTORY            *
 ********************************************/

void evaluator::readHistory()
{
    ui->historyList->clear();
    QFile file("history");
    if(file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        while (!stream.atEnd())
        {
            ui->historyList->addItem(stream.readLine());
        }

        file.close();
    }
    if (ui->historyList->count() == 0)
        ui->viewHisResButton->setDisabled(true);
    else
        ui->viewHisResButton->setEnabled(true);

}

bool evaluator::saveHistory()
{
    //Search for current expression in history
    QList<QListWidgetItem *> find = ui->historyList->findItems(lx->standardized(), Qt::MatchExactly);

    //Only if it doesn't exist in history, save it
    if (find.size() == 0) {
        QFile file("history");
        if(file.open(QIODevice::Append)) {
            QTextStream stream(&file);
            stream << lx->standardized() << '\n';
            stream.flush();
            file.close();
            readHistory();
            return 1;
        }
    }
    return 0;
}

/********************************************
 *          ENABLE/DISABLE ELEMENTS         *
 ********************************************/
void evaluator::enable_after_calculating()
{
    ui->viewHisResButton->setEnabled(true);
    ui->output->setEnabled(true);
    ui->evaluateButton->setEnabled(true);
    ui->copyButton->setEnabled(true);
}

void evaluator::disable_on_calculating()
{
    ui->viewHisResButton->setDisabled(true);
    ui->output->setDisabled(true);
    ui->evaluateButton->setDisabled(true);
    ui->copyButton->setDisabled(true);
}


void evaluator::enable_viewStdExp()
{
    ui->viewStdExp->setEnabled(true);
}

/********************************************
 *          CHANGE INTERFACE                *
 ********************************************/

void evaluator::changeUI(int state)
{
    if (state == Qt::Checked)
    {
        QCoreApplication::removeTranslator(&translator);
        enUIbox->setText(tr("English interface"));
        ui->retranslateUi(this);
    } else {
        QCoreApplication::installTranslator(&translator);
        enUIbox->setText(tr("English interface"));
        ui->retranslateUi(this);
    }
}
