#include "contactdialog.h"
#include "ui_contactdialog.h"
#include <stdexcept> // для std::invalid_argument

ContactDialog::ContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactDialog)
{
    ui->setupUi(this);
}

ContactDialog::~ContactDialog()
{
    delete ui;
}

void ContactDialog::SetContactData(const Contact &contact)
{
    // При установке данных в поля диалога мы можем не триммить,
    // так как данные из Contact уже должны быть валидированы и очищены,
    // либо приходить в корректном формате.
    ui->lineEditFirstName->setText(QString::fromStdString(contact.GetFirstName()));
    ui->lineEditMiddleName->setText(QString::fromStdString(contact.GetMiddleName()));
    ui->lineEditLastName->setText(QString::fromStdString(contact.GetLastName()));
    ui->lineEditDate->setText(QString::fromStdString(contact.GetDate()));
    ui->lineEditEmail->setText(QString::fromStdString(contact.GetEmail()));
    ui->lineEditPhoneNumber->setText(QString::fromStdString(contact.GetPhoneNumber()));
}

Contact ContactDialog::GetContactData() const
{
    // Извлекаем текст из полей ввода и удаляем пробелы в начале и конце:
    QString firstName = ui->lineEditFirstName->text().trimmed();
    QString middleName = ui->lineEditMiddleName->text().trimmed();
    QString lastName = ui->lineEditLastName->text().trimmed();
    QString date = ui->lineEditDate->text().trimmed();
    QString email = ui->lineEditEmail->text().trimmed();
    QString phoneNumber = ui->lineEditPhoneNumber->text().trimmed();

    Contact contact;
    try {
        // Передаем очищенные строки в Contact
        contact.SetFirstName(firstName.toStdString());
        contact.SetMiddleName(middleName.toStdString());
        contact.SetLastName(lastName.toStdString());
        contact.SetDate(date.toStdString());
        contact.SetEmail(email.toStdString());
        contact.SetPhoneNumber(phoneNumber.toStdString());
    } catch (const std::invalid_argument &e) {
        // Если валидация в Contact выбросила исключение, пробрасываем дальше
        throw std::invalid_argument(QString::fromStdString(e.what()).toStdString());
    }

    return contact;
}
