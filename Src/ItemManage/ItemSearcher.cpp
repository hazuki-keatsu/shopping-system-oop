/**
 * @file ItemSearcher.cpp
 * @brief 商品搜索类的实现
 * @author Hazuki Keatsu
 * @date 2025-11-17
 */

#include "ItemManage/ItemSearcher.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <iomanip>

/**
 * @brief 构造函数实现
 */
ItemSearcher::ItemSearcher(IItemRepository* itemManager)
    : itemManager(itemManager) {
}

/**
 * @brief 转换为小写
 */
std::string ItemSearcher::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

/**
 * @brief 检查字符串是否包含子串（不区分大小写）
 */
bool ItemSearcher::containsIgnoreCase(const std::string& str, const std::string& substr) {
    std::string lowerStr = toLowerCase(str);
    std::string lowerSubstr = toLowerCase(substr);
    return lowerStr.find(lowerSubstr) != std::string::npos;
}

/**
 * @brief 计算Levenshtein编辑距离
 * 
 * 使用动态规划算法计算两个字符串之间的最小编辑次数
 */
int ItemSearcher::calculateLevenshteinDistance(const std::string& s1, const std::string& s2) {
    const size_t len1 = s1.size();
    const size_t len2 = s2.size();
    
    // 创建DP表
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1));
    
    // 初始化第一行和第一列
    for (size_t i = 0; i <= len1; ++i) {
        dp[i][0] = i;
    }
    for (size_t j = 0; j <= len2; ++j) {
        dp[0][j] = j;
    }
    
    // 填充DP表
    for (size_t i = 1; i <= len1; ++i) {
        for (size_t j = 1; j <= len2; ++j) {
            if (std::tolower(s1[i - 1]) == std::tolower(s2[j - 1])) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = std::min({
                    dp[i - 1][j] + 1,      // 删除
                    dp[i][j - 1] + 1,      // 插入
                    dp[i - 1][j - 1] + 1   // 替换
                });
            }
        }
    }
    
    return dp[len1][len2];
}

/**
 * @brief 计算字符串相似度
 * 
 * 相似度 = 1 - (编辑距离 / 较长字符串长度)
 */
double ItemSearcher::calculateSimilarity(const std::string& s1, const std::string& s2) {
    if (s1.empty() && s2.empty()) {
        return 1.0;
    }
    
    int distance = calculateLevenshteinDistance(s1, s2);
    size_t maxLen = std::max(s1.length(), s2.length());
    
    return 1.0 - (static_cast<double>(distance) / maxLen);
}

/**
 * @brief 根据商品名称精确搜索
 */
std::vector<std::shared_ptr<Item>> ItemSearcher::searchByNameExact(const std::string& name) {
    std::vector<std::shared_ptr<Item>> results;
    
    for (const auto& item : itemManager->getAllItems()) {
        // 不区分大小写的比较
        if (toLowerCase(item->getItemName()) == toLowerCase(name)) {
            results.push_back(item);
        }
    }
    
    return results;
}

/**
 * @brief 根据商品类别精确搜索
 */
std::vector<std::shared_ptr<Item>> ItemSearcher::searchByCategoryExact(const std::string& category) {
    return itemManager->getItemsByCategory(category);
}

/**
 * @brief 根据价格范围搜索
 */
std::vector<std::shared_ptr<Item>> ItemSearcher::searchByPriceRange(double minPrice, double maxPrice) {
    std::vector<std::shared_ptr<Item>> results;
    
    for (const auto& item : itemManager->getAllItems()) {
        double price = item->getPrice();
        if (price >= minPrice && price <= maxPrice) {
            results.push_back(item);
        }
    }
    
    return results;
}

/**
 * @brief 模糊搜索（基于商品名称）
 */
std::vector<SearchResult> ItemSearcher::fuzzySearchByName(const std::string& keyword, double threshold) {
    std::vector<SearchResult> results;
    
    // 对所有商品计算相似度
    for (const auto& item : itemManager->getAllItems()) {
        // 计算与商品名称的相似度
        double nameSimilarity = calculateSimilarity(keyword, item->getItemName());
        
        // 也检查是否包含关键字（部分匹配）
        if (containsIgnoreCase(item->getItemName(), keyword)) {
            nameSimilarity = std::max(nameSimilarity, 0.7);  // 包含关键字至少给0.7分
        }
        
        // 检查描述中是否包含关键字
        if (containsIgnoreCase(item->getDescription(), keyword)) {
            nameSimilarity = std::max(nameSimilarity, 0.5);  // 描述包含关键字给0.5分
        }
        
        // 如果相似度超过阈值，加入结果
        if (nameSimilarity >= threshold) {
            results.push_back(SearchResult(item, nameSimilarity));
        }
    }
    
    // 按相似度降序排序
    std::sort(results.begin(), results.end(),
        [](const SearchResult& a, const SearchResult& b) {
            return a.similarityScore > b.similarityScore;
        });
    
    return results;
}

