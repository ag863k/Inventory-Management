# Inventory Management System

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows%7CLinux%7CmacOS-lightgrey.svg)

Professional console-based inventory management application built with modern C++17. Provides comprehensive dataset management, analytics, CSV operations, and business reporting capabilities.

## Features

- **Dual Entry Modes**: Basic and comprehensive item creation
- **Multi-Field Operations**: Search, filter, and manage across 12+ data fields
- **Financial Analytics**: Real-time profit calculations and inventory valuation
- **CSV Integration**: Full import/export capabilities with external systems
- **Category Management**: Organize and filter items by business categories
- **Stock Monitoring**: Automated low stock alerts and reorder recommendations
- **Data Validation**: Comprehensive input validation and error handling
- **Cross-Platform**: Windows, Linux, and macOS support

## Technical Specifications

**Requirements**
- C++17 or higher
- CMake 3.16+
- Standard Library only (no external dependencies)

**Architecture**
- Modern C++ design patterns (RAII, Smart Pointers)
- Separation of concerns architecture
- Comprehensive input validation system
- Memory-safe object management

## Installation

```bash
# Clone repository
git clone https://github.com/username/inventory-management-system.git
cd inventory-management-system

# Build with CMake
mkdir build && cd build
cmake ..
cmake --build . --config Release

# Run application
./bin/InventoryManager
```

## Data Schema

**Item Fields**
- ID, Name, Category, Quantity
- Cost Price, Selling Price, Supplier
- Barcode, Minimum Stock, Location
- Description, Expiry Date, Timestamps

**CSV Format**
```
ID,Name,Category,Quantity,CostPrice,SellingPrice,Supplier,Barcode,MinStock,Location,Description,ExpiryDate
1,Laptop,Electronics,15,899.99,1299.99,TechCorp,123456789,5,WH-A-01,Business laptop,2025-12-31
```

## Operations

**Menu Options**
1. Add Item (Basic/Comprehensive)
2. Display/Update/Delete Items
3. Search and Category Filtering
4. Stock Management and Alerts
5. Analytics and Reporting
6. CSV Import/Export
7. Help System

**Analytics Dashboard**
- Total inventory value and profit margins
- Low stock alerts and reorder recommendations
- Category breakdown and supplier analysis
- Performance metrics and statistics

## License

MIT License. See [LICENSE](LICENSE) file for details.

## Contributing

1. Fork the repository
2. Create feature branch (`git checkout -b feature/name`)
3. Commit changes (`git commit -m 'Add feature'`)
4. Push to branch (`git push origin feature/name`)
5. Open Pull Request

---

**Professional Inventory Management System** - Built with modern C++17

