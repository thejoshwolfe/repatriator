#include "EditMotorBoundsDialog.h"
#include "ui_EditMotorBoundsDialog.h"

EditMotorBoundsDialog * EditMotorBoundsDialog::s_instance = NULL;

EditMotorBoundsDialog::EditMotorBoundsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditMotorBoundsDialog)
{
    ui->setupUi(this);

    bool success;
    success = connect(this, SIGNAL(accepted()), this, SLOT(handleAccepted()));
    Q_ASSERT(success);
    success = connect(this, SIGNAL(rejected()), this, SLOT(handleRejected()));
    Q_ASSERT(success);
}

EditMotorBoundsDialog::~EditMotorBoundsDialog()
{
    delete ui;
}

void EditMotorBoundsDialog::changeEvent(QEvent *e)
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

EditMotorBoundsDialog * EditMotorBoundsDialog::instance()
{
    if (s_instance == NULL)
        s_instance = new EditMotorBoundsDialog();
    return s_instance;
}

bool EditMotorBoundsDialog::showEdit(QVector<ServerTypes::MotorBoundaries> *bounds, ServerTypes::Bookmark here)
{
    EditMotorBoundsDialog * dialog = instance();

    dialog->m_here = here;
    dialog->boundsToWidgets(*bounds);

    dialog->exec();

    if (dialog->m_accepted)
        *bounds = dialog->widgetsToBounds();

    return dialog->m_accepted;
}

void EditMotorBoundsDialog::handleAccepted()
{
    m_accepted = true;
}
void EditMotorBoundsDialog::handleRejected()
{
    m_accepted = false;
}

void EditMotorBoundsDialog::boundsToWidgets(QVector<ServerTypes::MotorBoundaries> bounds)
{
    ui->maxLineEditA->setText(QString::number(bounds.at(0).max));
    ui->minLineEditB->setText(QString::number(bounds.at(1).min));
    ui->maxLineEditB->setText(QString::number(bounds.at(1).max));
    ui->minLineEditX->setText(QString::number(bounds.at(2).min));
    ui->maxLineEditX->setText(QString::number(bounds.at(2).max));
    ui->minLineEditY->setText(QString::number(bounds.at(3).min));
    ui->maxLineEditY->setText(QString::number(bounds.at(3).max));
    ui->minLineEditZ->setText(QString::number(bounds.at(4).min));
    ui->maxLineEditZ->setText(QString::number(bounds.at(4).max));
}

QVector<ServerTypes::MotorBoundaries> EditMotorBoundsDialog::widgetsToBounds()
{
    QVector<ServerTypes::MotorBoundaries> bounds(5);
    bounds.replace(0, ServerTypes::MotorBoundaries(0, ui->maxLineEditA->text().toLongLong()));
    bounds.replace(1, ServerTypes::MotorBoundaries(ui->minLineEditB->text().toLongLong(), ui->maxLineEditB->text().toLongLong()));
    bounds.replace(2, ServerTypes::MotorBoundaries(ui->minLineEditX->text().toLongLong(), ui->maxLineEditX->text().toLongLong()));
    bounds.replace(3, ServerTypes::MotorBoundaries(ui->minLineEditY->text().toLongLong(), ui->maxLineEditY->text().toLongLong()));
    bounds.replace(4, ServerTypes::MotorBoundaries(ui->minLineEditZ->text().toLongLong(), ui->maxLineEditZ->text().toLongLong()));
    return bounds;
}

void EditMotorBoundsDialog::on_hereMaxButtonA_clicked() { ui->maxLineEditA->setText(QString::number(m_here.motor_positions.at(0)));}
void EditMotorBoundsDialog::on_hereMinButtonB_clicked() { ui->minLineEditB->setText(QString::number(m_here.motor_positions.at(1)));}
void EditMotorBoundsDialog::on_hereMaxButtonB_clicked() { ui->maxLineEditB->setText(QString::number(m_here.motor_positions.at(1)));}
void EditMotorBoundsDialog::on_hereMinButtonX_clicked() { ui->minLineEditX->setText(QString::number(m_here.motor_positions.at(2)));}
void EditMotorBoundsDialog::on_hereMaxButtonX_clicked() { ui->maxLineEditX->setText(QString::number(m_here.motor_positions.at(2)));}
void EditMotorBoundsDialog::on_hereMinButtonY_clicked() { ui->minLineEditY->setText(QString::number(m_here.motor_positions.at(3)));}
void EditMotorBoundsDialog::on_hereMaxButtonY_clicked() { ui->maxLineEditY->setText(QString::number(m_here.motor_positions.at(3)));}
void EditMotorBoundsDialog::on_hereMinButtonZ_clicked() { ui->minLineEditZ->setText(QString::number(m_here.motor_positions.at(4)));}
void EditMotorBoundsDialog::on_hereMaxButtonZ_clicked() { ui->maxLineEditZ->setText(QString::number(m_here.motor_positions.at(4)));}
