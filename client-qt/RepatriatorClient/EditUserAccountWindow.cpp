#include "EditUserAccountWindow.h"
#include "ui_EditUserAccountWindow.h"

EditUserAccountWindow::EditUserAccountWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditUserAccountWindow)
{
    ui->setupUi(this);
}

EditUserAccountWindow::~EditUserAccountWindow()
{
    delete ui;
}

void EditUserAccountWindow::changeEvent(QEvent *e)
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
