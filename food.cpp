#include <iostream>
#include <iterator>
#include <windows.h>
#include <sstream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <random>
#include <vector>
#include <conio.h>
#include <string>
#include <iomanip> // for std::setw

// ---------- Data structures (replacing pairs) ----------
struct Food
{
    double price;
    std::string name;
    int stock;
};

struct MenuCategory
{
    std::string name;
    std::vector<Food> foods;
};

struct ChosenFood
{
    std::string name;
    int quantity;
    double price;
};

int countUTF8Chars(const std::string &str)
{
    int count = 0;
    for (size_t i = 0; i < str.size();)
    {
        if (str[i] == '\033') // ANSI escape start
        {
            i++; // skip '\033'
            if (i < str.size() && str[i] == '[')
            {
                i++; // skip '['
                // skip until 'm'
                while (i < str.size() && str[i] != 'm')
                    i++;
                if (i < str.size())
                    i++; // skip 'm'
            }
        }
        else
        {
            unsigned char c = str[i];
            if ((c & 0x80) == 0)
                i += 1;
            else if ((c & 0xE0) == 0xC0)
                i += 2;
            else if ((c & 0xF0) == 0xE0)
                i += 3;
            else if ((c & 0xF8) == 0xF0)
                i += 4;
            else
                i += 1;
            count++;
        }
    }
    return count;
}

// ---------- UI utilities (unchanged logic) ----------
void makeTopBottomEdgeBorder(int bxLen, std::string &rEdgeChar, std::string &lEdgeChar, std::string &midEdge, std::vector<std::pair<std::string, std::string>> &colors, std::string &frame)
{
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

void makeTextWithColors(std::string &introPhrase, std::vector<std::pair<std::string, std::string>> &colors, int &xTabSize, int &yTabSize, std::string &midVertEdge, bool coloredText, std::string &frame)
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
        coloredText ? textFrame += colors[index].first + std::string(1, c) + colors[index].second : textFrame += "\033[1m" + std::string(1, c) + "\033[0m";
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
            i += 1;
            width += 1;
        }
        else if ((c & 0xE0) == 0xC0)
        {
            i += 2;
            width += 1;
        }
        else if ((c & 0xF0) == 0xE0)
        {
            i += 3;
            width += 1;
        }
        else if ((c & 0xF8) == 0xF0)
        {
            i += 4;
            width += 2;
        }
        else
            i += 1;
    }
    return width;
}

// ---------- Receipt printing (updated type) ----------
void printReceipt(const std::vector<ChosenFood> &chosenFoods, double payment)
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
        double lineTotal = cf.quantity * cf.price;
        total += lineTotal;

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2)
            << cf.name << " x" << cf.quantity
            << " = $" << lineTotal;
        lines.push_back({oss.str()});
    }

    // Totals
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << total;
        lines.push_back({"Total: $" + oss.str()});
    }
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << payment;
        lines.push_back({"Payment: $" + oss.str()});
    }
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << (payment - total);
        lines.push_back({"Change: $" + oss.str()});
    }

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

// ---------- Animation intro (unchanged) ----------
void introduction(float &accumilator, float &dt, float &speed, std::string &frame,
                  std::chrono::high_resolution_clock::time_point &lastFrame,
                  std::string &introPhrase, std::vector<std::pair<std::string, std::string>> &colors)
{
    static std::mt19937 gen(std::random_device{}()); // static RNG
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = now - lastFrame;
    dt = elapsed.count();
    accumilator += speed * dt;
    lastFrame = now;

    if (accumilator >= 0.3f)
    {
        std::shuffle(colors.begin(), colors.end(), gen);
        accumilator = 0.0f; // reset after animation step
    }

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
    makeTextWithColors(introPhrase, colors, xTabSize, yTabSize, midVertEdge, false, frame);
    makeTopBottomEdgeBorder(bxLen, rBottomEdge, lBottomEdge, midHorEdge, colors, frame);
    frame += '\n';
}

// ---------- Console helpers ----------
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

// ---------- Keyboard detection (unchanged) ----------
void detectKeyboard(int &index, int maxIndex, bool &pressEntered, bool &pressedEsc, bool &pressedPayment, bool &pressedCancel, bool &pressedBackspace)
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
        else if (k == 8 || k == 127)
        {
            pressedBackspace = true;
        }
        else if (k == 99 || k == 67)
        {
            pressedCancel = true;
        }
    }
}

