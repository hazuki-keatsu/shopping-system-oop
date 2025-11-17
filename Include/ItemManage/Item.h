/**
 * @file Item.h
 * @brief 商品类的定义
 * @author Hazuki Keatsu
 * @date 2025-11-17
 */

#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <map>

/**
 * @class Item
 * @brief 商品类，表示系统中的商品信息
 * 
 * 商品信息包括ID、名称、类别、价格、描述、库存等
 * 支持动态字段扩展
 */
class Item {
private:
    std::string itemId;         // 商品ID（唯一标识）
    std::string itemName;       // 商品名称
    std::string category;       // 商品类别
    double price;               // 商品价格
    std::string description;    // 商品描述
    int stock;                  // 库存数量

public:
    /**
     * @brief 默认构造函数
     */
    Item();
    
    /**
     * @brief 构造函数
     * @param itemId 商品ID
     * @param itemName 商品名称
     * @param category 商品类别
     * @param price 商品价格
     * @param description 商品描述
     * @param stock 库存数量
     */
    Item(const std::string& itemId, 
         const std::string& itemName,
         const std::string& category,
         double price,
         const std::string& description,
         int stock);
    
    // Getter方法
    std::string getItemId() const { return itemId; }
    std::string getItemName() const { return itemName; }
    std::string getCategory() const { return category; }
    double getPrice() const { return price; }
    std::string getDescription() const { return description; }
    int getStock() const { return stock; }
    
    // Setter方法
    void setItemId(const std::string& id) { itemId = id; }
    void setItemName(const std::string& name) { itemName = name; }
    void setCategory(const std::string& cat) { category = cat; }
    void setPrice(double p) { price = p; }
    void setDescription(const std::string& desc) { description = desc; }
    void setStock(int s) { stock = s; }
    
    /**
     * @brief 析构函数
     */
    ~Item();
};

#endif // ITEM_H
