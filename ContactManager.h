#include "Contact.h"
#include <fstream>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class ContactManager {
private:
  std::vector<Contact> Contacts;

public:
  void AddContact(const Contact &contact) { Contacts.push_back(contact); }
  void RemoveContact(size_t index) {
    if (index < Contacts.size()) {
      Contacts.erase(Contacts.begin() + index);
    } else {
      throw std::out_of_range("Индекс вне диапозона");
    }
  }

  std::vector<Contact>
  SearchContacts(const std::function<bool(const Contact &)> &criteria) const {
    std::vector<Contact> result;
    for (const auto &contact : Contacts) {
      if (criteria(contact)) {
        result.push_back(contact);
      }
    }
    return result;
  }

  const std::vector<Contact> &getallcontacts() const { return Contacts; }

  void SortContacts(
      const std::function<bool(const Contact &, const Contact &)> &comparator) {
    std::sort(Contacts.begin(), Contacts.end(), comparator);
  }

  void LoadFromFile(const std::string &filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
      throw std::runtime_error("Не удалось открыть файл для чтения");
    }
    Contacts.clear();
    std::string line;
    while (std::getline(inFile, line)) {
      std::stringstream ss(line);
      std::string firstname, lastname, middlename, date, email, phonenumber;
      std::getline(ss, firstname, ',');
      std::getline(ss, lastname, ',');
      std::getline(ss, middlename, ',');
      std::getline(ss, email, ',');
      std::getline(ss, phonenumber, ',');
      std::getline(ss, date, ',');
      try {
        Contact contact(firstname, lastname, middlename, date, email,
                        phonenumber);
        Contacts.push_back(contact);
      } catch (const std::invalid_argument &e) {
        std::cerr << "Ошибка при загрузке контакта (возможна не пройдена "
                     "проверка типа)"
                  << e.what() << "\n";
      }
    }
  }

  void SaveToFile(const std::string &filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
      throw std::runtime_error("Не удалось открыть файл для записи");
    }
    for (const auto &contact : Contacts) {
      outFile << contact.GetFirstName() << "," << contact.GetLastName() << ","
              << contact.GetMiddleName() << "," << contact.GetDate() << ","
              << contact.GetEmail() << "," << contact.GetPhoneNumber() << "\n";
    }
  }
};
