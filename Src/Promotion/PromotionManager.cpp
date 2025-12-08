/**
 * @file PromotionManager.cpp
 * @brief 促销管理器类的实现
 * @author Hazuki Keatsu
 * @date 2025-12-08
 */

#include "Promotion/PromotionManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

/**
 * @brief 构造函数实现
 */
PromotionManager::PromotionManager(const std::string& filePath)
    : filePath(filePath) {
}

/**
 * @brief 析构函数
 */
PromotionManager::~PromotionManager() {
}

/**
 * @brief 去除字符串首尾空格
 */
std::string PromotionManager::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

/**
 * @brief 解析CSV行
 * 
 * 按逗号分隔字段，并去除每个字段的首尾空格
 */
std::vector<std::string> PromotionManager::parseCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    
    while (std::getline(ss, field, ',')) {
        fields.push_back(trim(field));
    }
    
    return fields;
}

/**
 * @brief 将时间戳转换为字符串
 */
std::string PromotionManager::timeToString(time_t time) {
    return std::to_string(time);
}

/**
 * @brief 将字符串转换为时间戳
 */
time_t PromotionManager::stringToTime(const std::string& str) {
    if (str.empty()) {
        return 0;
    }
    try {
        return static_cast<time_t>(std::stoll(str));
    } catch (...) {
        return 0;
    }
}

/**
 * @brief 从CSV文件加载促销数据
 * 
 * CSV格式：
 * promotion_id,promotion_name,promotion_type,is_active,start_time,end_time,
 * target_item_id,discount_rate,threshold_amount,reduction_amount
 */
bool PromotionManager::loadFromFile() {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "警告: 无法打开促销数据文件: " << filePath << std::endl;
        return false;
    }
    
    promotions.clear();
    std::string line;
    
    // 跳过表头
    std::getline(file, line);
    
    // 逐行读取数据
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::vector<std::string> fields = parseCSVLine(line);
        if (fields.size() < 10) {
            std::cout << fields[0] << std::endl;
            continue;
        }
        
        std::string promotionId = fields[0];
        std::string promotionName = fields[1];
        std::string typeStr = fields[2];
        bool isActive = (fields[3] == "1" || fields[3] == "true");
        time_t startTime = stringToTime(fields[4]);
        time_t endTime = stringToTime(fields[5]);
        
        std::shared_ptr<Promotion> promotion;
        
        if (typeStr == "DISCOUNT") {
            // 折扣促销
            std::string targetItemId = fields[6];
            double discountRate = fields[7].empty() ? 1.0 : std::stod(fields[7]);
            
            promotion = std::make_shared<Promotion>(
                promotionId, promotionName, isActive,
                startTime, endTime, targetItemId, discountRate
            );
        } else if (typeStr == "FULL_REDUCTION") {
            // 满减促销
            double thresholdAmount = fields[8].empty() ? 0.0 : std::stod(fields[8]);
            double reductionAmount = fields[9].empty() ? 0.0 : std::stod(fields[9]);
            
            promotion = std::make_shared<Promotion>(
                promotionId, promotionName, isActive,
                startTime, endTime, thresholdAmount, reductionAmount
            );
        }
        
        if (promotion) {
            promotions.push_back(promotion);
        }
    }
    
    std::cout << "成功加载 " << promotions.size() << " 个促销信息。" << std::endl;
    file.close();
    return true;
}

/**
 * @brief 保存促销数据到CSV文件
 */
bool PromotionManager::saveToFile() {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "错误: 无法写入促销数据文件: " << filePath << std::endl;
        return false;
    }
    
    // 写入表头
    file << "promotion_id,promotion_name,promotion_type,is_active,start_time,end_time,"
         << "target_item_id,discount_rate,threshold_amount,reduction_amount\n";
    
    // 写入数据
    for (const auto& promotion : promotions) {
        file << promotion->getPromotionId() << ","
             << promotion->getPromotionName() << ",";
        
        if (promotion->getPromotionType() == PromotionType::DISCOUNT) {
            file << "DISCOUNT,";
        } else {
            file << "FULL_REDUCTION,";
        }
        
        file << (promotion->getIsActive() ? "1" : "0") << ","
             << timeToString(promotion->getStartTime()) << ","
             << timeToString(promotion->getEndTime()) << ",";
        
        if (promotion->getPromotionType() == PromotionType::DISCOUNT) {
            file << promotion->getTargetItemId() << ","
                 << promotion->getDiscountRate() << "_,_,";
        } else {
            file << ",_,_"
                 << promotion->getThresholdAmount() << ","
                 << promotion->getReductionAmount();
        }
        
        file << "\n";
    }
    
    file.close();
    return true;
}

