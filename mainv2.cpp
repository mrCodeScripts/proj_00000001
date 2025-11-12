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
#include <vector>

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
    if (yTabSize > 0)
    {
        int xfullSpace = (xTabSize * 2) + introPhrase.size();
        for (int j = 1; j <= yTabSize; j++)
        {
            // topBottomPadding += '\n';
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
    frame += topBottomPadding;
    frame += '\n' + leftPadding;
    frame += textFrame;
    frame += rightPadding + '\n';
    frame += topBottomPadding;
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

void detectKeyboard(int &index, int maxIndex, bool &pressEntered)
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
    }
}

void setChosenFoods(std::vector<std::pair<int, double>> &chosenFoods)
{
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
    std::vector<std::pair<int, int>> chosenFood; // foodIndex, foodQuantity

    removeCursor();
    while (choosingFood)
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

        if (hasFinalyChosenFood) {

        }

        std::string frame;
        introduction(accumilator, dt, speed, frame, lastFrame, colors);
        for (int i = 0; i < foods.size(); i++)
        {
            if (i == index)
            {
                frame += u8"►" + std::string("\033[1;92m") + " $" + std::to_string(foods[i].first) + " " + foods[i].second + "\033[0m" + '\n';
            }
            else
            {
                frame += "  $" + std::to_string(foods[i].first) + " " + foods[i].second + '\n';
            }
        }
        std::cout << frame;

        detectKeyboard(index, foods.size() - 1, hasFinalyChosenFood);
    }
}



void menuSelection (std::vector<std::pair<std::string, std::vector<std::pair<double, std::string>>>> &restaurantMenu, float &accumilator, float &dt, float &speed, std::chrono::high_resolution_clock::time_point &lastFrame, std::vector<std::pair<std::string, std::string>> &colors) {
    bool initialClear = false;
    bool choosing = true;
    bool choosingMenuIndex = true;
    bool choosingFoodIndex = false;
    bool hasFinallyChossenMenu = false;
    int menuIndex = 0;
    int foodIndex = 0; 

    std::vector<std::pair<int, double>> chosenFoods; // pair(id, price)

    removeCursor();

    while (choosing) {
        if (initialClear) {
            properClear();
        } else {
            hardClear();
            initialClear = true;
        }

        std::string frame;
        if (choosingMenuIndex && !(choosingFoodIndex) && !(hasFinallyChossenMenu)) {
            for (int i = 0; i < restaurantMenu.size(); i++) {
                if (i == menuIndex) {
                    frame += "> " + restaurantMenu[i].first + '\n';
                } else {
                    frame += " " + restaurantMenu[i].first + '\n';
                }
            }
        } else {
            std::vector<std::pair<double, std::string>> availableFoods;
            chooseFood(availableFoods, chosenFoods, accumilator, dt, speed, lastFrame, colors);
        }
        std::cout << frame;

        detectKeyboard(menuIndex, restaurantMenu.size() - 1, hasFinallyChossenMenu);
    }
}



int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    float accumilator; // accumilator = speed * dt
    float dt;
    float speed = 0.9f;
    auto lastFrame = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<std::string, std::string>> colors = {
        {"\033[1;91m", "\033[0m"}, // Bright Red
        {"\033[1;92m", "\033[0m"}, // Bright Green
        {"\033[1;93m", "\033[0m"}, // Bright Yellow
        {"\033[1;94m", "\033[0m"}, // Bright Blue
        {"\033[1;95m", "\033[0m"}, // Bright Magenta
        {"\033[1;96m", "\033[0m"}, // Bright Cyan
        {"\033[1;97m", "\033[0m"}  // Bright White
    };

    std::vector<std::pair<double, std::string>> foods = {
        {99.99, "Burger King"},
        {99.99, "Burger King"},
        {99.99, "Burger King"},
        {99.99, "Burger King"},
        {99.99, "Burger King"},
        {99.99, "Burger King"},
    };

    std::vector<std::pair<std::string, std::vector<std::pair<double, std::string>>>> restaurantMenu = {
        {"Menu 1", {
            { 99.99, "Burger King" },
            { 99.99, "Burger King" },
            { 99.99, "Burger King" },
            { 99.99, "Burger King" }
        }},
        {"Menu 2", {
            { 99.99, "Burger King" },
            { 99.99, "Burger King" },
            { 99.99, "Burger King" },
            { 99.99, "Burger King" },
            { 99.99, "Burger King" }
        }},
        {"Menu 3", {
            { 99.99, "Burger King" },
            { 99.99, "Burger King" },
            { 99.99, "Burger King" },
            { 99.99, "Burger King" }
        }},
    };

    menuSelection(restaurantMenu, accumilator, dt, speed, lastFrame, colors);

    return 0;
}