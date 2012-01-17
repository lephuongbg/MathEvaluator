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

    //Loading dialog box
    evalProgress = new QProgressDialog(this, Qt::WindowTitleHint);
    evalProgress->setWindowIcon(QIcon(":/Tutorial.ico"));
    evalProgress->setLabelText(tr("Calculating..."));
    evalProgress->setMinimum(0);
    evalProgress->setMaximum(0);
    evalProgress->setCancelButton(0);
    evalProgress->setFixedSize(evalProgress->size());
    connect(&watcher, SIGNAL(finished()), evalProgress, SLOT(close()));

    //Animation
    evalAnimation = new QPropertyAnimation(ui->evalWidget,"geometry");
    hisAnimation = new QPropertyAnimation(ui->historyWidget,"geometry");

    //Read history when start
    readHistory();

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
    delete evalProgress;
    delete evalAnimation;
    delete hisAnimation;
    delete optionMenu;
}

void evaluator::on_evaluateButton_clicked()
{
    QString expression = ui->input->text();
    if (expression == "")
        return;
    //Connect error info to output
    connect(lx,SIGNAL(error(QString)),ui->output,SLOT(setText(QString)));

    //Put the expression into evaluation class
    lx->set(expression.toStdString());

    //Move evaluating process to another thread
    QFuture<big_num> future = QtConcurrent::run(lx, &lex::evaluate);
    watcher.setFuture(future);

    //Show progress for long operation
    evalProgress->exec();

    //Get the result
    big_num result = future.result();

    //Display the result
    if (!(result.sign == 0))
    {
        QString out = QString::fromStdString(result.number);
        if (result.sign == -1)
            out.insert(0,'-');
        ui->output->setPlainText(out);

        //Enable view standard expression after done calculation
        ui->viewStdExp->setEnabled(true);
        if (ui->viewStdExp->isChecked())
            ui->input->setText(lx->standardized());

        //Save the successfully evaluated expression then reload
        if (saveHistory())
            readHistory();
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

void evaluator::enable_viewStdExp()
{
    ui->viewStdExp->setEnabled(true);
}

void evaluator::on_input_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1);
    ui->viewStdExp->setDisabled(true);
}

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
            return 1;
        }
    }
    return 0;
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

void evaluator::changeUI(int state)
{
    if (state == Qt::Checked)
    {
        QCoreApplication::removeTranslator(&translator);
        evalProgress->setLabelText(tr("Calculating..."));
        enUIbox->setText(tr("English interface"));
        ui->retranslateUi(this);
    } else {
        QCoreApplication::installTranslator(&translator);
        evalProgress->setLabelText(tr("Calculating..."));
        enUIbox->setText(tr("English interface"));
        ui->retranslateUi(this);
    }
}
