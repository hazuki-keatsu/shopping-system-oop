/**
 * @file ShoppingCartManager.h
 * @brief 购物车管理器类的定义
 * @author Hazuki Keatsu
 * @date 2025-11-24
 */

#ifndef SHOPPING_CART_MANAGER_H
#define SHOPPING_CART_MANAGER_H

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "ShoppingCart/ShoppingCart.h"
#include "Interfaces/DependencyInterfaces.h"

/**
 * @class ShoppingCartManager
 * @brief 购物车管理器类，管理所有用户的购物车
 * 
 * 该类负责：
 * 1. 管理所有用户的购物车（使用map存储）
 * 2. 从CSV文件加载购物车数据
 * 3. 将购物车数据保存到CSV文件
 * 4. 提供获取和创建用户购物车的接口
 */
class ShoppingCartManager {
private:
    std::string filePath;                                               // 购物车数据文件路径
    std::map<std::string, std::shared_ptr<ShoppingCart>> carts;         // 用户名到购物车的映射
    std::shared_ptr<IItemRepository> itemManager;                       // 商品管理器指针（用于查找商品）
    
    /**
     * @brief 去除字符串首尾空格
     * @param str 待处理的字符串
     * @return 处理后的字符串
     */
    std::string trim(const std::string& str);
    
    /**
     * @brief 解析数组字符串（如"[1,2,3]"）为vector
     * @param arrayStr 数组字符串
     * @return 解析后的整数向量
     */
    std::vector<int> parseArrayString(const std::string& arrayStr);
    
    /**
     * @brief 将整数向量转换为数组字符串（如"[1,2,3]"）
     * @param vec 整数向量
     * @return 数组字符串
     */
    std::string vectorToArrayString(const std::vector<int>& vec);

public:
    /**
     * @brief 构造函数
     * @param filePath 购物车数据文件路径
     * @param itemMgr 商品管理器指针
     */
    ShoppingCartManager(const std::string& filePath, std::shared_ptr<IItemRepository> itemMgr);
    
    /**
     * @brief 从CSV文件加载购物车数据
     * 
     * CSV文件格式：
     * username,item_ids,quantities
     * user1,"[1,2,3]","[2,1,5]"
     * 
     * @return 加载成功返回true，否则返回false
     */
    bool loadFromFile();
    
    /**
     * @brief 将购物车数据保存到CSV文件
     * @return 保存成功返回true，否则返回false
     */
    bool saveToFile();
    
    /**
     * @brief 获取指定用户的购物车
     * 
     * 如果用户购物车不存在，则创建一个新的空购物车
     * 
     * @param username 用户名
     * @param customer 用户对象指针
     * @return 购物车指针
     */
    std::shared_ptr<ShoppingCart> getCart(const std::string& username, 
                                           std::shared_ptr<Customer> customer);
    
    /**
     * @brief 检查指定用户是否有购物车
     * @param username 用户名
     * @return 存在返回true，否则返回false
     */
    bool hasCart(const std::string& username) const;
    
    /**
     * @brief 删除指定用户的购物车
     * @param username 用户名
     * @return 删除成功返回true，否则返回false
     */
    bool removeCart(const std::string& username);
    
    /**
     * @brief 清空所有购物车
     */
    void clearAllCarts();
    
    /**
     * @brief 获取购物车总数
     * @return 购物车数量
     */
    int getCartCount() const { return carts.size(); }
    
    /**
     * @brief 设置商品管理器
     * @param itemMgr 商品管理器指针
     */
    void setItemManager(std::shared_ptr<IItemRepository> itemMgr) { itemManager = itemMgr; }
    
    /**
     * @brief 析构函数
     */
    ~ShoppingCartManager();
};

#endif // SHOPPING_CART_MANAGER_H
