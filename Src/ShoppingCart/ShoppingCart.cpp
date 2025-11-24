/**
 * @file ShoppingCart.cpp
 * @brief 购物车类的实现
 * @author Hazuki Keatsu
 * @date 2025-11-24
 */

#include "ShoppingCart/ShoppingCart.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

/**
 * @brief 默认构造函数实现
 */
ShoppingCart::ShoppingCart() 
    : owner(nullptr) {
}

/**
 * @brief 构造函数实现
 */
ShoppingCart::ShoppingCart(std::shared_ptr<Customer> owner)
    : owner(owner) {
}

/**
 * @brief 构造函数实现
 */
ShoppingCart::ShoppingCart(std::shared_ptr<Customer> owner, 
                           const std::vector<std::pair<std::shared_ptr<Item>, int>>& items)
    : owner(owner), cartItems(items) {
}

/**
 * @brief 根据商品ID查找购物车中的商品
 */
std::vector<std::pair<std::shared_ptr<Item>, int>>::iterator 
ShoppingCart::findItemById(const std::string& itemId) {
    return std::find_if(cartItems.begin(), cartItems.end(),
        [&itemId](const std::pair<std::shared_ptr<Item>, int>& pair) {
            return pair.first->getItemId() == itemId;
        });
}

/**
 * @brief 向购物车中添加商品
 * 
 * 如果商品已存在，提示用户并询问是否增加数量
 * 如果商品不存在，直接添加
 */
bool ShoppingCart::addItem(std::shared_ptr<Item> item, int quantity) {
    // 参数验证
    if (!item) {
        std::cout << "错误：商品指针为空！" << std::endl;
        return false;
    }
    
    if (quantity <= 0) {
        std::cout << "错误：购买数量必须大于0！" << std::endl;
        return false;
    }
    
    // 检查库存
    if (quantity > item->getStock()) {
        std::cout << "错误：库存不足！当前库存：" << item->getStock() << std::endl;
        return false;
    }
    
    // 查找商品是否已在购物车中
    auto it = findItemById(item->getItemId());
    
    if (it != cartItems.end()) {
        // 商品已存在，询问用户
        std::cout << "提示：商品\"" << item->getItemName() << "\"已经在购物车中！" << std::endl;
        std::cout << "当前数量：" << it->second << std::endl;
        std::cout << "是否要增加购买数量？(y/n): ";
        
        char choice;
        std::cin >> choice;
        std::cin.ignore(1000, '\n'); // 清除输入缓冲区
        
        if (choice == 'y' || choice == 'Y') {
            int newQuantity = it->second + quantity;
            
            // 检查库存是否足够
            if (newQuantity > item->getStock()) {
                std::cout << "错误：库存不足！当前库存：" << item->getStock() 
                          << "，购物车已有：" << it->second << std::endl;
                return false;
            }
            
            it->second = newQuantity;
            std::cout << "成功！商品数量已更新为：" << newQuantity << std::endl;
            return true;
        } else {
            std::cout << "已取消添加操作。" << std::endl;
            return false;
        }
    } else {
        // 商品不存在，直接添加
        cartItems.push_back(std::make_pair(item, quantity));
        std::cout << "成功添加商品\"" << item->getItemName() << "\"到购物车，数量：" << quantity << std::endl;
        return true;
    }
}

/**
 * @brief 从购物车中删除单个商品
 */
bool ShoppingCart::removeItem(const std::string& itemId) {
    auto it = findItemById(itemId);
    
    if (it != cartItems.end()) {
        std::string itemName = it->first->getItemName();
        cartItems.erase(it);
        std::cout << "成功从购物车中删除商品：" << itemName << std::endl;
        return true;
    } else {
        std::cout << "错误：购物车中未找到ID为\"" << itemId << "\"的商品！" << std::endl;
        return false;
    }
}

/**
 * @brief 从购物车中删除多个商品
 */
int ShoppingCart::removeMultipleItems(const std::vector<std::string>& itemIds) {
    int successCount = 0;
    
    for (const auto& itemId : itemIds) {
        if (removeItem(itemId)) {
            successCount++;
        }
    }
    
    return successCount;
}

/**
 * @brief 修改购物车中商品的购买数量
 * 
 * 如果数量修改为0，询问用户是否删除该商品
 * 若选择是，删除该商品；若选择否，保持原有数量不变
 */
