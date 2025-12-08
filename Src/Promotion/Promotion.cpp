/**
 * @file Promotion.cpp
 * @brief 促销活动类的实现
 * @author Hazuki Keatsu
 * @date 2025-12-08
 */

#include "Promotion/Promotion.h"
#include <sstream>
#include <iomanip>

/**
 * @brief 默认构造函数实现
 */
Promotion::Promotion()
    : promotionId(""), promotionName(""), promotionType(PromotionType::DISCOUNT),
      isActive(false), startTime(0), endTime(0),
      targetItemId(""), discountRate(1.0),
      thresholdAmount(0.0), reductionAmount(0.0) {
}

/**
 * @brief 构造函数实现 - 折扣促销
 */
Promotion::Promotion(const std::string& promotionId,
                     const std::string& promotionName,
                     bool isActive,
                     time_t startTime,
                     time_t endTime,
                     const std::string& targetItemId,
                     double discountRate)
    : promotionId(promotionId), promotionName(promotionName),
      promotionType(PromotionType::DISCOUNT),
      isActive(isActive), startTime(startTime), endTime(endTime),
      targetItemId(targetItemId), discountRate(discountRate),
      thresholdAmount(0.0), reductionAmount(0.0) {
}

/**
 * @brief 构造函数实现 - 满减促销
 */
Promotion::Promotion(const std::string& promotionId,
                     const std::string& promotionName,
                     bool isActive,
                     time_t startTime,
                     time_t endTime,
                     double thresholdAmount,
                     double reductionAmount)
    : promotionId(promotionId), promotionName(promotionName),
      promotionType(PromotionType::FULL_REDUCTION),
      isActive(isActive), startTime(startTime), endTime(endTime),
      targetItemId(""), discountRate(1.0),
      thresholdAmount(thresholdAmount), reductionAmount(reductionAmount) {
}

/**
 * @brief 析构函数
 */
Promotion::~Promotion() {
}

/**
 * @brief 检查促销活动是否有效
 * 
 * 检查条件：
 * 1. isActive 必须为 true
 * 2. 当前时间必须在 startTime 和 endTime 之间
 */
bool Promotion::isValid() const {
    if (!isActive) {
        return false;
    }
    
    time_t currentTime = time(nullptr);
    return currentTime >= startTime && currentTime <= endTime;
}

/**
 * @brief 检查某个商品是否适用该促销
 * 
 * 仅对折扣促销有效：
 * - 如果 targetItemId 为 "-1"，表示全场适用
 * - 否则仅对匹配的商品ID适用
 */
bool Promotion::isApplicableToItem(const std::string& itemId) const {
    if (promotionType != PromotionType::DISCOUNT) {
        return false;
    }
    
    // "-1"表示全场折扣
    if (targetItemId == "-1") {
        return true;
    }
    
    return targetItemId == itemId;
}

/**
 * @brief 计算商品折扣后的价格
 * 
 * 折扣价格 = 原价 × 折扣率
 * 例如：原价100元，8折（0.8），折扣后为80元
 */
double Promotion::calculateDiscountForItem(double originalPrice) const {
    if (promotionType != PromotionType::DISCOUNT) {
        return originalPrice;
    }
    
    return originalPrice * discountRate;
}

/**
 * @brief 计算满减优惠金额
 * 
 * 如果订单总额达到门槛，返回减免金额
 * 否则返回0
 */
double Promotion::calculateReduction(double totalAmount) const {
    if (promotionType != PromotionType::FULL_REDUCTION) {
        return 0.0;
    }
    
    if (totalAmount >= thresholdAmount) {
        return reductionAmount;
    }
    
    return 0.0;
}

/**
 * @brief 获取促销标签文本
 * 
 * 折扣促销：显示为"X折"（如"8折"）
 * 满减促销：显示为"满X减Y"（如"满300减50"）
 */
std::string Promotion::getDisplayTag() const {
    std::ostringstream oss;
    
    if (promotionType == PromotionType::DISCOUNT) {
        // 将折扣率转换为折数（如0.8 -> 8折）
        int discountDisplay = static_cast<int>(discountRate * 10);
        oss << discountDisplay << "折";
    } else if (promotionType == PromotionType::FULL_REDUCTION) {
        // 格式化为"满X减Y"
        oss << "满" << static_cast<int>(thresholdAmount) 
            << "减" << static_cast<int>(reductionAmount);
    }
    
    return oss.str();
}
