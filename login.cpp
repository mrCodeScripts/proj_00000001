#include <iostream>
#include <vector>
#include <conio.h>
#include <string>
#include <windows.h>
#include <utility>
#include <algorithm>

bool isUsernameExist(std::vector<std::pair<std::string, std::string>> &users, std::string &username)
{
    auto it = std::find_if(users.begin(), users.end(), [&](auto &item)
                           { return item.first == username; });
    return it != users.end();
}

std::string getPassword(bool &loggingIn)
{
    std::string pwd;
    char c;
    while (true)
    {
        c = _getch();

        if (c == 13)
        { // ENTER
            std::cout << "\n";
            break;
        }

        if (c == 8) // BACKSPACE
        {
            if (!pwd.empty())
            {
                pwd.pop_back();
                std::cout << "\b \b";
            }
            continue;
        }

        if (c == 27)
        { // ESC
            loggingIn = false;
            return "";
        }

        pwd.push_back(c);
        std::cout << '*';
    }

    return pwd;
}

bool validateInfo(std::vector<std::pair<std::string, std::string>> &users, std::string &password, std::string &username)
{
    auto it = std::find_if(users.begin(), users.end(), [&](auto &item)
                           { return item.first == username; });
    if (it == users.end())
        return false;
    return (*it).second == password;
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    bool loggingIn = true;
    bool usernameDoesNotExist = false;
    bool passwordIncorrect = false;
    bool cleanNotif = false;

    std::string username;
    std::string password;

    std::vector<std::pair<std::string, std::string>> users = {
        {"Admin", "@dmin123"},
    };

    while (loggingIn)
    {
        system("cls");

        // ==== ERROR MESSAGES ====
        if (usernameDoesNotExist)
        {
            std::cout << "\033[31m" << u8R"(
╔═══════════════════════════════════════╗
║       USERNAME DOES NOT EXIST!        ║
╚═══════════════════════════════════════╝
)" << "\033[0m\n\n";
        }

        if (passwordIncorrect)
        {
            std::cout << "\033[31m" << u8R"(
╔═══════════════════════════════════════╗
║           INCORRECT PASSWORD!         ║
╚═══════════════════════════════════════╝
)" << "\033[0m\n\n";
        }

        // ==== USERNAME ====
        if (!username.empty() && isUsernameExist(users, username))
        {
            // User typed a valid username → keep it, remove error
            usernameDoesNotExist = false;
            std::cout << "\033[1;92mUsername: \033[0m" << username << "\n";
        }
        else
        {
            // Ask for username input
            std::cout << "\033[1;92mUsername: \033[0m";
            std::cin >> username;

            if (!isUsernameExist(users, username))
            {
                usernameDoesNotExist = true;
                continue; // repeat loop without clearing username (so next input is fresh)
            }
            if (!usernameDoesNotExist && !cleanNotif)
                continue;
        }

        std::cout << "\033[1;92mPassword: \033[0m";
        password = getPassword(loggingIn);
        if (!loggingIn)
            break;

        if (!validateInfo(users, password, username))
        {
            passwordIncorrect = true;
            continue;
        }

        system("cls");
        std::cout << "\033[1;92mYou are logged in!\033[0m\n";
        break;
    }

    return 0;
}
