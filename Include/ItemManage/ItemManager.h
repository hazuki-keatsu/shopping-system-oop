/**
 * @file ItemManager.h
 * @brief 商品管理器类，负责商品数据的管理和持久化
 * @author Hazuki Keatsu
 * @date 2025-11-17
 */

#ifndef ITEM_MANAGER_H
#define ITEM_MANAGER_H

#include "ItemManage/Item.h"
#include <vector>
#include <map>
#include <memory>
#include <string>

// 前向声明
class PromotionManager;

/**
 * @class ItemManager
 * @brief 商品管理器类，负责商品的增删改查和CSV文件操作
 * 
 * 特点：
 * 1. 使用vector存储所有商品对象（顺序存储）
 * 2. 使用map<类别, vector<商品指针>>建立类别索引
 * 3. 支持动态表头，可由管理员自定义字段
 */
class ItemManager {
private:
    std::vector<std::shared_ptr<Item>> items;           // 所有商品列表
    std::map<std::string, std::vector<std::shared_ptr<Item>>> categoryIndex;  // 类别索引
    std::vector<std::string> headers;                   // CSV表头（动态）
    std::string filePath;                               // 数据文件路径
    
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
     * @brief 重建类别索引
     * 
     * 在添加、删除或修改商品后调用，更新类别索引
     */
    void rebuildCategoryIndex();
    
    /**
     * @brief 生成新的商品ID
     * @return 新的唯一商品ID
     */
    std::string generateNewItemId();

public:
    /**
     * @brief 构造函数
     * @param filePath 商品数据文件路径
     */
    ItemManager(const std::string& filePath);
    
    /**
     * @brief 从CSV文件加载商品数据
     * @return 加载成功返回true，否则返回false
     */
    bool loadFromFile();
    
    /**
     * @brief 保存商品数据到CSV文件
     * @return 保存成功返回true，否则返回false
     */
    bool saveToFile();
    
    /**
     * @brief 添加新商品
     * @param item 商品对象
     * @return 添加成功返回true，否则返回false
     */
    bool addItem(std::shared_ptr<Item> item);
    
    /**
     * @brief 根据ID删除商品
     * @param itemId 商品ID
     * @return 删除成功返回true，否则返回false
     */
    bool deleteItem(const std::string& itemId);
    
    /**
     * @brief 根据ID查找商品
     * @param itemId 商品ID
     * @return 找到返回商品对象指针，否则返回nullptr
     */
    std::shared_ptr<Item> findItemById(const std::string& itemId);
    
    /**
     * @brief 根据类别获取商品列表
     * @param category 商品类别
     * @return 该类别下的所有商品
     */
    std::vector<std::shared_ptr<Item>> getItemsByCategory(const std::string& category);
    
    /**
     * @brief 获取所有商品列表
     * @return 所有商品
     */
    const std::vector<std::shared_ptr<Item>>& getAllItems() const { return items; }
    
    /**
     * @brief 获取所有类别
     * @return 类别列表
     */
    std::vector<std::string> getAllCategories() const;
    
    /**
     * @brief 获取CSV表头
     * @return 表头列表
     */
    const std::vector<std::string>& getHeaders() const { return headers; }
    
    /**
     * @brief 显示所有商品信息（表格形式）
     * @param promotionManager 促销管理器指针（可选，用于显示促销标签）
     */
    void displayAllItems(PromotionManager* promotionManager = nullptr) const;
    
    /**
     * @brief 检查商品ID是否存在
     * @param itemId 商品ID
     * @return 存在返回true，否则返回false
     */
    bool isItemIdExists(const std::string& itemId) const;
    
    /**
     * @brief 析构函数
     */
    ~ItemManager();
};

#endif // ITEM_MANAGER_H
