#include <iostream>
#include <string>
using namespace std;

int main() {
    // Food menus
    string menuNames[] = {"Menu 1", "Menu 2", "Menu 3"};
    string menu1Foods[] = {"Burger", "Cheese Burger", "Bacon Burger"};
    double menu1Prices[] = {99.99, 129.50, 149.75};

    string menu2Foods[] = {"Pepperoni Pizza", "Four Cheese", "Hawaiian"};
    double menu2Prices[] = {199.99, 249.99, 229.99};

    string menu3Foods[] = {"Iced Tea", "Coke", "Water"};
    double menu3Prices[] = {49.99, 59.99, 39.99};

    string *allFoods[] = {menu1Foods, menu2Foods, menu3Foods};
    double *allPrices[] = {menu1Prices, menu2Prices, menu3Prices};
    int menuSizes[] = {3, 3, 3};

    // Orders storage
    string orderNames[100];
    double orderPrices[100];
    int orderQuantities[100];
    int orderCount = 0;

    while (true) {
        cout << "\nSelect a menu:\n";
        for (int i = 0; i < 3; i++) cout << i + 1 << ". " << menuNames[i] << endl;
        cout << "0. Exit\n";
        int menuChoice;
        cin >> menuChoice;

        if (menuChoice == 0) break;
        menuChoice--; // array index

        while (true) {
            cout << "\n" << menuNames[menuChoice] << ":\n";
            for (int i = 0; i < menuSizes[menuChoice]; i++) {
                cout << i + 1 << ". " << allFoods[menuChoice][i] 
                     << " - $" << allPrices[menuChoice][i] << endl;
            }
            cout << "0. Go back\n";
            cout << "C. Cancel this menu\n";
            cout << "P. Pay\n";

            string choice;
            cin >> choice;

            if (choice == "0") break; // back to menu
            else if (choice == "C" || choice == "c") {
                // Cancel this menu items
                for (int i = 0; i < orderCount;) {
                    bool found = false;
                    for (int f = 0; f < menuSizes[menuChoice]; f++) {
                        if (orderNames[i] == allFoods[menuChoice][f]) found = true;
                    }
                    if (found) {
                        // remove by shifting
                        for (int j = i; j < orderCount - 1; j++) {
                            orderNames[j] = orderNames[j+1];
                            orderPrices[j] = orderPrices[j+1];
                            orderQuantities[j] = orderQuantities[j+1];
                        }
                        orderCount--;
                    } else i++;
                }
                cout << "Cancelled this menu selections.\n";
            }
            else if (choice == "P" || choice == "p") {
                // Calculate total
                double total = 0;
                cout << "\n----- RECEIPT -----\n";
                for (int i = 0; i < orderCount; i++) {
                    cout << orderNames[i] << " x" << orderQuantities[i] 
                         << " = $" << orderPrices[i] * orderQuantities[i] << endl;
                    total += orderPrices[i] * orderQuantities[i];
                }
                cout << "Total: $" << total << endl;

                // Payment
                double money;
                while (true) {
                    cout << "Enter payment amount: $";
                    cin >> money;
                    if (money >= total) {
                        cout << "Change: $" << money - total << endl;
                        cout << "Thank you for your order!\n";
                        orderCount = 0; // clear orders
                        break;
                    } else {
                        cout << "Not enough money. Please enter at least $" << total << endl;
                    }
                }
            }
            else {
                int foodChoice = stoi(choice) - 1;
                if (foodChoice >= 0 && foodChoice < menuSizes[menuChoice]) {
                    // check if already in orders
                    bool found = false;
                    for (int i = 0; i < orderCount; i++) {
                        if (orderNames[i] == allFoods[menuChoice][foodChoice]) {
                            orderQuantities[i]++;
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        orderNames[orderCount] = allFoods[menuChoice][foodChoice];
                        orderPrices[orderCount] = allPrices[menuChoice][foodChoice];
                        orderQuantities[orderCount] = 1;
                        orderCount++;
                    }
                } else {
                    cout << "Invalid selection.\n";
                }
            }
        }
    }

    cout << "\nExiting program. Goodbye!\n";
    return 0;
}

