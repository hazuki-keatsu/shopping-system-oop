/**
 * @file Order.h
 * @brief 订单类的定义
 * @author Hazuki Keatsu
 * @date 2025-12-01
 */

#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include "ItemManage/Item.h"
#include "Interfaces/DependencyInterfaces.h"

/**
 * @enum OrderStatus
 * @brief 订单状态枚举
 */
enum class OrderStatus {
    PENDING,        // 待发货
    SHIPPED,        // 已发货
    DELIVERED       // 已签收
};

/**
 * @struct OrderItem
 * @brief 订单中的商品项
 */
struct OrderItem {
    std::string itemId;      // 商品ID
    std::string itemName;    // 商品名称
    double price;            // 购买时的价格
    int quantity;            // 购买数量
    
    /**
     * @brief 构造函数
     */
    OrderItem(const std::string& id, const std::string& name, double p, int q)
        : itemId(id), itemName(name), price(p), quantity(q) {}
};

/**
 * @class Order
 * @brief 订单类，管理单个订单的信息
 * 
 * 订单信息包括：
 * 1. 订单编号（由订单生成时间和用户ID拼接生成）
 * 2. 所购商品信息（商品ID列表，通过ItemManager获取详细信息）
 * 3. 购买时间
 * 4. 订单总额
 * 5. 收货地址
 * 6. 订单状态（待发货、已发货、已签收）
 */
class Order {
private:
    std::string orderId;                    // 订单编号
    std::string userId;                     // 用户ID（用户名）
    std::vector<OrderItem> items;           // 订单中的商品列表
    time_t orderTime;                       // 订单生成时间
    double totalAmount;                     // 订单总额
    std::string shippingAddress;            // 收货地址
    OrderStatus status;                     // 订单状态
    time_t statusChangeTime;                // 状态最后修改时间

public:
    /**
     * @brief 默认构造函数
     */
    Order();
    
    /**
     * @brief 构造函数
     * @param userId 用户ID（用户名）
     * @param items 购买的商品列表（Item指针和数量的pair）
     * @param shippingAddress 收货地址
     * @param itemManager 商品管理器指针，用于获取商品信息和更新库存
     */
    Order(const std::string& userId, 
          const std::vector<std::pair<std::shared_ptr<Item>, int>>& items,
          const std::string& shippingAddress,
          class IItemRepository* itemManager);
    
    /**
     * @brief 从CSV数据构造订单（用于数据加载）
     * @param orderId 订单编号
     * @param userId 用户ID
     * @param items 订单商品列表
     * @param orderTime 订单时间
     * @param totalAmount 订单总额
     * @param shippingAddress 收货地址
     * @param status 订单状态
     * @param statusChangeTime 状态修改时间
     */
    Order(const std::string& orderId,
          const std::string& userId,
          const std::vector<OrderItem>& items,
          time_t orderTime,
          double totalAmount,
          const std::string& shippingAddress,
          OrderStatus status,
          time_t statusChangeTime);
    
    /**
     * @brief 生成订单编号
     * 
     * 使用订单生成时间和用户ID的拼接字符串在std::hash中生成
     * 
     * @param userId 用户ID
     * @param timestamp 时间戳
     * @return 订单编号
     */
    static std::string generateOrderId(const std::string& userId, time_t timestamp);
    
    // Getter方法
    std::string getOrderId() const { return orderId; }
    std::string getUserId() const { return userId; }
    const std::vector<OrderItem>& getItems() const { return items; }
    time_t getOrderTime() const { return orderTime; }
    double getTotalAmount() const { return totalAmount; }
    std::string getShippingAddress() const { return shippingAddress; }
    OrderStatus getStatus() const { return status; }
    time_t getStatusChangeTime() const { return statusChangeTime; }
    
    // Setter方法
    void setStatus(OrderStatus newStatus);
    void setShippingAddress(const std::string& address) { shippingAddress = address; }
    
    /**
     * @brief 获取订单状态的字符串表示
     * @return 状态字符串
     */
    std::string getStatusString() const;
    
    /**
     * @brief 将订单状态字符串转换为枚举值
     * @param statusStr 状态字符串
     * @return 订单状态枚举
     */
    static OrderStatus stringToStatus(const std::string& statusStr);
    
    /**
     * @brief 显示订单详细信息
     */
    void displayOrderInfo() const;
    
    /**
     * @brief 析构函数
     */
    ~Order();
};

#endif // ORDER_H
