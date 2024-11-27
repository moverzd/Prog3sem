#include "Contact.h"
#include <iostream>
#include <regex>
#include <ctime>
#include <vector>
#include <stdexcept>

//Regular expression
const std::regex NameRegex("^[A-Za-zА-Яа-яЁё][A-Za-zА-Яа-яЁё\\- ]*[A-Za-zА-Яа-яЁё]$");
const std::regex DateRegex("^\\d{2}-\\d{2}-\\d{4}$");
const std::regex EmailRegex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
const std::regex PhoneRegex("^\\+?[0-9\\-\\(\\) ]{7,}$");
const std::regex TrimSpacesRegex("^\\s+|\\s+$");
const std::regex RemoveAllSpacesRegex("\\s+");


//Static methods of validation
bool Contact::IsValidName(const std::string &name) {
    std::string trimmedName = std::regex_replace(name, TrimSpacesRegex, "");
    return std::regex_match(trimmedName, NameRegex);
}

bool Contact::IsValidDate(const std::string &date) {
    if (!std::regex_match(date, DateRegex)) {
        return false;
    }
    int day = std::stoi(date.substr(0, 2));
    int month = std::stoi(date.substr(3, 2));
    int year = std::stoi(date.substr(6, 4));
    if (month < 1 || month > 12) {
        return false;
    }
    std::vector<int> DaysInMonth = {
        31, 28 + (year % 4 == 0 ? 1 : 0), 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };
    if (day < 1 || day > DaysInMonth[month - 1]) {
        return false;
    }
    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);
    int currentYear = now->tm_year + 1900;
    int currentMonth = now->tm_mon + 1;
    int currentDay = now->tm_mday;
    if (year > currentYear ||
        (year == currentYear && month > currentMonth) ||
        (year == currentYear && month == currentMonth && day > currentDay)) {
        return false;
    }
    return true;
}

bool Contact::IsValidEmail(const std::string &email) {
    std::string trimmedEmail = std::regex_replace(email, RemoveAllSpacesRegex, "");
    return std::regex_match(trimmedEmail, EmailRegex);
}

bool Contact::IsValidPhoneNumber(const std::string &phoneNumber) {
    std::string trimmedPhone = std::regex_replace(phoneNumber, RemoveAllSpacesRegex, "");
    return std::regex_match(trimmedPhone, PhoneRegex);
}

// Constructors
Contact::Contact()
    : FirstName("NoFirstName"), MiddleName("NoMiddleName"),
      LastName("NoLastName"), Date("00-00-0000"),
      Email("noname@nomail.com"), PhoneNumber("+0000000000") {}

Contact::Contact(const std::string &firstName, const std::string &middleName,
                 const std::string &lastName, const std::string &date,
                 const std::string &email, const std::string &phoneNumber) {
    if (!IsValidName(firstName)) {
        throw std::invalid_argument("Incorrect first name");
    }
    if (!IsValidName(middleName)) {
        throw std::invalid_argument("Incorrect middle name");
    }
    if (!IsValidName(lastName)) {
        throw std::invalid_argument("Incorrect last name");
    }
    if (!IsValidDate(date)) {
        throw std::invalid_argument("Incorrect date");
    }
    if (!IsValidEmail(email)) {
        throw std::invalid_argument("Incorrect email");
    }
    if (!IsValidPhoneNumber(phoneNumber)) {
        throw std::invalid_argument("Incorrect phone number");
    }
    FirstName = firstName;
    MiddleName = middleName;
    LastName = lastName;
    Date = date;
    Email = email;
    PhoneNumber = phoneNumber;
}


// Getters
std::string Contact::GetFirstName() const { return FirstName; }
std::string Contact::GetMiddleName() const { return MiddleName; }
std::string Contact::GetLastName() const { return LastName; }
std::string Contact::GetDate() const { return Date; }
std::string Contact::GetEmail() const { return Email; }
std::string Contact::GetPhoneNumber() const { return PhoneNumber; }


// Setters
void Contact::SetFirstName(const std::string &firstName) {
    if (!IsValidName(firstName)) {
        throw std::invalid_argument("Некорректное имя.");
    }
    FirstName = firstName;
}

void Contact::SetMiddleName(const std::string &middleName) {
    if (!IsValidName(middleName)) {
        throw std::invalid_argument("Некорректное отчество.");
    }
    MiddleName = middleName;
}

void Contact::SetLastName(const std::string &lastName) {
    if (!IsValidName(lastName)) {
        throw std::invalid_argument("Некорректная фамилия.");
    }
    LastName = lastName;
}

void Contact::SetDate(const std::string &date) {
    if (!IsValidDate(date)) {
        throw std::invalid_argument("Некорректная дата.");
    }
    Date = date;
}

void Contact::SetEmail(const std::string &email) {
    if (!IsValidEmail(email)) {
        throw std::invalid_argument("Некорректный email.");
    }
    Email = email;
}

void Contact::SetPhoneNumber(const std::string &phoneNumber) {
    if (!IsValidPhoneNumber(phoneNumber)) {
        throw std::invalid_argument("Некорректный номер телефона.");
    }
    PhoneNumber = phoneNumber;
}

//Output info about contact
void Contact::PrintContact() const {
    std::cout << "Contact: " << LastName << " " << FirstName << " "
              << MiddleName << ", Date of birth: " << Date
              << ", Phonenumber: " << PhoneNumber << ", Email: "
              << Email << "\n";
}