/**
 * @brief 添加促销活动
 */
bool PromotionManager::addPromotion(std::shared_ptr<Promotion> promotion) {
    if (!promotion) {
        return false;
    }
    
    // 检查ID是否已存在
    for (const auto& p : promotions) {
        if (p->getPromotionId() == promotion->getPromotionId()) {
            std::cerr << "错误: 促销活动ID已存在: " 
                      << promotion->getPromotionId() << std::endl;
            return false;
        }
    }
    
    promotions.push_back(promotion);
    return saveToFile();
}

/**
 * @brief 删除促销活动
 */
bool PromotionManager::deletePromotion(const std::string& promotionId) {
    auto it = std::remove_if(promotions.begin(), promotions.end(),
        [&promotionId](const std::shared_ptr<Promotion>& p) {
            return p->getPromotionId() == promotionId;
        });
    
    if (it == promotions.end()) {
        std::cerr << "错误: 未找到促销活动ID: " << promotionId << std::endl;
        return false;
    }
    
    promotions.erase(it, promotions.end());
    return saveToFile();
}

/**
 * @brief 更新促销活动
 */
bool PromotionManager::updatePromotion(std::shared_ptr<Promotion> promotion) {
    if (!promotion) {
        return false;
    }
    
    for (auto& p : promotions) {
        if (p->getPromotionId() == promotion->getPromotionId()) {
            p = promotion;
            return saveToFile();
        }
    }
    
    std::cerr << "错误: 未找到促销活动ID: " 
              << promotion->getPromotionId() << std::endl;
    return false;
}

/**
 * @brief 修改促销名称
 */
bool PromotionManager::updatePromotionName(const std::string& promotionId, const std::string& newName) {
    auto promotion = findPromotionById(promotionId);
    if (!promotion) {
        std::cerr << "错误: 未找到促销活动ID: " << promotionId << std::endl;
        return false;
    }
    
    promotion->setPromotionName(newName);
    return saveToFile();
}

/**
 * @brief 修改促销有效期
 */
bool PromotionManager::updatePromotionTime(const std::string& promotionId, time_t newStartTime, time_t newEndTime) {
    auto promotion = findPromotionById(promotionId);
    if (!promotion) {
        std::cerr << "错误: 未找到促销活动ID: " << promotionId << std::endl;
        return false;
    }
    
    if (newEndTime <= newStartTime) {
        std::cerr << "错误: 结束时间必须晚于开始时间！" << std::endl;
        return false;
    }
    
    promotion->setStartTime(newStartTime);
    promotion->setEndTime(newEndTime);
    return saveToFile();
}

/**
 * @brief 修改折扣促销的折扣率
 */
bool PromotionManager::updateDiscountRate(const std::string& promotionId, double newRate) {
    auto promotion = findPromotionById(promotionId);
    if (!promotion) {
        std::cerr << "错误: 未找到促销活动ID: " << promotionId << std::endl;
        return false;
    }
    
    if (promotion->getPromotionType() != PromotionType::DISCOUNT) {
        std::cerr << "错误: 该促销不是折扣促销！" << std::endl;
        return false;
    }
    
    if (newRate <= 0 || newRate >= 1) {
        std::cerr << "错误: 折扣率必须在0到1之间！" << std::endl;
        return false;
    }
    
    promotion->setDiscountRate(newRate);
    return saveToFile();
}

/**
 * @brief 修改折扣促销的目标商品
 */
bool PromotionManager::updateDiscountTargetItem(const std::string& promotionId, const std::string& newItemId) {
    auto promotion = findPromotionById(promotionId);
    if (!promotion) {
        std::cerr << "错误: 未找到促销活动ID: " << promotionId << std::endl;
        return false;
    }
    
    if (promotion->getPromotionType() != PromotionType::DISCOUNT) {
        std::cerr << "错误: 该促销不是折扣促销！" << std::endl;
        return false;
    }
    
    promotion->setTargetItemId(newItemId);
    return saveToFile();
}

/**
 * @brief 修改满减促销的门槛金额
 */
