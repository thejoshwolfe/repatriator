#include "EditBookmarkDialog.h"
#include "ui_EditBookmarkDialog.h"

EditBookmarkDialog * EditBookmarkDialog::s_instance = NULL;

EditBookmarkDialog::EditBookmarkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditBookmarkDialog)
{
    ui->setupUi(this);

    bool success;
    success = connect(this, SIGNAL(accepted()), this, SLOT(handleAccepted()));
    Q_ASSERT(success);
    success = connect(this, SIGNAL(rejected()), this, SLOT(handleRejected()));
    Q_ASSERT(success);
}

EditBookmarkDialog::~EditBookmarkDialog()
{
    delete ui;
}

EditBookmarkDialog * EditBookmarkDialog::instance()
{
    if (s_instance == NULL)
        s_instance = new EditBookmarkDialog();
    return s_instance;
}

void EditBookmarkDialog::changeEvent(QEvent *e)
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

bool EditBookmarkDialog::showEdit(ServerTypes::Bookmark * bookmark, ServerTypes::Bookmark here)
{
    EditBookmarkDialog * dialog = instance();

    dialog->m_here = here;
    dialog->bookmarkToWidgets(*bookmark);

    dialog->exec();

    if (dialog->m_accepted)
        *bookmark = dialog->widgetsToBookmark();

    return dialog->m_accepted;
}

void EditBookmarkDialog::handleAccepted()
{
    m_accepted = true;
}
void EditBookmarkDialog::handleRejected()
{
    m_accepted = false;
}

void EditBookmarkDialog::bookmarkToWidgets(ServerTypes::Bookmark bookmark)
{
    ui->nameLineEdit->setText(bookmark.name);
    ui->motorLineEditA->setText(motorPositionToText(bookmark.motor_positions.at(0)));
    ui->motorLineEditB->setText(motorPositionToText(bookmark.motor_positions.at(1)));
    ui->motorLineEditX->setText(motorPositionToText(bookmark.motor_positions.at(2)));
    ui->motorLineEditY->setText(motorPositionToText(bookmark.motor_positions.at(3)));
    ui->motorLineEditZ->setText(motorPositionToText(bookmark.motor_positions.at(4)));
    switch (bookmark.auto_focus) {
        case ServerTypes::NotSpecified:
            ui->autoFocusIgnoreRadio->setChecked(true);
            break;
        case ServerTypes::SetOn:
            ui->autoFocusOnRadio->setChecked(true);
            break;
        case ServerTypes::SetOff:
            ui->autoFocusOffRadio->setChecked(true);
            break;
        default:
            Q_ASSERT(false);
    }
}

ServerTypes::Bookmark EditBookmarkDialog::widgetsToBookmark()
{
    ServerTypes::Bookmark bookmark;
    bookmark.name = ui->nameLineEdit->text();
    bookmark.motor_positions.append(textToMotorPosition(ui->motorLineEditA->text()));
    bookmark.motor_positions.append(textToMotorPosition(ui->motorLineEditB->text()));
    bookmark.motor_positions.append(textToMotorPosition(ui->motorLineEditX->text()));
    bookmark.motor_positions.append(textToMotorPosition(ui->motorLineEditY->text()));
    bookmark.motor_positions.append(textToMotorPosition(ui->motorLineEditZ->text()));
    if (ui->autoFocusIgnoreRadio->isChecked())
        bookmark.auto_focus = ServerTypes::NotSpecified;
    else if (ui->autoFocusOnRadio->isChecked())
        bookmark.auto_focus = ServerTypes::SetOn;
    else if (ui->autoFocusOffRadio->isChecked())
        bookmark.auto_focus = ServerTypes::SetOff;
    else
        Q_ASSERT(false);
    return bookmark;
}

QString EditBookmarkDialog::motorPositionToText(qint64 position)
{
    if (position == ServerTypes::MotorPositionNotSpecified)
        return "-";
    return QString::number(position);
}

qint64 EditBookmarkDialog::textToMotorPosition(QString text)
{
    bool success;
    qint64 result = text.toLongLong(&success);
    return success ? result : ServerTypes::MotorPositionNotSpecified;
}
