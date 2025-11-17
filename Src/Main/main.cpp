/**
 * @file main.cpp
 * @brief 购物系统主程序
 * @author Shopping System
 * @date 2025-11-15
 */

#include "Config.h"
#include "UserManage/User.h"
#include "UserManage/UserManager.h"
#include "Login/LoginSystem.h"
#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include <sstream>

/**
 * @brief 清空输入缓冲区
 */
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/**
 * @brief 显示主菜单
 */
void showMainMenu() {
    std::cout << "\n========== 购物系统 ==========" << std::endl;
    std::cout << "1. 顾客注册" << std::endl;
    std::cout << "2. 顾客登录" << std::endl;
    std::cout << "3. 管理员登录" << std::endl;
    std::cout << "4. 查看商品信息（无需登录）" << std::endl;
    std::cout << "0. 退出系统" << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << "请选择: ";
}

/**
 * @brief 显示顾客菜单
 */
void showCustomerMenu() {
    std::cout << "\n===== 顾客菜单 =====" << std::endl;
    std::cout << "1. 查看商品信息" << std::endl;
    std::cout << "2. 修改密码" << std::endl;
    std::cout << "3. 登出" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << "请选择: ";
}

/**
 * @brief 显示管理员菜单
 */
void showAdminMenu() {
    std::cout << "\n===== 管理员菜单 =====" << std::endl;
    std::cout << "1. 查看所有顾客信息" << std::endl;
    std::cout << "2. 登出" << std::endl;
    std::cout << "======================" << std::endl;
    std::cout << "请选择: ";
}

/**
 * @brief 查看商品信息
 * @param itemsFilePath 商品文件路径
 */
void viewItems(const std::string& itemsFilePath) {
    std::ifstream file(itemsFilePath);
    if (!file.is_open()) {
        std::cout << "无法打开商品信息文件。" << std::endl;
        return;
    }
    
    std::cout << "\n===== 商品信息 =====" << std::endl;
    std::string line;
    bool isFirstLine = true;
    int itemCount = 0;
    
    // 读取并解析CSV文件
    while (std::getline(file, line)) {
        if (isFirstLine) {
            // 显示表头
            std::cout << "商品ID\t商品名称\t\t价格(元)\t库存" << std::endl;
            std::cout << "--------------------------------------------------------" << std::endl;
            isFirstLine = false;
            continue;
        }
        
        // 解析CSV行
        std::stringstream ss(line);
        std::string itemId, itemName, price, stock;
        
        std::getline(ss, itemId, ',');
        std::getline(ss, itemName, ',');
        std::getline(ss, price, ',');
        std::getline(ss, stock, ',');
        
        // 格式化输出
        std::cout << itemId << "\t" << itemName;
        
        // 根据商品名称长度调整制表符
        if (itemName.length() < 8) {
            std::cout << "\t\t\t";
        } else if (itemName.length() < 16) {
            std::cout << "\t\t";
        } else if (itemName.length() < 24) {
            std::cout << "\t";
        }
        
        std::cout << price; 

        if (price.length() < 8) {
            std::cout << "\t\t";
        } else if (price.length() < 16) {
            std::cout << "\t";
        }

        std::cout << stock << std::endl;
        itemCount++;
    }
    
    std::cout << "========================================================" << std::endl;
    std::cout << "共 " << itemCount << " 件商品。" << std::endl;
    
    file.close();
}

/**
 * @brief 查看所有顾客信息（管理员功能）
 * @param userManager 用户管理器
 */
void viewAllCustomers(UserManager* userManager) {
    const auto& customers = userManager->getCustomers();
    
    std::cout << "\n===== 所有顾客信息 =====" << std::endl;
    std::cout << "用户名\t\t密码\t\t手机号" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    for (const auto& customer : customers) {
        std::cout << customer->getUsername() << "\t\t"
                  << customer->getPassword() << "\t\t"
                  << customer->getPhone() << std::endl;
    }
    
    std::cout << "========================" << std::endl;
    std::cout << "共 " << customers.size() << " 个顾客。" << std::endl;
}

/**
 * @brief 顾客注册流程
 * @param loginSystem 登录系统
 */
void registerProcess(LoginSystem* loginSystem) {
    std::string username, password, phone;
    
    std::cout << "\n===== 顾客注册 =====" << std::endl;
    std::cout << "请输入用户名: ";
    std::cin >> username;
    
    std::cout << "请输入密码: ";
    std::cin >> password;
    
    std::cout << "请输入手机号: ";
    std::cin >> phone;
    
    loginSystem->registerCustomer(username, password, phone);
}

