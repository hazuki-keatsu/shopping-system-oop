/**
 * @file ItemManager.cpp
 * @brief 商品管理器类的实现
 * @author Hazuki Keatsu
 * @date 2025-11-17
 */

#include "ItemManage/ItemManager.h"
#include "Promotion/PromotionManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iomanip>

/**
 * @brief 构造函数实现
 */
ItemManager::ItemManager(const std::string& filePath)
    : filePath(filePath) {
}

/**
 * @brief 去除字符串首尾空格
 */
std::string ItemManager::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

/**
 * @brief 解析CSV行数据
 */
std::vector<std::string> ItemManager::parseCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    
    // 按逗号分割字段
    while (std::getline(ss, field, ',')) {
        fields.push_back(trim(field));
    }
    
    return fields;
}

/**
 * @brief 从CSV文件加载商品数据
 */
bool ItemManager::loadFromFile() {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "商品数据文件不存在，将创建新文件。" << std::endl;
        return true;
    }
    
    std::string line;
    bool isFirstLine = true;
    
    // 清空现有数据
    items.clear();
    categoryIndex.clear();
    headers.clear();
    
    // 逐行读取文件
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        
        // 读取表头
        if (isFirstLine) {
            headers = parseCSVLine(line);
            isFirstLine = false;
            continue;
        }
        
        // 解析数据行
        std::vector<std::string> fields = parseCSVLine(line);
        if (fields.size() < 6) {
            continue;  // 至少需要基本的6个字段
        }
        
        // 创建Item对象（假设前6个字段为：id, name, category, price, description, stock）
        auto item = std::make_shared<Item>(
            fields[0],                          // item_id
            fields[1],                          // item_name
            fields[2],                          // category
            std::stod(fields[3]),              // price
            fields[4],                          // description
            std::stoi(fields[5])               // stock
        );
        
        items.push_back(item);
    }
    
    file.close();
    
    // 重建类别索引
    rebuildCategoryIndex();
    
    std::cout << "成功加载 " << items.size() << " 个商品数据。" << std::endl;
    return true;
}

/**
 * @brief 保存商品数据到CSV文件
 */
bool ItemManager::saveToFile() {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "无法打开文件进行写入: " << filePath << std::endl;
        return false;
    }
    
    // 写入表头
    if (headers.empty()) {
        // 默认表头
        file << "item_id,item_name,category,price,description,stock" << std::endl;
    } else {
        for (size_t i = 0; i < headers.size(); ++i) {
            file << headers[i];
            if (i < headers.size() - 1) {
                file << ",";
            }
        }
        file << std::endl;
    }
    
    // 写入每个商品的数据
    for (const auto& item : items) {
        file << item->getItemId() << ","
             << item->getItemName() << ","
             << item->getCategory() << ","
             << item->getPrice() << ","
             << item->getDescription() << ","
             << item->getStock();
        
        file << std::endl;
    }
    
    file.close();
    return true;
}

/**
 * @brief 重建类别索引
 */
void ItemManager::rebuildCategoryIndex() {
    categoryIndex.clear();
    
    for (const auto& item : items) {
        categoryIndex[item->getCategory()].push_back(item);
    }
}

/**
 * @brief 生成新的商品ID
 */
std::string ItemManager::generateNewItemId() {
    int maxId = 0;
    
    // 找到当前最大的ID
    for (const auto& item : items) {
        try {
            int id = std::stoi(item->getItemId());
            if (id > maxId) {
                maxId = id;
            }
        } catch (...) {
            // 忽略非数字ID
        }
    }
    
    // 返回下一个ID
    return std::to_string(maxId + 1);
}

/**
 * @brief 添加新商品
 */
bool ItemManager::addItem(std::shared_ptr<Item> item) {
    // 检查ID是否已存在
    if (isItemIdExists(item->getItemId())) {
        return false;
    }
    
    // 添加到列表
    items.push_back(item);
    
    // 更新类别索引
    categoryIndex[item->getCategory()].push_back(item);
    
    // 保存到文件
    return saveToFile();
}