bool PromotionManager::updateFullReductionThreshold(const std::string& promotionId, double newThreshold) {
    auto promotion = findPromotionById(promotionId);
    if (!promotion) {
        std::cerr << "错误: 未找到促销活动ID: " << promotionId << std::endl;
        return false;
    }
    
    if (promotion->getPromotionType() != PromotionType::FULL_REDUCTION) {
        std::cerr << "错误: 该促销不是满减促销！" << std::endl;
        return false;
    }
    
    if (newThreshold <= 0) {
        std::cerr << "错误: 门槛金额必须大于0！" << std::endl;
        return false;
    }
    
    if (newThreshold <= promotion->getReductionAmount()) {
        std::cerr << "错误: 门槛金额必须大于减免金额！" << std::endl;
        return false;
    }
    
    promotion->setThresholdAmount(newThreshold);
    return saveToFile();
}

/**
 * @brief 修改满减促销的减免金额
 */
bool PromotionManager::updateFullReductionAmount(const std::string& promotionId, double newReduction) {
    auto promotion = findPromotionById(promotionId);
    if (!promotion) {
        std::cerr << "错误: 未找到促销活动ID: " << promotionId << std::endl;
        return false;
    }
    
    if (promotion->getPromotionType() != PromotionType::FULL_REDUCTION) {
        std::cerr << "错误: 该促销不是满减促销！" << std::endl;
        return false;
    }
    
    if (newReduction <= 0) {
        std::cerr << "错误: 减免金额必须大于0！" << std::endl;
        return false;
    }
    
    if (newReduction >= promotion->getThresholdAmount()) {
        std::cerr << "错误: 减免金额必须小于门槛金额！" << std::endl;
        return false;
    }
    
    promotion->setReductionAmount(newReduction);
    return saveToFile();
}

/**
 * @brief 启用或禁用促销活动
 */
bool PromotionManager::setPromotionActive(const std::string& promotionId, bool isActive) {
    auto promotion = findPromotionById(promotionId);
    if (!promotion) {
        std::cerr << "错误: 未找到促销活动ID: " << promotionId << std::endl;
        return false;
    }
    
    promotion->setIsActive(isActive);
    return saveToFile();
}

/**
 * @brief 根据ID查找促销活动
 */
std::shared_ptr<Promotion> PromotionManager::findPromotionById(
    const std::string& promotionId) {
    for (const auto& p : promotions) {
        if (p->getPromotionId() == promotionId) {
            return p;
        }
    }
    return nullptr;
}

/**
 * @brief 获取某个商品当前有效的折扣促销
 * 
 * 如果有多个有效折扣，返回折扣率最低的（优惠最大）
 */
std::shared_ptr<Promotion> PromotionManager::getActiveDiscountForItem(
    const std::string& itemId) {
    std::shared_ptr<Promotion> bestDiscount = nullptr;
    double bestRate = 1.0;
    
    for (const auto& p : promotions) {
        if (p->getPromotionType() == PromotionType::DISCOUNT &&
            p->isValid() &&
            p->isApplicableToItem(itemId)) {
            
            if (p->getDiscountRate() < bestRate) {
                bestRate = p->getDiscountRate();
                bestDiscount = p;
            }
        }
    }
    
    return bestDiscount;
}

/**
 * @brief 获取当前所有有效的满减促销
 */
std::vector<std::shared_ptr<Promotion>> PromotionManager::getActiveFullReductions() {
    std::vector<std::shared_ptr<Promotion>> reductions;
    
    for (const auto& p : promotions) {
        if (p->getPromotionType() == PromotionType::FULL_REDUCTION &&
            p->isValid()) {
            reductions.push_back(p);
        }
    }
    
    // 按门槛金额升序排序
    std::sort(reductions.begin(), reductions.end(),
        [](const std::shared_ptr<Promotion>& a, const std::shared_ptr<Promotion>& b) {
            return a->getThresholdAmount() < b->getThresholdAmount();
        });
    
    return reductions;
}

/**
 * @brief 计算一组商品的促销优惠结果
 * 
 * 计算流程：
 * 1. 计算每个商品的折扣价
 * 2. 累加得到折扣后总额
 * 3. 应用满减优惠（可叠加多档）
 * 4. 返回详细结果
 */
