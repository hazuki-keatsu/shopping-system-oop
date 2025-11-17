/**
 * @file User.cpp
 * @brief 用户相关类的实现
 * @author Hazuki Keatsu
 * @date 2025-11-15
 */

#include "UserManage/User.h"

/**
 * @brief User构造函数实现
 */
User::User(const std::string& username, 
           const std::string& password, 
           const std::string& phone)
    : username(username), password(password), phone(phone) {
}

/**
 * @brief 设置密码
 */
void User::setPassword(const std::string& newPassword) {
    password = newPassword;
}

/**
 * @brief 验证密码
 */
bool User::verifyPassword(const std::string& pwd) const {
    return password == pwd;
}

/**
 * @brief User析构函数
 */
User::~User() {
}

/**
 * @brief Customer构造函数实现
 */
Customer::Customer(const std::string& username,
                   const std::string& password,
                   const std::string& phone)
    : User(username, password, phone) {
}

/**
 * @brief Customer析构函数
 */
Customer::~Customer() {
}

/**
 * @brief Admin构造函数实现
 */
Admin::Admin(const std::string& username,
             const std::string& password)
    : User(username, password, "") {
    // 管理员不需要手机号
}

/**
 * @brief Admin析构函数
 */
Admin::~Admin() {
}
