#include <iostream>
#include <string>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <algorithm>
#include <utility>

void removeCursor()
{
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInf;
    GetConsoleCursorInfo(hout, &cursorInf);
    cursorInf.bVisible = FALSE;
    SetConsoleCursorInfo(hout, &cursorInf);
}

void example1()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    bool animate = true;
    bool username_does_not_exist = false;
    std::string username;

    std::vector<std::pair<std::string, std::string>> users = {
        {"Admin", "123"}};

    while (animate)
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        std::cout << "Username: ";
        std::cin >> username;
        username_does_not_exist = false;

        auto it = std::find_if(users.begin(), users.end(), [&](auto &item)
                               { return item.first == username; });

        if (it == users.end())
        {
            username_does_not_exist = true;
        }
        else
        {
            std::cout << "\033[31m" << u8R"(
╔════════════════════════════════════════╗
║           LOGIN SUCCESSFUL!            ║
╚════════════════════════════════════════╝
            )" << "\033[0m"
                      << std::endl;
            break;
        }

        if (username_does_not_exist)
        {
            std::cout << "\033[31m" << u8R"(
╔═══════════════════════════════════════════╗
║           USERNAME DOES NOT EXIST         ║
╚═══════════════════════════════════════════╝
            )" << "\033[0m"
                      << std::endl;
        }
    }
}

void passwordExample()
{
    char c;
    std::string password;
    std::string actualPassword = "@dmin123";
    bool donePasswordInput = false;
    while (true)
    {
        c = _getch();

        if (!donePasswordInput)
        {
            if (c == 13)
            {
                donePasswordInput = true;
            }

            if (c == 8)
            {
                if (!password.empty())
                {
                    password.pop_back();
                    std::cout << "\b \b";
                }

                continue;
            }

            password.push_back(c);
            std::cout << '*';
        }

        if (donePasswordInput)
        {
            if (password == actualPassword)
            {
                std::cout << "\033[31m" << u8R"(
╔════════════════════════════════════════╗
║           LOGIN SUCCESSFUL!            ║
╚════════════════════════════════════════╝
            )" << "\033[0m"
                          << std::endl;
            }
            break;
        }
    }
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    passwordExample();

    return 0;
}