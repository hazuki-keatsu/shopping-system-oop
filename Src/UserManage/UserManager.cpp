/**
 * @file UserManager.cpp
 * @brief 用户管理器类的实现
 * @author Hazuki Keatsu
 * @date 2025-11-15
 */

#include "UserManage/UserManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

/**
 * @brief 构造函数实现
 */
UserManager::UserManager(const std::string& filePath)
    : filePath(filePath) {
}

/**
 * @brief 去除字符串首尾空格
 */
std::string UserManager::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

/**
 * @brief 解析CSV行数据
 * 
 * 简单的CSV解析，使用逗号作为分隔符
 */
std::vector<std::string> UserManager::parseCSVLine(const std::string& line) {
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
 * @brief 从CSV文件加载用户数据
 * 
 * CSV格式：username,password,phone
 */
bool UserManager::loadFromFile() {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        // 文件不存在时不报错，创建空列表
        std::cout << "用户数据文件不存在，将创建新文件。" << std::endl;
        return true;
    }
    
    std::string line;
    bool isFirstLine = true;
    
    // 清空现有数据
    customers.clear();
    
    // 逐行读取文件
    while (std::getline(file, line)) {
        // 跳过标题行
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }
        
        // 跳过空行
        if (line.empty()) {
            continue;
        }
        
        // 解析CSV行
        std::vector<std::string> fields = parseCSVLine(line);
        if (fields.size() >= 3) {
            // 创建Customer对象并添加到列表
            auto customer = std::make_shared<Customer>(fields[0], fields[1], fields[2]);
            customers.push_back(customer);
        }
    }
    
    file.close();
    std::cout << "成功加载 " << customers.size() << " 个用户数据。" << std::endl;
    return true;
}

/**
 * @brief 保存用户数据到CSV文件
 */
bool UserManager::saveToFile() {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "无法打开文件进行写入: " << filePath << std::endl;
        return false;
    }
    
    // 写入标题行
    file << "username,password,phone" << std::endl;
    
    // 写入每个顾客的数据
    for (const auto& customer : customers) {
        file << customer->getUsername() << ","
             << customer->getPassword() << ","
             << customer->getPhone() << std::endl;
    }
    
    file.close();
    return true;
}

/**
 * @brief 添加新顾客
 */
bool UserManager::addCustomer(std::shared_ptr<Customer> customer) {
    // 检查用户名是否已存在
    if (isUsernameExists(customer->getUsername())) {
        return false;
    }
    
    // 添加到列表
    customers.push_back(customer);
    
    // 保存到文件
    return saveToFile();
}

/**
 * @brief 根据用户名查找顾客
 */
std::shared_ptr<Customer> UserManager::findCustomer(const std::string& username) {
    // 使用lambda表达式查找用户
    auto it = std::find_if(customers.begin(), customers.end(),
        [&username](const std::shared_ptr<Customer>& c) {
            return c->getUsername() == username;
        });
    
    if (it != customers.end()) {
        return *it;
    }
    
    return nullptr;
}

/**
 * @brief 检查用户名是否已存在
 */
bool UserManager::isUsernameExists(const std::string& username) {
    return findCustomer(username) != nullptr;
}

/**
 * @brief 更新顾客密码
 */
bool UserManager::updatePassword(const std::string& username, const std::string& newPassword) {
    // 查找用户
    auto customer = findCustomer(username);
    if (customer == nullptr) {
        return false;
    }
    
    // 更新密码
    customer->setPassword(newPassword);
    
    // 保存到文件
    return saveToFile();
}

/**
 * @brief 析构函数
 */
UserManager::~UserManager() {
}