// ---------- Payment process (updated type) ----------
void paymentProcess(std::vector<ChosenFood> &chosenFoods, bool &choosing)
{
    bool donePaying = false;
    bool initialCls = false;
    bool paymentNotEnough = false;
    bool invalidInput = false;
    double payment = 0.0;
    double change = 0.0;
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
        if (invalidInput)
        {
            hardClear();
            std::cout << "\033[31m" << u8R"(
╔═══════════════════════════════════╗
║           INVALID INPUT           ║
╚═══════════════════════════════════╝
                    )" << "\033[0m"
                      << "\n"
                      << std::endl;
            std::cout << std::endl;
            invalidInput = false;
        }
        else if (paymentNotEnough)
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
            std::cout << std::to_string(chosenFoodsIndex) << ". " << cf.name << " x" << cf.quantity << " = $" << cf.quantity * cf.price << '\n';
            total += cf.quantity * cf.price;
            chosenFoodsIndex++;
        }
        std::cout << u8"\n═════════════════════════════════                          ";
        std::cout << "\033[1;32m \nTotal: $" << total << "                                ";
        std::cout << u8"\033[0m\n═════════════════════════════════                           \n";
        std::cout << "Payment: $";
        std::cin >> payment;
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            invalidInput = true;
            continue;
        }
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

