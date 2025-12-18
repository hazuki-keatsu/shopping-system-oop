/**
 * @file ItemSearcher.h
 * @brief 商品搜索类，提供精确和模糊搜索功能
 * @author Hazuki Keatsu
 * @date 2025-11-17
 */

#ifndef ITEM_SEARCHER_H
#define ITEM_SEARCHER_H

#include "ItemManage/Item.h"
#include "Interfaces/DependencyInterfaces.h"
#include <vector>
#include <memory>
#include <string>

/**
 * @enum SearchType
 * @brief 搜索类型枚举
 */
enum class SearchType {
    BY_NAME,        ///< 按商品名称搜索
    BY_CATEGORY,    ///< 按商品类别搜索
    BY_PRICE,       ///< 按价格范围搜索
    ALL             ///< 综合搜索（名称和类别）
};

/**
 * @struct SearchResult
 * @brief 搜索结果结构体，包含商品和相似度分数
 */
struct SearchResult {
    std::shared_ptr<Item> item;     // 商品指针
    double similarityScore;          // 相似度分数（0-1之间，1表示完全匹配）
    
    /**
     * @brief 构造函数
     */
    SearchResult(std::shared_ptr<Item> item, double score)
        : item(item), similarityScore(score) {}
};

/**
 * @class ItemSearcher
 * @brief 商品搜索类，提供多种搜索方式
 * 
 * 搜索策略：
 * 1. 优先进行精确搜索
 * 2. 如果精确搜索无结果，自动进行模糊搜索
 * 3. 模糊搜索使用Levenshtein编辑距离算法计算相似度
 */
class ItemSearcher {
private:
    IItemRepository* itemManager;   // 商品管理器指针
    
    /**
     * @brief 计算两个字符串的Levenshtein编辑距离
     * @param s1 字符串1
     * @param s2 字符串2
     * @return 编辑距离
     */
    int calculateLevenshteinDistance(const std::string& s1, const std::string& s2);
    
    /**
     * @brief 计算字符串相似度（基于编辑距离）
     * @param s1 字符串1
     * @param s2 字符串2
     * @return 相似度（0-1之间）
     */
    double calculateSimilarity(const std::string& s1, const std::string& s2);
    
    /**
     * @brief 转换为小写（用于不区分大小写的比较）
     * @param str 输入字符串
     * @return 小写字符串
     */
    std::string toLowerCase(const std::string& str);
    
    /**
     * @brief 检查字符串是否包含子串（不区分大小写）
     * @param str 主字符串
     * @param substr 子字符串
     * @return 包含返回true，否则返回false
     */
    bool containsIgnoreCase(const std::string& str, const std::string& substr);

public:
    /**
     * @brief 构造函数
     * @param itemManager 商品管理器指针
     */
    ItemSearcher(IItemRepository* itemManager);
    
    /**
     * @brief 根据商品名称精确搜索
     * @param name 商品名称
     * @return 搜索结果列表
     */
    std::vector<std::shared_ptr<Item>> searchByNameExact(const std::string& name);
    
    /**
     * @brief 根据商品类别精确搜索
     * @param category 商品类别
     * @return 搜索结果列表
     */
    std::vector<std::shared_ptr<Item>> searchByCategoryExact(const std::string& category);
    
    /**
     * @brief 根据价格范围搜索
     * @param minPrice 最低价格
     * @param maxPrice 最高价格
     * @return 搜索结果列表
     */
    std::vector<std::shared_ptr<Item>> searchByPriceRange(double minPrice, double maxPrice);
    
    /**
     * @brief 模糊搜索（基于商品名称）
     * @param keyword 关键字
     * @param threshold 相似度阈值（0-1之间，默认0.5）
     * @return 搜索结果列表（按相似度降序排列）
     */
    std::vector<SearchResult> fuzzySearchByName(const std::string& keyword, double threshold = 0.5);
    
    /**
     * @brief 综合搜索（先精确后模糊）
     * @param keyword 关键字
     * @param searchType 搜索类型（使用SearchType枚举）
     * @return 搜索结果列表
     */
    std::vector<SearchResult> search(const std::string& keyword, SearchType searchType = SearchType::ALL);
    
    /**
     * @brief 显示搜索结果
     * @param results 搜索结果列表
     * @param showSimilarity 是否显示相似度分数
     */
    void displaySearchResults(const std::vector<SearchResult>& results, bool showSimilarity = false);
    
    /**
     * @brief 析构函数
     */
    ~ItemSearcher();
};

#endif // ITEM_SEARCHER_H
