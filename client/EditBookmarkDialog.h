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
    void handleRejected();
    void handleAccepted();

};

#endif // EDITBOOKMARKDIALOG_H
