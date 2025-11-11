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

    std::string frame2;
    std::string introPhrase = "Welcome to our restaurant";

    int index = 0;
    for (const char &c : introPhrase)
    {
        frame2 += colors[index].first + std::string(1, c) + colors[index].second;
        index++;
        if (index >= colors.size())
            index = 0;
    }
    frame += frame2;
    frame += '\n';
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
            callback();
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
        introduction(accumilator, dt, speed, frame, lastFrame, colors);
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
    auto lastFrame = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<std::string, std::string>> colors = {
        {"\033[91m", "\033[0m"}, // Bright Red
        {"\033[92m", "\033[0m"}, // Bright Green
        {"\033[93m", "\033[0m"}, // Bright Yellow
        {"\033[94m", "\033[0m"}, // Bright Blue
        {"\033[95m", "\033[0m"}, // Bright Magenta
        {"\033[96m", "\033[0m"}, // Bright Cyan
        {"\033[97m", "\033[0m"}  // Bright White
    };
    std::vector<std::string> introPhrase = {
        u8R"(
██     ██ 
██     ██ 
██  █  ██ 
██ ███ ██ 
 ███ ███  
        )",
        u8R"(
███████ 
██      
█████   
██      
███████ 
        )",
        u8R"(
██      
██      
██      
██      
███████ 
        )",
        u8R"(
 ██████ 
██      
██      
██      
 ██████ 
        )",
        u8R"(
 ██████  
██    ██ 
██    ██ 
██    ██ 
 ██████  
        )",
        u8R"(
███    ███ 
████  ████ 
██ ████ ██ 
██  ██  ██ 
██      ██ 
        )",
        u8R"(
███████ 
██      
█████   
██      
███████ 
        )",
        u8R"(
    
    
    
    
    
        )",
        u8R"(
████████ 
   ██    
   ██    
   ██    
   ██    
        )",
        u8R"(
 ██████  
██    ██ 
██    ██ 
██    ██ 
 ██████  
        )",
        u8R"(
    
    
    
    
    
        )",
        u8R"(
 ██████  
██    ██ 
██    ██ 
██    ██ 
 ██████  
        )",
        u8R"(
██    ██ 
██    ██ 
██    ██ 
██    ██ 
 ██████  
        )",
        u8R"(
██████  
██   ██ 
██████  
██   ██ 
██   ██ 
        )",
        u8R"(
    
    
    
    
    
        )",
        u8R"(
██████  
██   ██ 
██████  
██   ██ 
██   ██ 
        )",
        u8R"(
███████ 
██      
█████   
██      
███████ 
        )",
        u8R"()",
        u8R"()",
        u8R"()",
        u8R"()",
        u8R"()",
    };

    std::vector<std::pair<double, std::string>> foods = {
        {99.99, "Burger King"},
        {99.99, "Burger King"},
        {99.99, "Burger King"},
        {99.99, "Burger King"},
        {99.99, "Burger King"},
        {99.99, "Burger King"},
    };

    std::vector<std::pair<int, double>> chosenFoods; // pair(id, price)

    chooseFood(foods, chosenFoods, accumilator, dt, speed, lastFrame, colors);

    return 0;
}