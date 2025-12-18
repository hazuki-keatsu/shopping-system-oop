/**
 * @file Order.cpp
 * @brief 订单类的实现
 * @author Hazuki Keatsu
 * @date 2025-12-01
 */

#include "Order/Order.h"
#include "Order/OrderException.h"
#include "Interfaces/DependencyInterfaces.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <functional>

/**
 * @brief 默认构造函数实现
 */
Order::Order() 
    : orderId(""), userId(""), orderTime(0), totalAmount(0.0), 
      shippingAddress(""), status(OrderStatus::PENDING), statusChangeTime(0) {
}

/**
 * @brief 构造函数实现
 * 
 * 创建新订单时：
 * 1. 生成订单编号
 * 2. 计算订单总额
 * 3. 验证并更新商品库存
 * 4. 记录订单时间
 */
Order::Order(const std::string& userId, 
             const std::vector<std::pair<std::shared_ptr<Item>, int>>& cartItems,
             const std::string& shippingAddress,
             IItemRepository* itemManager)
    : userId(userId), totalAmount(0.0), shippingAddress(shippingAddress), 
      status(OrderStatus::PENDING) {
    
    // 获取当前时间
    orderTime = std::time(nullptr);
    statusChangeTime = orderTime;
    
    // 生成订单编号
    orderId = generateOrderId(userId, orderTime);
    
    // 处理订单中的每个商品
    for (const auto& pair : cartItems) {
        std::shared_ptr<Item> item = pair.first;
        int quantity = pair.second;
        
        // 检查库存是否充足
        if (quantity > item->getStock()) {
            throw InsufficientStockException(item->getItemName(), quantity, item->getStock());
        }
        
        // 添加商品到订单
        OrderItem orderItem(item->getItemId(), item->getItemName(), 
                           item->getPrice(), quantity);
        items.push_back(orderItem);
        
        // 计算总额
        totalAmount += item->getPrice() * quantity;
        
        // 更新商品库存
        item->setStock(item->getStock() - quantity);
    }
    
    // 保存更新后的商品数据
    if (itemManager) {
        itemManager->saveToFile();
    }
}

/**
 * @brief 从CSV数据构造订单（用于数据加载）
 */
Order::Order(const std::string& orderId,
             const std::string& userId,
             const std::vector<OrderItem>& items,
             time_t orderTime,
             double totalAmount,
             const std::string& shippingAddress,
             OrderStatus status,
             time_t statusChangeTime)
    : orderId(orderId), userId(userId), items(items), orderTime(orderTime),
      totalAmount(totalAmount), shippingAddress(shippingAddress), 
      status(status), statusChangeTime(statusChangeTime) {
}

/**
 * @brief 生成订单编号
 * 
 * 使用订单生成时间和用户ID的拼接字符串在std::hash中生成
 */
std::string Order::generateOrderId(const std::string& userId, time_t timestamp) {
    // 拼接用户ID和时间戳
    std::stringstream ss;
    ss << userId << "_" << timestamp;
    std::string combined = ss.str();
    
    // 使用std::hash生成哈希值
    std::hash<std::string> hasher;
    size_t hashValue = hasher(combined);
    
    // 转换为字符串形式的订单编号
    std::stringstream orderIdStream;
    orderIdStream << "ORD" << std::setfill('0') << std::setw(16) << hashValue;
    
    return orderIdStream.str();
}

/**
 * @brief 设置订单状态
 * 
 * 更新订单状态，并记录状态修改时间
 */
void Order::setStatus(OrderStatus newStatus) {
    status = newStatus;
    statusChangeTime = std::time(nullptr);
}

/**
 * @brief 获取订单状态的字符串表示
 */
std::string Order::getStatusString() const {
    switch (status) {
        case OrderStatus::PENDING:
            return "待发货";
        case OrderStatus::SHIPPED:
            return "已发货";
        case OrderStatus::DELIVERED:
            return "已签收";
        default:
            return "未知状态";
    }
}

/**
 * @brief 将订单状态字符串转换为枚举值
 */
OrderStatus Order::stringToStatus(const std::string& statusStr) {
    if (statusStr == "待发货" || statusStr == "PENDING") {
        return OrderStatus::PENDING;
    } else if (statusStr == "已发货" || statusStr == "SHIPPED") {
        return OrderStatus::SHIPPED;
    } else if (statusStr == "已签收" || statusStr == "DELIVERED") {
        return OrderStatus::DELIVERED;
    } else {
        return OrderStatus::PENDING;  // 默认状态
    }
}

/**
 * @brief 显示订单详细信息
 */
void Order::displayOrderInfo() const {
    std::cout << "\n========== 订单详情 ==========" << std::endl;
    std::cout << "订单编号: " << orderId << std::endl;
    std::cout << "用户ID: " << userId << std::endl;
    
    // 格式化订单时间
    char timeBuffer[80];
    struct tm* timeinfo = std::localtime(&orderTime);
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    std::cout << "订单时间: " << timeBuffer << std::endl;
    
    std::cout << "订单状态: " << getStatusString() << std::endl;
    
    // 格式化状态修改时间
    timeinfo = std::localtime(&statusChangeTime);
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    std::cout << "状态更新时间: " << timeBuffer << std::endl;
    
    std::cout << "收货地址: " << shippingAddress << std::endl;
    
    std::cout << "\n商品列表:" << std::endl;
    std::cout << std::left
              << std::setw(10) << "商品ID"
              << std::setw(25) << "商品名称"
              << std::setw(12) << "单价"
              << std::setw(8) << "数量"
              << std::setw(12) << "小计"
              << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    
    // 显示每个商品
    for (const auto& item : items) {
        double subtotal = item.price * item.quantity;
        std::cout << std::left
                  << std::setw(10) << item.itemId
                  << std::setw(25) << item.itemName
                  << std::setw(12) << std::fixed << std::setprecision(2) << item.price
                  << std::setw(8) << item.quantity
                  << std::setw(12) << std::fixed << std::setprecision(2) << subtotal
                  << std::endl;
    }
    
    std::cout << "================================================================" << std::endl;
    std::cout << "订单总额: ¥" << std::fixed << std::setprecision(2) << totalAmount << std::endl;
    std::cout << "==============================\n" << std::endl;
}

/**
 * @brief 析构函数
 */
Order::~Order() {
}
