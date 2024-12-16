#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "contactdialog.h"

#include <QMessageBox> // Для окна сообщений
#include <QInputDialog> // Ввод через диалоговые окна
#include <QStandardItemModel> // Управление данными
#include <QSortFilterProxyModel> // Сортировка данными в таблоице
#include <QHeaderView> // Заголовки таблица
#include <QFileDialog> // Диалоговые окна для открытия и сохранения файла
#include <algorithm>
#include <QSet>
#include <QDebug>

// Конструктор
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) // Создание интерфейса
    , model(new QStandardItemModel(this)) // Инициализация модели данных
    , proxyModel(new QSortFilterProxyModel(this)) // Инициализация прокси-модели для сортировки и фильтрации
{

    ui->setupUi(this); // Инициализация из Design
    ui->tableViewContacts->setEditTriggers(QAbstractItemView::NoEditTriggers); // Решение пунктов To Do - просто выключим редактирование таблицы
    // Настройка таблицы
    ui->tableViewContacts->setSortingEnabled(true); // Сортировка по клику на заголовке столбцов
    ui->tableViewContacts->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // Равномерное растяжение таблица
    ui->tableViewContacts->setSelectionBehavior(QAbstractItemView::SelectItems); // Выбор отдельных ячеек
    ui->tableViewContacts->setSelectionMode(QAbstractItemView::SingleSelection); // Ограничение выбора одной ячейкой

    // Инициализация моделей
    SetupModel();
    SetupProxyModel();

    // Установка модели в QTableView
    ui->tableViewContacts->setModel(proxyModel);

    // Заполнение ComboBox полями для поиска
    ui->comboBoxSearchField->addItem("All Fields");
    ui->comboBoxSearchField->addItem("First Name");
    ui->comboBoxSearchField->addItem("Middle Name");
    ui->comboBoxSearchField->addItem("Last Name");
    ui->comboBoxSearchField->addItem("Date");
    ui->comboBoxSearchField->addItem("Email");
    ui->comboBoxSearchField->addItem("Phone Number");
}

// Деструктор
MainWindow::~MainWindow()
{
    delete ui;
}

// Определение структуры таблица
void MainWindow::SetupModel()
{
    model->setColumnCount(ColumnCount); // Установка столбцы в модели
    QStringList headers = { "First Name", "Middle Name", "Last Name", "Date", "Email", "Phone Number" }; //Заголовки таблицы
    model->setHorizontalHeaderLabels(headers);
}

// Настройка прокси модели для фильтрации
void MainWindow::SetupProxyModel()
{
    proxyModel->setSourceModel(model); // Установка model для прокси-модели
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive); // Нечувствительность к регистру фильтрации
    proxyModel->setFilterKeyColumn(-1); // По умолчанию фильтровать по всем колонкам
}

// Перегрузка данных модели из ContactManager, каждый контакт превращается в строку в QStandartItemModel
void MainWindow::UpdateModel()
{
    model->removeRows(0, model->rowCount()); // Очистка модели от старых данных

    const auto &contacts = manager.GetAllContacts();
    for (const auto &contact : contacts) {
        QList<QStandardItem *> items;
        items << new QStandardItem(QString::fromStdString(contact.GetFirstName()));
        items << new QStandardItem(QString::fromStdString(contact.GetMiddleName()));
        items << new QStandardItem(QString::fromStdString(contact.GetLastName()));
        items << new QStandardItem(QString::fromStdString(contact.GetDate()));
        items << new QStandardItem(QString::fromStdString(contact.GetEmail()));
        items << new QStandardItem(QString::fromStdString(contact.GetPhoneNumber()));

        model->appendRow(items); // Добавление новой строки в модель
    }
}


// Пересборка таблицы после изменения данных
void MainWindow::UpdateTable()
{
    UpdateModel();
    proxyModel->invalidate(); // Обновляем прокси-модель

    qDebug() << "Table updated with" << model->rowCount() << "contacts.";
}


// Слот добавления контакта
void MainWindow::on_pushButtonAdd_clicked()
{
    ContactDialog dialog(this);
    //Открываем диалога добавления окна
    if (dialog.exec() == QDialog::Accepted) {
        try {
            Contact newContact = dialog.GetContactData();
            manager.AddContact(newContact); // Добавление контакта в менеджер
            UpdateTable();
            QMessageBox::information(this, "Success", "Contact added successfully!");

            qDebug() << "Contact added successfully.";
        } catch (const std::invalid_argument &e) {
            QMessageBox::warning(this, "Error", QString::fromStdString(e.what()));

            qDebug() << "Error adding contact:" << QString::fromStdString(e.what());
        }
    }
}

