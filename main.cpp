#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <memory>
#include <limits>
#include <climits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <ctime>
#include <map>
#include <regex>
#include <chrono>
#include <random>
#include <numeric>

/**
 * @brief Represents an inventory item with comprehensive details
 */
class Item {
private:
    static int nextId;
    int id;
    std::string name;
    std::string category;
    std::string supplier;
    std::string barcode;
    int quantity;
    int minimumStock;
    double cost;
    double sellingPrice;
    std::time_t dateAdded;
    std::time_t lastModified;
    std::time_t expiryDate;
    std::string location;
    std::string description;

public:
    /**
     * @brief Constructs a new Item object with comprehensive parameters
     */
    Item(const std::string& name, const std::string& category, int quantity, 
         double cost, double sellingPrice = 0.0, const std::string& supplier = "",
         const std::string& location = "", int minimumStock = 5)
        : id(nextId++), name(name), category(category), supplier(supplier),
          quantity(quantity), minimumStock(minimumStock), cost(cost), 
          sellingPrice(sellingPrice), location(location) {
        
        dateAdded = lastModified = std::time(nullptr);
        expiryDate = 0; // No expiry by default
        generateBarcode();
        validateInput();
    }

    // Comprehensive getters
    int getId() const { return id; }
    const std::string& getName() const { return name; }
    const std::string& getCategory() const { return category; }
    const std::string& getSupplier() const { return supplier; }
    const std::string& getBarcode() const { return barcode; }
    int getQuantity() const { return quantity; }
    int getMinimumStock() const { return minimumStock; }
    double getCost() const { return cost; }
    double getSellingPrice() const { return sellingPrice; }
    double getTotalValue() const { return quantity * cost; }
    double getPotentialRevenue() const { return quantity * sellingPrice; }
    double getProfit() const { return (sellingPrice - cost) * quantity; }
    std::time_t getDateAdded() const { return dateAdded; }
    std::time_t getLastModified() const { return lastModified; }
    std::time_t getExpiryDate() const { return expiryDate; }
    const std::string& getLocation() const { return location; }
    const std::string& getDescription() const { return description; }

    // Enhanced setters with validation
    void setName(const std::string& newName) {
        if (newName.empty()) {
            throw std::invalid_argument("Item name cannot be empty");
        }
        name = newName;
        lastModified = std::time(nullptr);
    }

    void setCategory(const std::string& newCategory) {
        category = newCategory;
        lastModified = std::time(nullptr);
    }

    void setSupplier(const std::string& newSupplier) {
        supplier = newSupplier;
        lastModified = std::time(nullptr);
    }

    void setQuantity(int newQuantity) {
        if (newQuantity < 0) {
            throw std::invalid_argument("Quantity cannot be negative");
        }
        quantity = newQuantity;
        lastModified = std::time(nullptr);
    }

    void setMinimumStock(int newMinStock) {
        if (newMinStock < 0) {
            throw std::invalid_argument("Minimum stock cannot be negative");
        }
        minimumStock = newMinStock;
        lastModified = std::time(nullptr);
    }

    void setCost(double newCost) {
        if (newCost < 0.0) {
            throw std::invalid_argument("Cost cannot be negative");
        }
        cost = newCost;
        lastModified = std::time(nullptr);
    }

    void setSellingPrice(double newPrice) {
        if (newPrice < 0.0) {
            throw std::invalid_argument("Selling price cannot be negative");
        }
        sellingPrice = newPrice;
        lastModified = std::time(nullptr);
    }

    void setLocation(const std::string& newLocation) {
        location = newLocation;
        lastModified = std::time(nullptr);
    }

    void setDescription(const std::string& newDescription) {
        description = newDescription;
        lastModified = std::time(nullptr);
    }

    void setExpiryDate(std::time_t newExpiryDate) {
        expiryDate = newExpiryDate;
        lastModified = std::time(nullptr);
    }

    void updateQuantity(int delta) {
        int newQuantity = quantity + delta;
        if (newQuantity < 0) {
            throw std::invalid_argument("Cannot reduce quantity below zero");
        }
        quantity = newQuantity;
        lastModified = std::time(nullptr);
    }

    bool isLowStock() const {
        return quantity <= minimumStock;
    }

    bool isExpiringSoon(int daysThreshold = 30) const {
        if (expiryDate == 0) return false;
        
        auto now = std::time(nullptr);
        auto threshold = now + (daysThreshold * 24 * 60 * 60);
        return expiryDate <= threshold;
    }

    bool isExpired() const {
        if (expiryDate == 0) return false;
        return expiryDate <= std::time(nullptr);
    }

    double getProfitMargin() const {
        if (cost == 0.0) return 0.0;
        return ((sellingPrice - cost) / cost) * 100.0;
    }

