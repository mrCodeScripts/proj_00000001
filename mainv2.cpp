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

void makeTopBottomEdgeBorder(int bxLen, std::string &rEdgeChar, std::string &lEdgeChar, std::string &midEdge, std::vector<std::pair<std::string, std::string>> &colors, std::string &frame)
{
    // frame += '\n';
    for (int i = 0; i < bxLen; i++)
    {
        if (i == 0)
            frame += colors[0].first + lEdgeChar + colors[0].second;
        else if (i == (bxLen - 1))
            frame += colors[0].first + rEdgeChar + colors[0].second;
        else
            frame += colors[0].first + midEdge + colors[0].second;
    }
    frame += "                                          ";
    frame += '\n';
}
void makeTextWithColors(std::string &introPhrase, std::vector<std::pair<std::string, std::string>> &colors, int &xTabSize, int &yTabSize, std::string &midVertEdge, std::string &frame)
{
    int index = 0;
    std::string textFrame;
    std::string topBottomPadding;
    std::string leftPadding;
    std::string rightPadding;
    if (yTabSize > 0)
    {
        int xfullSpace = (xTabSize * 2) + introPhrase.size();
        for (int j = 1; j <= yTabSize; j++)
        {
            for (int i = 1; i <= xfullSpace; i++)
            {
                if (i == 1)
                    topBottomPadding += colors[0].first + midVertEdge + colors[0].second;
                else if (i == xfullSpace)
                    topBottomPadding += colors[0].first + midVertEdge + colors[0].second;
                else
                    topBottomPadding += " ";
            }
        }
    }
    if (xTabSize > 0)
    {
        for (int i = 1; i <= xTabSize; i++)
        {
            if (i == 1)
                leftPadding += colors[0].first + midVertEdge + colors[0].second;
            else
                leftPadding += ' ';
        }
        for (int i = 1; i <= xTabSize; i++)
        {
            if (i == xTabSize)
                rightPadding += colors[0].first + midVertEdge + colors[0].second;
            else
                rightPadding += ' ';
        }
    }
    for (const char &c : introPhrase)
    {
        textFrame += colors[index].first + std::string(1, c) + colors[index].second;
        index++;
        if (index >= colors.size())
            index = 0;
    }
    frame += topBottomPadding + "                                                                        ";
    frame += '\n' + leftPadding;
    frame += textFrame;
    frame += rightPadding + "                                                                        " + '\n';
    frame += topBottomPadding + "                                                                        " + '\n';
}

void introduction(float &accumilator, float &dt, float &speed, std::string &frame, std::chrono::high_resolution_clock::time_point &lastFrame, std::string &introPhrase, std::vector<std::pair<std::string, std::string>> &colors)
{
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = now - lastFrame;
    dt = elapsed.count();
    accumilator += speed * dt;
    lastFrame = now;
    if (accumilator >= 0.3f)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(colors.begin(), colors.end(), gen);
        accumilator -= 1.0f;
    }
    // std::string introPhrase = "";
    int introPhraseSize = introPhrase.size();
    int xTabSize = 5;
    int yTabSize = 1;
    int bxLen = xTabSize * 2 + introPhraseSize;
    std::string lTopEdge = u8"╔";
    std::string rTopEdge = u8"╗";
    std::string lBottomEdge = u8"╚";
    std::string rBottomEdge = u8"╝";
    std::string midVertEdge = u8"║";
    std::string midHorEdge = u8"═";
    makeTopBottomEdgeBorder(bxLen, rTopEdge, lTopEdge, midHorEdge, colors, frame);
    makeTextWithColors(introPhrase, colors, xTabSize, yTabSize, midVertEdge, frame);
    makeTopBottomEdgeBorder(bxLen, rBottomEdge, lBottomEdge, midHorEdge, colors, frame);
    frame += '\n';
}

// Utility functions (from your code)
void properClear()
{
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordinate = {0, 0};
    SetConsoleCursorPosition(hout, coordinate);
}

void removeCursor()
{
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO inf;
    GetConsoleCursorInfo(hout, &inf);
    inf.bVisible = false;
    SetConsoleCursorInfo(hout, &inf);
}