PromotionResult PromotionManager::calculatePromotionResult(
    const std::vector<std::pair<std::shared_ptr<Item>, int>>& items) {
    
    PromotionResult result;
    result.originalTotal = 0.0;
    result.afterDiscountTotal = 0.0;
    result.totalReduction = 0.0;
    
    // 第一步：计算折扣
    for (const auto& [item, quantity] : items) {
        double itemOriginalPrice = item->getPrice() * quantity;
        result.originalTotal += itemOriginalPrice;
        
        // 查找商品的折扣促销
        auto discount = getActiveDiscountForItem(item->getItemId());
        
        if (discount) {
            double discountedPrice = discount->calculateDiscountForItem(item->getPrice()) * quantity;
            result.afterDiscountTotal += discountedPrice;
            
            // 记录折扣明细
            double savingsForItem = itemOriginalPrice - discountedPrice;
            result.itemDiscounts.push_back({item->getItemName(), savingsForItem});
            
            // 记录应用的促销
            std::ostringstream oss;
            oss << item->getItemName() << " " << discount->getDisplayTag();
            result.appliedPromotions.push_back(oss.str());
        } else {
            result.afterDiscountTotal += itemOriginalPrice;
        }
    }
    
    // 第二步：计算满减
    auto fullReductions = getActiveFullReductions();
    for (const auto& reduction : fullReductions) {
        double reductionAmount = reduction->calculateReduction(result.afterDiscountTotal);
        if (reductionAmount > 0) {
            result.totalReduction += reductionAmount;
            result.appliedPromotions.push_back(reduction->getDisplayTag());
        }
    }
    
    // 计算最终金额
    result.finalTotal = result.afterDiscountTotal - result.totalReduction;
    result.totalSavings = result.originalTotal - result.finalTotal;
    
    return result;
}

/**
 * @brief 显示所有促销活动
 */
void PromotionManager::displayAllPromotions() {
    if (promotions.empty()) {
        std::cout << "暂无促销活动" << std::endl;
        return;
    }
    
    std::cout << "\n========== 所有促销活动 ==========\n";
    std::cout << std::left << std::setw(12) << "促销ID"
              << std::setw(20) << "促销名称"
              << std::setw(15) << "类型"
              << std::setw(10) << "状态"
              << std::setw(15) << "有效期至"
              << std::endl;
    std::cout << std::string(72, '-') << std::endl;
    
    for (const auto& p : promotions) {
        std::string typeStr = (p->getPromotionType() == PromotionType::DISCOUNT) 
                              ? "折扣促销" : "满减促销";
        std::string statusStr = p->getIsActive() ? "启用" : "禁用";
        
        time_t endTime = p->getEndTime();
        struct tm* timeinfo = localtime(&endTime);
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
        
        std::cout << std::left << std::setw(12) << p->getPromotionId()
                  << std::setw(20) << p->getPromotionName()
                  << std::setw(15) << typeStr
                  << std::setw(10) << statusStr
                  << std::setw(15) << buffer
                  << std::endl;
    }
    std::cout << "================================\n";
}

/**
 * @brief 显示有效的促销活动
 */
void PromotionManager::displayActivePromotions() {
    std::vector<std::shared_ptr<Promotion>> activePromotions;
    
    for (const auto& p : promotions) {
        if (p->isValid()) {
            activePromotions.push_back(p);
        }
    }
    
    if (activePromotions.empty()) {
        std::cout << "当前无有效的促销活动" << std::endl;
        return;
    }
    
    std::cout << "\n========== 当前有效促销 ==========\n";
    for (const auto& p : activePromotions) {
        std::cout << "• " << p->getPromotionName() 
                  << " [" << p->getDisplayTag() << "]" << std::endl;
    }
    std::cout << "================================\n";
}

/**
 * @brief 生成新的促销活动ID
 * 
 * 格式：PROMO + 三位数字（从001开始递增）
 */
std::string PromotionManager::generatePromotionId() {
    int maxNum = 0;
    
    for (const auto& p : promotions) {
        std::string id = p->getPromotionId();
        if (id.length() >= 8 && id.substr(0, 5) == "PROMO") {
            try {
                int num = std::stoi(id.substr(5));
                if (num > maxNum) {
                    maxNum = num;
                }
            } catch (...) {
                // 忽略解析错误
            }
        }
    }
    
    std::ostringstream oss;
    oss << "PROMO" << std::setfill('0') << std::setw(3) << (maxNum + 1);
    return oss.str();
}