    /**
     * @brief Converts item to CSV format for dataset operations
     */
    std::string toCSV() const {
        std::ostringstream oss;
        oss << id << "," << escapeCSV(name) << "," << escapeCSV(category) << ","
            << escapeCSV(supplier) << "," << barcode << "," << quantity << ","
            << minimumStock << "," << std::fixed << std::setprecision(2) << cost << ","
            << sellingPrice << "," << dateAdded << "," << lastModified << ","
            << expiryDate << "," << escapeCSV(location) << "," << escapeCSV(description);
        return oss.str();
    }

    /**
     * @brief Creates item from CSV string with comprehensive error handling
     */
    static std::unique_ptr<Item> fromCSV(const std::string& csvLine) {
        std::vector<std::string> fields = parseCSVLine(csvLine);
        
        if (fields.size() < 14) {
            throw std::invalid_argument("Invalid CSV format: insufficient fields");
        }

        try {
            auto item = std::make_unique<Item>(
                fields[1],                    // name
                fields[2],                    // category
                std::stoi(fields[5]),         // quantity
                std::stod(fields[7]),         // cost
                std::stod(fields[8]),         // selling price
                fields[3],                    // supplier
                fields[12],                   // location
                std::stoi(fields[6])          // minimum stock
            );
            
            item->id = std::stoi(fields[0]);
            item->barcode = fields[4];
            item->dateAdded = std::stoll(fields[9]);
            item->lastModified = std::stoll(fields[10]);
            item->expiryDate = std::stoll(fields[11]);
            item->description = fields[13];
            
            // Update nextId to avoid conflicts
            if (item->id >= nextId) {
                nextId = item->id + 1;
            }

            return item;
        } catch (const std::exception& e) {
            throw std::invalid_argument("Failed to parse CSV line: " + std::string(e.what()));
        }
    }

    /**
     * @brief Returns CSV header for dataset export
     */
    static std::string getCSVHeader() {
        return "ID,Name,Category,Supplier,Barcode,Quantity,MinimumStock,Cost,SellingPrice,DateAdded,LastModified,ExpiryDate,Location,Description";
    }

private:
    void validateInput() {
        if (name.empty()) {
            throw std::invalid_argument("Item name cannot be empty");
        }
        if (quantity < 0) {
            throw std::invalid_argument("Quantity cannot be negative");
        }
        if (cost < 0.0) {
            throw std::invalid_argument("Cost cannot be negative");
        }
        if (sellingPrice < 0.0) {
            throw std::invalid_argument("Selling price cannot be negative");
        }
    }

    void generateBarcode() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(100000000, 999999999);
        barcode = std::to_string(dis(gen));
    }

    static std::string escapeCSV(const std::string& field) {
        if (field.find(',') != std::string::npos || 
            field.find('"') != std::string::npos || 
            field.find('\n') != std::string::npos) {
            std::string escaped = "\"";
            for (char c : field) {
                if (c == '"') escaped += "\"\"";
                else escaped += c;
            }
            escaped += "\"";
            return escaped;
        }
        return field;
    }

    static std::vector<std::string> parseCSVLine(const std::string& line) {
        std::vector<std::string> fields;
        std::string field;
        bool inQuotes = false;
        
        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];
            
            if (c == '"') {
                if (inQuotes && i + 1 < line.length() && line[i + 1] == '"') {
                    field += '"';
                    ++i; // Skip next quote
                } else {
                    inQuotes = !inQuotes;
                }
            } else if (c == ',' && !inQuotes) {
                fields.push_back(field);
                field.clear();
            } else {
                field += c;
            }
        }
        fields.push_back(field);
        return fields;
    }
};

// Static member initialization
int Item::nextId = 1;

/**
 * @brief Professional Inventory Management System with advanced features
 */
class InventoryManager {
private:
    std::vector<std::unique_ptr<Item>> items;
    std::string dataFile;

public:
    explicit InventoryManager(const std::string& filename = "inventory.csv") 
        : dataFile(filename) {
        loadFromFile();
    }

    ~InventoryManager() {
        saveToFile();
    }

    /**
     * @brief Adds a new item to the inventory with comprehensive dataset support
     */
    bool addItem(const std::string& name, const std::string& category, int quantity, 
                 double cost, double sellingPrice = 0.0, const std::string& supplier = "",
                 const std::string& location = "", int minimumStock = 5) {
        try {
            auto item = std::make_unique<Item>(name, category, quantity, cost, 
                                             sellingPrice, supplier, location, minimumStock);
            items.push_back(std::move(item));
            saveToFile();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error adding item: " << e.what() << std::endl;
            return false;
        }
    }

