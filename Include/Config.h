/**
 * @file Config.h
 * @brief 配置管理类，用于读取和管理系统配置信息
 * @author Hazuki Keatsu
 * @date 2025-11-15
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include "Interfaces/DependencyInterfaces.h"

/**
 * @class Config
 * @brief 配置管理类，负责读取和解析config.yaml文件
 * 
 * 该类采用单例模式，提供全局唯一的配置访问点
 * 主要功能：
 * 1. 读取YAML配置文件
 * 2. 提供管理员账户信息
 * 3. 提供数据文件路径
 */
class Config : public IConfigProvider {
private:
    std::string adminUsername;      // 管理员用户名
    std::string adminPassword;      // 管理员密码
    std::string usersFilePath;      // 用户数据文件路径
    std::string itemsFilePath;      // 商品数据文件路径
    std::string shoppingCartFilePath; // 购物车数据文件路径
    std::string ordersFilePath;     // 订单数据文件路径
    std::string promotionsFilePath; // 促销数据文件路径
    
    // 自动更新时间配置
    bool autoUpdateEnabled;         // 是否开启自动更新
    int pendingToShippedSeconds;    // 待发货到已发货的秒数
    int shippedToDeliveredSeconds;  // 已发货到已签收的秒数

    static Config* instance;        // 单例实例指针
    
    /**
     * @brief 私有构造函数，防止外部实例化
     */
    Config();
    
    /**
     * @brief 解析YAML配置文件
     * @param filename 配置文件路径
     * @return 解析成功返回true，否则返回false
     */
    bool parseConfigFile(const std::string& filename);
    
    /**
     * @brief 去除字符串首尾空格
     * @param str 待处理的字符串
     * @return 处理后的字符串
     */
    std::string trim(const std::string& str);

public:
    /**
     * @brief 获取Config单例实例
     * @return Config实例指针
     */
    static Config* getInstance();
    
    /**
     * @brief 加载配置文件
     * @param filename 配置文件路径
     * @return 加载成功返回true，否则返回false
     */
    bool loadConfig(const std::string& filename = "config.yaml");
    
    /**
     * @brief 获取管理员用户名
     * @return 管理员用户名
     */
    std::string getAdminUsername() const override { return adminUsername; }
    
    /**
     * @brief 获取管理员密码
     * @return 管理员密码
     */
    std::string getAdminPassword() const override { return adminPassword; }
    
    /**
     * @brief 获取用户数据文件路径
     * @return 用户数据文件路径
     */
    std::string getUsersFilePath() const { return usersFilePath; }
    
    /**
     * @brief 获取商品数据文件路径
     * @return 商品数据文件路径
     */
    std::string getItemsFilePath() const { return itemsFilePath; }
    
    /**
     * @brief 获取购物车数据文件路径
     * @return 购物车数据文件路径
     */
    std::string getShoppingCartFilePath() const { return shoppingCartFilePath; }
    
    /**
     * @brief 获取订单数据文件路径
     * @return 订单数据文件路径
     */
    std::string getOrdersFilePath() const { return ordersFilePath; }
    
    /**
     * @brief 获取促销数据文件路径
     * @return 促销数据文件路径
     */
    std::string getPromotionsFilePath() const { return promotionsFilePath; }

    /**
     * @brief 获取是否开启自动更新
     * @return true开启，false关闭
     */
    bool isAutoUpdateEnabled() const { return autoUpdateEnabled; }

    /**
     * @brief 获取待发货到已发货的秒数
     * @return 秒数
     */
    int getPendingToShippedSeconds() const { return pendingToShippedSeconds; }

    /**
     * @brief 获取已发货到已签收的秒数
     * @return 秒数
     */
    int getShippedToDeliveredSeconds() const { return shippedToDeliveredSeconds; }
    
    /**
     * @brief 析构函数
     */
    ~Config();
};

#endif // CONFIG_H