void hardClear()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void detectKeyboard(int &index, int maxIndex, bool &pressEntered, bool &pressedEsc, bool &pressedPayment, bool &pressedCancel)
{
    if (_kbhit())
    {
        int k = _getch();
        if (k == 0 || k == 224)
        {
            k = _getch();
            switch (k)
            {
            case 72:
                index--;
                if (index < 0)
                    index = maxIndex;
                break;
            case 80:
                index++;
                if (index > maxIndex)
                    index = 0;
                break;
            }
        }
        else if (k == 13)
        {
            pressEntered = true;
        }
        else if (k == 27)
        {
            pressedEsc = true;
        }
        else if (k == 99 || k == 67)
        {
            pressedCancel = true;
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
    std::string introPhrase,
    std::vector<std::pair<std::string, std::string>> &colors)
{
    removeCursor();
    bool choosing = true;
    int index = 0;
    bool pressEnter = false;
    bool pressedEsc = false;
    bool pressedPayment = false;
    bool pressedCancelPayment = false;
    bool initialClear = false;

    while (choosing)
    {
        if (initialClear)
        {
            properClear();
        }
        else
        {
            hardClear();
            initialClear = true;
        }
        std::string frame;
        introduction(accumilator, dt, speed, frame, lastFrame, introPhrase, colors);
        for (int i = 0; i < foods.size(); i++)
        {
            bool selected = false;
            for (auto &cf : chosenFoods)
            {
                if (cf.first == foods[i].second)
                    selected = true;
            }

            if (i == index)
                frame += u8" ► \033[1;92m$" + std::to_string(foods[i].first) + " " + foods[i].second + "                       " + "\033[0m";
            else
                frame += u8"   $" + std::to_string(foods[i].first) + " " + foods[i].second + "                       ";

            if (selected)
                frame += " [selected]";
            frame += '\n';
        }

        // Show current total
        double total = 0;
        frame += "\nCurrent selections:\n";
        for (auto &cf : chosenFoods)
        {
            frame += "  " + cf.first + " x" + std::to_string(cf.second.first) + " = $" + std::to_string(cf.second.second * cf.second.first) + '\n';
            total += cf.second.first * cf.second.second;
        }
        frame += "Total: $" + std::to_string(total) + "                                " + "\n";
        frame += "                          ";

        std::cout << frame;

        detectKeyboard(index, foods.size() - 1, pressEnter, pressedEsc, pressedPayment, pressedCancelPayment);

        if (pressEnter)
        {
            pressEnter = false;
            auto it = std::find_if(chosenFoods.begin(), chosenFoods.end(), [&](auto &cf)
                                   { return cf.first == foods[index].second; });
            if (it != chosenFoods.end())
            {
                it->second.first++; // increase quantity
            }
            else
            {
                chosenFoods.push_back({foods[index].second, {1, foods[index].first}});
            }
        }

        if (pressedEsc)
        {
            pressedEsc = false;
            break; // exit food menu to main menu
        }

        if (pressedCancelPayment)
        {
            for (auto it = chosenFoods.begin(); it != chosenFoods.end();)
            {
                if (std::find_if(foods.begin(), foods.end(), [&](auto &f)
                                 { return f.second == it->first; }) != foods.end())
                {
                    it = chosenFoods.erase(it);
                }
                else
                    ++it;
            }
        }

        if (pressedPayment)
        {
            properClear();
            std::cout << "----- RECEIPT -----\n";
            double total = 0;
            for (auto &cf : chosenFoods)
            {
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
    bool pressedPayment = false;
    bool pressedCancelPayment = false;
    bool initialClear = false;

    std::vector<std::pair<std::string, std::pair<int, double>>> chosenFoods; // food name, {quantity, price}

    while (choosing)
    {
        if (initialClear)
        {
            properClear();
        }
        else
        {
            hardClear();
            initialClear = true;
        }
        std::string frame;
        std::string introPhrase = "Welcome to our restaurant (ENTER to open, ESC to exit)";
        introduction(accumilator, dt, speed, frame, lastFrame, introPhrase, colors);
        for (int i = 0; i < restaurantMenu.size(); i++)
        {
            if (i == menuIndex)
            {
                frame += u8" ► " + std::string("\033[1;92m") + restaurantMenu[i].first + "\033[0m" + "                       " + '\n';
            }
            else
            {
                frame += "   " + restaurantMenu[i].first + "                       " + '\n';
            }
        }

        double total = 0;
        if (!chosenFoods.empty())
        {
            frame += "\nCurrent selections across menus:\n";
            for (auto &cf : chosenFoods)
            {
                frame += "  " + cf.first + " x" + std::to_string(cf.second.first) + " = $" + std::to_string(cf.second.first * cf.second.second) + '\n';
                total += cf.second.first * cf.second.second;
            }
            frame += "Total: $" + std::to_string(total) + "\n";
            frame += "Press P to pay\n";
        }

        std::cout << frame;

        detectKeyboard(menuIndex, restaurantMenu.size() - 1, pressEnter, pressedEsc, pressedPayment, pressedCancelPayment);

        if (pressEnter)
        {
            pressEnter = false;
            std::string foodIntroPhrase = "Select your food (ENTER to add, ESC to go back, P to pay, C to cancel current menu)";
            chooseFood(menuIndex, restaurantMenu[menuIndex].second, chosenFoods, accumilator, dt, speed, lastFrame, foodIntroPhrase, colors);
        }

        if (pressedEsc)
        {
            pressedEsc = false;
            break;
        }

        if (pressedPayment)
        {
            properClear();
            std::cout << "----- RECEIPT -----\n";
            double total = 0;
            for (auto &cf : chosenFoods)
            {
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

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    float accumilator = 0;
    float dt = 0;
    float speed = 0.9f;
    auto lastFrame = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<std::string, std::string>> colors = {
        {"\033[1;91m", "\033[0m"}, {"\033[1;92m", "\033[0m"}, {"\033[1;93m", "\033[0m"}, {"\033[1;94m", "\033[0m"}, {"\033[1;95m", "\033[0m"}, {"\033[1;96m", "\033[0m"}, {"\033[1;97m", "\033[0m"}};

    std::vector<std::pair<std::string, std::vector<std::pair<double, std::string>>>> restaurantMenu = {
        {"Menu 1", {{99.99, "Burger"}, {129.50, "Cheese Burger"}, {149.75, "Bacon Burger"}}},
        {"Menu 2", {{199.99, "Pepperoni Pizza"}, {249.99, "Four Cheese"}, {229.99, "Hawaiian"}}},
        {"Menu 3", {{49.99, "Iced Tea"}, {59.99, "Coke"}, {39.99, "Water"}}},
    };

    menuSelection(restaurantMenu, accumilator, dt, speed, lastFrame, colors);
    return 0;
}
