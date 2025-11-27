#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <ostream>

int main()
{
    int tablex;

    std::cout << "Enter size: ";
    std::cin >> tablex;

    std::cout << "\033[2J\033[1;1H" << std::flush;

    for (int i = 0; i < tablex; i++)
    {
        for (int j = 0; j < tablex; j++)
        {
            int value = (i + 1) * (j + 1);
            std::cout << std::setw(2) << '|' << std::setw(3) << value;
        }
        std::cout << std::endl;
    }

    return 0;
}
