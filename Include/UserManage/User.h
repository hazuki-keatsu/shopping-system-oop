/**
 * @file User.h
 * @brief 用户相关类的定义
 * @author Hazuki Keatsu
 * @date 2025-11-15
 */

#ifndef USER_H
#define USER_H

#include <string>
#include <vector>

/**
 * @class User
 * @brief 用户基类，定义用户的基本属性和操作
 * 
 * 该类是抽象基类，为Customer和Admin提供统一接口
 * 包含用户名、密码、手机号等基本信息
 */
class User {
protected:
    std::string username;   // 用户名
    std::string password;   // 密码
    std::string phone;      // 手机号

public:
    /**
     * @brief 构造函数
     * @param username 用户名
     * @param password 密码
     * @param phone 手机号
     */
    User(const std::string& username = "", 
         const std::string& password = "", 
         const std::string& phone = "");
    
    /**
     * @brief 获取用户名
     * @return 用户名
     */
    std::string getUsername() const { return username; }
    
    /**
     * @brief 获取密码
     * @return 密码
     */
    std::string getPassword() const { return password; }
    
    /**
     * @brief 获取手机号
     * @return 手机号
     */
    std::string getPhone() const { return phone; }
    
    /**
     * @brief 设置密码
     * @param newPassword 新密码
     */
    void setPassword(const std::string& newPassword);
    
    /**
     * @brief 验证密码
     * @param pwd 待验证的密码
     * @return 密码正确返回true，否则返回false
     */
    bool verifyPassword(const std::string& pwd) const;
    
    /**
     * @brief 虚析构函数
     */
    virtual ~User();
};

/**
 * @class Customer
 * @brief 顾客类，继承自User
 * 
 * 顾客需要注册后才能登录和购买商品
 */
class Customer : public User {
public:
    /**
     * @brief 构造函数
     * @param username 用户名
     * @param password 密码
     * @param phone 手机号
     */
    Customer(const std::string& username = "",
             const std::string& password = "",
             const std::string& phone = "");
    
    /**
     * @brief 析构函数
     */
    ~Customer();
};

/**
 * @class Admin
 * @brief 管理员类，继承自User
 * 
 * 管理员通过配置文件预设，具有管理用户数据的权限
 */
class Admin : public User {
public:
    /**
     * @brief 构造函数
     * @param username 用户名
     * @param password 密码
     */
    Admin(const std::string& username = "",
          const std::string& password = "");
    
    /**
     * @brief 析构函数
     */
    ~Admin();
};

#endif // USER_H
