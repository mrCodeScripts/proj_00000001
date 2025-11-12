#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include <random>
#include <vector>
#include <utility>
#include <conio.h>
#include <string>
#include <iomanip> // for std::setw

// Utility functions (from your code)
void properClear() {
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordinate = {0, 0};
    SetConsoleCursorPosition(hout, coordinate);
}

void removeCursor() {
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO inf;
    GetConsoleCursorInfo(hout, &inf);
    inf.bVisible = false;
    SetConsoleCursorInfo(hout, &inf);
}

void hardClear() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void detectKeyboard(int &index, int maxIndex, bool &pressEntered, bool &pressedEsc) {
    if (_kbhit()) {
        int k = _getch();
        if (k == 0 || k == 224) {
            k = _getch();
            switch (k) {
                case 72: index--; if (index < 0) index = maxIndex; break;
                case 80: index++; if (index > maxIndex) index = 0; break;
            }
        } else if (k == 13) {
            pressEntered = true;
        } else if (k == 27) {
            pressedEsc = true;
        }
    }
}

// Function to show menu and handle ESC navigation, selection, cancellation, and payment
void chooseFood(
    int menuIndex,
    std::vector<std::pair<double, std::string>> &foods,
    std::vector<std::pair<std::string, std::pair<int, double>>> &chosenFoods,
    float &accumilator, float &dt, float &speed,
    std::chrono::high_resolution_clock::time_point &lastFrame,
    std::vector<std::pair<std::string, std::string>> &colors)
{
    removeCursor();
    bool choosing = true;
    int index = 0;
    bool pressEnter = false;
    bool pressedEsc = false;

    while (choosing) {
        properClear();
        std::string frame = "\nSelect your food (ENTER to add, ESC to go back, P to pay, C to cancel current menu):\n\n";

        // Show food list
        for (int i = 0; i < foods.size(); i++) {
            bool selected = false;
            for (auto &cf : chosenFoods) {
                if (cf.first == foods[i].second) selected = true;
            }

            if (i == index) frame += " ► ";
            else frame += "    ";

            frame += "$" + std::to_string(foods[i].first) + " " + foods[i].second;
            if (selected) frame += " [selected]";
            frame += '\n';
        }

        // Show current total
        double total = 0;
        frame += "\nCurrent selections:\n";
        for (auto &cf : chosenFoods) {
            frame += "  " + cf.first + " x" + std::to_string(cf.second.first) + " = $" + std::to_string(cf.second.second * cf.second.first) + '\n';
            total += cf.second.first * cf.second.second;
        }
        frame += "Total: $" + std::to_string(total) + "\n";

        std::cout << frame;

        // Handle keyboard input
        detectKeyboard(index, foods.size() - 1, pressEnter, pressedEsc);

        // Add food
        if (pressEnter) {
            pressEnter = false;
            auto it = std::find_if(chosenFoods.begin(), chosenFoods.end(), [&](auto &cf){ return cf.first == foods[index].second; });
            if (it != chosenFoods.end()) {
                it->second.first++; // increase quantity
            } else {
                chosenFoods.push_back({foods[index].second, {1, foods[index].first}});
            }
        }

        // ESC: go back
        if (pressedEsc) {
            pressedEsc = false;
            break; // exit food menu to main menu
        }

        // Cancel current menu
        if (_kbhit()) {
            int k = _getch();
            if (k == 'c' || k == 'C') {
                // Remove all items of this menu
                for (auto it = chosenFoods.begin(); it != chosenFoods.end();) {
                    if (std::find_if(foods.begin(), foods.end(), [&](auto &f){ return f.second == it->first; }) != foods.end()) {
                        it = chosenFoods.erase(it);
                    } else ++it;
                }
            }
            if (k == 'p' || k == 'P') {
                // Pay: generate receipt
                properClear();
                std::cout << "----- RECEIPT -----\n";
                double total = 0;
                for (auto &cf : chosenFoods) {
                    std::cout << cf.first << " x" << cf.second.first << " = $" << cf.second.first * cf.second.second << '\n';
                    total += cf.second.first * cf.second.second;
                }
                std::cout << "Total: $" << total << "\n";
                std::cout << "Thank you for your order!\n";
                chosenFoods.clear();
                system("pause");
            }
        }
    }
}

