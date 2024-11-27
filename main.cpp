#include <clocale>
#include <iostream>
#include <string>
#include "ContactManager.h"

// Function declarations
void AddContact(ContactManager &manager);
void RemoveContact(ContactManager &manager);
void EditContact(ContactManager &manager);
void EditContactFields(Contact &contact);
void ShowAllContacts(ContactManager &manager);

// Helper functions
bool GetInput(const std::string &prompt, std::string &input) {
    std::cout << prompt;
    std::getline(std::cin, input);
    if (input == "quit") {
        return false;
    }
    return true;
}

template <typename ValidationFunc>
// promt - invitig message, input - obvious, validateFunc - validator, errorMessage - obvious
bool GetValidatedInput(const std::string &prompt, std::string &input,
                       ValidationFunc validateFunc, const std::string &errorMessage) {
    while (true) {
        if (!GetInput(prompt, input)) {
            return false;
        }
        if (validateFunc(input)) {
            return true;
        } else {
            std::cout << errorMessage << " Please try again or enter 'quit' to cancel.\n";
        }
    }
}

int main() {
    std::locale::global(std::locale("ru_RU.UTF-8"));
    ContactManager manager;

    // Load contacts from file
    try {
        manager.LoadFromFile("contacts.csv");
        std::cout << "Contacts successfully loaded from file.\n";
    } catch (const std::exception &e) {
        std::cout << "Error loading contacts: " << e.what() << "\n";
    }

    while (true) {
        std::cout << "\nChoose an action:\n";
        std::cout << "1. Add contact\n";
        std::cout << "2. Remove contact\n";
        std::cout << "3. Edit contact\n";
        std::cout << "4. Show all contacts\n";
        std::cout << "0. Exit\n";
        std::cout << "Your choice: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 0) {
            break;
        }

        switch (choice) {
            case 1:
                AddContact(manager);
                break;
            case 2:
                RemoveContact(manager);
                break;
            case 3:
                EditContact(manager);
                break;
            case 4:
                ShowAllContacts(manager);
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    }

    // Save contacts to file before exiting
    try {
        manager.SaveToFile("contacts.csv");
        std::cout << "Contacts successfully saved to file.\n";
    } catch (const std::exception &e) {
        std::cout << "Error saving contacts: " << e.what() << "\n";
    }

    std::cout << "Program terminated.\n";
    return 0;
}

// Implementation of the function to add a contact
void AddContact(ContactManager &manager) {
    std::string firstName, middleName, lastName, date, email, phoneNumber;

    std::cout << "\nAdding a new contact:\n";

    if (!GetValidatedInput("First name: ", firstName, Contact::IsValidName, "Invalid first name.")) return;
    if (!GetValidatedInput("Middle name: ", middleName, Contact::IsValidName, "Invalid middle name.")) return;
    if (!GetValidatedInput("Last name: ", lastName, Contact::IsValidName, "Invalid last name.")) return;
    if (!GetValidatedInput("Date of birth (dd-mm-yyyy): ", date, Contact::IsValidDate, "Invalid date.")) return;
    if (!GetValidatedInput("Email: ", email, Contact::IsValidEmail, "Invalid email.")) return;
    if (!GetValidatedInput("Phone number: ", phoneNumber, Contact::IsValidPhoneNumber, "Invalid phone number.")) return;

    // Create contact and add to manager
    try {
        Contact contact(firstName, middleName, lastName, date, email, phoneNumber);
        manager.AddContact(contact);
        std::cout << "Contact successfully added.\n";
    } catch (const std::exception &e) {
        std::cout << "Error creating contact: " << e.what() << "\n";
        std::cout << "Please try again.\n";
    }
}