    /**
     * @brief Simple add item for backward compatibility
     */
    bool addItem(const std::string& name, int quantity, double cost) {
        return addItem(name, "General", quantity, cost);
    }

    /**
     * @brief Displays all items in a comprehensive formatted table
     */
    void displayItems() const {
        if (items.empty()) {
            std::cout << "\nNo items found in the inventory.\n";
            return;
        }

        std::cout << "\nCOMPREHENSIVE INVENTORY REPORT\n";
        std::cout << std::string(140, '=') << "\n";
        std::cout << std::left 
                  << std::setw(4) << "ID"
                  << std::setw(20) << "NAME"
                  << std::setw(12) << "CATEGORY"
                  << std::setw(8) << "QTY"
                  << std::setw(8) << "MIN"
                  << std::setw(10) << "COST"
                  << std::setw(10) << "SELL"
                  << std::setw(12) << "VALUE"
                  << std::setw(12) << "PROFIT"
                  << std::setw(15) << "SUPPLIER"
                  << std::setw(12) << "LOCATION"
                  << std::setw(8) << "STATUS\n";
        std::cout << std::string(140, '-') << "\n";

        double totalInventoryValue = 0.0;
        double totalPotentialProfit = 0.0;
        int lowStockCount = 0;

        for (const auto& item : items) {
            std::string status = "";
            if (item->isLowStock()) {
                status = "LOW";
                lowStockCount++;
            } else if (item->isExpiringSoon()) {
                status = "EXP SOON";
            } else if (item->isExpired()) {
                status = "EXPIRED";
            } else {
                status = "OK";
            }

            std::cout << std::left
                      << std::setw(4) << item->getId()
                      << std::setw(20) << (item->getName().length() > 19 ? 
                                          item->getName().substr(0, 16) + "..." : item->getName())
                      << std::setw(12) << (item->getCategory().length() > 11 ? 
                                          item->getCategory().substr(0, 8) + "..." : item->getCategory())
                      << std::setw(8) << item->getQuantity()
                      << std::setw(8) << item->getMinimumStock()
                      << std::setw(10) << std::fixed << std::setprecision(2) << "$" << item->getCost()
                      << std::setw(10) << "$" << item->getSellingPrice()
                      << std::setw(12) << "$" << item->getTotalValue()
                      << std::setw(12) << "$" << item->getProfit()
                      << std::setw(15) << (item->getSupplier().length() > 14 ? 
                                          item->getSupplier().substr(0, 11) + "..." : item->getSupplier())
                      << std::setw(12) << (item->getLocation().length() > 11 ? 
                                          item->getLocation().substr(0, 8) + "..." : item->getLocation())
                      << std::setw(8) << status << "\n";
            
            totalInventoryValue += item->getTotalValue();
            totalPotentialProfit += item->getProfit();
        }

        std::cout << std::string(140, '-') << "\n";
        std::cout << "SUMMARY: " << items.size() << " items | "
                  << "Value: $" << std::fixed << std::setprecision(2) << totalInventoryValue << " | "
                  << "Potential Profit: $" << totalPotentialProfit << " | "
                  << "Low Stock Items: " << lowStockCount << "\n\n";
    }

