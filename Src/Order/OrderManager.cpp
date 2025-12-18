/**
 * @file OrderManager.cpp
 * @brief 订单管理器类的实现
 * @author Hazuki Keatsu
 * @date 2025-12-01
 */

#include "Order/OrderManager.h"
#include "Order/OrderException.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>

/**
 * @brief 构造函数实现
 */
OrderManager::OrderManager(const std::string& filePath, std::shared_ptr<IItemRepository> itemManager)
    : filePath(filePath), itemManager(itemManager), autoUpdateEnabled(false),
      pendingToShippedSeconds(10), shippedToDeliveredSeconds(20) {
}

/**
 * @brief 去除字符串首尾空格
 */
std::string OrderManager::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

/**
 * @brief 解析CSV行数据
 */
std::vector<std::string> OrderManager::parseCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    
    // 按逗号分割字段
    while (std::getline(ss, field, ',')) {
        fields.push_back(trim(field));
    }
    
    return fields;
}

/**
 * @brief 解析订单商品信息字符串
 * 
 * 格式：itemId:name:price:quantity;itemId:name:price:quantity;...
 */
std::vector<OrderItem> OrderManager::parseOrderItems(const std::string& itemsStr) {
    std::vector<OrderItem> items;
    
    std::stringstream ss(itemsStr);
    std::string itemStr;
    
    // 按分号分割每个商品
    while (std::getline(ss, itemStr, ';')) {
        if (itemStr.empty()) continue;
        
        // 按冒号分割商品字段
        std::stringstream itemSs(itemStr);
        std::string itemId, name, priceStr, quantityStr;
        
        std::getline(itemSs, itemId, ':');
        std::getline(itemSs, name, ':');
        std::getline(itemSs, priceStr, ':');
        std::getline(itemSs, quantityStr, ':');
        
        try {
            double price = std::stod(priceStr);
            int quantity = std::stoi(quantityStr);
            items.push_back(OrderItem(itemId, name, price, quantity));
        } catch (...) {
            // 忽略解析错误的商品
            std::cerr << "警告：解析订单商品失败: " << itemStr << std::endl;
        }
    }
    
    return items;
}

/**
 * @brief 将订单商品列表转换为字符串
 */
std::string OrderManager::orderItemsToString(const std::vector<OrderItem>& items) {
    std::stringstream ss;
    
    for (size_t i = 0; i < items.size(); ++i) {
        const auto& item = items[i];
        ss << item.itemId << ":" << item.itemName << ":" 
           << item.price << ":" << item.quantity;
        
        if (i < items.size() - 1) {
            ss << ";";
        }
    }
    
    return ss.str();
}

/**
 * @brief 从CSV文件加载订单数据
 * 
 * CSV格式：order_id,user_id,items,order_time,total_amount,shipping_address,status,status_change_time
 */
bool OrderManager::loadFromFile() {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "订单数据文件不存在，将创建新文件。" << std::endl;
        return true;
    }
    
    std::string line;
    bool isFirstLine = true;
    
    // 清空现有数据
    std::lock_guard<std::mutex> lock(ordersMutex);
    orders.clear();
    
    // 逐行读取文件
    while (std::getline(file, line)) {
        // 跳过标题行
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }
        
        // 跳过空行
        if (line.empty()) {
            continue;
        }
        
        // 解析CSV行
        std::vector<std::string> fields = parseCSVLine(line);
        if (fields.size() >= 8) {
            try {
                std::string orderId = fields[0];
                std::string userId = fields[1];
                std::vector<OrderItem> items = parseOrderItems(fields[2]);
                time_t orderTime = std::stoll(fields[3]);
                double totalAmount = std::stod(fields[4]);
                std::string shippingAddress = fields[5];
                OrderStatus status = Order::stringToStatus(fields[6]);
                time_t statusChangeTime = std::stoll(fields[7]);
                
                // 创建Order对象
                auto order = std::make_shared<Order>(orderId, userId, items, orderTime,
                                                     totalAmount, shippingAddress, 
                                                     status, statusChangeTime);
                orders.push_back(order);
            } catch (const std::exception& e) {
                std::cerr << "警告：解析订单数据失败: " << e.what() << std::endl;
            }
        }
    }
    
    file.close();
    std::cout << "成功加载 " << orders.size() << " 个订单数据。" << std::endl;
    return true;
}

/**
 * @brief 保存订单数据到CSV文件
 */
