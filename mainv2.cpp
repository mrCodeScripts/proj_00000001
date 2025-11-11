#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <vector>
#include <utility>
#include <conio.h>
#include <string>

void introduction(float &accumilator, float &dt, float &speed, std::string &frame)
{

    std::cout << "Welcome to our restaurant" << std::endl;
}

template <typename F>
void detectKeyboard(int &index, int maxIndex, F callback)
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
                if (index < 0) index = maxIndex;
                break;
            case 80:
                index++;
                if (index > maxIndex) index = 0;
                break;
            }
        }
        else if (k == 13)
        {
            callback();
        }
    }
}

void setChosenFoods(std::vector<std::pair<int, double>> &chosenFoods)
{
}

void properClear() {
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordinate = {0, 0};
    SetConsoleCursorPosition(hout, coordinate);
}

void removeCursor () {
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

void chooseFood(std::vector<std::pair<double, std::string>> &foods, std::vector<std::pair<int, double>> &chosenFoods, float &accumilator, float &dt, float &speed)
{
    bool choosingFood = true;
    int index = 0;
    bool initialClear = false;

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


        std::string frame;
        introduction(accumilator, dt, speed, frame);
        for (int i = 0; i < foods.size(); i++)
        {
            if (i == index)
            {
                frame += u8"► $" + std::to_string(foods[i].first) + " " + foods[i].second + '\n';
            }
            else
            {
                frame += "  $" + std::to_string(foods[i].first) + " " + foods[i].second + '\n';
            }
        }
        std::cout << frame;

        detectKeyboard(index, foods.size() - 1, []() {});
    }
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    float accumilator; // accumilator = speed * dt
    float dt;
    float speed = 0.9f;

    std::vector<std::pair<double, std::string>> foods = {
        {99.99, "Burger King"},
        {99.99, "Burger King"},
        {99.99, "Burger King"},
        {99.99, "Burger King"},
        {99.99, "Burger King"},
        {99.99, "Burger King"},
    };

    std::vector<std::pair<int, double>> chosenFoods; // pair(id, price)

    chooseFood(foods, chosenFoods, accumilator, dt, speed);

    return 0;
}