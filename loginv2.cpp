// #include <iostream>
// #include <vector>
// #include <conio.h>
// #include <chrono>
// #include <windows.h>

// std::string getPassword()
// {
//     std::string pwd;
//     char c;
//     while (true)
//     {
//         c = _getch();

//         if (c == 13)
//         { // ENTER
//             std::cout << "\n";
//             break;
//         }

//         if (c == 8) // BACKSPACE
//         {
//             if (!pwd.empty())
//             {
//                 pwd.pop_back();
//                 std::cout << "\b \b";
//             }
//             continue;
//         }

//         // if (c == 27)
//         // { // ESC
//         //     // loggingIn = false;
//         //     return "";
//         // }

//         pwd.push_back(c);
//         std::cout << '*';
//     }

//     return pwd;
// }

// int main () {

//     std::string username;
//     std::string password;

//     std::string actualUsername = "Admin";
//     std::string actualPassword = "@dmin123";
//     bool animateLoginNotif = false;

//     while (true) {
//         if (!username.empty() && true) {
//             std::cout << "Username: " << username;
//         } else {
//             std::cout << "Username: ";
//             std::cin >> username;
//         }

//         std::cout << "Password: ";
//         password = getPassword();

//         if (username == actualUsername && password == actualPassword) {
//             animateLoginNotif = true;
//             break;
//         }
//     }

//     if (animateLoginNotif) std::cout << "\033[92m" << "You are now logged In!" << "\033[0m" << std::endl;

//     return 0;
// }

#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>

// ---- universal getch() for Linux ----
int getch_linux()
{
    termios oldt, newt;
    int ch;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO); // raw mode, no echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

// ---- password masking ----
std::string getPassword()
{
    std::string pwd;
    char c;

    while (true)
    {
        c = getch_linux();

        // ENTER (covers both "\n" and "\r")
        if (c == '\n' || c == '\r')
        {
            std::cout << "\n";
            break;
        }

        // BACKSPACE (covers both 8 and 127)
        if (c == 127 || c == 8)
        {
            if (!pwd.empty())
            {
                pwd.pop_back();
                std::cout << "\b \b" << std::flush;
            }
            continue;
        }

        // ignore weird control keys
        if (c < 32 || c > 126)
            continue;

        pwd.push_back(c);
        std::cout << "*" << std::flush;
    }

    return pwd;
}

int main()
{
    std::string username;
    std::string password;

    std::string actualUsername = "Admin";
    std::string actualPassword = "@dmin123";

    bool animateLoginNotif = false;

    while (true)
    {
        std::cout << "Username: ";
        std::cin >> username;

        std::cout << "Password: ";
        password = getPassword();

        if (username == actualUsername && password == actualPassword)
        {
            animateLoginNotif = true;
            break;
        }
    }

    if (animateLoginNotif)
        std::cout << "\033[92mYou are now logged In!\033[0m\n";

    return 0;
}

