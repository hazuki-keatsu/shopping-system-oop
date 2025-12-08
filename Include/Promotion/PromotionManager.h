/**
 * @file PromotionManager.h
 * @brief 促销管理器类的定义
 * @author Hazuki Keatsu
 * @date 2025-12-08
 */

#ifndef PROMOTION_MANAGER_H
#define PROMOTION_MANAGER_H

#include "Promotion/Promotion.h"
#include "ItemManage/Item.h"
#include <vector>
#include <memory>
#include <string>

/**
 * @struct PromotionResult
 * @brief 促销计算结果结构体
 * 
 * 用于封装促销计算的详细结果，包括原价、折扣、满减等信息
 */
struct PromotionResult {
    double originalTotal;           // 原价总额
    double afterDiscountTotal;      // 折扣后总额
    double finalTotal;              // 最终支付总额（折扣+满减后）
    double totalSavings;            // 总节省金额
    std::vector<std::string> appliedPromotions;  // 应用的促销描述列表
    std::vector<std::pair<std::string, double>> itemDiscounts;  // 商品折扣明细（商品名，折扣金额）
    double totalReduction;          // 满减总金额
};

/**
 * @class PromotionManager
 * @brief 促销管理器类，负责管理所有促销活动
 * 
 * 主要功能：
 * 1. 从CSV文件加载和保存促销数据
 * 2. 添加、删除、修改促销活动
 * 3. 查询有效的促销活动
 * 4. 计算订单的促销优惠
 */
class PromotionManager {
private:
    std::vector<std::shared_ptr<Promotion>> promotions;  // 促销活动列表
    std::string filePath;                                 // 数据文件路径
    
    /**
     * @brief 去除字符串首尾空格
     * @param str 待处理的字符串
     * @return 处理后的字符串
     */
    std::string trim(const std::string& str);
    
    /**
     * @brief 解析CSV行
     * @param line CSV文件的一行
     * @return 字段向量
     */
    std::vector<std::string> parseCSVLine(const std::string& line);
    
    /**
     * @brief 将时间戳转换为字符串（用于保存）
     * @param time 时间戳
     * @return 时间字符串
     */
    std::string timeToString(time_t time);
    
    /**
     * @brief 将字符串转换为时间戳（用于加载）
     * @param str 时间字符串
     * @return 时间戳
     */
    time_t stringToTime(const std::string& str);

public:
    /**
     * @brief 构造函数
     * @param filePath 促销数据文件路径
     */
    PromotionManager(const std::string& filePath);
    
    /**
     * @brief 析构函数
     */
    ~PromotionManager();
    
    /**
     * @brief 从CSV文件加载促销数据
     * @return 加载成功返回true，否则返回false
     */
    bool loadFromFile();
    
    /**
     * @brief 保存促销数据到CSV文件
     * @return 保存成功返回true，否则返回false
     */
    bool saveToFile();
    
    /**
     * @brief 添加促销活动
     * @param promotion 促销活动对象
     * @return 添加成功返回true，否则返回false
     */
    bool addPromotion(std::shared_ptr<Promotion> promotion);
    
    /**
     * @brief 删除促销活动
     * @param promotionId 促销活动ID
     * @return 删除成功返回true，否则返回false
     */
    bool deletePromotion(const std::string& promotionId);
    
    /**
     * @brief 更新促销活动
     * @param promotion 更新后的促销活动对象
     * @return 更新成功返回true，否则返回false
     */
    bool updatePromotion(std::shared_ptr<Promotion> promotion);
    
    /**
     * @brief 修改促销名称
     * @param promotionId 促销活动ID
     * @param newName 新名称
     * @return 修改成功返回true，否则返回false
     */
    bool updatePromotionName(const std::string& promotionId, const std::string& newName);
    
    /**
     * @brief 修改促销有效期
     * @param promotionId 促销活动ID
     * @param newStartTime 新的开始时间
     * @param newEndTime 新的结束时间
     * @return 修改成功返回true，否则返回false
     */
    bool updatePromotionTime(const std::string& promotionId, time_t newStartTime, time_t newEndTime);
    
    /**
     * @brief 修改折扣促销的折扣率
     * @param promotionId 促销活动ID
     * @param newRate 新的折扣率
     * @return 修改成功返回true，否则返回false（仅对折扣促销有效）
     */
    bool updateDiscountRate(const std::string& promotionId, double newRate);
    
    /**
     * @brief 修改折扣促销的目标商品
     * @param promotionId 促销活动ID
     * @param newItemId 新的目标商品ID（空字符串表示全场）
     * @return 修改成功返回true，否则返回false（仅对折扣促销有效）
     */
    bool updateDiscountTargetItem(const std::string& promotionId, const std::string& newItemId);
    
    /**
     * @brief 修改满减促销的门槛金额
     * @param promotionId 促销活动ID
     * @param newThreshold 新的门槛金额
     * @return 修改成功返回true，否则返回false（仅对满减促销有效）
     */
    bool updateFullReductionThreshold(const std::string& promotionId, double newThreshold);
    
    /**
     * @brief 修改满减促销的减免金额
     * @param promotionId 促销活动ID
     * @param newReduction 新的减免金额
     * @return 修改成功返回true，否则返回false（仅对满减促销有效）
     */
    bool updateFullReductionAmount(const std::string& promotionId, double newReduction);
    
    /**
     * @brief 启用或禁用促销活动
     * @param promotionId 促销活动ID
     * @param isActive 是否启用
     * @return 修改成功返回true，否则返回false
     */
    bool setPromotionActive(const std::string& promotionId, bool isActive);
    
    /**
     * @brief 根据ID查找促销活动
     * @param promotionId 促销活动ID
     * @return 找到返回促销活动对象，否则返回nullptr
     */
    std::shared_ptr<Promotion> findPromotionById(const std::string& promotionId);
    
    /**
     * @brief 获取某个商品当前有效的折扣促销
     * 
     * 如果有多个有效的折扣促销，返回折扣率最低的（优惠最大的）
     * 
     * @param itemId 商品ID
     * @return 有效的折扣促销对象，如果没有返回nullptr
     */
    std::shared_ptr<Promotion> getActiveDiscountForItem(const std::string& itemId);
    
    /**
     * @brief 获取当前所有有效的满减促销
     * @return 有效的满减促销列表
     */
    std::vector<std::shared_ptr<Promotion>> getActiveFullReductions();
    
    /**
     * @brief 计算一组商品的促销优惠结果
     * 
     * 计算流程：
     * 1. 遍历商品，计算每个商品的折扣价
     * 2. 累加得到折扣后总额
     * 3. 应用满减优惠
     * 4. 返回详细的促销结果
     * 
     * @param items 商品及数量的列表
     * @return 促销计算结果
     */
    PromotionResult calculatePromotionResult(
        const std::vector<std::pair<std::shared_ptr<Item>, int>>& items);
    
    /**
     * @brief 显示所有促销活动
     */
    void displayAllPromotions();
    
    /**
     * @brief 显示有效的促销活动
     */
    void displayActivePromotions();
    
    /**
     * @brief 获取所有促销活动
     * @return 促销活动列表
     */
    const std::vector<std::shared_ptr<Promotion>>& getAllPromotions() const {
        return promotions;
    }
    
    /**
     * @brief 生成新的促销活动ID
     * @return 新的促销活动ID
     */
    std::string generatePromotionId();
};

#endif // PROMOTION_MANAGER_H
