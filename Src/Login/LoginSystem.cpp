/**
 * @file LoginSystem.cpp
 * @brief 登录系统类的实现
 * @author Hazuki Keatsu
 * @date 2025-11-15
 */

#include "Login/LoginSystem.h"
#include <iostream>

/**
 * @brief 构造函数实现
 */
LoginSystem::LoginSystem(UserManager* userManager, Config* config)
    : userManager(userManager), 
      config(config),
      currentUserRole(UserRole::NONE),
      currentUser(nullptr) {
}

/**
 * @brief 验证管理员登录
 */
bool LoginSystem::verifyAdmin(const std::string& username, const std::string& password) {
    // 从配置文件获取管理员账号密码
    return (username == config->getAdminUsername() && 
            password == config->getAdminPassword());
}

/**
 * @brief 验证顾客登录
 */
bool LoginSystem::verifyCustomer(const std::string& username, const std::string& password) {
    // 从用户管理器查找顾客
    auto customer = userManager->findCustomer(username);
    if (customer == nullptr) {
        return false;
    }
    
    // 验证密码
    return customer->verifyPassword(password);
}

/**
 * @brief 用户登录
 */
bool LoginSystem::login(const std::string& username, const std::string& password, bool isAdmin) {
    // 检查是否已登录
    if (isLoggedIn()) {
        std::cout << "已有用户登录，请先登出。" << std::endl;
        return false;
    }
    
    // 根据登录类型进行验证
    if (isAdmin) {
        // 管理员登录
        if (verifyAdmin(username, password)) {
            currentUser = std::make_shared<Admin>(username, password);
            currentUserRole = UserRole::ADMIN;
            std::cout << "管理员登录成功！欢迎，" << username << std::endl;
            return true;
        } else {
            std::cout << "管理员登录失败：用户名或密码错误。" << std::endl;
            return false;
        }
    } else {
        // 顾客登录
        if (verifyCustomer(username, password)) {
            auto customer = userManager->findCustomer(username);
            currentUser = customer;
            currentUserRole = UserRole::CUSTOMER;
            std::cout << "顾客登录成功！欢迎，" << username << std::endl;
            return true;
        } else {
            std::cout << "顾客登录失败：用户名或密码错误。" << std::endl;
            return false;
        }
    }
}

/**
 * @brief 顾客注册
 */
bool LoginSystem::registerCustomer(const std::string& username, 
                                   const std::string& password, 
                                   const std::string& phone) {
    // 检查用户名是否已存在
    if (userManager->isUsernameExists(username)) {
        std::cout << "注册失败：用户名已存在。" << std::endl;
        return false;
    }
    
    // 验证输入合法性
    if (username.empty() || password.empty() || phone.empty()) {
        std::cout << "注册失败：用户名、密码和手机号不能为空。" << std::endl;
        return false;
    }
    
    // 创建新顾客对象
    auto newCustomer = std::make_shared<Customer>(username, password, phone);
    
    // 添加到用户管理器
    if (userManager->addCustomer(newCustomer)) {
        std::cout << "注册成功！请使用新账号登录。" << std::endl;
        return true;
    } else {
        std::cout << "注册失败：无法保存用户数据。" << std::endl;
        return false;
    }
}

/**
 * @brief 用户登出
 */
void LoginSystem::logout() {
    if (isLoggedIn()) {
        std::cout << "用户 " << currentUser->getUsername() << " 已登出。" << std::endl;
        currentUser = nullptr;
        currentUserRole = UserRole::NONE;
    }
}

/**
 * @brief 检查是否已登录
 */
bool LoginSystem::isLoggedIn() const {
    return currentUser != nullptr && currentUserRole != UserRole::NONE;
}

/**
 * @brief 修改当前顾客密码
 */
bool LoginSystem::changePassword(const std::string& oldPassword, const std::string& newPassword) {
    // 检查是否已登录
    if (!isLoggedIn()) {
        std::cout << "请先登录。" << std::endl;
        return false;
    }
    
    // 只有顾客可以修改密码
    if (currentUserRole != UserRole::CUSTOMER) {
        std::cout << "只有顾客可以修改密码。" << std::endl;
        return false;
    }
    
    // 验证旧密码
    if (!currentUser->verifyPassword(oldPassword)) {
        std::cout << "修改密码失败：旧密码错误。" << std::endl;
        return false;
    }
    
    // 验证新密码
    if (newPassword.empty()) {
        std::cout << "修改密码失败：新密码不能为空。" << std::endl;
        return false;
    }
    
    // 更新密码
    if (userManager->updatePassword(currentUser->getUsername(), newPassword)) {
        currentUser->setPassword(newPassword);
        std::cout << "密码修改成功！" << std::endl;
        return true;
    } else {
        std::cout << "修改密码失败：无法保存更改。" << std::endl;
        return false;
    }
}

/**
 * @brief 析构函数
 */
LoginSystem::~LoginSystem() {
}
