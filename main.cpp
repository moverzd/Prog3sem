#include <iostream>
#include <string>
#include <limits>
#include "ContactManager.h"


int main(){
    ContactManager manager;
    Contact cnt{"svetoslavov", "svetoslav", "svetoslavovich", "10-10-2023", "test@test.com", "+7(912)-512-52-52"};
    manager.AddContact(cnt);
    manager.SaveToFile("contacts.csv");
} 
