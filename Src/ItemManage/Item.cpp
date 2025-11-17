/**
 * @file Item.cpp
 * @brief 商品类的实现
 * @author Hazuki Keatsu
 * @date 2025-11-17
 */

#include "ItemManage/Item.h"

/**
 * @brief 默认构造函数实现
 */
Item::Item() 
    : itemId(""), itemName(""), category(""), price(0.0), description(""), stock(0) {
}

/**
 * @brief 构造函数实现
 */
Item::Item(const std::string& itemId, 
           const std::string& itemName,
           const std::string& category,
           double price,
           const std::string& description,
           int stock)
    : itemId(itemId), itemName(itemName), category(category),
      price(price), description(description), stock(stock) {
}

/**
 * @brief 析构函数
 */
Item::~Item() {
}
