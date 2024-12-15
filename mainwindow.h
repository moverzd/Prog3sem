#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "contactmanager.h"

#include <QMainWindow> // Создание главного окна
#include <QSortFilterProxyModel> // Возможность сортировки и фильтрации в таблице
#include <QStandardItemModel> // Представление хранения в QTableView


// От mainwindow.ui
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Перечисление для индексов колонок
enum ContactColumns {
    FirstName = 0,
    MiddleName,
    LastName,
    Date,
    Email,
    PhoneNumber,
    ColumnCount
};

class MainWindow : public QMainWindow
{
    Q_OBJECT // Для сигналов и слотов

public:
    // Конструктор
    MainWindow(QWidget *parent = nullptr);
    //Деструктор
    ~MainWindow();


private slots:
    void on_pushButtonAdd_clicked(); // Обработка нажатия на кнопку "Add"
    void on_pushButtonDelete_clicked(); // Обработка нажатия на кнопку "Delete"
    void on_pushButtonEdit_clicked(); // Обработка нажатия на кнопку "Edit"
    void on_pushButtonLoad_clicked(); // Обработка нажатия на кнопку "Load"
    void on_pushButtonSave_clicked(); // Обработка нажатия на кнопку "Save"
    void on_lineEditSearch_textChanged(const QString &text); // Обновление таблицы в моменте Фильтрации
    void on_comboBoxSearchField_currentIndexChanged(int index); // Обработка изменения выбранного поля поиска


private:
    Ui::MainWindow *ui; // Обращение виджетам и элементов в mainwindow.ui
    ContactManager manager; // Менеджер управления контактами

    QStandardItemModel *model; // Модель, хранящая контакты
    QSortFilterProxyModel *proxyModel; // Прокси модель, которая оборачивает model и добавляет сортировку и фильтрацию

    void UpdateModel(); // Обновление содержимого model на основе Contact Manager
    void UpdateTable(); // Обновление таблицы
    void SetupModel(); // Первоначальная установка таблицы
    void SetupProxyModel(); // Первоначальная настройка прокси модели, установка исходной модели и настройку фильтрации
};

#endif // MAINWINDOW_H