bool OrderManager::saveToFile() {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "无法打开文件进行写入: " << filePath << std::endl;
        return false;
    }
    
    // 写入标题行
    file << "order_id,user_id,items,order_time,total_amount,shipping_address,status,status_change_time" << std::endl;
    
    // 写入每个订单的数据
    std::lock_guard<std::mutex> lock(ordersMutex);
    for (const auto& order : orders) {
        file << order->getOrderId() << ","
             << order->getUserId() << ","
             << orderItemsToString(order->getItems()) << ","
             << order->getOrderTime() << ","
             << order->getTotalAmount() << ","
             << order->getShippingAddress() << ","
             << order->getStatusString() << ","
             << order->getStatusChangeTime() << std::endl;
    }
    
    file.close();
    return true;
}

/**
 * @brief 创建新订单
 */
std::shared_ptr<Order> OrderManager::createOrder(
    const std::string& userId,
    const std::vector<std::pair<std::shared_ptr<Item>, int>>& cartItems,
    const std::string& shippingAddress) {
    
    try {
        // 创建新订单（订单构造函数中会检查库存并更新）
        auto order = std::make_shared<Order>(userId, cartItems, shippingAddress, itemManager.get());
        
        // 添加到订单列表
        {
            std::lock_guard<std::mutex> lock(ordersMutex);
            orders.push_back(order);
        }
        
        // 保存到文件
        saveToFile();
        
        std::cout << "\n订单创建成功！订单编号：" << order->getOrderId() << std::endl;
        return order;
        
    } catch (const InsufficientStockException& e) {
        // 捕获库存不足异常
        std::cerr << "\n创建订单失败：" << e.what() << std::endl;
        return nullptr;
    } catch (const std::exception& e) {
        std::cerr << "\n创建订单失败：" << e.what() << std::endl;
        return nullptr;
    }
}

/**
 * @brief 根据订单ID查找订单
 */
std::shared_ptr<Order> OrderManager::findOrderById(const std::string& orderId) {
    std::lock_guard<std::mutex> lock(ordersMutex);
    
    auto it = std::find_if(orders.begin(), orders.end(),
        [&orderId](const std::shared_ptr<Order>& order) {
            return order->getOrderId() == orderId;
        });
    
    if (it != orders.end()) {
        return *it;
    }
    
    return nullptr;
}

/**
 * @brief 根据用户ID获取该用户的所有订单
 */
std::vector<std::shared_ptr<Order>> OrderManager::getOrdersByUserId(const std::string& userId) {
    std::vector<std::shared_ptr<Order>> userOrders;
    
    std::lock_guard<std::mutex> lock(ordersMutex);
    
    for (const auto& order : orders) {
        if (order->getUserId() == userId) {
            userOrders.push_back(order);
        }
    }
    
    return userOrders;
}

/**
 * @brief 更新订单状态
 */
bool OrderManager::updateOrderStatus(const std::string& orderId, OrderStatus newStatus) {
    auto order = findOrderById(orderId);
    
    if (order == nullptr) {
        std::cout << "错误：订单不存在！" << std::endl;
        return false;
    }
    
    order->setStatus(newStatus);
    saveToFile();
    
    std::cout << "订单状态已更新为：" << order->getStatusString() << std::endl;
    return true;
}

/**
 * @brief 显示所有订单信息（表格形式）
 */
void OrderManager::displayAllOrders() const {
    std::lock_guard<std::mutex> lock(ordersMutex);
    
    if (orders.empty()) {
        std::cout << "暂无订单信息。" << std::endl;
        return;
    }
    
    std::cout << "\n========== 订单列表 ==========" << std::endl;
    std::cout << std::left
              << std::setw(20) << "订单编号"
              << std::setw(15) << "用户ID"
              << std::setw(20) << "订单时间"
              << std::setw(12) << "订单总额"
              << std::setw(12) << "订单状态"
              << std::endl;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    
    for (const auto& order : orders) {
        // 格式化订单时间
        char timeBuffer[20];
        time_t orderTime = order->getOrderTime();
        struct tm* timeinfo = std::localtime(&orderTime);
        std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        
        std::cout << std::left
                  << std::setw(20) << order->getOrderId()
                  << std::setw(15) << order->getUserId()
                  << std::setw(20) << timeBuffer
                  << std::setw(12) << std::fixed << std::setprecision(2) << order->getTotalAmount()
                  << std::setw(12) << order->getStatusString()
                  << std::endl;
    }
    
    std::cout << "===============================================================================" << std::endl;
    std::cout << "共 " << orders.size() << " 个订单。" << std::endl;
}

