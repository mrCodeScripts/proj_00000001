#include <iostream>
#include <iomanip>
#include <limits>
#include <string>

int main() {
    int tableW, tableH;
    int highlightX = -1, highlightY = -1;

    // --- Get table dimensions ---
    std::cout << "Enter number of rows: ";
    std::cin >> tableH;
    std::cout << "Enter number of columns: ";
    std::cin >> tableW;

    // --- Get highlight coordinates ---
    std::cout << "Enter row to highlight (1-" << tableH << ", 0 for none): ";
    std::cin >> highlightY;
    std::cout << "Enter column to highlight (1-" << tableW << ", 0 for none): ";
    std::cin >> highlightX;

    // Convert to 0-based indices
    highlightY--;
    highlightX--;

    // --- Clear screen ---
    std::cout << "\033[2J\033[1;1H" << std::flush;

    // --- Render table ---
    for (int i = 0; i < tableH; i++) {
        for (int j = 0; j < tableW; j++) {
            int value = (i + 1) * (j + 1);

            if (i == highlightY && j == highlightX) {
                // Highlight the chosen number in bright green
                std::cout << "\033[1;92m" << std::setw(5) << value << "\033[0m";
            } else {
                std::cout << std::setw(5) << value;
            }
        }
        std::cout << std::endl;
    }

    return 0;
}