    /**
     * @brief Updates an existing item
     */
    bool updateItem(int id, const std::string& newName = "", int newQuantity = -1, double newCost = -1.0) {
        auto item = findItemById(id);
        if (!item) {
            std::cout << "Item with ID " << id << " not found.\n";
            return false;
        }

        try {
            bool updated = false;
            
            if (!newName.empty()) {
                item->setName(newName);
                updated = true;
            }
            
            if (newQuantity >= 0) {
                item->setQuantity(newQuantity);
                updated = true;
            }
            
            if (newCost >= 0.0) {
                item->setCost(newCost);
                updated = true;
            }

            if (updated) {
                saveToFile();
                std::cout << "Item updated successfully!\n";
                return true;
            } else {
                std::cout << "No valid updates provided.\n";
                return false;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error updating item: " << e.what() << std::endl;
            return false;
        }
    }

    /**
     * @brief Deletes an item from inventory
     */
    bool deleteItem(int id) {
        auto it = std::find_if(items.begin(), items.end(),
                              [id](const std::unique_ptr<Item>& item) {
                                  return item->getId() == id;
                              });

        if (it != items.end()) {
            std::cout << "Deleting item: " << (*it)->getName() << std::endl;
            items.erase(it);
            saveToFile();
            std::cout << "Item deleted successfully!\n";
            return true;
        }

        std::cout << "Item with ID " << id << " not found.\n";
        return false;
    }

    /**
     * @brief Searches items by name, category, supplier, or barcode
     */
    void searchItems(const std::string& searchTerm) const {
        std::vector<const Item*> matches;
        
        std::string lowerSearchTerm = toLower(searchTerm);
        
        for (const auto& item : items) {
            if (toLower(item->getName()).find(lowerSearchTerm) != std::string::npos ||
                toLower(item->getCategory()).find(lowerSearchTerm) != std::string::npos ||
                toLower(item->getSupplier()).find(lowerSearchTerm) != std::string::npos ||
                item->getBarcode().find(searchTerm) != std::string::npos) {
                matches.push_back(item.get());
            }
        }

        if (matches.empty()) {
            std::cout << "No items found matching '" << searchTerm << "'\n";
            return;
        }

        std::cout << "\nSEARCH RESULTS for '" << searchTerm << "'\n";
        std::cout << std::string(100, '=') << "\n";
        
        for (const auto* item : matches) {
            std::cout << "ID: " << item->getId() 
                      << " | Name: " << item->getName()
                      << " | Category: " << item->getCategory()
                      << " | Supplier: " << item->getSupplier()
                      << " | Barcode: " << item->getBarcode()
                      << "\n    Qty: " << item->getQuantity()
                      << " | Cost: $" << std::fixed << std::setprecision(2) << item->getCost()
                      << " | Sell: $" << item->getSellingPrice()
                      << " | Value: $" << item->getTotalValue()
                      << " | Location: " << item->getLocation() << "\n\n";
        }
        std::cout << "Found " << matches.size() << " matching item(s)\n\n";
    }

    /**
     * @brief Displays low stock items using minimum stock levels
     */
    void showLowStockItems() const {
        std::vector<const Item*> lowStockItems;
        
        for (const auto& item : items) {
            if (item->isLowStock()) {
                lowStockItems.push_back(item.get());
            }
        }

        if (lowStockItems.empty()) {
            std::cout << "No items with low stock detected.\n";
            return;
        }

        std::cout << "\nLOW STOCK ALERT\n";
        std::cout << std::string(80, '=') << "\n";
        
        for (const auto* item : lowStockItems) {
            std::cout << "CRITICAL - ID: " << item->getId() 
                      << " | Name: " << item->getName()
                      << " | Category: " << item->getCategory()
                      << "\n    Current: " << item->getQuantity() 
                      << " | Minimum: " << item->getMinimumStock()
                      << " | Supplier: " << item->getSupplier()
                      << " | Location: " << item->getLocation() << "\n\n";
        }
        std::cout << lowStockItems.size() << " item(s) need immediate restocking\n\n";
    }

    /**
     * @brief Adjusts item quantity (positive to add, negative to remove)
     */
    bool adjustQuantity(int id, int delta) {
        auto item = findItemById(id);
        if (!item) {
            std::cout << "Item with ID " << id << " not found.\n";
            return false;
        }

        try {
            item->updateQuantity(delta);
            saveToFile();
            
            std::string action = (delta > 0) ? "Added" : "Removed";
            std::cout << action << " " << std::abs(delta) 
                      << " units. New quantity: " << item->getQuantity() << "\n";
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error adjusting quantity: " << e.what() << std::endl;
            return false;
        }
    }

    /**
     * @brief Returns the number of items in inventory
     */
    size_t getItemCount() const {
        return items.size();
    }

    /**
     * @brief Calculates total inventory value
     */
    double getTotalInventoryValue() const {
        double total = 0.0;
        for (const auto& item : items) {
            total += item->getTotalValue();
        }
        return total;
    }

    /**
     * @brief Calculates total potential profit
     */
    double getTotalPotentialProfit() const {
        double total = 0.0;
        for (const auto& item : items) {
            total += item->getProfit();
        }
        return total;
    }

    /**
     * @brief Gets items by category
     */
    void displayItemsByCategory(const std::string& category) const {
        std::vector<const Item*> categoryItems;
        
        for (const auto& item : items) {
            if (toLower(item->getCategory()) == toLower(category)) {
                categoryItems.push_back(item.get());
            }
        }

        if (categoryItems.empty()) {
            std::cout << "No items found in category '" << category << "'\n";
            return;
        }

        std::cout << "\nITEMS IN CATEGORY: " << category << "\n";
        std::cout << std::string(60, '=') << "\n";
        
        for (const auto* item : categoryItems) {
            std::cout << "ID: " << item->getId() 
                      << " | Name: " << item->getName()
                      << " | Qty: " << item->getQuantity()
                      << " | Cost: $" << std::fixed << std::setprecision(2) << item->getCost()
                      << " | Value: $" << item->getTotalValue() << "\n";
        }
        std::cout << "\nFound " << categoryItems.size() << " item(s) in this category\n\n";
    }

    /**
     * @brief Exports inventory to CSV with full dataset
     */
    bool exportToCSV(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not create export file " << filename << std::endl;
            return false;
        }

        file << Item::getCSVHeader() << "\n";
        for (const auto& item : items) {
            file << item->toCSV() << "\n";
        }
        
        file.close();
        std::cout << "Inventory exported to " << filename << " successfully!\n";
        std::cout << "Exported " << items.size() << " items with full dataset.\n";
        return true;
    }