// ---------- Food selection (updated signature and internal usage) ----------
/*
void chooseFood(
    int menuIndex,
    std::vector<Food> &foods,
    std::vector<ChosenFood> &chosenFoods,
    float &accumilator, float &dt, float &speed,
    std::chrono::high_resolution_clock::time_point &lastFrame,
    std::string introPhrase,
    bool choosing1,
    int tabSizeRightPadding1,
    int tabSizeRightPadding2,
    std::vector<std::pair<std::string, std::string>> &colors)
{
    removeCursor();
    bool choosing2 = true;
    int index = 0;
    bool pressEnter = false;
    bool pressedEsc = false;
    bool pressedPayment = false;
    bool pressedCancelPayment = false;
    bool pressedBackspace = false;
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
        std::string frameInnerDisplay;
        for (int i = 0; i < (int)foods.size(); i++)
        {
            std::ostringstream ss;
            bool selected = false;

            for (auto &z : chosenFoods)
            {
                if (z.name == foods[i].name)
                    selected = true;
            }

            ss << std::fixed << std::setprecision(2) << foods[i].price;
            std::string formatedString = ss.str();
            std::string food_display;

            if (i == index)
            {
                food_display += u8"║ ► \033[1;92m$" + formatedString + " " + foods[i].name;
            }
            else
            {
                food_display += u8"║   \033[1;92m$" + formatedString + " " + foods[i].name;
            }

            // Calculate padding so stock info aligns
            int totalLen = countUTF8Chars(food_display);
            int padding = tabSizeRightPadding1 - totalLen;
            if (padding < 0)
                padding = 0;
            food_display += std::string(padding, ' ');

            // Add stock info
            food_display += (foods[i].stock > 0 ? "\033[0m║\033[1;34m [stock: " + std::to_string(foods[i].stock) + "]\033[0m"
                                                : "\033[0m║\033[1;31m [OUT OF STOCK]\033[0m");

            int totalLen2 = countUTF8Chars(food_display);
            int padding2 = tabSizeRightPadding2 - totalLen2;
            if (padding2 < 0)
                padding2 = 0;
            food_display += std::string(padding2, ' ');
            food_display += u8"║";
            if (selected)
            {
                food_display += " \033[1;34m [selected] \033[0m";
            }

            frameInnerDisplay += food_display + '\n';
            // for (int t = 0; t < totalLen; t++) frame += u8"═";
        }

        for (int t = 0; t <= tabSizeRightPadding2; t++)
        {
            if (t == 0)
            {
                frame += u8"╔";
            }
            else if (t == tabSizeRightPadding1)
            {
                frame += u8"╦";
            }
            else if (t == tabSizeRightPadding2)
            {
                frame += u8"╗\n";
            }
            else
            {
                frame += u8"═";
            }
        }
        std::string midHeader;
        for (int t = 0; t <= tabSizeRightPadding2; t++)
        {
            if (t == 0)
            {
                frame += u8"║";
            }
            else if (t == tabSizeRightPadding1)
            {
                frame += u8"║";
            }
            else if (t == tabSizeRightPadding2)
            {
                frame += u8"║\n";
            }
            else
            {
                frame += u8" ";
            }
        }
        // https://en.wikipedia.org/wiki/Box-drawing_characters
        for (int t = 0; t <= tabSizeRightPadding2; t++)
        {
            if (t == 0)
            {
                frame += u8"╠";
            }
            else if (t == tabSizeRightPadding1)
            {
                frame += u8"╬";
            }
            else if (t == tabSizeRightPadding2)
            {
                frame += u8"╣\n";
            }
            else
            {
                frame += u8"═";
            }
        }
        frame += frameInnerDisplay;

        double total = 0;
        frame += "\nCurrent selections:\n";
        int indFood = 1;
        for (auto &cf : chosenFoods)
        {
            frame += " [" + std::to_string(indFood) + "] " + cf.name + " x" + std::to_string(cf.quantity) + " = $" + std::to_string(cf.price * cf.quantity) + '\n';
            total += cf.quantity * cf.price;
            indFood++;
        }

        // Add spacing and total
        frame += std::string(4 * 38, ' ') + "\n"; // spacing lines
        std::ostringstream sis;
        sis << std::fixed << std::setprecision(2) << total;
        std::string tot = sis.str();
        frame += u8"\n═════════════════════════════════                          ";
        frame += "\033[1;32m \nTotal: $" + tot + "                                ";
        frame += u8"\033[0m\n═════════════════════════════════                           \n";
        frame += std::string(12 * 46, ' ') + "\n"; // extra spacing at bottom

        std::cout << frame;

        detectKeyboard(index, (int)foods.size() - 1, pressEnter, pressedEsc, pressedPayment, pressedCancelPayment, pressedBackspace);

        // ---- ENTER: add food ----
        if (pressEnter)
        {
            pressEnter = false;
            auto it = std::find_if(chosenFoods.begin(), chosenFoods.end(), [&](auto &cf)
                                   { return cf.name == foods[index].name; });
            auto itFoods = std::find_if(foods.begin(), foods.end(), [&](auto &f)
                                        { return f.name == foods[index].name; });
            if (it != chosenFoods.end())
            {
                if (itFoods != foods.end() && (*itFoods).stock > 0)
                {
                    (*it).quantity++;
                    (*itFoods).stock--;
                }
            }
            else
            {
                if (itFoods != foods.end() && (*itFoods).stock > 0)
                {
                    chosenFoods.push_back({foods[index].name, 1, foods[index].price});
                    (*itFoods).stock--;
                }
            }
        }

        // ---- BACKSPACE: remove food ----
        if (pressedBackspace && !chosenFoods.empty())
        {
            pressedBackspace = false;
            auto it = std::find_if(chosenFoods.begin(), chosenFoods.end(), [&](auto &cf)
                                   { return cf.name == foods[index].name; });
            auto itFoods = std::find_if(foods.begin(), foods.end(), [&](auto &f)
                                        { return f.name == foods[index].name; });
            if (it != chosenFoods.end())
            {
                if (it->quantity <= 1)
                {
                    chosenFoods.erase(it);
                    if (itFoods != foods.end())
                        (*itFoods).stock++;
                }
                else
                {
                    it->quantity--;
                    if (itFoods != foods.end())
                        (*itFoods).stock++;
                }
            }
        }

        // ---- ESC: exit ----
        if (pressedEsc)
        {
            pressedEsc = false;
            break;
        }

        // ---- Cancel payment / reset menu ----
        if (pressedCancelPayment)
        {
            for (auto it = chosenFoods.begin(); it != chosenFoods.end();)
            {
                if (std::find_if(foods.begin(), foods.end(), [&](auto &f)
                                 { return f.name == it->name; }) != foods.end())
                {
                    it = chosenFoods.erase(it);
                }
                else
                    ++it;
            }
        }
    }
}
*/

