#include <ctime>
#include <iostream>
#include <regex> // Регулярные выражения
#include <string>
#include <vector>


// Функция для проверки высокосного года для валидации Date
bool IsLeapYear(int year) {
  return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
};

class Contact {
private:
  std::string FirstName;
  std::string MiddleName;
  std::string LastName;
  std::string Date;
  std::string Email;
  std::string PhoneNumber;

  bool ValidateName(const std::string &name) {
    // Удаление пробелов
    std::string trimmedName =
        std::regex_replace(name, std::regex("^\\s+|\\s+$"), "");
    // Проверочное выражение
    std::regex nameRegex(
        "^[A-Za-zА-Яа-яЁё][A-Za-zА-Яа-яЁё0-9\\- ]*[A-Za-zА-Яа-яЁё0-9]$");
    return std::regex_match(trimmedName, nameRegex);
  };
  bool ValidateDate(const std::string &date) {
    std::regex dateRegex("^\\d{2}-\\d{2}-\\d{4}$");
    if (!std::regex_match(date, dateRegex)) {
      return false;
    }
    int day = std::stoi(date.substr(0, 2));
    int month = std::stoi(date.substr(3, 2));
    int year = std::stoi(date.substr(6, 4));
    if (month < 1 || month > 12) {
      return false;
    }
    // Количество дней в каждом месяце

    std::vector<int> DaysInMonth = {
        31, (IsLeapYear(year) ? 29 : 28), 31, 30, 31, 30, 31, 31, 30, 31, 30,
        31};
    if (day < 1 || day > DaysInMonth[month - 1]) {
      return false;
    }
    // Текущая дата
    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);

    int currentYear = now->tm_year + 1900;
    int currentMonth = now->tm_mon + 1;
    int currentDay = now->tm_mday;

    // Проверка даты меньше текущей
    if (year > currentYear || (year == currentYear && month > currentMonth) ||
        (year == currentYear && month == currentMonth && day >= currentDay)) {
      return false;
    }
    return true;
  };

  bool ValidateEmail(const std::string &email) {
    std::string trimmedEmail =
        std::regex_replace(email, std::regex("\\s+"), "");
    std::regex emailRegex("^[A-Za-z0-9]+@[A-Za-z0-9]+\\.[A-Za-z]{2,}$");
    return std::regex_match(trimmedEmail, emailRegex);
  };

  bool ValidatePhoneNumber(const std::string &phoneNumber) {
    std::string trimmedPhone =
        std::regex_replace(phoneNumber, std::regex("\\s+"), "");
    std::regex phoneRegex("^((\\+7|8)\\(?\\d{3}\\)?-?\\d{7}|(\\+7|8)\\(?\\d{3}\\)?-?\\d{3}-\\d{2}-\\d{2})$");
    return std::regex_match(trimmedPhone, phoneRegex);
  };

public:
  Contact() {
    FirstName = "NoFirstName";
    MiddleName = "NoMiddleName";
    LastName = "NoLastName";
    Date = "00-00-0000";
    Email = "noname@nomail.con";
    PhoneNumber = "+0(000)000-00-00";
  };
  Contact(const std::string &firstName, const std::string &middleName,
          const std::string &lastName, const std::string &date,
          const std::string &email, const std::string &phoneNumber)
      : FirstName(firstName), MiddleName(middleName), LastName(lastName),
        Date(date), Email(email), PhoneNumber(phoneNumber) {
    if (!ValidateName(FirstName)) {
      throw std::invalid_argument("Некорректное имя.");
    }
    if (!ValidateName(MiddleName)) {
      throw std::invalid_argument("Некорректное отчество.");
    }
    if (!ValidateName(LastName)) {
      throw std::invalid_argument("Некорректная фамилия.");
    }
    if (!ValidateDate(Date)) {
      throw std::invalid_argument("Некорректная дата.");
    }
    if (!ValidateEmail(Email)) {
      throw std::invalid_argument("Некорректный e-mail.");
    }
    if (!ValidatePhoneNumber(PhoneNumber)) {
      throw std::invalid_argument("Некорректный номер телефона.");
    }
  };

  // Геттеры
  std::string GetLastName() const { return LastName; };
  std::string GetFirstName() const { return FirstName; }
  std::string GetMiddleName() const { return MiddleName; };
  std::string GetDate() const { return Date; };
  std::string GetPhoneNumber() const { return PhoneNumber; };
  std::string GetEmail() const { return Email; };

  // Сеттеры
  void SetFirstName(const std::string &name) {
    if (ValidateName(name)) {
      FirstName = name;
    }
  }
  void SetMiddleName(const std::string &name) {
    if (ValidateName(name)) {
      MiddleName = name;
    }
  }
  void SetLastName(const std::string &name) {
    if (ValidateName(name)) {
      LastName = name;
    };
  }
  void SetDate(const std::string &date) {
    if (ValidateDate(date)) {
      Date = date;
    }
  };
  void SetEmail(const std::string &em) {
    if (ValidateEmail(em)) {
      Email = em;
    }
  };
  void SetPhoneNumber(const std::string &pn) {
    if (ValidatePhoneNumber(pn)) {
      PhoneNumber = pn;
    }
  }
};

void PrintContact(const Contact &cnt) {
  std::cout << "Person Contact\n";
  std::cout << cnt.GetLastName() << " " << cnt.GetFirstName() << " "
            << cnt.GetMiddleName() << " " << cnt.GetDate() << " "
            << cnt.GetPhoneNumber() << " " << cnt.GetEmail() << "\n";
}