    /**
     * @brief Imports inventory from CSV with dataset support
     */
    bool importFromCSV(const std::string& filename, bool clearExisting = false) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open import file " << filename << std::endl;
            return false;
        }

        if (clearExisting) {
            items.clear();
        }

        std::string line;
        std::getline(file, line); // Skip header
        
        int imported = 0;
        int errors = 0;

        while (std::getline(file, line)) {
            if (!line.empty()) {
                try {
                    auto item = Item::fromCSV(line);
                    items.push_back(std::move(item));
                    imported++;
                } catch (const std::exception& e) {
                    std::cerr << "Warning: Could not import line: " << line 
                              << " - " << e.what() << std::endl;
                    errors++;
                }
            }
        }
        
        file.close();
        saveToFile();

        std::cout << "Import completed!\n";
        std::cout << "Successfully imported: " << imported << " items\n";
        if (errors > 0) {
            std::cout << "Errors encountered: " << errors << " lines\n";
        }
        
        return true;
    }

    /**
     * @brief Gets comprehensive analytics report
     */
    void displayAnalytics() const {
        if (items.empty()) {
            std::cout << "No data available for analytics.\n";
            return;
        }

        std::cout << "\nINVENTORY ANALYTICS REPORT\n";
        std::cout << std::string(50, '=') << "\n";
        
        // Basic stats
        std::cout << "Total Items: " << items.size() << "\n";
        std::cout << "Total Inventory Value: $" << std::fixed << std::setprecision(2) 
                  << getTotalInventoryValue() << "\n";
        std::cout << "Total Potential Profit: $" << getTotalPotentialProfit() << "\n";

        // Category breakdown
        std::map<std::string, int> categoryCount;
        std::map<std::string, double> categoryValue;
        
        for (const auto& item : items) {
            categoryCount[item->getCategory()]++;
            categoryValue[item->getCategory()] += item->getTotalValue();
        }

        std::cout << "\nCATEGORY BREAKDOWN:\n";
        for (const auto& [category, count] : categoryCount) {
            std::cout << "  " << category << ": " << count << " items, $" 
                      << std::fixed << std::setprecision(2) << categoryValue[category] << " value\n";
        }

        // Stock status
        int lowStock = 0, expiring = 0, expired = 0;
        for (const auto& item : items) {
            if (item->isLowStock()) lowStock++;
            if (item->isExpiringSoon()) expiring++;
            if (item->isExpired()) expired++;
        }

        std::cout << "\nSTOCK STATUS:\n";
        std::cout << "  Low Stock Items: " << lowStock << "\n";
        std::cout << "  Expiring Soon: " << expiring << "\n";
        std::cout << "  Expired Items: " << expired << "\n";

        // Top value items
        auto sortedItems = items;
        std::sort(sortedItems.begin(), sortedItems.end(),
                 [](const auto& a, const auto& b) {
                     return a->getTotalValue() > b->getTotalValue();
                 });

        std::cout << "\nTOP 5 HIGHEST VALUE ITEMS:\n";
        for (size_t i = 0; i < std::min(size_t(5), sortedItems.size()); ++i) {
            const auto& item = sortedItems[i];
            std::cout << "  " << (i+1) << ". " << item->getName() 
                      << " - $" << std::fixed << std::setprecision(2) << item->getTotalValue() << "\n";
        }
        
        std::cout << "\n";
    }

private:
    /**
     * @brief Finds item by ID
     */
    Item* findItemById(int id) {
        auto it = std::find_if(items.begin(), items.end(),
                              [id](const std::unique_ptr<Item>& item) {
                                  return item->getId() == id;
                              });
        return (it != items.end()) ? it->get() : nullptr;
    }

    /**
     * @brief Converts string to lowercase
     */
    static std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    /**
     * @brief Formats time for display
     */
    static std::string formatTime(std::time_t time) {
        char buffer[20];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", std::localtime(&time));
        return std::string(buffer);
    }

    /**
     * @brief Saves inventory to file with comprehensive dataset
     */
    void saveToFile() const {
        std::ofstream file(dataFile);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not save to file " << dataFile << std::endl;
            return;
        }

        file << Item::getCSVHeader() << "\n";
        for (const auto& item : items) {
            file << item->toCSV() << "\n";
        }
        file.close();
    }

    /**
     * @brief Loads inventory from file
     */
    void loadFromFile() {
        std::ifstream file(dataFile);
        if (!file.is_open()) {
            return; // File doesn't exist yet, which is fine
        }

        std::string line;
        std::getline(file, line); // Skip header

        while (std::getline(file, line)) {
            if (!line.empty()) {
                try {
                    auto item = Item::fromCSV(line);
                    items.push_back(std::move(item));
                } catch (const std::exception& e) {
                    std::cerr << "Warning: Could not load item from line: " << line << std::endl;
                }
            }
        }
        file.close();
    }
};

