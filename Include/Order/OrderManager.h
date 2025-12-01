/**
 * @file OrderManager.h
 * @brief 订单管理器类，负责订单的管理和持久化
 * @author Hazuki Keatsu
 * @date 2025-12-01
 */

#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H

#include "Order/Order.h"
#include "ItemManage/ItemManager.h"
#include <vector>
#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>

/**
 * @class OrderManager
 * @brief 订单管理器类，负责订单的增删改查和CSV文件操作
 * 
 * 主要功能：
 * 1. 从CSV文件加载订单数据
 * 2. 保存订单数据到CSV文件
 * 3. 创建新订单
 * 4. 查询订单
 * 5. 管理订单状态
 * 6. 自动状态更新（待发货->已发货->已签收）
 */
class OrderManager {
private:
    std::vector<std::shared_ptr<Order>> orders;     // 所有订单列表
    std::string filePath;                           // 数据文件路径
    std::shared_ptr<ItemManager> itemManager;       // 商品管理器
    
    // 自动状态更新相关
    std::atomic<bool> autoUpdateEnabled;            // 自动更新是否启用
    std::thread autoUpdateThread;                   // 自动更新线程
    mutable std::mutex ordersMutex;                 // 订单列表互斥锁（mutable以支持const函数）
    int pendingToShippedSeconds;                    // 待发货到已发货的秒数
    int shippedToDeliveredSeconds;                  // 已发货到已签收的秒数
    
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
    
    /**
     * @brief 自动更新订单状态的线程函数
     */
    void autoUpdateOrderStatus();
    
    /**
     * @brief 解析订单商品信息字符串
     * @param itemsStr 商品信息字符串（格式：itemId:name:price:quantity;...）
     * @return 订单商品列表
     */
    std::vector<OrderItem> parseOrderItems(const std::string& itemsStr);
    
    /**
     * @brief 将订单商品列表转换为字符串
     * @param items 订单商品列表
     * @return 商品信息字符串
     */
    std::string orderItemsToString(const std::vector<OrderItem>& items);

public:
    /**
     * @brief 构造函数
     * @param filePath 订单数据文件路径
     * @param itemManager 商品管理器指针
     */
    OrderManager(const std::string& filePath, std::shared_ptr<ItemManager> itemManager);
    
    /**
     * @brief 从CSV文件加载订单数据
     * @return 加载成功返回true，否则返回false
     */
    bool loadFromFile();
    
    /**
     * @brief 保存订单数据到CSV文件
     * @return 保存成功返回true，否则返回false
     */
    bool saveToFile();
    
    /**
     * @brief 创建新订单
     * @param userId 用户ID
     * @param cartItems 购物车商品列表
     * @param shippingAddress 收货地址
     * @return 创建成功返回订单对象指针，失败返回nullptr
     */
    std::shared_ptr<Order> createOrder(const std::string& userId,
                                       const std::vector<std::pair<std::shared_ptr<Item>, int>>& cartItems,
                                       const std::string& shippingAddress);
    
    /**
     * @brief 根据订单ID查找订单
     * @param orderId 订单ID
     * @return 找到返回订单对象指针，否则返回nullptr
     */
    std::shared_ptr<Order> findOrderById(const std::string& orderId);
    
    /**
     * @brief 根据用户ID获取该用户的所有订单
     * @param userId 用户ID
     * @return 订单列表
     */
    std::vector<std::shared_ptr<Order>> getOrdersByUserId(const std::string& userId);
    
    /**
     * @brief 获取所有订单
     * @return 所有订单列表
     */
    const std::vector<std::shared_ptr<Order>>& getAllOrders() const { return orders; }
    
    /**
     * @brief 更新订单状态
     * @param orderId 订单ID
     * @param newStatus 新状态
     * @return 更新成功返回true，否则返回false
     */
    bool updateOrderStatus(const std::string& orderId, OrderStatus newStatus);
    
    /**
     * @brief 显示所有订单信息（表格形式）
     */
    void displayAllOrders() const;
    
    /**
     * @brief 显示用户的订单信息
     * @param userId 用户ID
     */
    void displayUserOrders(const std::string& userId) const;
    
    /**
     * @brief 启用自动状态更新
     * @param pendingToShipped 待发货到已发货的秒数（默认10秒）
     * @param shippedToDelivered 已发货到已签收的秒数（默认20秒）
     */
    void enableAutoUpdate(int pendingToShipped = 10, int shippedToDelivered = 20);
    
    /**
     * @brief 禁用自动状态更新
     */
    void disableAutoUpdate();
    
    /**
     * @brief 析构函数
     */
    ~OrderManager();
};

#endif // ORDER_MANAGER_H
