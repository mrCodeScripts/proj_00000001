#include <iostream>
#include <windows.h>
#include <sstream>
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

std::size_t displayWidth(const std::string &s)
{
    std::size_t width = 0;
    for (std::size_t i = 0; i < s.size();)
    {
        unsigned char c = s[i];
        if ((c & 0x80) == 0)
        {
            // ASCII
            i += 1;
            width += 1;
        }
        else if ((c & 0xE0) == 0xC0)
        {
            i += 2;
            width += 1;
        } // 2-byte UTF-8
        else if ((c & 0xF0) == 0xE0)
        {
            i += 3;
            width += 1;
        } // 3-byte UTF-8
        else if ((c & 0xF8) == 0xF0)
        {
            i += 4;
            width += 2;
        } // 4-byte (emoji) ~double width
        else
            i += 1;
    }
    return width;
}

void printReceipt(const std::vector<std::pair<std::string, std::pair<int, double>>> &chosenFoods, double payment)
{
    std::ostringstream frame;
    double total = 0.0;

    struct ItemLine
    {
        std::string text;
    };
    std::vector<ItemLine> lines;

    for (const auto &cf : chosenFoods)
    {
        double lineTotal = cf.second.first * cf.second.second;
        total += lineTotal;

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2)
            << cf.first << " x" << cf.second.first
            << " = $" << lineTotal;
        lines.push_back({oss.str()});
    }

    // Totals
    lines.push_back({"Total: $" + std::to_string(total)});
    lines.push_back({"Payment: $" + std::to_string(payment)});
    lines.push_back({"Change: $" + std::to_string(payment - total)});

    // Find max display width
    std::size_t maxLen = 0;
    for (const auto &l : lines)
        maxLen = std::max(maxLen, displayWidth(l.text));

    std::size_t boxWidth = maxLen + 4;

    const std::string topLeft = "╔", topRight = "╗";
    const std::string bottomLeft = "╚", bottomRight = "╝";
    const std::string vertical = "║";
    const std::string horizontal = "═";
    const std::string middleLeft = "╠", middleRight = "╣";

    auto repeat = [](const std::string &s, std::size_t count)
    {
        std::string r;
        r.reserve(s.size() * count);
        for (std::size_t i = 0; i < count; ++i)
            r += s;
        return r;
    };

    frame << topLeft << repeat(horizontal, boxWidth - 2) << topRight << "\n";

    // Header centered
    std::string header = "RECEIPT";
    std::size_t padLeft = (boxWidth - 2 - header.size()) / 2;
    std::size_t padRight = boxWidth - 2 - header.size() - padLeft;
    frame << vertical << std::string(padLeft, ' ') << header
          << std::string(padRight, ' ') << vertical << "\n";

    frame << middleLeft << repeat(horizontal, boxWidth - 2) << middleRight << "\n";

    // Lines aligned using displayWidth
    for (const auto &l : lines)
    {
        std::size_t padding = boxWidth - 2 - displayWidth(l.text);
        frame << vertical << l.text << std::string(padding, ' ') << vertical << "\n";
    }

    frame << bottomLeft << repeat(horizontal, boxWidth - 2) << bottomRight << "\n";

    std::cout << frame.str();
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

// void properClear() {
//     HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
//     CONSOLE_SCREEN_BUFFER_INFO csbi;
//     DWORD count;
//     DWORD cellCount;
//     COORD homeCoords = {0, 0};

//     if (hStdOut == INVALID_HANDLE_VALUE) return;

//     if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
//     cellCount = csbi.dwSize.X * csbi.dwSize.Y;

//     FillConsoleOutputCharacter(hStdOut, (TCHAR)' ', cellCount, homeCoords, &count);
//     FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, homeCoords, &count);
//     SetConsoleCursorPosition(hStdOut, homeCoords);
// }

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
        else if (k == 112 || k == 80)
        {
            pressedPayment = true;
        }
        else if (k == 99 || k == 67)
        {
            pressedCancel = true;
        }
    }
}

void paymentProcess(std::vector<std::pair<std::string, std::pair<int, double>>> &chosenFoods, bool &choosing)
{
    bool donePaying = false;
    bool initialCls = false;
    bool paymentNotEnough = false;
    double payment;
    double change;
    while (true)
    {
        if (initialCls)
        {
            properClear();
        }
        else
        {
            hardClear();
            initialCls = true;
        }
        if (paymentNotEnough)
        {
            hardClear();
            std::cout << "\033[31m" << u8R"(
╔═══════════════════════════════════════╗
║           Payment Not Enough          ║
╚═══════════════════════════════════════╝
                    )" << "\033[0m"
                      << "\n"
                      << std::endl;
            std::cout << std::endl;
        }
        double total = 0;
        int chosenFoodsIndex = 1;
        for (auto &cf : chosenFoods)
        {
            std::cout << std::to_string(chosenFoodsIndex) << ". " << cf.first << " x" << cf.second.first << " = $" << cf.second.first * cf.second.second << '\n';
            total += cf.second.first * cf.second.second;
            chosenFoodsIndex++;
        }
        std::cout << "\n\n\nTotal: $" << total << "                       " << "\n";
        std::cout << "Payment: $";
        std::cin >> payment;
        if (total > payment)
        {
            paymentNotEnough = true;
        }
        else
        {
            change = payment - total;
            donePaying = true;
        }
        std::cout << "Change: $" << std::fixed << std::setprecision(2) << change << "\n\n\n"
                  << std::endl;

        printReceipt(chosenFoods, payment);

        if (donePaying)
            break;
    }
    chosenFoods.clear();
    std::cout << "\033[92m" << u8R"(
╔══════════════════════════════════════════╗
║           THANK YOU FOR BUYING!          ║
╚══════════════════════════════════════════╝
                    )"
              << "\033[0m"
              << "\n"
              << std::endl;
    std::cout << std::endl;
    choosing = false;
}

