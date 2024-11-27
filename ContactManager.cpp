#include "ContactManager.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Delete spacing
std::string Trim(const std::string &str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}


void ContactManager::AddContact(const Contact &contact) {
    Contacts.push_back(contact);
}

void ContactManager::RemoveContact(size_t index) {
    if (index < Contacts.size()) {
        Contacts.erase(Contacts.begin() + index);
    } else {
        throw std::out_of_range("Индекс вне диапазона");
    }
}

// Search by criteria(lambda function)
std::vector<Contact> ContactManager::SearchContacts(const std::function<bool(const Contact &)> &criteria) const {
    std::vector<Contact> result;
    for (const auto &contact : Contacts) {
        if (criteria(contact)) {
            result.push_back(contact);
        }
    }
    return result;
}

Contact& ContactManager::GetContact(size_t index){
  if(index >= Contacts.size()){
    throw std::out_of_range("Index of out range");
  }
  return Contacts[index];
}

const std::vector<Contact> &ContactManager::GetAllContacts() const {
    return Contacts;
}

void ContactManager::SortContacts(const std::function<bool(const Contact &, const Contact &)> &comparator) {
    std::sort(Contacts.begin(), Contacts.end(), comparator);
}

void ContactManager::LoadFromFile(const std::string &filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        throw std::runtime_error("Couldn't open file\n");
    }
    Contacts.clear();
    std::string line;
    while (std::getline(inFile, line)) {
        std::stringstream ss(line);
        std::string firstname, middlename, lastname, date, email, phonenumber;

        // FirstName,MiddleName,LastName,Date,Email,PhoneNumber

        std::getline(ss, firstname, ',');
        std::getline(ss, middlename, ',');
        std::getline(ss, lastname, ',');
        std::getline(ss, date, ',');
        std::getline(ss, email, ',');
        std::getline(ss, phonenumber, ',');

        // Deleting spaces
        firstname = Trim(firstname);
        middlename = Trim(middlename);
        lastname = Trim(lastname);
        date = Trim(date);
        email = Trim(email);
        phonenumber = Trim(phonenumber);

        try {
            Contact contact(firstname, middlename, lastname, date, email, phonenumber);
            Contacts.push_back(contact);
        } catch (const std::invalid_argument &e) {
            std::cerr << "Can't upload contact: " << e.what() << "\n";
        }
    }
}

void ContactManager::SaveToFile(const std::string &filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw std::runtime_error("Couldn't write in file");
    }
    for (const auto &contact : Contacts) {
        outFile << contact.GetFirstName() << ","
                << contact.GetMiddleName() << ","
                << contact.GetLastName() << ","
                << contact.GetDate() << ","
                << contact.GetEmail() << ","
                << contact.GetPhoneNumber() << "\n";
    }
}
