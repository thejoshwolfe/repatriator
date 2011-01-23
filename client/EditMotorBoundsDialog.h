#ifndef EDITMOTORBOUNDSDIALOG_H
#define EDITMOTORBOUNDSDIALOG_H

#include <QDialog>

#include "ServerTypes.h"

namespace Ui {
    class EditMotorBoundsDialog;
}

class EditMotorBoundsDialog : public QDialog {
    Q_OBJECT
public:
    static bool showEdit(QVector<ServerTypes::MotorBoundaries> * bounds, ServerTypes::Bookmark here);

private:
    EditMotorBoundsDialog(QWidget *parent = 0);
    ~EditMotorBoundsDialog();

    static EditMotorBoundsDialog * s_instance;
    static EditMotorBoundsDialog * instance();

    ServerTypes::Bookmark m_here;
    bool m_accepted;

    void boundsToWidgets(QVector<ServerTypes::MotorBoundaries> bounds);
    QVector<ServerTypes::MotorBoundaries> widgetsToBounds();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::EditMotorBoundsDialog *ui;

private slots:
    void on_hereMaxButtonZ_clicked();
    void on_hereMinButtonZ_clicked();
    void on_hereMaxButtonY_clicked();
    void on_hereMinButtonY_clicked();
    void on_hereMaxButtonX_clicked();
    void on_hereMinButtonX_clicked();
    void on_hereMaxButtonB_clicked();
    void on_hereMinButtonB_clicked();
    void on_hereMaxButtonA_clicked();
    void handleRejected();
    void handleAccepted();
};

#endif // EDITMOTORBOUNDSDIALOG_H