void chooseFood(
    int menuIndex,
    std::vector<Food> &foods,
    std::vector<ChosenFood> &chosenFoods,
    float &accumilator, float &dt, float &speed,
    std::chrono::high_resolution_clock::time_point &lastFrame,
    std::string introPhrase,
    bool choosing1,
    int tabSizeRightPadding1,
    int tabSizeRightPadding2,
    std::vector<std::pair<std::string, std::string>> &colors)
{
    removeCursor();
    bool choosing2 = true;
    int index = 0;
    bool pressEnter = false;
    bool pressedEsc = false;
    bool pressedPayment = false;
    bool pressedCancelPayment = false;
    bool pressedBackspace = false;
    bool initialClear = false;

    while (choosing2)
    {
        if (initialClear)
            properClear();
        else
        {
            hardClear();
            initialClear = true;
        }

        std::string frame;
        introduction(accumilator, dt, speed, frame, lastFrame, introPhrase, colors);

        // --- Determine dynamic column widths ---
        int col1Width = tabSizeRightPadding1; // Foods
        int col2Width = tabSizeRightPadding2; // Stocks
        int col3Width = 10; // Selected (fixed)

        for (auto &f : foods)
        {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2) << f.price;
            std::string priceName = "$" + ss.str() + " " + f.name;
            col1Width = std::max(col1Width, countUTF8Chars(priceName) + 2); // +2 for arrow/padding

            std::string stockStr = (f.stock > 0 ? "\033[1;34m[stock: " + std::to_string(f.stock) + "]\033[0m" : "\033[1;31m[OUT OF STOCK]\033[0m");
            col2Width = std::max(col2Width, countUTF8Chars(stockStr) + 2); // +2 padding
        }

        std::string frameInnerDisplay;

        // --- Rows ---
        for (int i = 0; i < (int)foods.size(); i++)
        {
            std::ostringstream ss;
            bool selected = false;
            for (auto &z : chosenFoods)
                if (z.name == foods[i].name)
                    selected = true;

            ss << std::fixed << std::setprecision(2) << foods[i].price;
            std::string priceName = "$" + ss.str() + " " + foods[i].name;

            // Column 1: Food
            std::string col1Content = (i == index ? "\033[1;32m► " : "  ") + priceName + "\033[0m";
            col1Content += std::string(col1Width - countUTF8Chars(col1Content), ' ');

            // Column 2: Stock
            std::string col2Content = (foods[i].stock > 0 ? "\033[1;34m[stock: " + std::to_string(foods[i].stock) + "]\033[0m" : "\033[1;31m[OUT OF STOCK]\033[0m");
            col2Content += std::string(col2Width - countUTF8Chars(col2Content), ' ');

            // Column 3: Selected
            std::string col3Content = selected ? "\033[1;32m[selected]\033[0m" : "";
            col3Content += std::string(col3Width - countUTF8Chars(col3Content), ' ');

            frameInnerDisplay += u8"║" + col1Content + u8"║" + col2Content + u8"║" + col3Content + u8"║\n";
        }

        // --- Top border ---
        frame += u8"╔";
        for (int t = 1; t <= col1Width; t++)
            frame += u8"═";
        frame += u8"╦";
        for (int t = 1; t <= col2Width; t++)
            frame += u8"═";
        frame += u8"╦";
        for (int t = 1; t <= col3Width; t++)
            frame += u8"═";
        frame += u8"╗\n";

        // --- Header ---
        std::string midHeader = u8"║";
        std::string h1 = " \033[1mFoods \033[0m";
        h1 += std::string(col1Width - countUTF8Chars(h1), ' ');
        midHeader += h1 + u8"║";

        std::string h2 = " \033[1mStocks \033[0m";
        h2 += std::string(col2Width - countUTF8Chars(h2), ' ');
        midHeader += h2 + u8"║";

        std::string h3 = " \033[1mSelected \033[0m";
        h3 += std::string(col3Width - countUTF8Chars(h3), ' ');
        midHeader += h3 + u8"║\n";

        frame += midHeader;

        // --- Header separator ---
        frame += u8"╠";
        for (int t = 1; t <= col1Width; t++)
            frame += u8"═";
        frame += u8"╬";
        for (int t = 1; t <= col2Width; t++)
            frame += u8"═";
        frame += u8"╬";
        for (int t = 1; t <= col3Width; t++)
            frame += u8"═";
        frame += u8"╣\n";

        // --- Rows ---
        frame += frameInnerDisplay;

        // --- Table lower edge ---
        frame += u8"╚";
        for (int t = 1; t <= col1Width; t++)
            frame += u8"═";
        frame += u8"╩";
        for (int t = 1; t <= col2Width; t++)
            frame += u8"═";
        frame += u8"╩";
        for (int t = 1; t <= col3Width; t++)
            frame += u8"═";
        frame += u8"╝\n";




        // --- Total selections ---
        double total = 0;
        frame += "\nCurrent selections:\n";
        int indFood = 1;
        for (auto &cf : chosenFoods)
        {
            frame += " [" + std::to_string(indFood) + "] " + cf.name + " x" + std::to_string(cf.quantity) + " = $" + std::to_string(cf.price * cf.quantity) + '\n';
            total += cf.quantity * cf.price;
            indFood++;
        }

        // --- Total display ---
        frame += std::string(4 * 38, ' ') + "\n"; // spacing lines
        std::ostringstream sis;
        sis << std::fixed << std::setprecision(2) << total;
        std::string tot = sis.str();
        frame += u8"\n                                 \n═════════════════════════════════                          ";
        frame += "\033[1;32m \nTotal: $" + tot + "                                ";
        frame += u8"\033[0m\n═════════════════════════════════                           \n";
        frame += std::string(12 * 46, ' ') + "\n"; // extra spacing at bottom

        std::cout << frame;

        detectKeyboard(index, (int)foods.size() - 1, pressEnter, pressedEsc, pressedPayment, pressedCancelPayment, pressedBackspace);

        // ---- ENTER: add food ----
        if (pressEnter)
        {
            pressEnter = false;
            auto it = std::find_if(chosenFoods.begin(), chosenFoods.end(), [&](auto &cf)
                                   { return cf.name == foods[index].name; });
            auto itFoods = std::find_if(foods.begin(), foods.end(), [&](auto &f)
                                        { return f.name == foods[index].name; });
            if (it != chosenFoods.end())
            {
                if (itFoods != foods.end() && (*itFoods).stock > 0)
                {
                    (*it).quantity++;
                    (*itFoods).stock--;
                }
            }
            else
            {
                if (itFoods != foods.end() && (*itFoods).stock > 0)
                {
                    chosenFoods.push_back({foods[index].name, 1, foods[index].price});
                    (*itFoods).stock--;
                }
            }
        }

        // ---- BACKSPACE: remove food ----
        if (pressedBackspace && !chosenFoods.empty())
        {
            pressedBackspace = false;
            auto it = std::find_if(chosenFoods.begin(), chosenFoods.end(), [&](auto &cf)
                                   { return cf.name == foods[index].name; });
            auto itFoods = std::find_if(foods.begin(), foods.end(), [&](auto &f)
                                        { return f.name == foods[index].name; });
            if (it != chosenFoods.end())
            {
                if (it->quantity <= 1)
                {
                    chosenFoods.erase(it);
                    if (itFoods != foods.end())
                        (*itFoods).stock++;
                }
                else
                {
                    it->quantity--;
                    if (itFoods != foods.end())
                        (*itFoods).stock++;
                }
            }
        }

        // ---- ESC: exit ----
        if (pressedEsc)
        {
            pressedEsc = false;
            break;
        }

        // ---- Cancel payment / reset menu ----
        if (pressedCancelPayment)
        {
            for (auto &i : chosenFoods) {
                auto it = std::find_if(foods.begin(), foods.end(), [&](auto &f){
                    return f.name == i.name;
                });

                if (it != foods.end()) {
                    (*it).stock += i.quantity;
                    i.quantity = 0;
                    auto it2 = std::find_if(chosenFoods.begin(), chosenFoods.end(), [&](auto &f){
                        return f.name == i.name;
                    });
                    chosenFoods.erase(it2);
                }
            }
            // for (auto it = chosenFoods.begin(); it != chosenFoods.end();)
            // {
            //     if (std::find_if(foods.begin(), foods.end(), [&](auto &f)
            //                      { return f.name == it->name; }) != foods.end())
            //     {
            //         it = chosenFoods.erase(it);
            //     }
            //     else
            //         ++it;
            // }
        }
    }
}


