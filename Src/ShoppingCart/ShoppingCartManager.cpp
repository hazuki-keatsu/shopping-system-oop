/**
 * @file ShoppingCartManager.cpp
 * @brief 购物车管理器类的实现
 * @author Hazuki Keatsu
 * @date 2025-11-24
 */

#include "ShoppingCart/ShoppingCartManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

/**
 * @brief 构造函数实现
 */
ShoppingCartManager::ShoppingCartManager(const std::string& filePath, 
                                         std::shared_ptr<IItemRepository> itemMgr)
    : filePath(filePath), itemManager(itemMgr) {
}

/**
 * @brief 去除字符串首尾空格
 */
std::string ShoppingCartManager::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

/**
 * @brief 解析数组字符串（如"[1,2,3]"）为vector
 * 
 * 支持格式：[1,2,3] 或 "[1,2,3]"
 */
std::vector<int> ShoppingCartManager::parseArrayString(const std::string& arrayStr) {
    std::vector<int> result;
    std::string str = trim(arrayStr);
    
    // 去除引号
    if (!str.empty() && str.front() == '"') {
        str = str.substr(1);
    }
    if (!str.empty() && str.back() == '"') {
        str = str.substr(0, str.length() - 1);
    }
    
    // 去除方括号
    if (str.empty() || str.front() != '[' || str.back() != ']') {
        return result;  // 格式不正确，返回空向量
    }
    
    str = str.substr(1, str.length() - 2);  // 去掉 [ 和 ]
    
    // 如果是空数组
    if (trim(str).empty()) {
        return result;
    }
    
    // 按逗号分割并解析每个数字
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        item = trim(item);
        if (!item.empty()) {
            try {
                result.push_back(std::stoi(item));
            } catch (const std::exception& e) {
                std::cerr << "解析数字失败: " << item << std::endl;
            }
        }
    }
    
    return result;
}

/**
 * @brief 将整数向量转换为数组字符串（如"[1,2,3]"）
 */
std::string ShoppingCartManager::vectorToArrayString(const std::vector<int>& vec) {
    if (vec.empty()) {
        return "[]";
    }
    
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        ss << vec[i];
        if (i < vec.size() - 1) {
            ss << ",";
        }
    }
    ss << "]";
    
    return ss.str();
}

/**
 * @brief 从CSV文件加载购物车数据
 * 
 * CSV文件格式：
 * username,item_ids,quantities
 * user1,"[1,2,3]","[2,1,5]"
 */
bool ShoppingCartManager::loadFromFile() {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "购物车数据文件不存在，将创建新文件。" << std::endl;
        return true;
    }
    
    // 清空现有数据
    carts.clear();
    
    std::string line;
    bool isFirstLine = true;
    
    // 逐行读取文件
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        
        // 跳过表头
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }
        
        // 解析CSV行：username,item_ids,quantities
        std::stringstream ss(line);
        std::string username, itemIdsStr, quantitiesStr;
        
        // 读取用户名（第一个字段）
        if (!std::getline(ss, username, ',')) {
            continue;
        }
        username = trim(username);
        
        // 读取商品ID数组（第二个字段，可能包含引号）
        if (!std::getline(ss, itemIdsStr, ',')) {
            continue;
        }
        
        // 读取数量数组（第三个字段，可能包含引号）
        // 注意：如果字段包含引号，需要特殊处理
        std::getline(ss, quantitiesStr);
        
        // 解析商品ID数组和数量数组
        std::vector<int> itemIds = parseArrayString(itemIdsStr);
        std::vector<int> quantities = parseArrayString(quantitiesStr);
        
        // 检查两个数组长度是否一致
        if (itemIds.size() != quantities.size()) {
            std::cerr << "警告：用户 " << username << " 的购物车数据不一致，跳过。" << std::endl;
            continue;
        }
        
        // 创建购物车并添加商品
        if (!itemManager) {
            std::cerr << "错误：商品管理器未初始化！" << std::endl;
            file.close();
            return false;
        }
        
        // 创建一个临时的Customer对象（注意：这里只用于购物车，不是完整的用户对象）
        auto customer = std::make_shared<Customer>(username, "", "");
        auto cart = std::make_shared<ShoppingCart>(customer);
        
        // 添加商品到购物车
        for (size_t i = 0; i < itemIds.size(); ++i) {
            std::string itemId = std::to_string(itemIds[i]);
            auto item = itemManager->findItemById(itemId);
            
            if (item) {
                // 直接添加商品（不进行重复检查）
                cart->addItemDirect(item, quantities[i]);
            } else {
                std::cerr << "警告：商品ID " << itemId << " 不存在，跳过。" << std::endl;
            }
        }
        
        // 将购物车添加到管理器
        carts[username] = cart;
    }
    
    file.close();
    
    std::cout << "成功加载 " << carts.size() << " 个购物车数据。" << std::endl;
    return true;
}