/**
 * @brief Utility class for input validation and menu operations
 */
class InputValidator {
public:
    /**
     * @brief Gets a valid integer input from user
     */
    static int getIntInput(const std::string& prompt, int min = INT_MIN, int max = INT_MAX) {
        int value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value && value >= min && value <= max) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            }
            
            std::cout << "Invalid input. Please enter a number";
            if (min != INT_MIN || max != INT_MAX) {
                std::cout << " between " << min << " and " << max;
            }
            std::cout << ".\n";
            
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    /**
     * @brief Gets a valid double input from user
     */
    static double getDoubleInput(const std::string& prompt, double min = 0.0) {
        double value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value && value >= min) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            }
            
            std::cout << "Invalid input. Please enter a positive number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    /**
     * @brief Gets a non-empty string input from user
     */
    static std::string getStringInput(const std::string& prompt) {
        std::string input;
        while (true) {
            std::cout << prompt;
            std::getline(std::cin, input);
            
            if (!input.empty()) {
                return input;
            }
            
            std::cout << "Input cannot be empty. Please try again.\n";
        }
    }

    /**
     * @brief Gets yes/no confirmation from user
     */
    static bool getConfirmation(const std::string& prompt) {
        std::string input;
        while (true) {
            std::cout << prompt << " (y/n): ";
            std::getline(std::cin, input);
            
            if (input == "y" || input == "Y" || input == "yes" || input == "Yes") {
                return true;
            } else if (input == "n" || input == "N" || input == "no" || input == "No") {
                return false;
            }
            
            std::cout << "Please enter 'y' for yes or 'n' for no.\n";
        }
    }
};

/**
 * @brief Professional menu system for the inventory management application
 */
class MenuSystem {
private:
    InventoryManager& inventory;

public:
    explicit MenuSystem(InventoryManager& inv) : inventory(inv) {}

    /**
     * @brief Displays the main menu with comprehensive options
     */
    void displayMainMenu() const {
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "PROFESSIONAL INVENTORY MANAGEMENT SYSTEM v2.0\n";
        std::cout << std::string(60, '=') << "\n";
        std::cout << "Current Stats: " << inventory.getItemCount() << " items | "
                  << "Total Value: $" << std::fixed << std::setprecision(2) 
                  << inventory.getTotalInventoryValue() << " | "
                  << "Potential Profit: $" << inventory.getTotalPotentialProfit() << "\n";
        std::cout << std::string(60, '-') << "\n";
        std::cout << " 1.  Add New Item (Basic)\n";
        std::cout << " 2.  Add New Item (Comprehensive)\n";
        std::cout << " 3.  Display All Items\n";
        std::cout << " 4.  Update Item\n";
        std::cout << " 5.  Delete Item\n";
        std::cout << " 6.  Search Items\n";
        std::cout << " 7.  Adjust Quantity\n";
        std::cout << " 8.  Low Stock Alert\n";
        std::cout << " 9.  View by Category\n";
        std::cout << "10.  Analytics Report\n";
        std::cout << "11.  Export to CSV\n";
        std::cout << "12.  Import from CSV\n";
        std::cout << "13.  Help\n";
        std::cout << " 0.  Exit\n";
        std::cout << std::string(60, '-') << "\n";
    }

    /**
     * @brief Runs the main application loop
     */
    void run() {
        std::cout << "Welcome to Professional Inventory Management System!\n";
        std::cout << "Loading existing inventory data...\n";

        int choice;
        do {
            displayMainMenu();
            choice = InputValidator::getIntInput("Choose an option (0-13): ", 0, 13);
            
            std::cout << "\n";
            handleMenuChoice(choice);
            
            if (choice != 0) {
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
            }
            
        } while (choice != 0);

        std::cout << "Saving inventory data...\n";
        std::cout << "Thank you for using Professional Inventory Management System!\n";
    }

private:
    /**
     * @brief Handles user menu selection with comprehensive options
     */
    void handleMenuChoice(int choice) {
        switch (choice) {
            case 1: handleAddItem(); break;
            case 2: handleAddItemComprehensive(); break;
            case 3: handleDisplayItems(); break;
            case 4: handleUpdateItem(); break;
            case 5: handleDeleteItem(); break;
            case 6: handleSearchItems(); break;
            case 7: handleAdjustQuantity(); break;
            case 8: handleLowStockAlert(); break;
            case 9: handleViewByCategory(); break;
            case 10: handleAnalyticsReport(); break;
            case 11: handleExportCSV(); break;
            case 12: handleImportCSV(); break;
            case 13: handleHelp(); break;
            case 0: break;
            default:
                std::cout << "Invalid option. Please try again.\n";
        }
    }