void getTabLin () {

}

// ---------- Menu selection (updated types) ----------
void menuSelection(
    std::vector<MenuCategory> &restaurantMenu,
    float &accumilator, float &dt, float &speed,
    std::chrono::high_resolution_clock::time_point &lastFrame,
    std::vector<std::pair<std::string, std::string>> &colors,
    std::vector<std::pair<std::string, std::string>> &paymentColors,
    float &payAccumilator, float &paydt, float &payspeed)
{
    removeCursor();
    bool choosing = true;
    int menuIndex = 0;
    bool pressEnter = false;
    bool pressedEsc = false;
    bool pressedPayment = false;
    bool pressedCancelPayment = false;
    bool pressedBackspace = false;
    bool initialClear = false;
    std::vector<ChosenFood> chosenFoods; // updated type
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
        for (int i = 0; i < (int)restaurantMenu.size(); i++)
        {
            if (i == menuIndex)
            {
                frame += u8" ► " + std::string("\033[1;92m") + restaurantMenu[i].name + "\033[0m" + "                                                                         " + '\n';
            }
            else
            {
                frame += "   " + restaurantMenu[i].name + "                                                                                   " + '\n';
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
                double iprice = cf.quantity * cf.price;
                oss << std::fixed << std::setprecision(2) << iprice;
                std::string formattedShit = oss.str();
                frame += " [" + std::to_string(ind) + "] " + cf.name + " x" + std::to_string(cf.quantity) + " = $" + formattedShit + '\n';
                total += iprice;
                ind++;
            }
            std::ostringstream sis;
            sis << std::fixed << std::setprecision(2) << total;
            std::string tot = sis.str();
            frame += u8"\n═════════════════════════════════                          ";
            frame += "\033[1;32m \nTotal: $" + tot + "                                ";
            frame += u8"\033[0m\n═════════════════════════════════                           \n";

            std::string paymentPhrase = "PRESS 'P' FOR PAYMENT :)";
            introduction(payAccumilator, paydt, payspeed, frame, lastFrame, paymentPhrase, paymentColors);
        }

        std::cout << frame;

        detectKeyboard(menuIndex, (int)restaurantMenu.size() - 1, pressEnter, pressedEsc, pressedPayment, pressedCancelPayment, pressedBackspace);

        if (pressEnter)
        {
            pressEnter = false;
            std::string foodIntroPhrase = "Select your food (ENTER to add, BACKSPACE to decrease, ESC to go back, C to cancel current menu)";
            chooseFood(menuIndex, restaurantMenu[menuIndex].foods, chosenFoods, accumilator, dt, speed, lastFrame, foodIntroPhrase, choosing, 30, 35, colors);
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

// ---------- main (constructing restaurant menu with structs) ----------
int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    float accumilator = 0;
    float dt = 0;
    float speed = 0.9f;
    float payAccumilator = 0;
    float paydt = 0;
    float paySpeed = 3.9f;
    auto lastFrame = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<std::string, std::string>> colors = {
        {"\033[1;91m", "\033[0m"}, {"\033[1;92m", "\033[0m"}, {"\033[1;93m", "\033[0m"}, {"\033[1;94m", "\033[0m"}, {"\033[1;95m", "\033[0m"}, {"\033[1;96m", "\033[0m"}, {"\033[1;97m", "\033[0m"}};
    std::vector<std::pair<std::string, std::string>> paymentColors = {
        {"\033[1;91m", "\033[0m"},
        {"\033[1;95m", "\033[0m"},
        {"\033[1;94m", "\033[0m"},
        {"\033[1;96m", "\033[0m"},
        {"\033[1;92m", "\033[0m"},
        {"\033[1;93m", "\033[0m"},
        {"\033[1;97m", "\033[0m"}};

    // Build the restaurantMenu using MenuCategory and Food
    std::vector<MenuCategory> restaurantMenu = {
        {"🍔 Food Menu",
         {
             {2.99, "Fries", 10},
             {4.99, "Burger", 10},
             {5.49, "Cheese Burger", 10},
             {5.99, "Bacon Burger", 10},
             {3.99, "Hotdog", 10},
             {6.49, "Pizza Slice", 10},
         }},
        {"🍰 Dessert Menu",
         {
             {1.49, "Cookies", 10},
             {2.29, "Cupcake", 10},
             {1.99, "Donut", 10},
             {2.99, "Ice Cream", 10},
             {3.49, "Cheesecake", 10},
             {3.99, "Apple Pie", 10},
         }},
        {"🥤 Drinks Menu",
         {
             {0.99, "Soda", 10},
             {1.49, "Juice", 10},
             {1.99, "Coffee", 10},
             {2.49, "Milk Tea", 10},
             {3.99, "Beer", 10},
             {4.49, "Wine", 10},
         }}};

    menuSelection(restaurantMenu, accumilator, dt, speed, lastFrame, colors, paymentColors, payAccumilator, paydt, paySpeed);
    return 0;
}