// Implementation of the function to remove a contact
void RemoveContact(ContactManager &manager) {
    std::cout << "\nRemoving a contact:\n";
    const auto &contacts = manager.GetAllContacts();

    if (contacts.empty()) {
        std::cout << "No contacts to remove.\n";
        return;
    }

    // Display list of contacts with indices
    for (size_t i = 0; i < contacts.size(); ++i) {
        std::cout << i + 1 << ". ";
        contacts[i].PrintContact();
    }

    size_t index;
    while (true) {
        std::cout << "Enter the number of the contact to remove (or 0 to cancel): ";
        if (!(std::cin >> index)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        if (index == 0) {
            std::cout << "Removal cancelled.\n";
            return;
        } else if (index > contacts.size()) {
            std::cout << "Invalid number. Please try again.\n";
            continue;
        } else {
            index -= 1;
            break;
        }
    }

    try {
        manager.RemoveContact(index);
        std::cout << "Contact successfully removed.\n";
    } catch (const std::exception &e) {
        std::cout << "Error removing contact: " << e.what() << "\n";
    }
}

// Implementation of the function to show all contacts
void ShowAllContacts(ContactManager &manager) {
    const auto &contacts = manager.GetAllContacts();
    if (contacts.empty()) {
        std::cout << "Contact list is empty.\n";
    } else {
        std::cout << "\nList of all contacts:\n";
        for (const auto &contact : contacts) {
            contact.PrintContact();
        }
    }
}

// Implementation of the function to edit a contact
void EditContact(ContactManager &manager) {
    std::cout << "\nEditing a contact:\n";
    const auto &contacts = manager.GetAllContacts();

    if (contacts.empty()) {
        std::cout << "No contacts to edit.\n";
        return;
    }

    // Display list of contacts with indices
    for (size_t i = 0; i < contacts.size(); ++i) {
        std::cout << i + 1 << ". ";
        contacts[i].PrintContact();
    }

    size_t index;
    while (true) {
        std::cout << "Enter the number of the contact to edit (or 0 to cancel): ";
        if (!(std::cin >> index)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (index == 0) {
            std::cout << "Editing cancelled.\n";
            return;
        } else if (index > contacts.size()) {
            std::cout << "Invalid number. Please try again.\n";
            continue;
        } else {
            index -= 1;
            break;
        }
    }

    Contact &contact = manager.GetContact(index);
    EditContactFields(contact);
}

// Implementation of the function to edit contact fields
void EditContactFields(Contact &contact) {
    while (true) {
        std::cout << "\nSelect the field to edit:\n";
        std::cout << "1. First name\n";
        std::cout << "2. Middle name\n";
        std::cout << "3. Last name\n";
        std::cout << "4. Date of birth\n";
        std::cout << "5. Email\n";
        std::cout << "6. Phone number\n";
        std::cout << "0. Return to main menu\n";
        std::cout << "Your choice: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (choice == 0) {
            break;
        }

        std::string newValue;

        try {
            switch (choice) {
                case 1:
                    if (GetValidatedInput("New first name: ", newValue, Contact::IsValidName, "Invalid first name.")) {
                        contact.SetFirstName(newValue);
                        std::cout << "First name updated successfully.\n";
                    }
                    break;
                case 2:
                    if (GetValidatedInput("New middle name: ", newValue, Contact::IsValidName, "Invalid middle name.")) {
                        contact.SetMiddleName(newValue);
                        std::cout << "Middle name updated successfully.\n";
                    }
                    break;
                case 3:
                    if (GetValidatedInput("New last name: ", newValue, Contact::IsValidName, "Invalid last name.")) {
                        contact.SetLastName(newValue);
                        std::cout << "Last name updated successfully.\n";
                    }
                    break;
                case 4:
                    if (GetValidatedInput("New date of birth (dd-mm-yyyy): ", newValue, Contact::IsValidDate, "Invalid date.")) {
                        contact.SetDate(newValue);
                        std::cout << "Date of birth updated successfully.\n";
                    }
                    break;
                case 5:
                    if (GetValidatedInput("New email: ", newValue, Contact::IsValidEmail, "Invalid email.")) {
                        contact.SetEmail(newValue);
                        std::cout << "Email updated successfully.\n";
                    }
                    break;
                case 6:
                    if (GetValidatedInput("New phone number: ", newValue, Contact::IsValidPhoneNumber, "Invalid phone number.")) {
                        contact.SetPhoneNumber(newValue);
                        std::cout << "Phone number updated successfully.\n";
                    }
                    break;
                default:
                    std::cout << "Invalid choice. Please try again.\n";
                    break;
            }
        } catch (const std::exception &e) {
            std::cout << "Error updating field: " << e.what() << "\n";
        }
    }
}
