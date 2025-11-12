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

void makeTopBottomEdgeBorder(int bxLen, std::string &rEdgeChar, std::string &lEdgeChar, std::string &midEdge, std::vector<std::pair<std::string, std::string>> &colors, std::string &frame)
{
    frame += '\n';
    for (int i = 0; i < bxLen; i++)
    {
        if (i == 0)
            frame += colors[0].first + lEdgeChar + colors[0].second;
        else if (i == (bxLen - 1))
            frame += colors[0].first + rEdgeChar + colors[0].second;
        else
            frame += colors[0].first + midEdge + colors[0].second;
    }
    frame += '\n';
}

void makeTextWithColors(std::string &introPhrase, std::vector<std::pair<std::string, std::string>> &colors, int &xTabSize, int &yTabSize, std::string &midVertEdge, std::string &frame)
{
    int index = 0;
    std::string textFrame;
    std::string topBottomPadding;
    std::string leftPadding;
    std::string rightPadding;

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

    frame += '\n' + leftPadding;
    frame += textFrame;
    frame += rightPadding + '\n';
}

void introduction(float &accumilator, float &dt, float &speed, std::string &frame, std::chrono::high_resolution_clock::time_point &lastFrame, std::vector<std::pair<std::string, std::string>> &colors)
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

    std::string introPhrase = "Welcome to our restaurant";
    int introPhraseSize = introPhrase.size();
    int xTabSize = 5;
    int bxLen = xTabSize * 2 + introPhraseSize;

    std::string lTopEdge = u8"╔";
    std::string rTopEdge = u8"╗";
    std::string lBottomEdge = u8"╚";
    std::string rBottomEdge = u8"╝";
    std::string midVertEdge = u8"║";
    std::string midHorEdge = u8"═";

    makeTopBottomEdgeBorder(bxLen, rTopEdge, lTopEdge, midHorEdge, colors, frame);
    makeTextWithColors(introPhrase, colors, xTabSize, xTabSize, midVertEdge, frame);
    makeTopBottomEdgeBorder(bxLen, rBottomEdge, lBottomEdge, midHorEdge, colors, frame);
    frame += '\n';
}

void detectKeyboard(int &index, int maxIndex, bool &pressEntered, bool &pressEsc)
{
    if (_kbhit())
    {
        int k = _getch();
        if (k == 0 || k == 224)
        {
            k = _getch();
            switch (k)
            {
            case 72: // up
                index--;
                if (index < 0)
                    index = maxIndex;
                break;
            case 80: // down
                index++;
                if (index > maxIndex)
                    index = 0;
                break;
            }
        }
        else if (k == 13)
            pressEntered = true;
        else if (k == 27)
            pressEsc = true;
    }
}

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

void chooseFood(std::vector<std::pair<double, std::string>> &foods, std::vector<std::pair<int, double>> &chosenFoods, float &accumilator, float &dt, float &speed, std::chrono::high_resolution_clock::time_point &lastFrame, std::vector<std::pair<std::string, std::string>> &colors)
{
    bool choosingFood = true;
    int index = 0;
    bool initialClear = false;
    bool hasFinalyChosenFood = false;
    bool pressEsc = false;

    removeCursor();

    while (choosingFood)
    {
        if (initialClear)
            properClear();
        else
        {
            hardClear();
            initialClear = true;
        }

        std::string frame;
        introduction(accumilator, dt, speed, frame, lastFrame, colors);

        frame += "Choose your food (ENTER to add, ESC to return)\n\n";
        for (int i = 0; i < foods.size(); i++)
        {
            if (i == index)
                frame += u8"► \033[1;92m$" + std::to_string(foods[i].first) + " " + foods[i].second + "\033[0m\n";
            else
                frame += "  $" + std::to_string(foods[i].first) + " " + foods[i].second + "\n";
        }

        std::cout << frame;
        detectKeyboard(index, foods.size() - 1, hasFinalyChosenFood, pressEsc);

        if (hasFinalyChosenFood)
        {
            chosenFoods.push_back({index, foods[index].first});
            hasFinalyChosenFood = false;
        }
        if (pressEsc)
            choosingFood = false;
    }
}

void menuSelection(std::vector<std::pair<std::string, std::vector<std::pair<double, std::string>>>> &restaurantMenu, std::vector<std::pair<std::string, int>> &selectedFoods, float &accumilator, float &dt, float &speed, std::chrono::high_resolution_clock::time_point &lastFrame, std::vector<std::pair<std::string, std::string>> &colors)
{
    bool initialClear = false;
    bool choosing = true;
    bool hasFinallyChosenMenu = false;
    bool pressEsc = false;
    int menuIndex = 0;

    std::vector<std::pair<int, double>> chosenFoods;

    removeCursor();

    while (choosing)
    {
        if (initialClear)
            properClear();
        else
        {
            hardClear();
            initialClear = true;
        }

        std::string frame;
        introduction(accumilator, dt, speed, frame, lastFrame, colors);
        frame += "Select Menu (ENTER to open)\n\n";

        for (int i = 0; i < restaurantMenu.size(); i++)
        {
            if (i == menuIndex)
                frame += u8"► \033[1;93m" + restaurantMenu[i].first + "\033[0m\n";
            else
                frame += "  " + restaurantMenu[i].first + "\n";
        }

        std::cout << frame;

        detectKeyboard(menuIndex, restaurantMenu.size() - 1, hasFinallyChosenMenu, pressEsc);

        if (hasFinallyChosenMenu)
        {
            chooseFood(restaurantMenu[menuIndex].second, chosenFoods, accumilator, dt, speed, lastFrame, colors);
            hasFinallyChosenMenu = false;
        }

        if (pressEsc)
            choosing = false;
    }

    hardClear();
    double total = 0;
    std::cout << "\n\033[1;94mYour Chosen Foods:\033[0m\n";
    for (auto &cf : chosenFoods)
    {
        total += cf.second;
        std::cout << "- $" << cf.second << "\n";
    }
    std::cout << "\n\033[1;92mTotal: $" << total << "\033[0m\n\n";
    std::cout << "Press any key to exit...";
    _getch();
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
        {"\033[1;91m", "\033[0m"},
        {"\033[1;92m", "\033[0m"},
        {"\033[1;93m", "\033[0m"},
        {"\033[1;94m", "\033[0m"},
        {"\033[1;95m", "\033[0m"},
        {"\033[1;96m", "\033[0m"},
        {"\033[1;97m", "\033[0m"}};
    
    std::vector<std::pair<std::string, int>> chosenFoods; // {foodId, foodQuantity}

    std::vector<std::pair<std::string, std::vector<std::pair<double, std::string>>>> restaurantMenu = {
        {"🍔 Food", {{99.99, "Classic Burger"}, {129.50, "Cheese Burger"}, {149.75, "Bacon Burger"}}},
        {"🍧 Desert", {{199.99, "Pepperoni Pizza"}, {249.99, "Four Cheese"}, {229.99, "Hawaiian"}}},
        {"🥤 Drinks", {{49.99, "Iced Tea"}, {59.99, "Coke"}, {39.99, "Water"}}}};

    menuSelection(restaurantMenu, chosenFoods, accumilator, dt, speed, lastFrame, colors);

    return 0;
}
