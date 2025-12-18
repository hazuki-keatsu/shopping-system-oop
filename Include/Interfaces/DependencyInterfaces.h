/**
 * @file 依赖接口
 * @brief 核心依赖接口用来实现类之间的依赖注入，提升代码解耦性
 */

#ifndef DEPENDENCY_INTERFACES_H
#define DEPENDENCY_INTERFACES_H

#include <memory>
#include <string>
#include <vector>
#include "UserManage/User.h"
#include "ItemManage/Item.h"

/**
 * @brief 用于身份验证和路径的配置 Abstraction Provider
 */
class IConfigProvider {
public:
    virtual ~IConfigProvider() = default;
    virtual std::string getAdminUsername() const = 0;
    virtual std::string getAdminPassword() const = 0;
};

/**
 * @brief 用于身份验证和报告的用户存储库抽象
 */
class IUserRepository {
public:
    virtual ~IUserRepository() = default;
    virtual bool loadFromFile() = 0;
    virtual bool saveToFile() = 0;
    virtual bool addCustomer(std::shared_ptr<Customer> customer) = 0;
    virtual std::shared_ptr<Customer> findCustomer(const std::string& username) = 0;
    virtual bool isUsernameExists(const std::string& username) = 0;
    virtual bool updatePassword(const std::string& username, const std::string& newPassword) = 0;
    virtual const std::vector<std::shared_ptr<Customer>>& getCustomers() const = 0;
};

/**
 * @brief 被搜索、订单、购物车和报表使用的商品仓库抽象
 */
class IItemRepository {
public:
    virtual ~IItemRepository() = default;
    virtual bool loadFromFile() = 0;
    virtual bool saveToFile() = 0;
    virtual bool addItem(std::shared_ptr<Item> item) = 0;
    virtual bool deleteItem(const std::string& itemId) = 0;
    virtual std::shared_ptr<Item> findItemById(const std::string& itemId) = 0;
    virtual std::vector<std::shared_ptr<Item>> getItemsByCategory(const std::string& category) = 0;
    virtual const std::vector<std::shared_ptr<Item>>& getAllItems() const = 0;
    virtual bool isItemIdExists(const std::string& itemId) const = 0;
};

#endif // DEPENDENCY_INTERFACES_H
