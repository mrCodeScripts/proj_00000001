/*
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <thread>
#include <chrono>

// --- Detect keyboard (arrow keys + enter + esc) ---
void detectKeyboard(int &index, int maxIndex, int &chosen, bool &keyError)
{
    if (_kbhit())
    {
        int k = _getch();
        if (k == 0 || k == 224) // special keys
        {
            k = _getch();
            switch (k)
            {
            case 72: // up
                keyError = false;
                index--;
                if (index < 0)
                    index = maxIndex;
                break;
            case 80: // down
                keyError = false;
                index++;
                if (index > maxIndex)
                    index = 0;
                break;
            default:
                keyError = true;
                break;
            }
        }
        else if (k == 13) // enter
        {
            keyError = false;
            chosen = index;
            index = -2;
        }
        else if (k == 27) // ESC
        {
            chosen = -999;
            index = -2;
        }
        else
        {
            keyError = true;
        }
    }
}

// --- Cursor & screen ---
void hideCursor()
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO inf;
    GetConsoleCursorInfo(handle, &inf);
    inf.bVisible = false;
    SetConsoleCursorInfo(handle, &inf);
}

void showCursor()
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO inf;
    GetConsoleCursorInfo(handle, &inf);
    inf.bVisible = true;
    SetConsoleCursorInfo(handle, &inf);
}

void clearScreen()
{
    system("cls");
}

// --- Generic menu function ---
void choicesYM(int &chosen, std::vector<std::pair<std::string, std::string>> &choices,
               const std::string &title)
{
    bool choosing = true;
    int index = 0;
    bool keyError = false;
    hideCursor();

    while (choosing)
    {
        clearScreen();
        std::cout << "\n==========================\n";
        std::cout << "| " << title << " |\n";
        std::cout << "==========================\n\n";

        for (int i = 0; i < choices.size(); i++)
        {
            if (i == index)
                std::cout << "👉 " << choices[i].first << "\n";
            else
                std::cout << "   " << choices[i].first << "\n";
        }

        if (keyError)
            std::cout << "\nINVALID INPUT!\n";

        detectKeyboard(index, static_cast<int>(choices.size() - 1), chosen, keyError);
        if (index == -2)
            choosing = false;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    showCursor();
}

// --- Username & Password ---
void askUsername(std::string &username, bool &errorName,
                 std::vector<std::pair<std::string, std::string>> &allowedUsers)
{
    std::cout << "Username: ";
    std::getline(std::cin, username);
    errorName = true;
    for (const auto &u : allowedUsers)
    {
        if (u.first == username)
        {
            errorName = false;
            break;
        }
    }
}

std::string getPassword()
{
    std::string pwd;
    char c;
    while (true)
    {
        c = _getch();
        if (c == 13) // Enter
        {
            std::cout << "\n";
            break;
        }
        if (c == 8) // Backspace
        {
            if (!pwd.empty())
            {
                pwd.pop_back();
                std::cout << "\b \b";
            }
            continue;
        }
        pwd.push_back(c);
        std::cout << '*';
    }
    return pwd;
}

void askPassword(std::string username, std::string &password, bool &errPassword,
                 std::vector<std::pair<std::string, std::string>> &allowedUsers)
{
    password = getPassword();
    errPassword = true;
    for (const auto &u : allowedUsers)
    {
        if (u.first == username && u.second == password)
        {
            errPassword = false;
            break;
        }
    }
}

// --- Age input ---
void getAge()
{
    int age;
    std::cout << "\nENTER YOUR AGE: ";
    std::cin >> age;
    std::cin.ignore();

    if (age >= 15 && age <= 17)
        std::cout << "ELIGIBLE FOR SK CATEGORY (SANGGUNIANG KABATAAN)\n";
    else if (age >= 18 && age <= 30)
        std::cout << "ELIGIBLE FOR SK AND BARANGGAY CATEGORY\n";
    else if (age >= 31 && age <= 60)
        std::cout << "ELIGIBLE FOR BARANGGAY OFFICIAL CATEGORY\n";
    else if (age >= 61)
        std::cout << "ELIGIBLE FOR BARANGGAY OFFICIAL WITH ASSISTANT CATEGORY\n";
    else
        std::cout << "NOT ELIGIBLE TO PARTICIPATE.\n";
}

// --- Vote menu ---
void vote()
{
    int choice = 0;
    std::vector<std::pair<std::string, std::string>> voteType = {
        {"Iliterate", "I"},
        {"Person with disabilities", "P"},
        {"Senior Citizen", "C"},
        {"None", "N"},
    };

    choicesYM(choice, voteType, "CHOOSE TYPE OF VOTER");

    switch (choice)
    {
    case 0:
        std::cout << "\n==========================\n| YOU NEED AN ASSISTANT! |\n==========================\n";
        getAge();
        break;
    case 1:
    case 2:
    case 3:
        getAge();
        break;
    default:
        std::cout << "INVALID SELECTION!\n";
        break;
    }
}

// --- Main ---
int main()
{
    int MAX_ATTEMPTS = 3;
    int LOCK_DURATION = 10;

    std::vector<std::pair<std::string, std::string>> allowedUsers = {
        {"Admin", "12345"},
        {"Voter", "12345"}};

    std::string username, password;
    bool errorName = false, errorPassword = false;
    int attemptsLeft = MAX_ATTEMPTS;
    bool locked = false;

    while (true)
    {
        clearScreen();

        if (locked)
        {
            for (int i = LOCK_DURATION; i > 0; i--)
            {
                clearScreen();
                std::cout << "============================\n"
                             "| ACCOUNT LOCKED!          |\n"
                             "| Wait " << i << " seconds |\n"
                             "============================\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            locked = false;
            attemptsLeft = MAX_ATTEMPTS;
        }

        if (errorName)
            std::cout << "============================\n| Username does not exist! |\n============================\n\n";
        if (errorPassword)
            std::cout << "=======================\n| Incorrect Password! |\n=======================\n\n";

        askUsername(username, errorName, allowedUsers);
        if (errorName)
            continue;

        std::cout << "Password: ";
        askPassword(username, password, errorPassword, allowedUsers);

        if (errorPassword)
        {
            attemptsLeft--;
            if (attemptsLeft <= 0)
            {
                locked = true;
                continue;
            }
            continue;
        }

        // Successful login
        attemptsLeft = MAX_ATTEMPTS;
        clearScreen();
        std::cout << "=========================\n| Now you are logged in! |\n=========================\n";
        std::cout << "WELCOME " << username << "\n\n";

        if (username == "Admin")
        {
            int adminChoice = 0;
            std::vector<std::pair<std::string, std::string>> adminChoices = {{"Yes", "yes"}, {"No", "no"}};
            choicesYM(adminChoice, adminChoices, "ACCESS ADMIN SETTINGS?");
            if (adminChoice == 0)
            {
                std::cout << "ADMIN SETTINGS ACCESSED\n";
            }
            else
            {
                vote();
            }
        }
        else
        {
            vote();
        }

        std::cout << "\nPress ESC to logout and return to login screen...\n";
        while (true)
        {
            int k = _getch();
            if (k == 27) // ESC
                break;
        }
    }

    return 0;
}

 */



