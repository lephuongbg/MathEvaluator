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

    //Read history when start
    readHistory();

    //Connect evaluation class info to the program
    connect(lx,SIGNAL(error(QString)),ui->output,SLOT(setText(QString)));
    connect(lx,SIGNAL(result_str(QString)),ui->output,SLOT(setText(QString)));

    //Save successfully evaluated expression
    connect(lx,SIGNAL(success()),this,SLOT(saveHistory()));

    //About Dialog
    aD = new aboutDialog(this);
    aD->setFixedSize(aD->size());

    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(changeWgtPage(int)));
}

evaluator::~evaluator()
{
    delete ui;
    delete lx;
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
    ui->listWidget->setCurrentRow(0);
}

void evaluator::on_viewHisResButton_clicked()
{
    ui->input->setText(ui->historyList->currentItem()->text());
    on_evaluateButton_clicked();
    ui->listWidget->setCurrentRow(0);
}

void evaluator::on_clearButton_clicked()
{
    QFile file("history");
    file.resize(0);
    readHistory();
}

void evaluator::on_listWidget_currentRowChanged(int currentRow)
{
    switch (currentRow)
    {
    case 1:     //historyPage
        if (lx->standardized() == "")
            ui->historyList->setCurrentRow(0);
        else
        {
            QList<QListWidgetItem*> list = ui->historyList->findItems(lx->standardized(), Qt::MatchExactly);
            ui->historyList->setCurrentItem(list.at(0));
        }
        break;
    default:
        break;
    }
}

void evaluator::on_aboutButton_clicked()
{
    aD->show();
}

void evaluator::changeWgtPage(int page)
{
    Q_UNUSED(page);
    if (!ui->disableAniBox->isChecked())
    {
        QWidget *thePage = ui->stackedWidget->currentWidget();
        QPropertyAnimation *slideout = new QPropertyAnimation(thePage, "geometry", this);
        connect(slideout, SIGNAL(finished()), this, SLOT(nextWgtPage()));

        slideout->setStartValue(thePage->geometry());
        if (page > ui->stackedWidget->currentIndex()) {
            slideout->setEndValue(QRect(thePage->x(), this->height()*-1, thePage->width(), thePage->height())); //slide up
        } else {
            slideout->setEndValue(QRect(thePage->x(), this->height(), thePage->width(), thePage->height())); //slide down
        }
        slideout->setDuration(250);
        slideout->start();
    } else {
        ui->stackedWidget->setCurrentIndex(ui->listWidget->currentRow());
    }
}

void evaluator::nextWgtPage()
{
    int oldIndex = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(ui->listWidget->currentRow());
    QWidget *thePage = ui->stackedWidget->currentWidget();
    QPropertyAnimation *slidein = new QPropertyAnimation(thePage, "geometry", this);
    if (oldIndex < ui->stackedWidget->currentIndex()) {
        slidein->setStartValue(QRect(thePage->x(), this->height(), thePage->width(), thePage->height()));
    } else {
        slidein->setStartValue(QRect(thePage->x(), this->height()*(-1), thePage->width(), thePage->height()));
    }
    slidein->setEndValue(thePage->geometry());
    slidein->setDuration(250);
    slidein->start();
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

void evaluator::on_uiBox_toggled(bool checked)
{
    if (checked)
    {
        QCoreApplication::removeTranslator(&translator);
    } else {
        QCoreApplication::installTranslator(&translator);
    }
    ui->retranslateUi(this);
}
