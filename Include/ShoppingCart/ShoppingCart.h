/**
 * @file ShoppingCart.h
 * @brief 购物车类的定义
 * @author Hazuki Keatsu
 * @date 2025-11-24
 */

#ifndef SHOPPING_CART_H
#define SHOPPING_CART_H

#include <vector>
#include <memory>
#include <utility>
#include "ItemManage/Item.h"
#include "UserManage/User.h"

/**
 * @class ShoppingCart
 * @brief 购物车类，管理单个用户的购物车信息
 * 
 * 每个顾客都有一个购物车，用于存储待购买的商品及数量
 * 提供添加、删除、修改、查询商品等功能
 */
class ShoppingCart {
private:
    std::shared_ptr<Customer> owner;                                    // 购物车所有者
    std::vector<std::pair<std::shared_ptr<Item>, int>> cartItems;       // 购物车中的商品及数量

public:
    /**
     * @brief 默认构造函数
     */
    ShoppingCart();
    
    /**
     * @brief 构造函数
     * @param owner 购物车所有者（顾客对象）
     */
    ShoppingCart(std::shared_ptr<Customer> owner);
    
    /**
     * @brief 构造函数
     * @param owner 购物车所有者（顾客对象）
     * @param items 初始商品列表
     */
    ShoppingCart(std::shared_ptr<Customer> owner, 
                 const std::vector<std::pair<std::shared_ptr<Item>, int>>& items);
    
    /**
     * @brief 获取购物车所有者
     * @return 所有者指针
     */
    std::shared_ptr<Customer> getOwner() const { return owner; }
    
    /**
     * @brief 获取购物车中的所有商品
     * @return 商品列表（商品指针和数量的pair）
     */
    const std::vector<std::pair<std::shared_ptr<Item>, int>>& getCartItems() const { return cartItems; }
    
    /**
     * @brief 向购物车中添加商品
     * 
     * 如果商品已存在，提示用户并询问是否增加数量
     * 如果商品不存在，直接添加
     * 
     * @param item 要添加的商品
     * @param quantity 购买数量
     * @return 添加成功返回true，否则返回false
     */
    bool addItem(std::shared_ptr<Item> item, int quantity);
    
    /**
     * @brief 从购物车中删除单个商品
     * @param itemId 要删除的商品ID
     * @return 删除成功返回true，否则返回false
     */
    bool removeItem(const std::string& itemId);
    
    /**
     * @brief 从购物车中删除多个商品
     * @param itemIds 要删除的商品ID列表
     * @return 成功删除的商品数量
     */
    int removeMultipleItems(const std::vector<std::string>& itemIds);
    
    /**
     * @brief 修改购物车中商品的购买数量
     * 
     * 如果数量修改为0，询问用户是否删除该商品
     * 若选择是，删除该商品；若选择否，保持原有数量不变
     * 
     * @param itemId 商品ID
     * @param newQuantity 新的购买数量
     * @return 修改成功返回true，否则返回false
     */
    bool updateItemQuantity(const std::string& itemId, int newQuantity);
    
    /**
     * @brief 查询购物车中的商品信息
     * 
     * 显示购物车中所有商品的详细信息，包括：
     * - 商品名称、价格、数量
     * - 单项小计
     * - 购物车商品总数
     * - 购物车商品总价
     */
    void displayCart() const;
    
    /**
     * @brief 计算购物车中商品的总数量
     * @return 商品总数量
     */
    int getTotalItemCount() const;
    
    /**
     * @brief 计算购物车中商品的总价格
     * @return 商品总价格
     */
    double getTotalPrice() const;
    
    /**
     * @brief 清空购物车
     */
    void clear();
    
    /**
     * @brief 检查购物车是否为空
     * @return 为空返回true，否则返回false
     */
    bool isEmpty() const { return cartItems.empty(); }
    
    /**
     * @brief 根据商品ID查找购物车中的商品
     * @param itemId 商品ID
     * @return 找到返回指向该商品的迭代器，未找到返回end()
     */
    std::vector<std::pair<std::shared_ptr<Item>, int>>::iterator 
        findItemById(const std::string& itemId);
    
    /**
     * @brief 直接添加商品到购物车（不进行重复检查，用于加载数据）
     * @param item 商品指针
     * @param quantity 数量
     */
    void addItemDirect(std::shared_ptr<Item> item, int quantity);
    
    /**
     * @brief 析构函数
     */
    ~ShoppingCart();
};

#endif // SHOPPING_CART_H
