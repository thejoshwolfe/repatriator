#ifndef EDITBOOKMARKDIALOG_H
#define EDITBOOKMARKDIALOG_H

#include <QDialog>

#include "ServerTypes.h"

namespace Ui {
    class EditBookmarkDialog;
}

class EditBookmarkDialog : public QDialog {
    Q_OBJECT

public:
    static bool showEdit(ServerTypes::Bookmark * bookmark, ServerTypes::Bookmark here);


protected:
    void changeEvent(QEvent *e);

private:
    Ui::EditBookmarkDialog *ui;

    EditBookmarkDialog(QWidget *parent = 0);
    ~EditBookmarkDialog();

    static EditBookmarkDialog * s_instance;
    static EditBookmarkDialog * instance();

    ServerTypes::Bookmark m_here;
    bool m_accepted;

    void bookmarkToWidgets(ServerTypes::Bookmark bookmark);
    ServerTypes::Bookmark widgetsToBookmark();

    static QString motorPositionToText(qint64 position);
    static qint64 textToMotorPosition(QString text);

private slots:
    void on_ignoreButtonZ_clicked();
    void on_ignoreButtonY_clicked();
    void on_ignoreButtonX_clicked();
    void on_ignoreButtonB_clicked();
    void on_ignoreButtonA_clicked();
    void on_hereButtonZ_clicked();
    void on_hereButtonY_clicked();
    void on_hereButtonX_clicked();
    void on_hereButtonB_clicked();
    void on_hereButtonA_clicked();
    void handleRejected();
    void handleAccepted();

};

#endif // EDITBOOKMARKDIALOG_H
