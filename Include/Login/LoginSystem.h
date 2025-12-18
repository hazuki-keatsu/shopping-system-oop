/**
 * @file LoginSystem.h
 * @brief 登录系统类，处理用户登录、注册等认证功能
 * @author Hazuki Keatsu
 * @date 2025-11-15
 */

#ifndef LOGIN_SYSTEM_H
#define LOGIN_SYSTEM_H

#include "UserManage/User.h"
#include "Interfaces/DependencyInterfaces.h"
#include "Config.h"
#include <memory>
#include <string>

/**
 * @enum UserRole
 * @brief 用户角色枚举
 */
enum class UserRole {
    NONE,       // 未登录
    CUSTOMER,   // 顾客
    ADMIN       // 管理员
};

/**
 * @class LoginSystem
 * @brief 登录系统类，负责用户认证和会话管理
 * 
 * 主要功能：
 * 1. 管理员登录
 * 2. 顾客登录
 * 3. 顾客注册
 * 4. 会话管理
 */
class LoginSystem {
private:
    IUserRepository* userManager;       // 用户管理器
    IConfigProvider* config;            // 配置管理器
    
    UserRole currentUserRole;           // 当前登录用户角色
    std::shared_ptr<User> currentUser;  // 当前登录用户对象
    
    /**
     * @brief 验证管理员登录
     * @param username 用户名
     * @param password 密码
     * @return 验证成功返回true，否则返回false
     */
    bool verifyAdmin(const std::string& username, const std::string& password);
    
    /**
     * @brief 验证顾客登录
     * @param username 用户名
     * @param password 密码
     * @return 验证成功返回true，否则返回false
     */
    bool verifyCustomer(const std::string& username, const std::string& password);

public:
    /**
     * @brief 构造函数
     * @param userManager 用户管理器指针
     * @param config 配置管理器指针
     */
    LoginSystem(IUserRepository* userManager, IConfigProvider* config);
    
    /**
     * @brief 用户登录
     * @param username 用户名
     * @param password 密码
     * @param isAdmin 是否以管理员身份登录
     * @return 登录成功返回true，否则返回false
     */
    bool login(const std::string& username, const std::string& password, bool isAdmin);
    
    /**
     * @brief 顾客注册
     * @param username 用户名
     * @param password 密码
     * @param phone 手机号
     * @return 注册成功返回true，否则返回false
     */
    bool registerCustomer(const std::string& username, const std::string& password, const std::string& phone);
    
    /**
     * @brief 用户登出
     */
    void logout();
    
    /**
     * @brief 检查是否已登录
     * @return 已登录返回true，否则返回false
     */
    bool isLoggedIn() const;
    
    /**
     * @brief 获取当前用户角色
     * @return 用户角色
     */
    UserRole getCurrentUserRole() const { return currentUserRole; }
    
    /**
     * @brief 获取当前用户对象
     * @return 用户对象指针
     */
    std::shared_ptr<User> getCurrentUser() const { return currentUser; }
    
    /**
     * @brief 修改当前顾客密码
     * @param oldPassword 旧密码
     * @param newPassword 新密码
     * @return 修改成功返回true，否则返回false
     */
    bool changePassword(const std::string& oldPassword, const std::string& newPassword);
    
    /**
     * @brief 析构函数
     */
    ~LoginSystem();
};

#endif // LOGIN_SYSTEM_H
