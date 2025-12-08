/**
 * @file Promotion.h
 * @brief 促销活动类的定义
 * @author Hazuki Keatsu
 * @date 2025-12-08
 */

#ifndef PROMOTION_H
#define PROMOTION_H

#include <string>
#include <ctime>

/**
 * @enum PromotionType
 * @brief 促销活动类型枚举
 */
enum class PromotionType {
    DISCOUNT,       // 折扣促销
    FULL_REDUCTION  // 满减促销
};

/**
 * @class Promotion
 * @brief 促销活动类，表示系统中的促销活动信息
 * 
 * 促销活动包括两种类型：
 * 1. 限时折扣：对特定商品或全场商品应用折扣率
 * 2. 满减活动：订单总额达到门槛金额时减免一定金额
 * 
 * 所有促销活动都有有效期限制
 */
class Promotion {
private:
    std::string promotionId;        // 促销活动ID（唯一标识）
    std::string promotionName;      // 促销名称
    PromotionType promotionType;    // 促销类型
    bool isActive;                  // 是否启用
    time_t startTime;               // 有效期开始时间
    time_t endTime;                 // 有效期结束时间
    
    // 折扣促销特有字段
    std::string targetItemId;       // 目标商品ID（空表示全场）
    double discountRate;            // 折扣率（如0.8表示8折）
    
    // 满减促销特有字段
    double thresholdAmount;         // 满减门槛金额
    double reductionAmount;         // 减免金额

public:
    /**
     * @brief 默认构造函数
     */
    Promotion();
    
    /**
     * @brief 构造函数 - 折扣促销
     * @param promotionId 促销活动ID
     * @param promotionName 促销名称
     * @param isActive 是否启用
     * @param startTime 有效期开始时间
     * @param endTime 有效期结束时间
     * @param targetItemId 目标商品ID（"-1"表示全场）
     * @param discountRate 折扣率
     */
    Promotion(const std::string& promotionId,
              const std::string& promotionName,
              bool isActive,
              time_t startTime,
              time_t endTime,
              const std::string& targetItemId,
              double discountRate);
    
    /**
     * @brief 构造函数 - 满减促销
     * @param promotionId 促销活动ID
     * @param promotionName 促销名称
     * @param isActive 是否启用
     * @param startTime 有效期开始时间
     * @param endTime 有效期结束时间
     * @param thresholdAmount 满减门槛金额
     * @param reductionAmount 减免金额
     */
    Promotion(const std::string& promotionId,
              const std::string& promotionName,
              bool isActive,
              time_t startTime,
              time_t endTime,
              double thresholdAmount,
              double reductionAmount);
    
    /**
     * @brief 析构函数
     */
    ~Promotion();
    
    // Getter方法
    std::string getPromotionId() const { return promotionId; }
    std::string getPromotionName() const { return promotionName; }
    PromotionType getPromotionType() const { return promotionType; }
    bool getIsActive() const { return isActive; }
    time_t getStartTime() const { return startTime; }
    time_t getEndTime() const { return endTime; }
    std::string getTargetItemId() const { return targetItemId; }
    double getDiscountRate() const { return discountRate; }
    double getThresholdAmount() const { return thresholdAmount; }
    double getReductionAmount() const { return reductionAmount; }
    
    // Setter方法
    void setPromotionId(const std::string& id) { promotionId = id; }
    void setPromotionName(const std::string& name) { promotionName = name; }
    void setIsActive(bool active) { isActive = active; }
    void setStartTime(time_t time) { startTime = time; }
    void setEndTime(time_t time) { endTime = time; }
    void setTargetItemId(const std::string& id) { targetItemId = id; }
    void setDiscountRate(double rate) { discountRate = rate; }
    void setThresholdAmount(double amount) { thresholdAmount = amount; }
    void setReductionAmount(double amount) { reductionAmount = amount; }
    
    /**
     * @brief 检查促销活动是否有效
     * 
     * 有效的条件：
     * 1. isActive为true
     * 2. 当前时间在有效期内
     * 
     * @return 有效返回true，否则返回false
     */
    bool isValid() const;
    
    /**
     * @brief 检查某个商品是否适用该促销
     * 
     * 仅对折扣促销有效：
     * 1. 如果targetItemId为空，表示全场适用
     * 2. 否则仅对指定商品ID适用
     * 
     * @param itemId 商品ID
     * @return 适用返回true，否则返回false
     */
    bool isApplicableToItem(const std::string& itemId) const;
    
    /**
     * @brief 计算商品折扣后的价格
     * 
     * 仅对折扣促销有效
     * 
     * @param originalPrice 原价
     * @return 折扣后的价格
     */
    double calculateDiscountForItem(double originalPrice) const;
    
    /**
     * @brief 计算满减优惠金额
     * 
     * 仅对满减促销有效
     * 如果总额达到门槛，返回减免金额，否则返回0
     * 
     * @param totalAmount 订单总额
     * @return 减免金额
     */
    double calculateReduction(double totalAmount) const;
    
    /**
     * @brief 获取促销标签文本
     * 
     * 用于在商品列表中显示促销信息
     * 折扣促销：返回"X折"
     * 满减促销：返回"满X减Y"
     * 
     * @return 促销标签文本
     */
    std::string getDisplayTag() const;
};

#endif // PROMOTION_H
