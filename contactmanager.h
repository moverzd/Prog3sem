#ifndef CONTACTMANAGER_H
#define CONTACTMANAGER_H

#include "contact.h"

#include <vector>
#include <functional>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>


class ContactManager {
public:
    void AddContact(const Contact &contact);
    void RemoveContact(size_t index);
    void UpdateContact(size_t index, const Contact &contact);
    std::vector<Contact> SearchContacts(const std::function<bool(const Contact &)> &criteria) const;
    Contact GetContact(size_t index) const;
    const std::vector<Contact> &GetAllContacts() const;
    void SortContacts(const std::function<bool(const Contact &, const Contact &)> &comparator);
    void LoadFromFile(const std::string &filename);
    void SaveToFile(const std::string &filename) const;

    void LoadFromDB(const QString &connectionName = "default");
    void SaveToDB(const QString &connectionName = "default");


private:
    std::vector<Contact> Contacts;

    QSqlDatabase db;  // добавляем член класса для работы с БД
};

#endif // CONTACTMANAGER_H
