#include "contact.h"
#include <regex>
#include <stdexcept>
#include <iostream>
#include <ctime>

// Регулярные выражения
static const std::regex NameRegex("^[A-Za-zА-Яа-яЁё][A-Za-zА-Яа-яЁё\\- ]*[A-Za-zА-Яа-яЁё]$");
static const std::regex DateRegex("^\\d{2}-\\d{2}-\\d{4}$");
static const std::regex EmailRegex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
static const std::regex PhoneRegex("^\\+?[0-9\\-\\(\\) ]{7,}$");
static const std::regex TrimSpacesRegex("^\\s+|\\s+$");
static const std::regex RemoveAllSpacesRegex("\\s+");

// Конструктор по умолчанию
Contact::Contact()
    : FirstName("NoFirstName"), MiddleName("NoMiddleName"),
    LastName("NoLastName"), Date("00-00-0000"),
    Email("noname@nomail.com"), PhoneNumber("+0000000000") {}

// Конструктор с параметрами
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

// Геттеры
std::string Contact::GetFirstName() const { return FirstName; }
std::string Contact::GetMiddleName() const { return MiddleName; }
std::string Contact::GetLastName() const { return LastName; }
std::string Contact::GetDate() const { return Date; }
std::string Contact::GetEmail() const { return Email; }
std::string Contact::GetPhoneNumber() const { return PhoneNumber; }

// Сеттеры с валидацией
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

// Метод для получения всех полей
std::vector<std::string> Contact::GetAllFields() const {
    return { FirstName, MiddleName, LastName, Date, Email, PhoneNumber };
}

// Метод для печати контакта (опционально)
void Contact::PrintContact() const {
    std::cout << "Contact: " << LastName << " " << FirstName << " "
              << MiddleName << ", Date of birth: " << Date
              << ", Phone number: " << PhoneNumber << ", Email: "
              << Email << "\n";
}

// Проверка имени
bool Contact::IsValidName(const std::string &name) const {
    std::string trimmedName = std::regex_replace(name, TrimSpacesRegex, "");
    return std::regex_match(trimmedName, NameRegex);
}

// Проверка даты
bool Contact::IsValidDate(const std::string &date) const {
    if (!std::regex_match(date, DateRegex)) {
        return false;
    }

    // Разбор даты
    int day = std::stoi(date.substr(0, 2));
    int month = std::stoi(date.substr(3, 2));
    int year = std::stoi(date.substr(6, 4));

    if (month < 1 || month > 12) {
        return false;
    }

    // Високосные года
    bool isLeap = false;
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        isLeap = true;

    std::vector<int> DaysInMonth = {
        31, 28 + isLeap, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    if (day < 1 || day > DaysInMonth[month - 1]) {
        return false;
    }

    // Проверка, что дата не в будущем
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

// Проверка email
bool Contact::IsValidEmail(const std::string &email) const {
    std::string trimmedEmail = std::regex_replace(email, RemoveAllSpacesRegex, "");
    return std::regex_match(trimmedEmail, EmailRegex);
}

// Проверка номера телефона
bool Contact::IsValidPhoneNumber(const std::string &phoneNumber) const {
    std::string trimmedPhone = std::regex_replace(phoneNumber, RemoveAllSpacesRegex, "");
    return std::regex_match(trimmedPhone, PhoneRegex);
}
