#include "contactmanager.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <QFile>
#include <QStringList>

// Добавление контакта
void ContactManager::AddContact(const Contact &contact)
{
    Contacts.push_back(contact);
    std::cout << "Added contact: " << contact.GetFirstName() << " " << contact.GetLastName() << "\n";
}

// Удаление контакта по индексу
void ContactManager::RemoveContact(size_t index)
{
    if (index >= Contacts.size()) {
        throw std::out_of_range("Index out of range");
    }
    std::cout << "Removing contact: " << Contacts[index].GetFirstName() << " " << Contacts[index].GetLastName() << "\n";
    Contacts.erase(Contacts.begin() + index);
}

// Обновление контакта по индексу
void ContactManager::UpdateContact(size_t index, const Contact &contact)
{
    if (index >= Contacts.size()) {
        throw std::out_of_range("Index out of range");
    }
    Contacts[index] = contact;
    std::cout << "Updated contact at index " << index << ": " << contact.GetFirstName() << " " << contact.GetLastName() << "\n";
}

// Поиск контактов по критерию
std::vector<Contact> ContactManager::SearchContacts(const std::function<bool(const Contact &)> &criteria) const
{
    std::vector<Contact> result;
    for (const auto &contact : Contacts) {
        if (criteria(contact)) {
            result.push_back(contact);
        }
    }
    return result;
}

// Получение контакта по индексу
Contact ContactManager::GetContact(size_t index) const
{
    if (index >= Contacts.size()) {
        throw std::out_of_range("Index out of range");
    }
    return Contacts[index];
}

// Получение всех контактов
const std::vector<Contact> &ContactManager::GetAllContacts() const
{
    return Contacts;
}

// Сортировка контактов по критерию
void ContactManager::SortContacts(const std::function<bool(const Contact &, const Contact &)> &comparator)
{
    std::sort(Contacts.begin(), Contacts.end(), comparator);
}

// Загрузка контактов из файла
void ContactManager::LoadFromFile(const std::string &filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        throw std::runtime_error("Couldn't open file for reading");
    }

    Contacts.clear();
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(inFile, line)) {
        lineNumber++;
        std::stringstream ss(line);
        std::string firstName, middleName, lastName, date, email, phoneNumber;

        // Предполагается, что CSV разделён запятыми
        if (!std::getline(ss, firstName, ',')) continue;
        if (!std::getline(ss, middleName, ',')) continue;
        if (!std::getline(ss, lastName, ',')) continue;
        if (!std::getline(ss, date, ',')) continue;
        if (!std::getline(ss, email, ',')) continue;
        if (!std::getline(ss, phoneNumber, ',')) continue;

        try {
            Contact contact(firstName, middleName, lastName, date, email, phoneNumber);
            Contacts.push_back(contact);
            std::cout << "Loaded contact from line " << lineNumber << ": " << firstName << " " << lastName << "\n";
        } catch (const std::invalid_argument &e) {
            std::cerr << "Error in line " << lineNumber << ": " << e.what() << "\n";
            // Можно продолжить загрузку или завершить
        }
    }

    inFile.close();
}

// Сохранение контактов в файл
void ContactManager::SaveToFile(const std::string &filename) const
{
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw std::runtime_error("Couldn't open file for writing");
    }

    for (const auto &contact : Contacts) {
        outFile << contact.GetFirstName() << ","
                << contact.GetMiddleName() << ","
                << contact.GetLastName() << ","
                << contact.GetDate() << ","
                << contact.GetEmail() << ","
                << contact.GetPhoneNumber() << "\n";
    }

    outFile.close();
    std::cout << "Contacts saved to " << filename << "\n";
}



void ContactManager::LoadFromDB(const QString &connectionName) {
    // Проверяем, существует ли соединение с данным именем
    if (!QSqlDatabase::contains(connectionName)) {
        db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName("contacts.db");

    } else {
        db = QSqlDatabase::database(connectionName);
    }

    if (!db.isOpen()) {
        if (!db.open()) {
            throw std::runtime_error(db.lastError().text().toStdString());
        }
    }

    QSqlQuery query(db);
    // Создание таблицы, если её нет
    if (!query.exec("CREATE TABLE IF NOT EXISTS contacts ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "firstName TEXT,"
                    "middleName TEXT,"
                    "lastName TEXT,"
                    "date TEXT,"
                    "email TEXT,"
                    "phoneNumber TEXT)")) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    Contacts.clear();

    if (!query.exec("SELECT firstName, middleName, lastName, date, email, phoneNumber FROM contacts")) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    while (query.next()) {
        Contact contact;
        contact.SetFirstName(query.value(0).toString().toStdString());
        contact.SetMiddleName(query.value(1).toString().toStdString());
        contact.SetLastName(query.value(2).toString().toStdString());
        contact.SetDate(query.value(3).toString().toStdString());
        contact.SetEmail(query.value(4).toString().toStdString());
        contact.SetPhoneNumber(query.value(5).toString().toStdString());
        Contacts.push_back(contact);
    }
}

void ContactManager::SaveToDB(const QString &connectionName) {
    // Проверяем, существует ли соединение с данным именем
    if (!QSqlDatabase::contains(connectionName)) {
        db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName("contacts.db");
    } else {
        db = QSqlDatabase::database(connectionName);
    }

    if (!db.isOpen()) {
        if (!db.open()) {
            throw std::runtime_error(db.lastError().text().toStdString());
        }
    }
    QSqlQuery query(db);

    // Очистка таблицы перед сохранением
    if (!query.exec("DELETE FROM contacts")) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    // Выполняем вставку для каждого контакта
    for (const auto &contact : Contacts) {
        // Подготавливаем запрос заново для каждого контакта
        if (!query.prepare("INSERT INTO contacts (firstName, middleName, lastName, date, email, phoneNumber) "
                           "VALUES (?, ?, ?, ?, ?, ?)")) {
            throw std::runtime_error(query.lastError().text().toStdString());
        }

        query.addBindValue(QString::fromStdString(contact.GetFirstName()));
        query.addBindValue(QString::fromStdString(contact.GetMiddleName()));
        query.addBindValue(QString::fromStdString(contact.GetLastName()));
        query.addBindValue(QString::fromStdString(contact.GetDate()));
        query.addBindValue(QString::fromStdString(contact.GetEmail()));
        query.addBindValue(QString::fromStdString(contact.GetPhoneNumber()));

        if (!query.exec()) {
            throw std::runtime_error(query.lastError().text().toStdString());
        }
    }
}



