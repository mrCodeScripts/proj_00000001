#include <iostream>
#include <vector>
#include <conio.h>
#include <chrono>
#include <windows.h>

std::string getPassword()
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

        // if (c == 27)
        // { // ESC
        //     // loggingIn = false;
        //     return "";
        // }

        pwd.push_back(c);
        std::cout << '*';
    }

    return pwd;
}

int main () {

    std::string username;
    std::string password;

    std::string actualUsername = "Admin";
    std::string actualPassword = "@dmin123";
    bool animateLoginNotif = false;

    while (true) {
        if (!username.empty() && true) {
            std::cout << "Username: " << username;
        } else {
            std::cout << "Username: ";
            std::cin >> username;
        }

        std::cout << "Password: ";
        password = getPassword();

        if (username == actualUsername && password == actualPassword) {
            animateLoginNotif = true;
            break;
        }
    }

    if (animateLoginNotif) std::cout << "\033[92m" << "You are now logged In!" << "\033[0m" << std::endl;

    return 0;
}