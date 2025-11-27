#include <iostream>
#include <cctype> // for std::tolower()
#include <windows.h>

/**
 * Function best for asking Yes or No.
 */
char askYN(const std::string &msg)
{
    char c;
    while (true)
    {
        std::cout << msg;
        std::cin >> c;
        c = std::tolower(c);

        if (c == 'y' || c == 'n')
            return c;

        std::cout << "\033[1;31mINVALID INPUT. Please enter Y or N.\033[0m\n";
    }
}

void createTable(int &width, int &height, int &headerRows, int &headerCol, std::string &leftTopEdge, std::string &rightTopEdge, std::string &leftBottomEdge, std::string &rightBottomEdge, std::string &horizontalEdge, std::string &verticalEdge, std::string &frame)
{
    for (int i = 0; i < width; i++) {
        frame += horizontalEdge;
    }
}

int main()
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    int WIDTH = 30;
    int HEIGHT = 30;
    int HEADER_ROWS = 1;
    int HEADER_COL = 3;
    std::string LEFT_TOP_EDGE = u8"┌";
    std::string RIGHT_TOP_EDGE = u8"┐";
    std::string LEFT_BOTTOM_EDGE = u8"└";
    std::string RIGHT_BOTTOM_EDGE = u8"┘";
    std::string HORIZONTAL_EDGE = u8"-";
    std::string VERTICAL_EDGE = u8"│";

    std::string frame;

    createTable(WIDTH, HEIGHT, HEADER_ROWS, HEADER_COL, LEFT_TOP_EDGE, RIGHT_TOP_EDGE, LEFT_BOTTOM_EDGE, RIGHT_BOTTOM_EDGE, HORIZONTAL_EDGE, VERTICAL_EDGE, frame);

    std::cout << frame;

    return 0;
}