#include <iostream>
#include <vector>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>
#include <thread>

// --- Linux getch replacement ---
int getch_linux()
{
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

// --- Detect keyboard for menu navigation ---
void detectKeyboard(int &index, int maxIndex, int &chosen, bool &keyError)
{
    int k = getch_linux();
    if (k != EOF)
    {
        if (k == 27) // ESC sequence
        {
            int next1 = getch_linux();
            int next2 = getch_linux();
            if (next1 == 91)
            {
                switch (next2)
                {
                case 65: // UP
                    keyError = false;
                    index--;
                    if (index < 0)
                        index = maxIndex;
                    break;
                case 66: // DOWN
                    keyError = false;
                    index++;
                    if (index > maxIndex)
                        index = 0;
                    break;
                default:
                    keyError = true;
                }
            }
            else // ESC alone
            {
                chosen = -999;
                index = -2;
            }
        }
        else if (k == 10) // Enter
        {
            keyError = false;
            chosen = index;
            index = -2;
        }
        else
        {
            keyError = true;
        }
    }
}

// --- Clear screen & cursor ---
void properClear() { std::cout << "\033[2J\033[1;1H"; }
void hideCursor() { std::cout << "\033[?25l"; }
void showCursor() { std::cout << "\033[?25h"; }

// --- Generic menu function ---
void choicesYM(int &chosen, std::vector<std::pair<std::string, std::string>> &choices,
               const std::string &title)
{
    bool choosing = true;
    int index = 0;
    bool keyError = false;

    hideCursor();
    while (choosing)
    {
        properClear();
        std::cout << "\n==========================\n";
        std::cout << "| " << title << " |\n";
        std::cout << "==========================\n\n";

        for (int i = 0; i < choices.size(); i++)
        {
            if (i == index)
                std::cout << "👉 " << choices[i].first << "\n";
            else
                std::cout << "   " << choices[i].first << "\n";
        }

        if (keyError)
            std::cout << "\nINVALID INPUT!\n";

        detectKeyboard(index, static_cast<int>(choices.size() - 1), chosen, keyError);
        if (index == -2)
            choosing = false;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    showCursor();
}

// --- Username & password ---
void askUsername(std::string &username, bool &errorName,
                 std::vector<std::pair<std::string, std::string>> &allowedUsers)
{
    std::cout << "Username: ";
    std::getline(std::cin, username);

    errorName = true;
    for (const auto &u : allowedUsers)
    {
        if (u.first == username)
        {
            errorName = false;
            break;
        }
    }
}

std::string getPassword()
{
    std::string pwd;
    char c;
    while (true)
    {
        c = getch_linux();
        if (c == 10) // Enter
        {
            std::cout << "\n";
            break;
        }
        if (c == 127) // Backspace
        {
            if (!pwd.empty())
            {
                pwd.pop_back();
                std::cout << "\b \b";
            }
            continue;
        }
        pwd.push_back(c);
        std::cout << '*';
    }
    return pwd;
}

void askPassword(const std::string &username, std::string &password, bool &errPassword,
                 std::vector<std::pair<std::string, std::string>> &allowedUsers)
{
    password = getPassword();
    errPassword = true;
    for (auto &u : allowedUsers)
        if (u.first == username && u.second == password)
        {
            errPassword = false;
            break;
        }
}

// --- Age input ---
void getAge()
{
    int age;
    std::cout << "\nENTER YOUR AGE: ";
    std::cin >> age;
    std::cin.ignore();

    if (age >= 15 && age <= 17)
        std::cout << "ELIGIBLE FOR SK CATEGORY (SANGGUNIANG KABATAAN)\n";
    else if (age >= 18 && age <= 30)
        std::cout << "ELIGIBLE FOR SK AND BARANGGAY CATEGORY\n";
    else if (age >= 31 && age <= 60)
        std::cout << "ELIGIBLE FOR BARANGGAY OFFICIAL CATEGORY\n";
    else if (age >= 61)
        std::cout << "ELIGIBLE FOR BARANGGAY OFFICIAL WITH ASSISTANT CATEGORY\n";
    else
        std::cout << "NOT ELIGIBLE TO PARTICIPATE.\n";
}

// --- Vote menu ---
void vote()
{
    int choice = 0;
    std::vector<std::pair<std::string, std::string>> voteType = {
        {"Iliterate", "I"},
        {"Person with disabilities", "P"},
        {"Senior Citizen", "C"},
        {"None", "N"},
    };

    choicesYM(choice, voteType, "CHOOSE TYPE OF VOTER");

    switch (choice)
    {
    case 0:
        std::cout << "\n==========================\n| YOU NEED AN ASSISTANT! |\n==========================\n";
        getAge();
        break;
    case 1:
    case 2:
    case 3:
        getAge();
        break;
    default:
        std::cout << "INVALID SELECTION!\n";
        break;
    }
}

// --- Main ---
int main()
{
    int MAX_ATTEMPTS = 3;
    int LOCK_DURATION = 10;

    std::vector<std::pair<std::string, std::string>> allowedUsers = {
        {"Admin", "12345"},
        {"Voter", "12345"}};

    std::string username, password;
    bool errorName = false, errorPassword = false;
    int attemptsLeft = MAX_ATTEMPTS;
    bool locked = false;

    bool usernameAlreadySet = false;

    while (true)
    {
        properClear();

        // Display attempts remaining at top
        std::cout << "LOGIN ATTEMPTS REMAINING: " << attemptsLeft << "\n\n";

        // Locked timer
        if (locked)
        {
            for (int i = LOCK_DURATION; i > 0; i--)
            {
                properClear();
                std::cout << "ACCOUNT LOCKED! Wait " << i << " seconds...\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            locked = false;
            attemptsLeft = MAX_ATTEMPTS;
            usernameAlreadySet = false; // reset username after lock
            continue;
        }

        // Username input
        if (!usernameAlreadySet)
        {
            askUsername(username, errorName, allowedUsers);
            if (errorName)
                continue;
            usernameAlreadySet = true;
        }
        else
        {
            std::cout << "Username: " << username << "\n\n";
        }

        std::cout << "Password: ";
        askPassword(username, password, errorPassword, allowedUsers);

        if (errorPassword)
        {
            attemptsLeft--;
            if (attemptsLeft <= 0)
            {
                locked = true;
            }
            continue;
        }

        // Successful login
        attemptsLeft = MAX_ATTEMPTS;
        properClear();
        std::cout << "=========================\n| Now you are logged in! |\n=========================\n";
        std::cout << "WELCOME " << username << "\n\n";

        if (username == "Admin")
        {
            int adminChoice = 0;
            std::vector<std::pair<std::string, std::string>> adminChoices = {{"Yes", "yes"}, {"No", "no"}};
            choicesYM(adminChoice, adminChoices, "ACCESS ADMIN SETTINGS?");
            if (adminChoice == 0)
            {
                std::cout << "ADMIN SETTINGS ACCESSED\n";
            }
            else
            {
                vote(); // if admin says No, go to voting
            }
        }
        else
        {
            vote(); // normal user goes directly to voting
        }

        std::cout << "\nPress ESC to logout and return to login screen...\n";
        while (true)
        {
            int k = getch_linux();
            if (k == 27) // ESC
            {
                usernameAlreadySet = false; // allow choosing another account
                break;
            }
        }
    }

    return 0;
}


/*



*/
