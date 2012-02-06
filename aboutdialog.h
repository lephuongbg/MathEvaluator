#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

namespace Ui {
class aboutDialog;
}

class aboutDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit aboutDialog(QWidget *parent = 0);
    ~aboutDialog();
    void changeEvent(QEvent *);
    
public:
    Ui::aboutDialog *ui;
};

#endif // ABOUTDIALOG_H
