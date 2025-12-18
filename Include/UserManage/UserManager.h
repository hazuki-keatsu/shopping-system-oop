/**
 * @file UserManager.h
 * @brief 用户管理器类，负责用户数据的持久化和管理
 * @author Hazuki Keatsu
 * @date 2025-11-15
 */

#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include "UserManage/User.h"
#include "Interfaces/DependencyInterfaces.h"
#include <vector>
#include <memory>

/**
 * @class UserManager
 * @brief 用户管理器类，负责用户数据的增删改查和CSV文件操作
 * 
 * 主要功能：
 * 1. 从CSV文件加载用户数据
 * 2. 保存用户数据到CSV文件
 * 3. 添加新用户
 * 4. 查找用户
 * 5. 更新用户信息
 */
class UserManager : public IUserRepository {
private:
    std::vector<std::shared_ptr<Customer>> customers;  // 顾客列表
    std::string filePath;                              // 数据文件路径
    
    /**
     * @brief 解析CSV行数据
     * @param line CSV行字符串
     * @return 解析后的字段向量
     */
    std::vector<std::string> parseCSVLine(const std::string& line);
    
    /**
     * @brief 去除字符串首尾空格
     * @param str 待处理的字符串
     * @return 处理后的字符串
     */
    std::string trim(const std::string& str);

public:
    /**
     * @brief 构造函数
     * @param filePath 用户数据文件路径
     */
    UserManager(const std::string& filePath);
    
    /**
     * @brief 从CSV文件加载用户数据
     * @return 加载成功返回true，否则返回false
     */
    bool loadFromFile() override;
    
    /**
     * @brief 保存用户数据到CSV文件
     * @return 保存成功返回true，否则返回false
     */
    bool saveToFile() override;
    
    /**
     * @brief 添加新顾客
     * @param customer 顾客对象
     * @return 添加成功返回true，否则返回false
     */
    bool addCustomer(std::shared_ptr<Customer> customer) override;
    
    /**
     * @brief 根据用户名查找顾客
     * @param username 用户名
     * @return 找到返回顾客对象指针，否则返回nullptr
     */
    std::shared_ptr<Customer> findCustomer(const std::string& username) override;
    
    /**
     * @brief 检查用户名是否已存在
     * @param username 用户名
     * @return 存在返回true，否则返回false
     */
    bool isUsernameExists(const std::string& username) override;
    
    /**
     * @brief 更新顾客密码
     * @param username 用户名
     * @param newPassword 新密码
     * @return 更新成功返回true，否则返回false
     */
    bool updatePassword(const std::string& username, const std::string& newPassword) override;
    
    /**
     * @brief 获取所有顾客列表
     * @return 顾客列表
     */
    const std::vector<std::shared_ptr<Customer>>& getCustomers() const override { return customers; }
    
    /**
     * @brief 析构函数
     */
    ~UserManager() override;
};

#endif // USER_MANAGER_H