void chooseFood(
    int menuIndex,
    std::vector<std::pair<double, std::string>> &foods,
    std::vector<std::pair<std::string, std::pair<int, double>>> &chosenFoods,
    float &accumilator, float &dt, float &speed,
    std::chrono::high_resolution_clock::time_point &lastFrame,
    std::string introPhrase,
    bool choosing1,
    std::vector<std::pair<std::string, std::string>> &colors)
{
    removeCursor();
    bool choosing2 = true;
    int index = 0;
    bool pressEnter = false;
    bool pressedEsc = false;
    bool pressedPayment = false;
    bool pressedCancelPayment = false;
    bool initialClear = false;

    while (choosing2)
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
            std::ostringstream ss;
            bool selected = false;
            for (auto &cf : chosenFoods)
            {
                if (cf.first == foods[i].second)
                    selected = true;
            }

            ss << std::fixed << std::setprecision(2) << foods[i].first;
            std::string formatedString = ss.str();
            if (i == index)
            {
                frame += u8" ► \033[1;92m$" + formatedString + " " + foods[i].second + "                       " + "\033[0m";
            }
            else
            {
                frame += u8"   $" + formatedString + " " + foods[i].second + "                       ";
            }
            if (selected)
                frame += " [selected]";
            frame += '\n';
        }

        double total = 0;
        frame += "\nCurrent selections:\n";
        int indFood = 1;
        for (auto &cf : chosenFoods)
        {
            frame += " [" + std::to_string(indFood) + "] " + cf.first + " x" + std::to_string(cf.second.first) + " = $" + std::to_string(cf.second.second * cf.second.first) + '\n';
            total += cf.second.first * cf.second.second;
            indFood++;
        }
        std::ostringstream sis;
        sis << std::fixed << std::setprecision(2) << total;
        std::string tot = sis.str();
        frame += "Total: $" + tot + "                                " + "\n";
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
                (*it).second.first++;
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
        std::string introPhrase = "Welcome to our restaurant (ENTER to open, P to payment, ESC to exit)";
        introduction(accumilator, dt, speed, frame, lastFrame, introPhrase, colors);
        for (int i = 0; i < restaurantMenu.size(); i++)
        {
            if (i == menuIndex)
            {
                frame += u8" ► " + std::string("\033[1;92m") + restaurantMenu[i].first + "\033[0m" + "                                                                         " + '\n';
            }
            else
            {
                frame += "   " + restaurantMenu[i].first + "                                                                                   " + '\n';
            }
        }

        double total = 0;
        if (!chosenFoods.empty())
        {
            frame += "\n\n\n\nCurrent selections across menus:                                 \n";
            int ind = 1;
            for (auto &cf : chosenFoods)
            {
                std::ostringstream oss;
                double iprice = cf.second.first * cf.second.second;
                oss << std::fixed << std::setprecision(2) << iprice;
                std::string formattedShit = oss.str();
                frame += " [" + std::to_string(ind) + "] " + cf.first + " x" + std::to_string(cf.second.first) + " = $" + formattedShit + '\n';
                total += cf.second.first * cf.second.second;
                ind++;
            }
            std::ostringstream sis;
            sis << std::fixed << std::setprecision(2) << total;
            std::string tot = sis.str();
            frame += "\n\nTotal: $" + tot + "                         " + "\n";
            frame += "Press P to pay                              \n";
        }

        std::cout << frame;

        detectKeyboard(menuIndex, restaurantMenu.size() - 1, pressEnter, pressedEsc, pressedPayment, pressedCancelPayment);

        if (pressEnter)
        {
            pressEnter = false;
            std::string foodIntroPhrase = "Select your food (ENTER to add, ESC to go back, C to cancel current menu)";
            chooseFood(menuIndex, restaurantMenu[menuIndex].second, chosenFoods, accumilator, dt, speed, lastFrame, foodIntroPhrase, choosing, colors);
            initialClear = false;
        }

        if (pressedEsc)
        {
            pressedEsc = false;
            break;
        }

        if (pressedPayment)
        {
            paymentProcess(chosenFoods, choosing);
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
        {"🍔 Food Menu",
         {{2.99, "🍟 Fries"},
          {4.99, "🍔 Burger"},
          {5.49, "🧀 Cheese Burger"},
          {5.99, "🥓 Bacon Burger"},
          {3.99, "🌭 Hotdog"},
          {6.49, "🍕 Pizza Slice"}}},
        {"🍰 Dessert Menu",
         {{1.49, "🍪 Cookies"},
          {2.29, "🧁 Cupcake"},
          {1.99, "🍩 Donut"},
          {2.99, "🍦 Ice Cream"},
          {3.49, "🍰 Cheesecake"},
          {3.99, "🥧 Apple Pie"}}},
        {"🥤 Drinks Menu",
         {{0.99, "🥤 Soda"},
          {1.49, "🧃 Juice"},
          {1.99, "☕ Coffee"},
          {2.49, "🧋 Milk Tea"},
          {3.99, "🍺 Beer"},
          {4.49, "🍷 Wine"}}}};

    menuSelection(restaurantMenu, accumilator, dt, speed, lastFrame, colors);
    return 0;
}