    void handleAddItem() {
        std::cout << "ADD NEW ITEM (BASIC)\n";
        std::cout << std::string(25, '-') << "\n";

        std::string name = InputValidator::getStringInput("Enter item name: ");
        int quantity = InputValidator::getIntInput("Enter quantity: ", 0);
        double cost = InputValidator::getDoubleInput("Enter cost per unit ($): ");

        if (inventory.addItem(name, quantity, cost)) {
            std::cout << "Item '" << name << "' added successfully!\n";
        } else {
            std::cout << "Failed to add item. Please check your input.\n";
        }
    }

    void handleAddItemComprehensive() {
        std::cout << "ADD NEW ITEM (COMPREHENSIVE)\n";
        std::cout << std::string(35, '-') << "\n";

        std::string name = InputValidator::getStringInput("Enter item name: ");
        std::string category = InputValidator::getStringInput("Enter category: ");
        int quantity = InputValidator::getIntInput("Enter quantity: ", 0);
        double cost = InputValidator::getDoubleInput("Enter cost per unit ($): ");
        double sellingPrice = InputValidator::getDoubleInput("Enter selling price ($): ");
        
        std::cout << "\nOptional Fields (press Enter to skip):\n";
        std::cout << "Supplier: ";
        std::string supplier;
        std::getline(std::cin, supplier);
        
        std::cout << "Location: ";
        std::string location;
        std::getline(std::cin, location);
        
        int minStock = InputValidator::getIntInput("Enter minimum stock level (default 5): ", 0);
        if (minStock == 0) minStock = 5;

        if (inventory.addItem(name, category, quantity, cost, sellingPrice, supplier, location, minStock)) {
            std::cout << "Comprehensive item '" << name << "' added successfully!\n";
        } else {
            std::cout << "Failed to add item. Please check your input.\n";
        }
    }

    void handleDisplayItems() {
        std::cout << "DISPLAY ALL ITEMS\n";
        std::cout << std::string(20, '-') << "\n";
        inventory.displayItems();
    }

    void handleUpdateItem() {
        std::cout << "UPDATE ITEM\n";
        std::cout << std::string(15, '-') << "\n";

        int id = InputValidator::getIntInput("Enter item ID to update: ", 1);
        
        std::cout << "\nWhat would you like to update?\n";
        std::cout << "1. Name only\n";
        std::cout << "2. Quantity only\n";
        std::cout << "3. Cost only\n";
        std::cout << "4. Multiple fields\n";
        
        int updateChoice = InputValidator::getIntInput("Choose update type (1-4): ", 1, 4);
        
        std::string newName = "";
        int newQuantity = -1;
        double newCost = -1.0;

        switch (updateChoice) {
            case 1:
                newName = InputValidator::getStringInput("Enter new name: ");
                break;
            case 2:
                newQuantity = InputValidator::getIntInput("Enter new quantity: ", 0);
                break;
            case 3:
                newCost = InputValidator::getDoubleInput("Enter new cost ($): ");
                break;
            case 4:
                if (InputValidator::getConfirmation("Update name?")) {
                    newName = InputValidator::getStringInput("Enter new name: ");
                }
                if (InputValidator::getConfirmation("Update quantity?")) {
                    newQuantity = InputValidator::getIntInput("Enter new quantity: ", 0);
                }
                if (InputValidator::getConfirmation("Update cost?")) {
                    newCost = InputValidator::getDoubleInput("Enter new cost ($): ");
                }
                break;
        }

        inventory.updateItem(id, newName, newQuantity, newCost);
    }

    void handleDeleteItem() {
        std::cout << "DELETE ITEM\n";
        std::cout << std::string(15, '-') << "\n";

        int id = InputValidator::getIntInput("Enter item ID to delete: ", 1);
        
        if (InputValidator::getConfirmation("Are you sure you want to delete this item?")) {
            inventory.deleteItem(id);
        } else {
            std::cout << "Delete operation cancelled.\n";
        }
    }

    void handleSearchItems() {
        std::cout << "SEARCH ITEMS\n";
        std::cout << std::string(15, '-') << "\n";

        std::string searchTerm = InputValidator::getStringInput(
            "Enter search term (name/category/supplier/barcode): ");
        inventory.searchItems(searchTerm);
    }

