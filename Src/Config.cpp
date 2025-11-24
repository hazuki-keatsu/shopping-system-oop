/**
 * @file Config.cpp
 * @brief 配置管理类的实现
 * @author Hazuki Keatsu
 * @date 2025-11-15
 */

#include "Config.h"
#include <fstream>
#include <iostream>
#include <sstream>

// 初始化静态成员
Config* Config::instance = nullptr;

/**
 * @brief 私有构造函数的实现
 */
Config::Config() 
    : adminUsername("admin"),
      adminPassword("admin123"),
      usersFilePath("data/users.csv"),
      itemsFilePath("data/items.csv"),
      shoppingCartFilePath("data/shopping_cart.csv") {
    // 设置默认值
}

/**
 * @brief 获取单例实例
 */
Config* Config::getInstance() {
    if (instance == nullptr) {
        instance = new Config();
    }
    return instance;
}

/**
 * @brief 去除字符串首尾空格
 */
std::string Config::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

/**
 * @brief 解析YAML配置文件
 * 
 * 简单的YAML解析实现，支持两级结构：
 * section:
 *   key: value
 */
bool Config::parseConfigFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法打开配置文件: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    std::string currentSection;
    
    // 逐行解析配置文件
    while (std::getline(file, line)) {
        line = trim(line);
        
        // 跳过空行和注释行
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // 识别section（不包含缩进的行）
        if (line.find(':') != std::string::npos && line[0] != ' ') {
            size_t colonPos = line.find(':');
            currentSection = trim(line.substr(0, colonPos));
            continue;
        }
        
        // 解析键值对（包含缩进的行）
        if (line[0] == ' ' && line.find(':') != std::string::npos) {
            size_t colonPos = line.find(':');
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // 根据section和key设置相应的配置值
            if (currentSection == "admin") {
                if (key == "username") {
                    adminUsername = value;
                } else if (key == "password") {
                    adminPassword = value;
                }
            } else if (currentSection == "data_files") {
                if (key == "users") {
                    usersFilePath = value;
                } else if (key == "items") {
                    itemsFilePath = value;
                } else if (key == "shopping_cart") {
                    shoppingCartFilePath = value;
                }
            }
        }
    }
    
    file.close();
    return true;
}

/**
 * @brief 加载配置文件
 */
bool Config::loadConfig(const std::string& filename) {
    return parseConfigFile(filename);
}

/**
 * @brief 析构函数
 */
Config::~Config() {
    // 清理资源
}