bool ShoppingCart::updateItemQuantity(const std::string& itemId, int newQuantity) {
    auto it = findItemById(itemId);
    
    if (it == cartItems.end()) {
        std::cout << "错误：购物车中未找到ID为\"" << itemId << "\"的商品！" << std::endl;
        return false;
    }
    
    // 如果新数量为0，询问是否删除
    if (newQuantity == 0) {
        std::cout << "提示：您将商品数量修改为0，是否要删除该商品？(y/n): ";
        char choice;
        std::cin >> choice;
        std::cin.ignore(1000, '\n'); // 清除输入缓冲区
        
        if (choice == 'y' || choice == 'Y') {
            std::string itemName = it->first->getItemName();
            cartItems.erase(it);
            std::cout << "已删除商品：" << itemName << std::endl;
            return true;
        } else {
            std::cout << "已取消操作，保持原有数量：" << it->second << std::endl;
            return false;
        }
    }
    
    // 检查新数量是否为负数
    if (newQuantity < 0) {
        std::cout << "错误：购买数量不能为负数！" << std::endl;
        return false;
    }
    
    // 检查库存是否足够
    if (newQuantity > it->first->getStock()) {
        std::cout << "错误：库存不足！当前库存：" << it->first->getStock() << std::endl;
        return false;
    }
    
    // 更新数量
    int oldQuantity = it->second;
    it->second = newQuantity;
    std::cout << "成功更新商品\"" << it->first->getItemName() 
              << "\"的数量：" << oldQuantity << " -> " << newQuantity << std::endl;
    return true;
}

/**
 * @brief 查询购物车中的商品信息
 * 
 * 显示购物车中所有商品的详细信息，包括：
 * - 商品名称、价格、数量
 * - 单项小计
 * - 购物车商品总数
 * - 购物车商品总价
 */
void ShoppingCart::displayCart() const {
    if (cartItems.empty()) {
        std::cout << "\n您的购物车是空的！" << std::endl;
        return;
    }
    
    std::cout << "\n========== 购物车信息 ==========" << std::endl;
    if (owner) {
        std::cout << "用户：" << owner->getUsername() << std::endl;
    }
    std::cout << "================================" << std::endl;
    
    // 设置输出格式
    std::cout << std::left 
              << std::setw(10) << "商品ID"
              << std::setw(25) << "商品名称"
              << std::setw(12) << "单价"
              << std::setw(8) << "数量"
              << std::setw(12) << "小计" 
              << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    
    // 显示每个商品的信息
    for (const auto& pair : cartItems) {
        std::shared_ptr<Item> item = pair.first;
        int quantity = pair.second;
        double subtotal = item->getPrice() * quantity;
        
        std::cout << std::left 
                  << std::setw(10) << item->getItemId()
                  << std::setw(25) << item->getItemName()
                  << std::setw(12) << std::fixed << std::setprecision(2) << item->getPrice()
                  << std::setw(8) << quantity
                  << std::setw(12) << std::fixed << std::setprecision(2) << subtotal
                  << std::endl;
    }
    
    std::cout << "================================================================" << std::endl;
    
    // 显示统计信息
    std::cout << "商品总数：" << getTotalItemCount() << " 件" << std::endl;
    std::cout << "总价：¥" << std::fixed << std::setprecision(2) << getTotalPrice() << std::endl;
    std::cout << "================================\n" << std::endl;
}

/**
 * @brief 计算购物车中商品的总数量
 */
int ShoppingCart::getTotalItemCount() const {
    int totalCount = 0;
    for (const auto& pair : cartItems) {
        totalCount += pair.second;
    }
    return totalCount;
}

/**
 * @brief 计算购物车中商品的总价格
 */
double ShoppingCart::getTotalPrice() const {
    double totalPrice = 0.0;
    for (const auto& pair : cartItems) {
        totalPrice += pair.first->getPrice() * pair.second;
    }
    return totalPrice;
}

/**
 * @brief 清空购物车
 */
void ShoppingCart::clear() {
    cartItems.clear();
    std::cout << "购物车已清空！" << std::endl;
}

/**
 * @brief 直接添加商品到购物车（不进行重复检查，用于加载数据）
 */
void ShoppingCart::addItemDirect(std::shared_ptr<Item> item, int quantity) {
    if (item && quantity > 0) {
        cartItems.push_back(std::make_pair(item, quantity));
    }
}

/**
 * @brief 析构函数
 */
ShoppingCart::~ShoppingCart() {
    // 清理资源
}
