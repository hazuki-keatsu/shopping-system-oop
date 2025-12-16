/**
 * @file CustomerReportService.cpp
 * @brief 顾客购买数据统计分析服务的实现
 * @author Hazuki Keatsu
 * @date 2025-12-16
 */

#include "Services/CustomerReportService.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <set>
#include <filesystem>

namespace fs = std::filesystem;

/**
 * @brief 获取顾客的所有订单
 * 
 * 通过订单管理器查询属于该顾客的所有订单
 */
std::vector<std::shared_ptr<Order>> CustomerReportService::getCustomerOrders(
    const Customer& customer, 
    OrderManager& orderManager) {
    
    // 使用订单管理器的查询功能获取该用户的所有订单
    return orderManager.getOrdersByUserId(customer.getUsername());
}

/**
 * @brief 分析订单数据，统计类别和商品信息
 * 
 * 遍历所有订单，累计各类别和各商品的购买金额、数量和频度
 */
void CustomerReportService::analyzeOrders(
    const std::vector<std::shared_ptr<Order>>& orders,
    ItemManager* itemManager,
    std::map<std::string, CategoryStatistics>& categoryStats,
    std::map<std::string, ItemStatistics>& itemStats) {
    
    // 用于统计每个商品在不同订单中出现的次数
    std::map<std::string, int> itemOrderCount;
    
    // 用于统计每个类别在不同订单中出现的次数
    std::map<std::string, int> categoryOrderCount;
    
    // 遍历所有订单
    for (const auto& order : orders) {
        // 记录当前订单中已经统计过的类别和商品
        // 避免一个订单中同一类别/商品多次计数频度
        std::set<std::string> categoriesInOrder;
        std::set<std::string> itemsInOrder;
        
        // 遍历订单中的每个商品项
        const auto& orderItems = order->getItems();
        for (const auto& orderItem : orderItems) {
            const std::string& itemId = orderItem.itemId;
            const std::string& itemName = orderItem.itemName;
            
            // 从ItemManager获取商品的实际类别
            std::string category = "未知类别";
            if (itemManager != nullptr) {
                auto item = itemManager->findItemById(itemId);
                if (item != nullptr) {
                    category = item->getCategory();
                }
            }
            
            double itemTotalPrice = orderItem.price * orderItem.quantity;
            
            // 更新商品统计数据
            if (itemStats.find(itemId) == itemStats.end()) {
                // 首次出现该商品，创建新的统计记录
                itemStats[itemId] = ItemStatistics(itemId, itemName, category, 
                                                   itemTotalPrice, orderItem.quantity, 0);
            } else {
                // 累加购买金额和数量
                itemStats[itemId].totalAmount += itemTotalPrice;
                itemStats[itemId].purchaseQuantity += orderItem.quantity;
            }
            
            // 如果该商品在当前订单中首次出现，增加频度
            if (itemsInOrder.find(itemId) == itemsInOrder.end()) {
                itemStats[itemId].purchaseFrequency++;
                itemsInOrder.insert(itemId);
            }
            
            // 更新类别统计数据
            if (categoryStats.find(category) == categoryStats.end()) {
                // 首次出现该类别，创建新的统计记录
                categoryStats[category] = CategoryStatistics(category, itemTotalPrice, 0);
            } else {
                // 累加购买金额
                categoryStats[category].totalAmount += itemTotalPrice;
            }
            
            // 如果该类别在当前订单中首次出现，增加频度
            if (categoriesInOrder.find(category) == categoriesInOrder.end()) {
                categoryStats[category].purchaseFrequency++;
                categoriesInOrder.insert(category);
            }
        }
    }
}

/**
 * @brief 将统计数据写入CSV文件
 * 
 * 生成包含类别统计和商品统计的CSV报告
 * 文件格式：
 * - 第一部分：类别统计（类别名称、总金额、购买频度）
 * - 第二部分：商品统计（商品ID、商品名称、类别、总金额、购买数量、购买频度）
 */