    void handleAdjustQuantity() {
        std::cout << "ADJUST QUANTITY\n";
        std::cout << std::string(18, '-') << "\n";

        int id = InputValidator::getIntInput("Enter item ID: ", 1);
        
        std::cout << "Enter quantity adjustment (positive to add, negative to remove): ";
        int delta = InputValidator::getIntInput("", INT_MIN, INT_MAX);

        inventory.adjustQuantity(id, delta);
    }

    void handleLowStockAlert() {
        std::cout << "LOW STOCK ALERT\n";
        std::cout << std::string(18, '-') << "\n";
        inventory.showLowStockItems();
    }

    void handleViewByCategory() {
        std::cout << "VIEW BY CATEGORY\n";
        std::cout << std::string(20, '-') << "\n";

        std::string category = InputValidator::getStringInput("Enter category name: ");
        inventory.displayItemsByCategory(category);
    }

    void handleAnalyticsReport() {
        std::cout << "ANALYTICS REPORT\n";
        std::cout << std::string(20, '-') << "\n";
        inventory.displayAnalytics();
    }

    void handleExportCSV() {
        std::cout << "EXPORT TO CSV\n";
        std::cout << std::string(15, '-') << "\n";

        std::string filename = InputValidator::getStringInput(
            "Enter export filename (e.g., export.csv): ");
        
        if (inventory.exportToCSV(filename)) {
            std::cout << "Export completed successfully!\n";
        }
    }

    void handleImportCSV() {
        std::cout << "IMPORT FROM CSV\n";
        std::cout << std::string(18, '-') << "\n";

        std::string filename = InputValidator::getStringInput(
            "Enter CSV filename to import: ");
        
        bool clearExisting = InputValidator::getConfirmation(
            "Clear existing inventory before import?");

        if (inventory.importFromCSV(filename, clearExisting)) {
            std::cout << "Import completed!\n";
        }
    }

    void handleInventorySummary() {
        std::cout << "INVENTORY SUMMARY\n";
        std::cout << std::string(20, '-') << "\n";
        
        std::cout << "Total Items: " << inventory.getItemCount() << "\n";
        std::cout << "Total Inventory Value: $" << std::fixed << std::setprecision(2) 
                  << inventory.getTotalInventoryValue() << "\n";
        std::cout << "Total Potential Profit: $" << inventory.getTotalPotentialProfit() << "\n";
        
        inventory.showLowStockItems();
    }

    void handleHelp() {
        std::cout << "HELP & INSTRUCTIONS\n";
        std::cout << std::string(35, '=') << "\n";
        std::cout << "BASIC OPERATIONS:\n";
        std::cout << "- Add Item (Basic): Quick item entry with name, quantity, cost\n";
        std::cout << "- Add Item (Comprehensive): Full dataset entry with all fields\n";
        std::cout << "- Display Items: View all items in comprehensive table format\n";
        std::cout << "- Update Item: Modify existing item details\n";
        std::cout << "- Delete Item: Remove items from inventory\n";
        std::cout << "- Search: Find items by name, category, supplier, or barcode\n";
        std::cout << "- Adjust Quantity: Add or remove stock quantities\n";
        std::cout << "\nADVANCED FEATURES:\n";
        std::cout << "- Low Stock Alert: Automated alerts based on minimum stock levels\n";
        std::cout << "- View by Category: Filter items by category\n";
        std::cout << "- Analytics Report: Comprehensive business intelligence\n";
        std::cout << "- Export to CSV: Full dataset export for external analysis\n";
        std::cout << "- Import from CSV: Bulk import with dataset validation\n";
        std::cout << "\nDATASET FIELDS:\n";
        std::cout << "- Basic: ID, Name, Category, Quantity, Cost, Selling Price\n";
        std::cout << "- Extended: Supplier, Barcode, Location, Description\n";
        std::cout << "- Management: Min/Max Stock, Expiry Date, Batch Number\n";
        std::cout << "- Analytics: Profit Margins, Total Values, Date Tracking\n";
        std::cout << "\nTIPS:\n";
        std::cout << "• Data auto-saves to 'inventory_data.csv' with full dataset\n";
        std::cout << "• All inputs are validated for data integrity\n";
        std::cout << "• Use comprehensive add for business-critical items\n";
        std::cout << "• Regular analytics review helps optimize inventory\n";
        std::cout << "• Export/import supports external data analysis tools\n";
    }
};

/**
 * @brief Main application entry point
 */
int main() {
    try {
        // Initialize the inventory manager with persistent storage
        InventoryManager inventory("inventory_data.csv");
        
        // Create and run the menu system
        MenuSystem menu(inventory);
        menu.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Critical Error: " << e.what() << std::endl;
        std::cerr << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    return 0;
}
