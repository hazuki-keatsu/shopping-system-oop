/**
 * @file CustomerReportService.h
 * @brief 顾客购买数据统计分析服务
 * @author Hazuki Keatsu
 * @date 2025-12-16
 */

#ifndef CUSTOMER_REPORT_SERVICE_H
#define CUSTOMER_REPORT_SERVICE_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include "UserManage/User.h"
#include "Order/Order.h"
#include "Order/OrderManager.h"
#include "Interfaces/DependencyInterfaces.h"

/**
 * @struct CategoryStatistics
 * @brief 商品类别统计数据结构
 * 
 * 包含按类别统计的购买金额和购买频度
 */
struct CategoryStatistics {
    std::string category;           // 商品类别
    double totalAmount;             // 该类别总购买金额
    int purchaseFrequency;          // 该类别购买频度（购买次数）
    
    /**
     * @brief 默认构造函数
     */
    CategoryStatistics() : category(""), totalAmount(0.0), purchaseFrequency(0) {}
    
    /**
     * @brief 构造函数
     * @param cat 商品类别
     * @param amount 购买金额
     * @param frequency 购买频度
     */
    CategoryStatistics(const std::string& cat, double amount, int frequency)
        : category(cat), totalAmount(amount), purchaseFrequency(frequency) {}
};

/**
 * @struct ItemStatistics
 * @brief 商品统计数据结构
 * 
 * 包含具体商品的详细统计信息
 */
struct ItemStatistics {
    std::string itemId;             // 商品ID
    std::string itemName;           // 商品名称
    std::string category;           // 商品类别
    double totalAmount;             // 该商品总购买金额
    int purchaseQuantity;           // 总购买数量
    int purchaseFrequency;          // 购买频度（购买次数）
    
    /**
     * @brief 默认构造函数
     */
    ItemStatistics() 
        : itemId(""), itemName(""), category(""), 
          totalAmount(0.0), purchaseQuantity(0), purchaseFrequency(0) {}
    
    /**
     * @brief 构造函数
     */
    ItemStatistics(const std::string& id, const std::string& name, 
                   const std::string& cat, double amount, int quantity, int frequency)
        : itemId(id), itemName(name), category(cat), 
          totalAmount(amount), purchaseQuantity(quantity), purchaseFrequency(frequency) {}
};

/**
 * @class CustomerReportService
 * @brief 顾客购买数据统计分析服务类
 * 
 * 该类负责生成顾客的购买数据统计报告，包括：
 * 1. 按商品类别统计购买金额和频度
 * 2. 按具体商品统计购买信息
 * 3. 生成CSV格式的统计报告
 * 
 * 该服务遵循高内聚、低耦合的设计原则
 */
class CustomerReportService {
private:
    /**
     * @brief 从订单管理器中提取顾客的所有订单
     * @param customer 顾客对象
     * @param orderManager 订单管理器
     * @return 顾客的订单列表
     */
    static std::vector<std::shared_ptr<Order>> getCustomerOrders(const Customer& customer, OrderManager& orderManager);
    
    /**
     * @brief 从订单列表中统计商品数据
     * @param orders 订单列表
     * @param itemManager 商品管理器，用于获取商品类别信息
     * @param categoryStats 按类别统计的数据（输出参数）
     * @param itemStats 按商品统计的数据（输出参数）
     */
    static void analyzeOrders(const std::vector<std::shared_ptr<Order>>& orders,
                             IItemRepository* itemManager,
                             std::map<std::string, CategoryStatistics>& categoryStats,
                             std::map<std::string, ItemStatistics>& itemStats);
    
    /**
     * @brief 将统计数据写入CSV文件
     * @param username 用户名
     * @param categoryStats 类别统计数据
     * @param itemStats 商品统计数据
     * @param outputPath 输出路径
     * @return 是否成功
     */
    static bool writeStatisticsToCSV(const std::string& username,
                                     const std::map<std::string, CategoryStatistics>& categoryStats,
                                     const std::map<std::string, ItemStatistics>& itemStats,
                                     const std::string& outputPath);

public:
    /**
     * @brief 为顾客生成购买数据统计报告
     * 
     * 该函数会分析顾客的所有订单，统计：
     * - 各商品类别的购买金额和购买频度
     * - 各具体商品的购买金额、数量和频度
     * 
     * 生成的报告以CSV格式保存，包含两个部分：
     * 1. 类别统计表
     * 2. 商品详细统计表
     * 
     * @param customer 顾客对象
     * @param orderManager 订单管理器，用于获取订单数据
     * @param itemManager 商品管理器，用于获取商品类别信息
     * @param outputPath 输出路径（默认为reports/）
     * @return 是否成功生成报告
     */
    static bool GenerateReportFromCustomer(Customer& customer, 
                                          OrderManager& orderManager,
                                          IItemRepository* itemManager,
                                          const std::string& outputPath = "reports/");
    
    /**
     * @brief 显示统计报告到控制台
     * 
     * 在控制台显示顾客的购买数据统计信息
     * 
     * @param customer 顾客对象
     * @param orderManager 订单管理器
     * @param itemManager 商品管理器
     */
    static void DisplayReportToConsole(Customer& customer, 
                                      OrderManager& orderManager,
                                      IItemRepository* itemManager);
};

#endif // CUSTOMER_REPORT_SERVICE_H
