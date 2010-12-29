#include "EditConnectionWindow.h"
#include "ui_EditConnectionWindow.h"

EditConnectionWindow::EditConnectionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditConnectionWindow)
{
    ui->setupUi(this);
}

EditConnectionWindow::~EditConnectionWindow()
{
    delete ui;
}

void EditConnectionWindow::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
