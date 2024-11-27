
#ifndef CONTACT_MANAGER_H
#define CONTACT_MANAGER_H

#include "Contact.h"
#include <functional>
#include <string>
#include <vector>

class ContactManager {
private:
    std::vector<Contact> Contacts;

public:
    void AddContact(const Contact &contact);

    void RemoveContact(size_t index);

    std::vector<Contact> SearchContacts(const std::function<bool(const Contact &)> &criteria) const;
    
    Contact& GetContact(size_t index);

    const std::vector<Contact> &GetAllContacts() const;

    void SortContacts(const std::function<bool(const Contact &, const Contact &)> &comparator);

    void LoadFromFile(const std::string &filename);

    void SaveToFile(const std::string &filename) const;
};

#endif // CONTACT_MANAGER_H
