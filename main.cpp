#include <iostream>
#include <string>
#include <utility>
#include <windows.h>
#include <cstdlib>
#include <vector>
#include <conio.h>
#include <chrono>
#include <thread>

// --- Detect Keyboard ---
void detectKeyboard (int &index, int maxIndex, int &chosen, bool &keyError) {
    if (_kbhit()) {
        int k = _getch();
        if (k == 0 || k == 224) { // handle special keys
            k = _getch();
            switch (k) {
                case 72: // up
                    keyError = false;
                    index--;
                    if (index < 0) index = maxIndex;
                    break;
                case 80: // down
                    keyError = false;
                    index++;
                    if (index > maxIndex) index = 0;
                    break;
                default:
                    keyError = true;
                    break;
            }
        } else if (k == 13) { // enter
            keyError = false;
            chosen = index;
            index = -2;
        } else {
            keyError = true;
        }
    }
}

// --- Generic Menu Function ---
void choicesYM (int &chosen, std::vector<std::pair<std::string, std::string>> &choices, 
                const std::string &title) {
    bool choosing = true;
    int index = 0;
    bool keyError = false;

    while (choosing) {
        system("cls");

        std::cout << "\n==========================\n";
        std::cout << "| " << title << " |\n";
        std::cout << "==========================\n\n";

        for (int i = 0; i < choices.size(); i++) {
            if (i == index)
                std::cout << "👉 " << choices[i].first << std::endl;
            else
                std::cout << "   " << choices[i].first << std::endl;
        }

        if (keyError) {
            std::cout << R"(

========================================================
| INVALID CHOICE. PLEASE USE ↑ (UP) and ↓ (DOWN) KEYS! |
========================================================
)";
        }

        detectKeyboard(index, static_cast<int>(choices.size() - 1), chosen, keyError);
        if (index == -2) choosing = false;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

// --- Username & Password ---
void askUsername (std::string &username, bool &errorName, 
                  std::vector<std::pair<std::string, std::string>> &allowedUsers) {
    std::cout << "Username: ";
    std::getline(std::cin, username);

    errorName = true;
    for (const auto &u : allowedUsers) {
        if (u.first == username) {
            errorName = false;
            break;
        }
    }
}

void askPassword (std::string username, std::string &password, bool &errPassword, 
                  std::vector<std::pair<std::string, std::string>> &allowedUsers) { 
    std::cout << "Password: ";
    std::getline(std::cin, password);

    errPassword = true;
    for (const auto &u : allowedUsers) {
        if (u.first == username && u.second == password) {
            errPassword = false;
            break;
        }
    }
}

// --- Age Input ---
void getAge () {
    int age;
    std::cout << "ENTER YOUR AGE: ";
    std::cin >> age;
    std::cin.ignore(); // prevent leftover newline issues

    if (age >= 15 && age <= 17)
        std::cout << "ELIGIBLE FOR SK CATEGORY (SANGGUNIANG KABATAAN)" << std::endl;
    else if (age >= 18 && age <= 30)
        std::cout << "ELIGIBLE FOR SK AND BARANGGAY CATEGORY" << std::endl;
    else if (age >= 31 && age <= 60)
        std::cout << "ELIGIBLE FOR BARANGGAY OFFICIAL CATEGORY" << std::endl;
    else if (age >= 61)
        std::cout << "ELIGIBLE FOR BARANGGAY OFFICIAL WITH ASSISTANT CATEGORY" << std::endl;
    else
        std::cout << "NOT ELIGIBLE TO PARTICIPATE." << std::endl;
}

// --- Vote Menu ---
void vote () {
    int choice = 0;
    std::vector<std::pair<std::string, std::string>> voteType = {
        {"Iliterate", "I"},
        {"Person with disabilities", "P"},
        {"Senior Citizen", "C"},
        {"None", "N"},
    };

    choicesYM(choice, voteType, "CHOOSE TYPE OF VOTER");

    switch (choice) {
        case 0:
            std::cout << R"(
==========================
| YOU NEED AN ASSISTANT! |
==========================
)";
            getAge();
            break;
        case 1:
        case 2:
        case 3:
            getAge();
            break;
        default:
            std::cout << "INVALID SELECTION!" << std::endl;
            break;
    }
}

// --- Main ---
int main () {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::ios_base::sync_with_stdio(false);

    std::vector<std::pair<std::string, std::string>> allowedUsers = {
        {"Admin", "12345"},
        {"Voter", "12345"}
    };

    std::string username, password;
    bool errorName = false, errorPassword = false;
    int accessAdminSettings = 0;

    while (true) {
        system("cls");

        if (errorName)
            std::cout << "============================\n"
                         "| Username does not exist! |\n"
                         "============================\n\n";
        if (errorPassword)
            std::cout << "=======================\n"
                         "| Incorrect Password! |\n"
                         "=======================\n\n";

        askUsername(username, errorName, allowedUsers);
        if (errorName) continue;
        askPassword(username, password, errorPassword, allowedUsers);
        if (errorPassword) continue;

        break;
    }

    std::cout << R"(
==========================
| Now you are logged in! |
==========================
)" << std::endl;
    std::cout << "WELCOME " << username << std::endl << std::endl;

    std::vector<std::pair<std::string, std::string>> adminChoices = {
        {"Yes", "yes"},
        {"No", "no"}
    };

    choicesYM(accessAdminSettings, adminChoices, "ACCESS ADMIN SETTINGS?");

    if (accessAdminSettings == 0) {
        std::cout << R"(
===========================
| ADMIN SETTINGS UNLOCKED |
===========================
)" << std::endl;
    } else {
        std::cout << R"(
==========================
| LIMITED ACCESS GRANTED |
==========================
)" << std::endl;
    }

    vote();

    return 0;
}
