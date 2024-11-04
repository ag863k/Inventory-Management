#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

struct Item {
    int id;
    std::string name;
    int quantity;
    double cost;
};

class Inventory {
private:
    std::vector<Item> items;
    int nextId = 1;

public:
    void addItem(const std::string &name, int quantity, double cost) {
        items.push_back({nextId++, name, quantity, cost});
        std::cout << "Item added successfully!\n";
    }

    void displayItems() {
        if (items.empty()) {
            std::cout << "No items in the inventory.\n";
            return;
        }

        std::cout << std::left << std::setw(5) << "ID" 
                  << std::setw(15) << "Name" 
                  << std::setw(10) << "Quantity" 
                  << std::setw(10) << "Cost\n";
        std::cout << "---------------------------------------\n";
        for (const auto &item : items) {
            std::cout << std::left << std::setw(5) << item.id 
                      << std::setw(15) << item.name 
                      << std::setw(10) << item.quantity 
                      << std::setw(10) << item.cost << "\n";
        }
    }

    void updateItem(int id, int newQuantity, double newCost) {
        for (auto &item : items) {
            if (item.id == id) {
                item.quantity = newQuantity;
                item.cost = newCost;
                std::cout << "Item updated successfully!\n";
                return;
            }
        }
        std::cout << "Item not found.\n";
    }

    void deleteItem(int id) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->id == id) {
                items.erase(it);
                std::cout << "Item deleted successfully!\n";
                return;
            }
        }
        std::cout << "Item not found.\n";
    }
};

void displayMenu() {
    std::cout << "\nInventory Management System\n";
    std::cout << "1. Add Item\n";
    std::cout << "2. Display Items\n";
    std::cout << "3. Update Item\n";
    std::cout << "4. Delete Item\n";
    std::cout << "5. Exit\n";
    std::cout << "Choose an option: ";
}

int main() {
    Inventory inventory;
    int choice;

    while (true) {
        displayMenu();
        std::cin >> choice;

        if (choice == 1) {
            std::string name;
            int quantity;
            double cost;

            std::cout << "Enter item name: ";
            std::cin >> name;
            std::cout << "Enter quantity: ";
            std::cin >> quantity;
            std::cout << "Enter cost: ";
            std::cin >> cost;

            inventory.addItem(name, quantity, cost);
        } 
        else if (choice == 2) {
            inventory.displayItems();
        } 
        else if (choice == 3) {
            int id, quantity;
            double cost;

            std::cout << "Enter item ID to update: ";
            std::cin >> id;
            std::cout << "Enter new quantity: ";
            std::cin >> quantity;
            std::cout << "Enter new cost: ";
            std::cin >> cost;

            inventory.updateItem(id, quantity, cost);
        } 
        else if (choice == 4) {
            int id;
            std::cout << "Enter item ID to delete: ";
            std::cin >> id;
            inventory.deleteItem(id);
        } 
        else if (choice == 5) {
            std::cout << "Exiting program.\n";
            break;
        } 
        else {
            std::cout << "Invalid option. Please try again.\n";
        }
    }

    return 0;
}