/**
 * @brief 将购物车数据保存到CSV文件
 */
bool ShoppingCartManager::saveToFile() {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "无法打开文件进行写入: " << filePath << std::endl;
        return false;
    }
    
    // 写入表头
    file << "username,item_ids,quantities" << std::endl;
    
    // 写入每个用户的购物车数据
    for (const auto& pair : carts) {
        const std::string& username = pair.first;
        const auto& cart = pair.second;
        
        // 获取购物车中的商品
        const auto& items = cart->getCartItems();
        
        if (items.empty()) {
            // 空购物车也要保存
            file << username << ",\"[]\",\"[]\"" << std::endl;
            continue;
        }
        
        // 构建商品ID数组和数量数组
        std::vector<int> itemIds;
        std::vector<int> quantities;
        
        for (const auto& itemPair : items) {
            itemIds.push_back(std::stoi(itemPair.first->getItemId()));
            quantities.push_back(itemPair.second);
        }
        
        // 转换为字符串
        std::string itemIdsStr = vectorToArrayString(itemIds);
        std::string quantitiesStr = vectorToArrayString(quantities);
        
        // 写入文件（用引号包围数组字符串）
        file << username << ",\"" << itemIdsStr << "\",\"" << quantitiesStr << "\"" << std::endl;
    }
    
    file.close();
    std::cout << "购物车数据已保存到文件。" << std::endl;
    return true;
}

/**
 * @brief 获取指定用户的购物车
 * 
 * 如果用户购物车不存在，则创建一个新的空购物车
 */
std::shared_ptr<ShoppingCart> ShoppingCartManager::getCart(const std::string& username, 
                                                            std::shared_ptr<Customer> customer) {
    // 查找是否已存在购物车
    auto it = carts.find(username);
    
    if (it != carts.end()) {
        return it->second;
    } else {
        // 创建新的购物车
        auto newCart = std::make_shared<ShoppingCart>(customer);
        carts[username] = newCart;
        return newCart;
    }
}

/**
 * @brief 检查指定用户是否有购物车
 */
bool ShoppingCartManager::hasCart(const std::string& username) const {
    return carts.find(username) != carts.end();
}

/**
 * @brief 删除指定用户的购物车
 */
bool ShoppingCartManager::removeCart(const std::string& username) {
    auto it = carts.find(username);
    
    if (it != carts.end()) {
        carts.erase(it);
        std::cout << "已删除用户 " << username << " 的购物车。" << std::endl;
        return true;
    } else {
        std::cout << "用户 " << username << " 没有购物车。" << std::endl;
        return false;
    }
}

/**
 * @brief 清空所有购物车
 */
void ShoppingCartManager::clearAllCarts() {
    carts.clear();
    std::cout << "已清空所有购物车。" << std::endl;
}

/**
 * @brief 析构函数
 */
ShoppingCartManager::~ShoppingCartManager() {
    // 清理资源
}