/**
 * @brief 显示用户的订单信息
 */
void OrderManager::displayUserOrders(const std::string& userId) const {
    std::lock_guard<std::mutex> lock(ordersMutex);
    
    std::vector<std::shared_ptr<Order>> userOrders;
    for (const auto& order : orders) {
        if (order->getUserId() == userId) {
            userOrders.push_back(order);
        }
    }
    
    if (userOrders.empty()) {
        std::cout << "\n您还没有订单。" << std::endl;
        return;
    }
    
    std::cout << "\n========== 我的订单 ==========" << std::endl;
    std::cout << std::left
              << std::setw(20) << "订单编号"
              << std::setw(20) << "订单时间"
              << std::setw(12) << "订单总额"
              << std::setw(12) << "订单状态"
              << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;
    
    for (const auto& order : userOrders) {
        // 格式化订单时间
        char timeBuffer[20];
        time_t orderTime = order->getOrderTime();
        struct tm* timeinfo = std::localtime(&orderTime);
        std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        
        std::cout << std::left
                  << std::setw(20) << order->getOrderId()
                  << std::setw(20) << timeBuffer
                  << std::setw(12) << std::fixed << std::setprecision(2) << order->getTotalAmount()
                  << std::setw(12) << order->getStatusString()
                  << std::endl;
    }
    
    std::cout << "============================================================" << std::endl;
    std::cout << "共 " << userOrders.size() << " 个订单。" << std::endl;
}

/**
 * @brief 自动更新订单状态的线程函数
 */
void OrderManager::autoUpdateOrderStatus() {
    while (autoUpdateEnabled) {
        // 每秒检查一次
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        time_t currentTime = std::time(nullptr);
        bool needSave = false;
        
        {
            std::lock_guard<std::mutex> lock(ordersMutex);
            
            for (auto& order : orders) {
                time_t timeSinceStatusChange = currentTime - order->getStatusChangeTime();
                
                // 待发货 -> 已发货
                if (order->getStatus() == OrderStatus::PENDING && 
                    timeSinceStatusChange >= pendingToShippedSeconds) {
                    order->setStatus(OrderStatus::SHIPPED);
                    needSave = true;
                    // std::cout << "\n[自动更新] 订单 " << order->getOrderId() 
                    //           << " 状态已更新为：已发货" << std::endl;
                }
                // 已发货 -> 已签收
                else if (order->getStatus() == OrderStatus::SHIPPED && 
                         timeSinceStatusChange >= shippedToDeliveredSeconds) {
                    order->setStatus(OrderStatus::DELIVERED);
                    needSave = true;
                    // std::cout << "\n[自动更新] 订单 " << order->getOrderId() 
                    //           << " 状态已更新为：已签收" << std::endl;
                }
            }
        }
        
        if (needSave) {
            saveToFile();
        }
    }
}

/**
 * @brief 启用自动状态更新
 */
void OrderManager::enableAutoUpdate(int pendingToShipped, int shippedToDelivered) {
    if (autoUpdateEnabled) {
        std::cout << "自动状态更新已经启用。" << std::endl;
        return;
    }
    
    pendingToShippedSeconds = pendingToShipped;
    shippedToDeliveredSeconds = shippedToDelivered;
    autoUpdateEnabled = true;
    
    // 启动自动更新线程
    autoUpdateThread = std::thread(&OrderManager::autoUpdateOrderStatus, this);
    
    std::cout << "自动状态更新已启用。" << std::endl;
    std::cout << "待发货->已发货: " << pendingToShipped << "秒" << std::endl;
    std::cout << "已发货->已签收: " << shippedToDelivered << "秒" << std::endl;
}

/**
 * @brief 禁用自动状态更新
 */
void OrderManager::disableAutoUpdate() {
    if (!autoUpdateEnabled) {
        return;
    }
    
    autoUpdateEnabled = false;
    
    // 等待线程结束
    if (autoUpdateThread.joinable()) {
        autoUpdateThread.join();
    }
    
    std::cout << "自动状态更新已禁用。" << std::endl;
}

/**
 * @brief 析构函数
 */
OrderManager::~OrderManager() {
    disableAutoUpdate();
}
