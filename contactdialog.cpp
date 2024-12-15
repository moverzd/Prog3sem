#include "contactdialog.h"
#include "ui_contactdialog.h" // для Ui:ContactDialog
#include <stdexcept> // для std::invalid_argument

//Конструктор
ContactDialog::ContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactDialog)
{
    ui->setupUi(this); // настройка пользовательского интерфейса.
    // То есть создание всех виджетов, установка свойств, сигналов и слотов из ui файла
}

//Деструктор
ContactDialog::~ContactDialog()
{
    delete ui;
}

// Установка данных контакта в поля окна
void ContactDialog::SetContactData(const Contact &contact)
{
    ui->lineEditFirstName->setText(QString::fromStdString(contact.GetFirstName())); // Установка текста в поле -> преобразование
    ui->lineEditMiddleName->setText(QString::fromStdString(contact.GetMiddleName()));
    ui->lineEditLastName->setText(QString::fromStdString(contact.GetLastName()));
    ui->lineEditDate->setText(QString::fromStdString(contact.GetDate()));
    ui->lineEditEmail->setText(QString::fromStdString(contact.GetEmail()));
    ui->lineEditPhoneNumber->setText(QString::fromStdString(contact.GetPhoneNumber()));
}

// Получение полей и формаирование Contact
Contact ContactDialog::GetContactData() const
{
    Contact contact;
    try {
        contact.SetFirstName(ui->lineEditFirstName->text().toStdString()); // Извлечение из поля ввода -> преобразование -> установка
        contact.SetMiddleName(ui->lineEditMiddleName->text().toStdString());
        contact.SetLastName(ui->lineEditLastName->text().toStdString());
        contact.SetDate(ui->lineEditDate->text().toStdString());
        contact.SetEmail(ui->lineEditEmail->text().toStdString());
        contact.SetPhoneNumber(ui->lineEditPhoneNumber->text().toStdString());
    } catch (const std::invalid_argument &e) { // Блок валидации
        throw std::invalid_argument(QString::fromStdString(e.what()).toStdString());
    }
    return contact;
}