void menuSelection(
    std::vector<std::pair<std::string, std::vector<std::pair<double, std::string>>>> &restaurantMenu,
    float &accumilator, float &dt, float &speed,
    std::chrono::high_resolution_clock::time_point &lastFrame,
    std::vector<std::pair<std::string, std::string>> &colors)
{
    removeCursor();
    bool choosing = true;
    int menuIndex = 0;
    bool pressEnter = false;
    bool pressedEsc = false;

    std::vector<std::pair<std::string, std::pair<int,double>>> chosenFoods;

    while (choosing) {
        properClear();
        std::string frame = "\nSelect a Menu (ENTER to open, ESC to exit):\n\n";
        for (int i = 0; i < restaurantMenu.size(); i++) {
            if (i == menuIndex) frame += " ► ";
            else frame += "    ";
            frame += restaurantMenu[i].first + '\n';
        }

        // Show current selections total
        double total = 0;
        if (!chosenFoods.empty()) {
            frame += "\nCurrent selections across menus:\n";
            for (auto &cf : chosenFoods) {
                frame += "  " + cf.first + " x" + std::to_string(cf.second.first) + " = $" + std::to_string(cf.second.first * cf.second.second) + '\n';
                total += cf.second.first * cf.second.second;
            }
            frame += "Total: $" + std::to_string(total) + "\n";
            frame += "Press P to pay\n";
        }

        std::cout << frame;

        detectKeyboard(menuIndex, restaurantMenu.size() - 1, pressEnter, pressedEsc);

        if (pressEnter) {
            pressEnter = false;
            chooseFood(menuIndex, restaurantMenu[menuIndex].second, chosenFoods, accumilator, dt, speed, lastFrame, colors);
        }

        if (pressedEsc) {
            pressedEsc = false;
            break; // exit program
        }

        if (_kbhit()) {
            int k = _getch();
            if (k == 'p' || k == 'P') {
                // Pay
                properClear();
                std::cout << "----- RECEIPT -----\n";
                double total = 0;
                for (auto &cf : chosenFoods) {
                    std::cout << cf.first << " x" << cf.second.first << " = $" << cf.second.first * cf.second.second << '\n';
                    total += cf.second.first * cf.second.second;
                }
                std::cout << "Total: $" << total << "\n";
                std::cout << "Thank you for your order!\n";
                chosenFoods.clear();
                system("pause");
            }
        }
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    float accumilator = 0; 
    float dt = 0;
    float speed = 0.9f;
    auto lastFrame = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<std::string,std::string>> colors = {
        {"\033[1;91m","\033[0m"}, {"\033[1;92m","\033[0m"}, {"\033[1;93m","\033[0m"},
        {"\033[1;94m","\033[0m"}, {"\033[1;95m","\033[0m"}, {"\033[1;96m","\033[0m"},
        {"\033[1;97m","\033[0m"}
    };

    std::vector<std::pair<std::string, std::vector<std::pair<double,std::string>>>> restaurantMenu = {
        {"Menu 1", {{99.99,"Burger"}, {129.50,"Cheese Burger"}, {149.75,"Bacon Burger"}}},
        {"Menu 2", {{199.99,"Pepperoni Pizza"}, {249.99,"Four Cheese"}, {229.99,"Hawaiian"}}},
        {"Menu 3", {{49.99,"Iced Tea"}, {59.99,"Coke"}, {39.99,"Water"}}},
    };

    menuSelection(restaurantMenu, accumilator, dt, speed, lastFrame, colors);

    return 0;
}