/**
 * @brief 综合搜索（先精确后模糊）
 */
std::vector<SearchResult> ItemSearcher::search(const std::string& keyword, SearchType searchType) {
    std::vector<SearchResult> results;
    
    if (searchType == SearchType::BY_NAME || searchType == SearchType::ALL) {
        // 先尝试精确搜索
        auto exactResults = searchByNameExact(keyword);
        if (!exactResults.empty()) {
            std::cout << "找到 " << exactResults.size() << " 个精确匹配结果（按名称）。" << std::endl;
            for (const auto& item : exactResults) {
                results.push_back(SearchResult(item, 1.0));  // 精确匹配相似度为1.0
            }
            return results;
        }
    }
    
    if (searchType == SearchType::BY_CATEGORY || searchType == SearchType::ALL) {
        // 尝试按类别搜索
        auto categoryResults = searchByCategoryExact(keyword);
        if (!categoryResults.empty()) {
            std::cout << "找到 " << categoryResults.size() << " 个类别匹配结果。" << std::endl;
            for (const auto& item : categoryResults) {
                results.push_back(SearchResult(item, 1.0));
            }
            return results;
        }
    }

    if (searchType == SearchType::BY_PRICE) {
        // 尝试按价格区间搜索
        // 期望格式: "最小价格-最大价格" 例如: "1000-5000"
        try {
            size_t dashPos = keyword.find('-');
            if (dashPos != std::string::npos) {
                std::string minPriceStr = keyword.substr(0, dashPos);
                std::string maxPriceStr = keyword.substr(dashPos + 1);
                
                double minPrice = std::stod(minPriceStr);
                double maxPrice = std::stod(maxPriceStr);
                
                auto priceResults = searchByPriceRange(minPrice, maxPrice);
                if (!priceResults.empty()) {
                    std::cout << "找到 " << priceResults.size() << " 个价格区间匹配结果（" 
                              << minPrice << " - " << maxPrice << "元）。" << std::endl;
                    for (const auto& item : priceResults) {
                        results.push_back(SearchResult(item, 1.0));
                    }
                    return results;
                } else {
                    std::cout << "在价格区间 " << minPrice << " - " << maxPrice 
                              << " 元内未找到商品。" << std::endl;
                    return results;
                }
            } else {
                std::cout << "价格区间格式错误！请使用格式：最小价格-最大价格（例如：1000-5000）" << std::endl;
                return results;
            }
        } catch (const std::exception& e) {
            std::cout << "价格解析失败！请输入有效的数字。" << std::endl;
            return results;
        }
    }
    
    // 如果精确搜索无结果，进行模糊搜索
    std::cout << "精确搜索无结果，正在进行模糊搜索..." << std::endl;
    results = fuzzySearchByName(keyword, 0.4);  // 降低阈值以获得更多结果
    
    if (!results.empty()) {
        std::cout << "找到 " << results.size() << " 个模糊匹配结果。" << std::endl;
    } else {
        std::cout << "未找到相关商品。" << std::endl;
    }
    
    return results;
}

/**
 * @brief 显示搜索结果
 */
void ItemSearcher::displaySearchResults(const std::vector<SearchResult>& results, bool showSimilarity) {
    if (results.empty()) {
        std::cout << "没有找到相关商品。" << std::endl;
        return;
    }
    
    std::cout << "\n========== 搜索结果 ==========" << std::endl;
    std::cout << std::left
              << std::setw(8) << "ID"
              << std::setw(20) << "名称"
              << std::setw(12) << "类别"
              << std::setw(10) << "价格"
              << std::setw(8) << "库存";
    
    if (showSimilarity) {
        std::cout << std::setw(10) << "相似度";
    }
    
    std::cout << std::endl;
    std::cout << "-------------------------------------------------------------------------------------" << std::endl;
    
    for (const auto& result : results) {
        const auto& item = result.item;
        std::cout << std::left
                  << std::setw(8) << item->getItemId()
                  << std::setw(20) << item->getItemName()
                  << std::setw(12) << item->getCategory()
                  << std::setw(10) << std::fixed << std::setprecision(2) << item->getPrice()
                  << std::setw(8) << item->getStock();
        
        if (showSimilarity) {
            std::cout << std::setw(10) << std::fixed << std::setprecision(2) 
                      << (result.similarityScore * 100) << "%";
        }
        
        std::cout << std::endl;
    }
    
    std::cout << "=============================" << std::endl;
    std::cout << "共找到 " << results.size() << " 件商品。" << std::endl;
}

/**
 * @brief 析构函数
 */
ItemSearcher::~ItemSearcher() {
}
