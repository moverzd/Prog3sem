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
    ui->tableViewContacts->setSelectionBehavior(QAbstractItemView::SelectItems); // Выделение отдельных ячеек
    ui->tableViewContacts->setSelectionMode(QAbstractItemView::SingleSelection); // Можно выделить только одну ячейку

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
    proxyModel->setSourceModel(model); // Устанавливаем модель данных
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive); // Нечувствительность к регистру
    proxyModel->setFilterKeyColumn(-1); // Фильтрация по всем колонкам

    // Подключаем сигнал для динамической смены роли сортировки
    connect(ui->tableViewContacts->horizontalHeader(), &QHeaderView::sectionClicked, this, [this](int column) {
        if (column == 3) { // Колонка с датами (индекс 3)
            proxyModel->setSortRole(Qt::UserRole); // Используем скрытое числовое значение
        } else {
            proxyModel->setSortRole(Qt::DisplayRole); // Стандартная сортировка
        }
    });
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

        // Обработка даты
        QString dateStr = QString::fromStdString(contact.GetDate());
        QDate date = QDate::fromString(dateStr, "dd-MM-yyyy");
        QStandardItem *dateItem = new QStandardItem(dateStr);
        if (date.isValid()) {
            dateItem->setData(date.toString("yyyyMMdd").toInt(), Qt::UserRole); // Сохраняем скрытое числовое значение
        }
        items << dateItem;

        items << new QStandardItem(QString::fromStdString(contact.GetEmail()));
        items << new QStandardItem(QString::fromStdString(contact.GetPhoneNumber()));

        model->appendRow(items); // Добавление строки
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
{
    // Получаем текущий индекс (выбранная ячейка)
    QModelIndex currentIndex = ui->tableViewContacts->currentIndex();

    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Warning", "Please select a cell to delete the corresponding contact.");
        return;
    }

    // Получаем строку из прокси-модели и преобразуем в исходную модель
    int row = proxyModel->mapToSource(currentIndex).row();

    // Подтверждение удаления
    int ret = QMessageBox::warning(this, "Confirm Deletion",
                                   QString("Are you sure you want to delete this contact?"),
                                   QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes)
        return;

    try {
        manager.RemoveContact(static_cast<size_t>(row)); // Удаляем контакт из менеджера
        model->removeRow(row);                          // Удаляем строку из модели
        QMessageBox::information(this, "Success", "Contact deleted successfully!");
        qDebug() << "Deleted contact at row:" << row;
    } catch (const std::out_of_range &e) {
        qDebug() << "Error deleting contact at row:" << row << ":" << e.what();
    }
}


void MainWindow::on_pushButtonEdit_clicked()
{
    // Получаем текущий индекс (выбранная ячейка)
    QModelIndex currentIndex = ui->tableViewContacts->currentIndex();

    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Warning", "Please select a cell to edit.");
        return;
    }

    // Получаем строку и столбец
    int row = proxyModel->mapToSource(currentIndex).row();
    int column = proxyModel->mapToSource(currentIndex).column();

    QString currentValue = model->item(row, column)->text();

    // Запрашиваем новое значение для выбранной ячейки
    bool ok;
    QString newValue = QInputDialog::getText(this, tr("Edit Field"),
                                             tr("Edit %1:").arg(model->headerData(column, Qt::Horizontal).toString()),
                                             QLineEdit::Normal, currentValue, &ok);

    if (ok && !newValue.isEmpty() && newValue != currentValue) {
        try {
            // Получаем контакт и обновляем конкретное поле
            Contact contact = manager.GetContact(static_cast<size_t>(row));

            switch (column) {
            case 0: contact.SetFirstName(newValue.trimmed().toStdString()); break;
            case 1: contact.SetMiddleName(newValue.trimmed().toStdString()); break;
            case 2: contact.SetLastName(newValue.trimmed().toStdString()); break;
            case 3: contact.SetDate(newValue.trimmed().toStdString()); break;
            case 4: contact.SetEmail(newValue.trimmed().toStdString()); break;
            case 5: contact.SetPhoneNumber(newValue.trimmed().toStdString()); break;
            default: break;
            }

            // Обновляем контакт в менеджере
            manager.UpdateContact(static_cast<size_t>(row), contact);

            // Обновляем модель данных
            model->setData(model->index(row, column), newValue.trimmed());

            QMessageBox::information(this, "Success", "Field updated successfully!");
            qDebug() << "Updated row:" << row << "column:" << column << "with value:" << newValue;
        } catch (const std::exception &e) {
            QMessageBox::warning(this, "Error", QString::fromStdString(e.what()));
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


void MainWindow::on_pushButtonLoadDB_clicked() {
    // Открываем диалог выбора файла базы данных
    QString dbFile = QFileDialog::getOpenFileName(this,
                                                  tr("Select Database File"),
                                                  "",
                                                  tr("SQLite Database Files (*.db *.sqlite);;All Files (*)"));
    if(dbFile.isEmpty())
        return;  // Если пользователь отменил выбор

    QString connectionName = "customConnectionLoad";

    // Настраиваем соединение с выбранным файлом БД
    if(!QSqlDatabase::contains(connectionName)) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(dbFile);
        if(!db.open()) {
            QMessageBox::critical(this, tr("Database Error"), db.lastError().text());
            return;
        }
    } else {
        QSqlDatabase db = QSqlDatabase::database(connectionName);
        if(db.databaseName() != dbFile) {
            db.close();
            db.setDatabaseName(dbFile);
            if(!db.open()) {
                QMessageBox::critical(this, tr("Database Error"), db.lastError().text());
                return;
            }
        }
    }

    try {
        manager.LoadFromDB(connectionName);
        UpdateTable();
        QMessageBox::information(this, tr("Success"), tr("Contacts loaded from database successfully!"));
        qDebug() << "Contacts loaded from database.";
    } catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Error"), e.what());
        qDebug() << "Error loading contacts from database:" << e.what();
    }
}

void MainWindow::on_pushButtonSaveDB_clicked() {
    // Открываем диалог выбора файла для сохранения базы данных
    QString dbFile = QFileDialog::getSaveFileName(this,
                                                  tr("Select Database File to Save"),
                                                  "",
                                                  tr("SQLite Database Files (*.db *.sqlite);;All Files (*)"));
    if(dbFile.isEmpty())
        return;  // Пользователь отменил выбор

    QString connectionName = "customConnectionSave";

    // Настраиваем соединение с выбранным файлом БД для сохранения
    if(!QSqlDatabase::contains(connectionName)) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(dbFile);
        if(!db.open()) {
            QMessageBox::critical(this, tr("Database Error"), db.lastError().text());
            return;
        }
    } else {
        QSqlDatabase db = QSqlDatabase::database(connectionName);
        if(db.databaseName() != dbFile) {
            db.close();
            db.setDatabaseName(dbFile);
            if(!db.open()) {
                QMessageBox::critical(this, tr("Database Error"), db.lastError().text());
                return;
            }
        }
    }

    try {
        manager.SaveToDB(connectionName);
        QMessageBox::information(this, tr("Success"), tr("Contacts saved to database successfully!"));
        qDebug() << "Contacts saved to database successfully.";
    } catch(const std::exception &e) {
        QMessageBox::warning(this, tr("Error"), e.what());
        qDebug() << "Error saving contacts to database:" << e.what();
    }
}
