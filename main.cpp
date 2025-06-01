#include <iostream>
#include <unordered_map>
#include <map>
#include <memory>
#include <stdexcept>

class DuplicateItemException : public std::runtime_error {
public:
    DuplicateItemException(const std::string& msg) : std::runtime_error(msg) {}
};

class ItemNotFoundException : public std::runtime_error {
public:
    ItemNotFoundException(const std::string& msg) : std::runtime_error(msg) {}
};

class StoredItem {
private:
    std::string id;
    std::string description;
    std::string location;

public:
    StoredItem(std::string id, std::string desc, std::string loc)
        : id(id), description(desc), location(loc) {}

    std::string getId() const { return id; }
    std::string getDescription() const { return description; }
    std::string getLocation() const { return location; }
};

class StorageManager {
private:
    std::unordered_map<std::string, std::shared_ptr<StoredItem>> itemById;
    std::map<std::string, std::shared_ptr<StoredItem>> itemByDescription;

public:
    void addItem(const std::shared_ptr<StoredItem>& item) {
        if (itemById.find(item->getId()) != itemById.end()) {
            throw DuplicateItemException("Item with ID " + item->getId() + " already exists!");
        }
        itemById[item->getId()] = item;
        itemByDescription[item->getDescription()] = item;
    }

    std::shared_ptr<StoredItem> findById(const std::string& id) const {
        auto it = itemById.find(id);
        if (it == itemById.end()) {
            throw ItemNotFoundException("Item with ID " + id + " not found!");
        }
        return it->second;
    }

    void removeItem(const std::string& id) {
        auto it = itemById.find(id);
        if (it == itemById.end()) {
            throw ItemNotFoundException("Item with ID " + id + " not found!");
        }
        
        std::string description = it->second->getDescription();
        itemById.erase(it);
        itemByDescription.erase(description);
    }

    void listItemsByDescription() const {
        std::cout << "Items in Description Order:" << std::endl;
        for (const auto& pair : itemByDescription) {
            std::cout << "- " << pair.second->getDescription() 
                      << ": " << pair.second->getLocation() << std::endl;
        }
    }
};

void testDuplicateAddition() {
    StorageManager manager;
    auto item1 = std::make_shared<StoredItem>("TEST001", "Test Item", "Test Location");
    auto item2 = std::make_shared<StoredItem>("TEST001", "Another Test Item", "Another Location");
    
    manager.addItem(item1);
    try {
        manager.addItem(item2);
        std::cout << "Test Failed: Should have thrown DuplicateItemException" << std::endl;
    } catch (const DuplicateItemException& e) {
        std::cout << "Test Passed: Caught duplicate item exception" << std::endl;
    }
}

void testItemNotFound() {
    StorageManager manager;
    try {
        manager.findById("NONEXISTENT");
        std::cout << "Test Failed: Should have thrown ItemNotFoundException" << std::endl;
    } catch (const ItemNotFoundException& e) {
        std::cout << "Test Passed: Caught item not found exception" << std::endl;
    }
}

int main() {
    StorageManager manager;
    
    auto item1 = std::make_shared<StoredItem>("ITEM001", "LED Light", "Aisle 3, Shelf 1");
    auto item2 = std::make_shared<StoredItem>("ITEM002", "Fan Motor", "Aisle 2, Shelf 5");
    
    std::cout << "Adding item: " << item1->getId() << " - " << item1->getDescription() << std::endl;
    manager.addItem(item1);
    
    std::cout << "Adding item: " << item2->getId() << " - " << item2->getDescription() << std::endl;
    manager.addItem(item2);
    
    std::cout << "Attempting to add " << item1->getId() << " again..." << std::endl;
    try {
        manager.addItem(item1);
    } catch (const DuplicateItemException& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    
    std::cout << "Retrieving " << item2->getId() << "..." << std::endl;
    try {
        auto found = manager.findById("ITEM002");
        std::cout << "Found: " << found->getDescription() << " at " << found->getLocation() << std::endl;
    } catch (const ItemNotFoundException& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    
    std::cout << "Removing ITEM003..." << std::endl;
    try {
        manager.removeItem("ITEM003");
    } catch (const ItemNotFoundException& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    
    manager.listItemsByDescription();
    
    return 0;
}