/**
 * @brief 根据ID删除商品
 */
bool ItemManager::deleteItem(const std::string& itemId) {
    // 查找并删除商品
    auto it = std::find_if(items.begin(), items.end(),
        [&itemId](const std::shared_ptr<Item>& item) {
            return item->getItemId() == itemId;
        });
    
    if (it != items.end()) {
        items.erase(it);
        
        // 重建类别索引
        rebuildCategoryIndex();
        
        // 保存到文件
        return saveToFile();
    }
    
    return false;
}

/**
 * @brief 根据ID查找商品
 */
std::shared_ptr<Item> ItemManager::findItemById(const std::string& itemId) {
    auto it = std::find_if(items.begin(), items.end(),
        [&itemId](const std::shared_ptr<Item>& item) {
            return item->getItemId() == itemId;
        });
    
    if (it != items.end()) {
        return *it;
    }
    
    return nullptr;
}

/**
 * @brief 根据类别获取商品列表
 */
std::vector<std::shared_ptr<Item>> ItemManager::getItemsByCategory(const std::string& category) {
    auto it = categoryIndex.find(category);
    if (it != categoryIndex.end()) {
        return it->second;
    }
    return std::vector<std::shared_ptr<Item>>();
}

/**
 * @brief 获取所有类别
 */
std::vector<std::string> ItemManager::getAllCategories() const {
    std::vector<std::string> categories;
    for (const auto& pair : categoryIndex) {
        categories.push_back(pair.first);
    }
    return categories;
}

/**
 * @brief 显示所有商品信息（表格形式）
 * 
 * 如果提供了 PromotionManager，会在商品名称后显示促销标签
 */
void ItemManager::displayAllItems(PromotionManager* promotionManager) const {
    if (items.empty()) {
        std::cout << "暂无商品信息。" << std::endl;
        return;
    }
    
    std::cout << "\n========== 商品列表 ==========" << std::endl;
    std::cout << std::left
              << std::setw(8) << "ID"
              << std::setw(25) << "名称"
              << std::setw(12) << "类别"
              << std::setw(10) << "价格"
              << std::setw(30) << "描述"
              << std::setw(8) << "库存"
              << std::endl;
    std::cout << "-------------------------------------------------------------------------------------" << std::endl;
    
    for (const auto& item : items) {
        // 构建商品名称（包含促销标签）
        std::string itemNameWithTag = item->getItemName();
        
        // 如果提供了促销管理器，检查是否有促销活动
        if (promotionManager != nullptr) {
            auto discount = promotionManager->getActiveDiscountForItem(item->getItemId());
            if (discount != nullptr) {
                itemNameWithTag += " [" + discount->getDisplayTag() + "]";
            }
        }
        
        std::cout << std::left
                  << std::setw(8) << item->getItemId()
                  << std::setw(25) << itemNameWithTag
                  << std::setw(12) << item->getCategory()
                  << std::setw(10) << std::fixed << std::setprecision(2) << item->getPrice()
                  << std::setw(30) << item->getDescription()
                  << std::setw(8) << item->getStock()
                  << std::endl;
    }
    
    std::cout << "=============================" << std::endl;
    std::cout << "共 " << items.size() << " 件商品。" << std::endl;
    
    // 显示当前有效的满减活动
    if (promotionManager != nullptr) {
        auto fullReductions = promotionManager->getActiveFullReductions();
        if (!fullReductions.empty()) {
            std::cout << "\n【当前满减活动】：";
            for (size_t i = 0; i < fullReductions.size(); ++i) {
                if (i > 0) std::cout << "、";
                std::cout << fullReductions[i]->getDisplayTag();
            }
            std::cout << std::endl;
        }
    }
}

/**
 * @brief 检查商品ID是否存在
 */
bool ItemManager::isItemIdExists(const std::string& itemId) const {
    return std::find_if(items.begin(), items.end(),
        [&itemId](const std::shared_ptr<Item>& item) {
            return item->getItemId() == itemId;
        }) != items.end();
}

/**
 * @brief 析构函数
 */
ItemManager::~ItemManager() {
}