//Слот удаления контакта
void MainWindow::on_pushButtonDelete_clicked()
{   //Извлечение списка индексов строк, выбранных в QTableView
    QModelIndexList selectedRows = ui->tableViewContacts->selectionModel()->selectedRows();
    // Предупреждение, если работаем с пустой строкой
    if (selectedRows.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select at least one contact to delete.");
        return;
    }

    // Сборка уникальных номеров строк
    QSet<int> rowsToDelete;
    for (const QModelIndex &index : selectedRows) {
        rowsToDelete.insert(proxyModel->mapToSource(index).row()); // Преобразование индексов из прокси-модели в исходную
    }

    // Подтверждение удаления
    int ret = QMessageBox::warning(this, "Confirm Deletion",
                                   QString("Are you sure you want to delete %1 contact(s)?").arg(rowsToDelete.size()),
                                   QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes)
        return;

    // Удаление контактов с конца, чтобы избежать сдвига индексов
    QList<int> sortedRows = rowsToDelete.values();
    std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>());
    for (int row : sortedRows) {
        try {
            manager.RemoveContact(static_cast<size_t>(row));
            qDebug() << "Deleted contact at row:" << row;
        } catch (const std::out_of_range &e) {
            qDebug() << "Error deleting contact at row" << row << ":" << e.what();
        }
    }

    UpdateTable();
    QMessageBox::information(this, "Success", "Selected contact(s) deleted successfully!");
}

void MainWindow::on_pushButtonEdit_clicked()
{
    QModelIndexList selectedIndexes = ui->tableViewContacts->selectionModel()->selectedIndexes();
    if (selectedIndexes.size() != 1) {
        QMessageBox::warning(this, "Warning", "Please select exactly one field to edit.");
        return;
    }

    QModelIndex proxyIndex = selectedIndexes.first();
    QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
    int row = sourceIndex.row();
    int column = sourceIndex.column();

    QString currentValue = model->item(row, column)->text();

    bool ok;
    QString newValue = QInputDialog::getText(this, tr("Edit Field"),
                                             tr("Edit %1:").arg(model->headerData(column, Qt::Horizontal).toString()),
                                             QLineEdit::Normal, currentValue, &ok);

    if (ok && !newValue.isEmpty() && newValue != currentValue) {
        try {
            // Получаем контакт, вносим изменения и проверяем
            Contact contact = manager.GetContact(static_cast<size_t>(row));

            // Устанавливаем новое значение (trimmed, если нужно)
            QString trimmedValue = newValue.trimmed();
            switch (column) {
            case FirstName:
                contact.SetFirstName(trimmedValue.toStdString());
                break;
            case MiddleName:
                contact.SetMiddleName(trimmedValue.toStdString());
                break;
            case LastName:
                contact.SetLastName(trimmedValue.toStdString());
                break;
            case Date:
                contact.SetDate(trimmedValue.toStdString());
                break;
            case Email:
                contact.SetEmail(trimmedValue.toStdString());
                break;
            case PhoneNumber:
                contact.SetPhoneNumber(trimmedValue.toStdString());
                break;
            default:
                break;
            }

            // Если никаких исключений не было — обновляем контакт в менеджере
            manager.UpdateContact(static_cast<size_t>(row), contact);

            // Теперь можно безопасно обновить модель
            model->setData(sourceIndex, trimmedValue);
            QMessageBox::information(this, "Success", "Field updated successfully!");
        } catch (const std::exception &e) {
            // Если возникла ошибка, показываем её, но не обновляем модель и не вызываем UpdateTable()
            QMessageBox::warning(this, "Error", QString::fromStdString(e.what()));
            // Таблица остаётся без изменений
        }
    }
}


// Слот для кнопки "Load"
void MainWindow::on_pushButtonLoad_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open File", "", "CSV Files (*.csv);;All Files (*)");
    if (filename.isEmpty()) return; // Пользователь отменил выбор файла

    try {
        manager.LoadFromFile(filename.toStdString());
        UpdateTable();
        QMessageBox::information(this, "Success", "Contacts loaded successfully!");
        qDebug() << "Contacts loaded successfully.";
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Error", QString::fromStdString(e.what()));
        qDebug() << "Error loading contacts:" << QString::fromStdString(e.what());
    }
}

// Слот для кнопки "Save"
void MainWindow::on_pushButtonSave_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save File", "", "CSV Files (*.csv);;All Files (*)");
    if (filename.isEmpty()) return; // Польязователь отменил выбор места сохранения

    try {
        manager.SaveToFile(filename.toStdString());
        QMessageBox::information(this, "Success", "Contacts saved successfully!");
        qDebug() << "Contacts saved successfully.";
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Error", QString::fromStdString(e.what()));
        qDebug() << "Error saving contacts:" << QString::fromStdString(e.what());
    }
}

// Слот для изменения текста в поле поиска
void MainWindow::on_lineEditSearch_textChanged(const QString &text)
{
    proxyModel->setFilterFixedString(text);
}

// Слот для изменения выбранного поля поиска
void MainWindow::on_comboBoxSearchField_currentIndexChanged(int index)
{
    if (index == 0) {
        proxyModel->setFilterKeyColumn(-1); // Фильтрация по всем колонкам
    } else {
        proxyModel->setFilterKeyColumn(index - 1); // Соответствие индексу колонок модели
    }
}
