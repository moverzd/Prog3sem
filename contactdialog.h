#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include <QDialog> // Для создания окон

#include "contact.h"

namespace Ui {
class ContactDialog;
}

class ContactDialog : public QDialog // Наследование от QDialog, то есть создание диалового окна
{
    Q_OBJECT

public:
    explicit ContactDialog(QWidget *parent = nullptr); // Конструктор с блокировкой неявного преобразования
    ~ContactDialog(); // Деструктор

    // Установка данных контакта (для редактирования)
    void SetContactData(const Contact &contact);

    // Получение данных контакта после редактирования/добавления
    Contact GetContactData() const;

private:
    Ui::ContactDialog *ui; // Указатель на объект интерфейса, через QtDesigner
};

#endif // CONTACTDIALOG_H