/**
 * @brief 登录流程
 * @param loginSystem 登录系统
 * @param isAdmin 是否为管理员登录
 */
void loginProcess(LoginSystem* loginSystem, bool isAdmin) {
    std::string username, password;
    
    if (isAdmin) {
        std::cout << "\n===== 管理员登录 =====" << std::endl;
    } else {
        std::cout << "\n===== 顾客登录 =====" << std::endl;
    }
    
    std::cout << "请输入用户名: ";
    std::cin >> username;
    
    std::cout << "请输入密码: ";
    std::cin >> password;
    
    loginSystem->login(username, password, isAdmin);
}

/**
 * @brief 修改密码流程
 * @param loginSystem 登录系统
 */
void changePasswordProcess(LoginSystem* loginSystem) {
    std::string oldPassword, newPassword, confirmPassword;
    
    std::cout << "\n===== 修改密码 =====" << std::endl;
    std::cout << "请输入旧密码: ";
    std::cin >> oldPassword;
    
    std::cout << "请输入新密码: ";
    std::cin >> newPassword;
    
    std::cout << "请确认新密码: ";
    std::cin >> confirmPassword;
    
    if (newPassword != confirmPassword) {
        std::cout << "两次输入的新密码不一致！" << std::endl;
        return;
    }
    
    loginSystem->changePassword(oldPassword, newPassword);
}

/**
 * @brief 主函数
 */
int main() {
    // 初始化配置
    Config* config = Config::getInstance();
    if (!config->loadConfig("config.yaml")) {
        std::cerr << "配置文件加载失败，使用默认配置。" << std::endl;
    }
    
    // 初始化用户管理器
    UserManager userManager(config->getUsersFilePath());
    userManager.loadFromFile();
    
    // 初始化登录系统
    LoginSystem loginSystem(&userManager, config);
    
    // 主循环
    bool running = true;
    while (running) {
        // 根据登录状态显示不同菜单
        if (!loginSystem.isLoggedIn()) {
            // 未登录状态：显示主菜单
            showMainMenu();
            int choice;
            std::cin >> choice;
            
            // 检查输入是否有效
            if (std::cin.fail()) {
                clearInputBuffer();
                std::cout << "无效输入，请输入数字。" << std::endl;
                continue;
            }
            
            switch (choice) {
                case 1:
                    // 顾客注册
                    registerProcess(&loginSystem);
                    break;
                    
                case 2:
                    // 顾客登录
                    loginProcess(&loginSystem, false);
                    break;
                    
                case 3:
                    // 管理员登录
                    loginProcess(&loginSystem, true);
                    break;
                    
                case 4:
                    // 查看商品信息
                    viewItems(config->getItemsFilePath());
                    break;
                    
                case 0:
                    // 退出系统
                    std::cout << "感谢使用购物系统，再见！" << std::endl;
                    running = false;
                    break;
                    
                default:
                    std::cout << "无效选择，请重新输入。" << std::endl;
                    break;
            }
        } else if (loginSystem.getCurrentUserRole() == UserRole::CUSTOMER) {
            // 顾客已登录：显示顾客菜单
            showCustomerMenu();
            int choice;
            std::cin >> choice;
            
            if (std::cin.fail()) {
                clearInputBuffer();
                std::cout << "无效输入，请输入数字。" << std::endl;
                continue;
            }
            
            switch (choice) {
                case 1:
                    // 查看商品信息
                    viewItems(config->getItemsFilePath());
                    break;
                    
                case 2:
                    // 修改密码
                    changePasswordProcess(&loginSystem);
                    break;
                    
                case 3:
                    // 登出
                    loginSystem.logout();
                    break;
                    
                default:
                    std::cout << "无效选择，请重新输入。" << std::endl;
                    break;
            }
        } else if (loginSystem.getCurrentUserRole() == UserRole::ADMIN) {
            // 管理员已登录：显示管理员菜单
            showAdminMenu();
            int choice;
            std::cin >> choice;
            
            if (std::cin.fail()) {
                clearInputBuffer();
                std::cout << "无效输入，请输入数字。" << std::endl;
                continue;
            }
            
            switch (choice) {
                case 1:
                    // 查看所有顾客信息
                    viewAllCustomers(&userManager);
                    break;
                    
                case 2:
                    // 登出
                    loginSystem.logout();
                    break;
                    
                default:
                    std::cout << "无效选择，请重新输入。" << std::endl;
                    break;
            }
        }
    }
    
    return 0;
}