bool CustomerReportService::writeStatisticsToCSV(
    const std::string& username,
    const std::map<std::string, CategoryStatistics>& categoryStats,
    const std::map<std::string, ItemStatistics>& itemStats,
    const std::string& outputPath) {
    
    try {
        // 确保输出目录存在
        fs::path dirPath(outputPath);
        if (!fs::exists(dirPath)) {
            fs::create_directories(dirPath);
        }
        
        // 生成文件名：username_report_YYYYMMDD.csv
        std::stringstream filenameStream;
        time_t now = time(nullptr);
        struct tm* timeInfo = localtime(&now);
        filenameStream << outputPath << username << "_report_"
                      << std::put_time(timeInfo, "%Y%m%d") << ".csv";
        std::string filename = filenameStream.str();
        
        // 打开文件进行写入
        std::ofstream outFile(filename);
        if (!outFile.is_open()) {
            std::cerr << "无法创建报告文件: " << filename << std::endl;
            return false;
        }
        
        // 写入CSV头部信息
        outFile << "顾客购买数据统计报告\n";
        outFile << "用户名: " << username << "\n";
        outFile << "生成时间: " << std::put_time(timeInfo, "%Y-%m-%d %H:%M:%S") << "\n";
        outFile << "\n";
        
        // 第一部分：按类别统计
        outFile << "=== 按类别统计 ===\n";
        outFile << "商品类别,总购买金额(元),购买频度(次)\n";
        
        // 按总金额排序
        std::vector<std::pair<std::string, CategoryStatistics>> sortedCategories(
            categoryStats.begin(), categoryStats.end());
        std::sort(sortedCategories.begin(), sortedCategories.end(),
                 [](const auto& a, const auto& b) {
                     return a.second.totalAmount > b.second.totalAmount;
                 });
        
        for (const auto& pair : sortedCategories) {
            const auto& stats = pair.second;
            outFile << stats.category << ","
                   << std::fixed << std::setprecision(2) << stats.totalAmount << ","
                   << stats.purchaseFrequency << "\n";
        }
        
        outFile << "\n";
        
        // 第二部分：按商品统计
        outFile << "=== 按商品统计 ===\n";
        outFile << "商品ID,商品名称,商品类别,总购买金额(元),总购买数量,购买频度(次)\n";
        
        // 按总金额排序
        std::vector<std::pair<std::string, ItemStatistics>> sortedItems(
            itemStats.begin(), itemStats.end());
        std::sort(sortedItems.begin(), sortedItems.end(),
                 [](const auto& a, const auto& b) {
                     return a.second.totalAmount > b.second.totalAmount;
                 });
        
        for (const auto& pair : sortedItems) {
            const auto& stats = pair.second;
            outFile << stats.itemId << ","
                   << stats.itemName << ","
                   << stats.category << ","
                   << std::fixed << std::setprecision(2) << stats.totalAmount << ","
                   << stats.purchaseQuantity << ","
                   << stats.purchaseFrequency << "\n";
        }
        
        outFile.close();
        std::cout << "报告已成功生成: " << filename << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "生成报告时发生错误: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief 为顾客生成购买数据统计报告
 * 
 * 这是主要的公共接口函数，整合了数据获取、分析和输出的全部流程
 */
bool CustomerReportService::GenerateReportFromCustomer(
    Customer& customer, 
    OrderManager& orderManager,
    ItemManager* itemManager,
    const std::string& outputPath) {
    
    std::cout << "正在为用户 " << customer.getUsername() << " 生成购买数据统计报告..." << std::endl;
    
    // 第一步：获取顾客的所有订单
    std::vector<std::shared_ptr<Order>> customerOrders = getCustomerOrders(customer, orderManager);
    
    if (customerOrders.empty()) {
        std::cout << "该用户暂无订单记录，无法生成报告。" << std::endl;
        return false;
    }
    
    std::cout << "找到 " << customerOrders.size() << " 个订单记录" << std::endl;
    
    // 第二步：分析订单数据
    std::map<std::string, CategoryStatistics> categoryStats;
    std::map<std::string, ItemStatistics> itemStats;
    
    analyzeOrders(customerOrders, itemManager, categoryStats, itemStats);
    
    std::cout << "统计到 " << categoryStats.size() << " 个商品类别, "
              << itemStats.size() << " 个商品" << std::endl;
    
    // 第三步：将统计结果写入CSV文件
    bool success = writeStatisticsToCSV(customer.getUsername(), 
                                       categoryStats, itemStats, outputPath);
    
    return success;
}

/**
 * @brief 在控制台显示统计报告
 * 
 * 除了生成CSV文件外，还可以直接在控制台显示统计信息
 */
void CustomerReportService::DisplayReportToConsole(
    Customer& customer, 
    OrderManager& orderManager,
    ItemManager* itemManager) {
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "顾客购买数据统计报告" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "用户名: " << customer.getUsername() << std::endl;
    
    // 获取订单
    std::vector<std::shared_ptr<Order>> customerOrders = getCustomerOrders(customer, orderManager);
    
    if (customerOrders.empty()) {
        std::cout << "该用户暂无订单记录。" << std::endl;
        return;
    }
    
    std::cout << "订单总数: " << customerOrders.size() << std::endl;
    std::cout << "\n";
    
    // 分析订单
    std::map<std::string, CategoryStatistics> categoryStats;
    std::map<std::string, ItemStatistics> itemStats;
    analyzeOrders(customerOrders, itemManager, categoryStats, itemStats);
    
    // 显示类别统计
    std::cout << "--- 按类别统计 ---" << std::endl;
    std::cout << std::left << std::setw(20) << "商品类别" 
              << std::right << std::setw(20) << "总金额(元)" 
              << std::setw(20) << "购买频度(次)" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    // 按金额排序
    std::vector<std::pair<std::string, CategoryStatistics>> sortedCategories(
        categoryStats.begin(), categoryStats.end());
    std::sort(sortedCategories.begin(), sortedCategories.end(),
             [](const auto& a, const auto& b) {
                 return a.second.totalAmount > b.second.totalAmount;
             });
    
    for (const auto& pair : sortedCategories) {
        const auto& stats = pair.second;
        std::cout << std::left << std::setw(20) << stats.category
                 << std::right << std::setw(15) << std::fixed << std::setprecision(2) 
                 << stats.totalAmount
                 << std::setw(15) << stats.purchaseFrequency << std::endl;
    }
    
    std::cout << "\n";
    
    // 显示商品统计（只显示前10个）
    std::cout << "--- 按商品统计（前10名）---" << std::endl;
    std::cout << std::left << std::setw(15) << "商品ID"
              << std::setw(25) << "商品名称"
              << std::right << std::setw(12) << "总金额"
              << std::setw(10) << "数量"
              << std::setw(10) << "频度" << std::endl;
    std::cout << std::string(72, '-') << std::endl;
    
    // 按金额排序
    std::vector<std::pair<std::string, ItemStatistics>> sortedItems(
        itemStats.begin(), itemStats.end());
    std::sort(sortedItems.begin(), sortedItems.end(),
             [](const auto& a, const auto& b) {
                 return a.second.totalAmount > b.second.totalAmount;
             });
    
    int count = 0;
    for (const auto& pair : sortedItems) {
        if (count++ >= 10) break;
        
        const auto& stats = pair.second;
        std::cout << std::left << std::setw(15) << stats.itemId
                 << std::setw(25) << stats.itemName
                 << std::right << std::setw(12) << std::fixed << std::setprecision(2) 
                 << stats.totalAmount
                 << std::setw(10) << stats.purchaseQuantity
                 << std::setw(10) << stats.purchaseFrequency << std::endl;
    }
    
    std::cout << "========================================\n" << std::endl;
}
