/**
 * @file main.cpp
 * @brief 购物系统主程序
 * @author Hazuki Keatsu
 * @date 2025-11-15
 */

#include "Config.h"
#include "UserManage/User.h"
#include "UserManage/UserManager.h"
#include "Login/LoginSystem.h"
#include "ItemManage/Item.h"
#include "ItemManage/ItemManager.h"
#include "ItemManage/ItemSearcher.h"
#include "ShoppingCart/ShoppingCart.h"
#include "ShoppingCart/ShoppingCartManager.h"
#include "Order/Order.h"
#include "Order/OrderManager.h"
#include "Promotion/Promotion.h"
#include "Promotion/PromotionManager.h"
#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include <sstream>
#include <iomanip>

/**
 * @brief 清空输入缓冲区
 */
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/**
 * @brief 显示主菜单
 */
void showMainMenu() {
    std::cout << "\n========== 购物系统 ==========" << std::endl;
    std::cout << "1. 顾客注册" << std::endl;
    std::cout << "2. 顾客登录" << std::endl;
    std::cout << "3. 管理员登录" << std::endl;
    std::cout << "4. 搜索商品" << std::endl;
    std::cout << "5. 查看所有商品" << std::endl;
    std::cout << "0. 退出系统" << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << "请选择: ";
}

/**
 * @brief 显示顾客菜单
 */
void showCustomerMenu() {
    std::cout << "\n===== 顾客菜单 =====" << std::endl;
    std::cout << "1. 查看商品信息" << std::endl;
    std::cout << "2. 搜索商品" << std::endl;
    std::cout << "3. 我的购物车" << std::endl;
    std::cout << "4. 我的订单" << std::endl;
    std::cout << "5. 修改密码" << std::endl;
    std::cout << "6. 登出" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << "请选择: ";
}

/**
 * @brief 显示管理员菜单
 */
void showAdminMenu() {
    std::cout << "\n===== 管理员菜单 =====" << std::endl;
    std::cout << "1. 查看所有顾客信息" << std::endl;
    std::cout << "2. 查看所有商品信息" << std::endl;
    std::cout << "3. 添加商品" << std::endl;
    std::cout << "4. 修改商品" << std::endl;
    std::cout << "5. 删除商品" << std::endl;
    std::cout << "6. 订单管理" << std::endl;
    std::cout << "7. 促销管理" << std::endl;
    std::cout << "8. 登出" << std::endl;
    std::cout << "======================" << std::endl;
    std::cout << "请选择: ";
}

/**
 * @brief 展示订单促销预览并确认
 * @param items 商品列表
 * @param promotionManager 促销管理器
 * @return 用户是否确认下单
 */
bool confirmOrderWithPromotion(
    const std::vector<std::pair<std::shared_ptr<Item>, int>>& items,
    PromotionManager* promotionManager
) {
    if (!promotionManager) {
        // 如果没有促销管理器，直接返回确认
        return true;
    }
    
    // 计算促销结果
    PromotionResult result = promotionManager->calculatePromotionResult(items);
    
    // 展示订单预览
    std::cout << "\n========== 订单预览 ==========" << std::endl;
    std::cout << "商品明细：" << std::endl;
    for (const auto& [item, quantity] : items) {
        std::cout << "  " << item->getItemName() << " x" << quantity 
                  << " = ¥" << std::fixed << std::setprecision(2) 
                  << (item->getPrice() * quantity);
        
        // 检查是否有折扣
        auto discount = promotionManager->getActiveDiscountForItem(item->getItemId());
        if (discount) {
            std::cout << " [" << discount->getDisplayTag() << "]";
        }
        std::cout << std::endl;
    }
    
    std::cout << "--------------------------------" << std::endl;
    std::cout << "商品原价：¥" << std::fixed << std::setprecision(2) 
              << result.originalTotal << std::endl;
    
    // 显示折扣明细
    if (!result.itemDiscounts.empty()) {
        double totalDiscount = 0;
        for (const auto& [itemName, discount] : result.itemDiscounts) {
            totalDiscount += discount;
        }
        std::cout << "折扣优惠：-¥" << std::fixed << std::setprecision(2) 
                  << totalDiscount << "（";
        for (size_t i = 0; i < result.appliedPromotions.size(); ++i) {
            if (result.appliedPromotions[i].find("满") == std::string::npos) {
                if (i > 0) std::cout << "、";
                std::cout << result.appliedPromotions[i];
            }
        }
        std::cout << "）" << std::endl;
        std::cout << "小计：¥" << std::fixed << std::setprecision(2) 
                  << result.afterDiscountTotal << std::endl;
    }
    
    // 显示满减优惠
    if (result.totalReduction > 0) {
        std::cout << "满减优惠：-¥" << std::fixed << std::setprecision(2) 
                  << result.totalReduction << "（";
        bool first = true;
        for (const auto& promo : result.appliedPromotions) {
            if (promo.find("满") != std::string::npos) {
                if (!first) std::cout << "、";
                std::cout << promo;
                first = false;
            }
        }
        std::cout << "）" << std::endl;
    }
    
    std::cout << "==============================" << std::endl;
    std::cout << "实付金额：¥" << std::fixed << std::setprecision(2) 
              << result.finalTotal;
    
    if (result.totalSavings > 0) {
        std::cout << " 【已省¥" << std::fixed << std::setprecision(2) 
                  << result.totalSavings << "】";
    }
    std::cout << std::endl;
    std::cout << "==============================" << std::endl;
    
    // 确认下单
    std::cout << "\n是否确认下单？(y/n): ";
    char confirm;
    std::cin >> confirm;
    
    return (confirm == 'y' || confirm == 'Y');
}

/**
 * @brief 处理购买输入的辅助函数
 * @param itemManager 商品管理器
 * @param orderManager 订单管理器
 * @param loginSystem 登录系统
 * @param promotionManager 促销管理器（可选）
 */
void processPurchaseInput(ItemManager* itemManager, OrderManager* orderManager, LoginSystem* loginSystem, PromotionManager* promotionManager = nullptr) {
    std::vector<std::pair<std::shared_ptr<Item>, int>> itemsToBuy;

    while (true) {
        if (itemsToBuy.empty()) {
            std::cout << "\n请输入要购买的商品ID (输入0返回): ";
        } else {
            std::cout << "\n请输入下一个要购买的商品ID (输入0结算，输入-1取消): ";
        }
        
        std::string itemId;
        std::cin >> itemId;

        if (itemId == "0") {
            if (itemsToBuy.empty()) return;
            break;
        }
        if (itemId == "-1") return;

        // Check login
        if (!loginSystem->isLoggedIn()) {
            std::cout << "请先登录！" << std::endl;
            return;
        }

        auto item = itemManager->findItemById(itemId);
        if (!item) {
            std::cout << "商品不存在！" << std::endl;
            continue;
        }

        std::cout << "请输入购买数量: ";
        int quantity;
        std::cin >> quantity;

        if (std::cin.fail() || quantity <= 0) {
            clearInputBuffer();
            std::cout << "无效数量！" << std::endl;
            continue;
        }

        if (item->getStock() < quantity) {
            std::cout << "库存不足！当前库存: " << item->getStock() << std::endl;
            continue;
        }

        itemsToBuy.push_back({item, quantity});
        std::cout << "已添加 " << item->getItemName() << " x" << quantity << " 到订单。" << std::endl;
    }
    
    // 展示促销预览并确认订单
    if (!confirmOrderWithPromotion(itemsToBuy, promotionManager)) {
        std::cout << "已取消下单。" << std::endl;
        return;
    }

    std::cout << "请输入收货地址: ";
    std::string address;
    std::cin.ignore();
    std::getline(std::cin, address);

    auto user = loginSystem->getCurrentUser();
    auto order = orderManager->createOrder(user->getUsername(), itemsToBuy, address);

    if (order) {
        orderManager->saveToFile();
    } else {
        std::cout << "订单创建失败！" << std::endl;
    }
}

/**
 * @brief 查看商品信息（使用ItemManager）
 * @param itemManager 商品管理器
 * @param orderManager 订单管理器（可选）
 * @param loginSystem 登录系统（可选）
 * @param promotionManager 促销管理器（可选）
 */
void viewItems(ItemManager* itemManager, OrderManager* orderManager = nullptr, LoginSystem* loginSystem = nullptr, PromotionManager* promotionManager = nullptr) {
    itemManager->displayAllItems(promotionManager);
    if (orderManager && loginSystem) {
        processPurchaseInput(itemManager, orderManager, loginSystem, promotionManager);
    }
}

/**
 * @brief 查看所有顾客信息（管理员功能）
 * @param userManager 用户管理器
 */
void viewAllCustomers(UserManager* userManager) {
    const auto& customers = userManager->getCustomers();
    
    std::cout << "\n===== 所有顾客信息 =====" << std::endl;
    std::cout << "用户名\t\t密码\t\t手机号" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    for (const auto& customer : customers) {
        std::cout << customer->getUsername() << "\t\t"
                  << customer->getPassword() << "\t\t"
                  << customer->getPhone() << std::endl;
    }
    
    std::cout << "========================" << std::endl;
    std::cout << "共 " << customers.size() << " 个顾客。" << std::endl;
}

/**
 * @brief 顾客注册流程
 * @param loginSystem 登录系统
 */
void registerProcess(LoginSystem* loginSystem) {
    std::string username, password, phone;
    
    std::cout << "\n===== 顾客注册 =====" << std::endl;
    std::cout << "请输入用户名: ";
    std::cin >> username;
    
    std::cout << "请输入密码: ";
    std::cin >> password;
    
    std::cout << "请输入手机号: ";
    std::cin >> phone;
    
    loginSystem->registerCustomer(username, password, phone);
}

/**
 * @brief 登录流程
 * @param loginSystem 登录系统
 * @param isAdmin 是否为管理员登录
 */
void loginProcess(LoginSystem* loginSystem, bool isAdmin) {
    std::string username, password;
    
    if (isAdmin) {
        std::cout << "\n===== 管理员登录 =====" << std::endl;
    } else {
        std::cout << "\n===== 顾客登录 =====" << std::endl;
    }
    
    std::cout << "请输入用户名: ";
    std::cin >> username;
    
    std::cout << "请输入密码: ";
    std::cin >> password;
    
    loginSystem->login(username, password, isAdmin);
}

/**
 * @brief 修改密码流程
 * @param loginSystem 登录系统
 */
void changePasswordProcess(LoginSystem* loginSystem) {
    std::string oldPassword, newPassword, confirmPassword;
    
    std::cout << "\n===== 修改密码 =====" << std::endl;
    std::cout << "请输入旧密码: ";
    std::cin >> oldPassword;
    
    std::cout << "请输入新密码: ";
    std::cin >> newPassword;
    
    std::cout << "请确认新密码: ";
    std::cin >> confirmPassword;
    
    if (newPassword != confirmPassword) {
        std::cout << "两次输入的新密码不一致！" << std::endl;
        return;
    }
    
    loginSystem->changePassword(oldPassword, newPassword);
}

/**
 * @brief 添加商品流程（管理员功能）
 * @param itemManager 商品管理器
 */
void addItemProcess(ItemManager* itemManager) {
    std::string itemId, itemName, category, description;
    double price;
    int stock;
    
    std::cout << "\n===== 添加商品 =====" << std::endl;
    
    // 自动生成ID或手动输入
    std::cout << "请输入商品ID（留空自动生成）: ";
    std::cin.ignore();
    std::getline(std::cin, itemId);
    
    if (itemId.empty()) {
        // 自动生成ID逻辑在ItemManager中
        int maxId = 0;
        for (const auto& item : itemManager->getAllItems()) {
            try {
                int id = std::stoi(item->getItemId());
                if (id > maxId) maxId = id;
            } catch (...) {}
        }
        itemId = std::to_string(maxId + 1);
        std::cout << "自动生成ID: " << itemId << std::endl;
    }
    
    // 检查ID是否已存在
    if (itemManager->isItemIdExists(itemId)) {
        std::cout << "商品ID已存在！" << std::endl;
        return;
    }
    
    std::cout << "请输入商品名称: ";
    std::getline(std::cin, itemName);
    
    std::cout << "请输入商品类别: ";
    std::getline(std::cin, category);
    
    std::cout << "请输入商品价格: ";
    std::cin >> price;
    
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "价格输入无效！" << std::endl;
        return;
    }
    
    std::cin.ignore();
    std::cout << "请输入商品描述: ";
    std::getline(std::cin, description);
    
    std::cout << "请输入库存数量: ";
    std::cin >> stock;
    
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "库存输入无效！" << std::endl;
        return;
    }
    
    // 创建商品对象
    auto newItem = std::make_shared<Item>(itemId, itemName, category, price, description, stock);
    
    // 添加到管理器
    if (itemManager->addItem(newItem)) {
        std::cout << "商品添加成功！" << std::endl;
        // 显示所有商品
        itemManager->displayAllItems();
    } else {
        std::cout << "商品添加失败！" << std::endl;
    }
}

/**
 * @brief 修改商品流程（管理员功能）
 * @param itemManager 商品管理器
 */
void modifyItemProcess(ItemManager* itemManager) {
    std::string itemId;
    
    std::cout << "\n===== 修改商品 =====" << std::endl;
    
    // 先显示所有商品
    itemManager->displayAllItems();
    
    std::cout << "\n请输入要修改的商品ID: ";
    std::cin >> itemId;
    
    // 查找商品
    auto item = itemManager->findItemById(itemId);
    if (item == nullptr) {
        std::cout << "商品不存在！" << std::endl;
        return;
    }
    
    // 显示当前商品信息
    std::cout << "\n当前商品信息：" << std::endl;
    std::cout << "ID: " << item->getItemId() << std::endl;
    std::cout << "名称: " << item->getItemName() << std::endl;
    std::cout << "类别: " << item->getCategory() << std::endl;
    std::cout << "价格: " << item->getPrice() << std::endl;
    std::cout << "描述: " << item->getDescription() << std::endl;
    std::cout << "库存: " << item->getStock() << std::endl;
    
    // 修改字段
    std::cout << "\n请选择要修改的字段：" << std::endl;
    std::cout << "1. 名称" << std::endl;
    std::cout << "2. 类别" << std::endl;
    std::cout << "3. 价格" << std::endl;
    std::cout << "4. 描述" << std::endl;
    std::cout << "5. 库存" << std::endl;
    std::cout << "0. 完成修改" << std::endl;
    
    bool modifying = true;
    while (modifying) {
        std::cout << "\n请选择: ";
        int choice;
        std::cin >> choice;
        
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "无效输入！" << std::endl;
            continue;
        }
        
        std::cin.ignore();
        
        switch (choice) {
            case 1: {
                std::string newName;
                std::cout << "请输入新名称: ";
                std::getline(std::cin, newName);
                item->setItemName(newName);
                std::cout << "名称已更新。" << std::endl;
                break;
            }
            case 2: {
                std::string newCategory;
                std::cout << "请输入新类别: ";
                std::getline(std::cin, newCategory);
                item->setCategory(newCategory);
                std::cout << "类别已更新。" << std::endl;
                break;
            }
            case 3: {
                double newPrice;
                std::cout << "请输入新价格: ";
                std::cin >> newPrice;
                if (!std::cin.fail()) {
                    item->setPrice(newPrice);
                    std::cout << "价格已更新。" << std::endl;
                } else {
                    std::cin.clear();
                    std::cout << "价格输入无效！" << std::endl;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            }
            case 4: {
                std::string newDesc;
                std::cout << "请输入新描述: ";
                std::getline(std::cin, newDesc);
                item->setDescription(newDesc);
                std::cout << "描述已更新。" << std::endl;
                break;
            }
            case 5: {
                int newStock;
                std::cout << "请输入新库存: ";
                std::cin >> newStock;
                if (!std::cin.fail()) {
                    item->setStock(newStock);
                    std::cout << "库存已更新。" << std::endl;
                } else {
                    std::cin.clear();
                    std::cout << "库存输入无效！" << std::endl;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            }
            case 0:
                modifying = false;
                break;
            default:
                std::cout << "无效选择！" << std::endl;
                break;
        }
    }
    
    // 保存更改
    if (itemManager->saveToFile()) {
        std::cout << "\n商品修改成功！" << std::endl;
        // 显示所有商品
        itemManager->displayAllItems();
    } else {
        std::cout << "保存失败！" << std::endl;
    }
}

/**
 * @brief 删除商品流程（管理员功能）
 * @param itemManager 商品管理器
 */
void deleteItemProcess(ItemManager* itemManager) {
    std::string itemId;
    
    std::cout << "\n===== 删除商品 =====" << std::endl;
    
    // 先显示所有商品
    itemManager->displayAllItems();
    
    std::cout << "\n请输入要删除的商品ID: ";
    std::cin >> itemId;
    
    // 查找商品
    auto item = itemManager->findItemById(itemId);
    if (item == nullptr) {
        std::cout << "商品不存在！" << std::endl;
        return;
    }
    
    // 确认删除
    std::cout << "确认删除商品 \"" << item->getItemName() << "\" (ID: " << itemId << ")? (y/n): ";
    char confirm;
    std::cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        if (itemManager->deleteItem(itemId)) {
            std::cout << "商品删除成功！" << std::endl;
            // 显示所有商品
            itemManager->displayAllItems();
        } else {
            std::cout << "商品删除失败！" << std::endl;
        }
    } else {
        std::cout << "已取消删除。" << std::endl;
    }
}

/**
 * @brief 管理订单流程（管理员功能）
 * @param orderManager 订单管理器
 */
void manageOrdersProcess(OrderManager* orderManager) {
    while (true) {
        std::cout << "\n===== 订单管理 =====" << std::endl;
        orderManager->displayAllOrders();
        
        std::cout << "\n请选择操作：" << std::endl;
        std::cout << "1. 修改订单状态" << std::endl;
        std::cout << "0. 返回上级菜单" << std::endl;
        std::cout << "请选择: ";
        
        int choice;
        std::cin >> choice;
        
        if (std::cin.fail()) {
            clearInputBuffer();
            std::cout << "无效输入！" << std::endl;
            continue;
        }
        
        if (choice == 0) {
            break;
        } else if (choice == 1) {
            std::cout << "请输入要修改的订单ID: ";
            std::string orderId;
            std::cin >> orderId;
            
            auto order = orderManager->findOrderById(orderId);
            if (!order) {
                std::cout << "订单不存在！" << std::endl;
                continue;
            }
            
            std::cout << "当前状态: " << order->getStatusString() << std::endl;
            std::cout << "请选择新状态：" << std::endl;
            std::cout << "1. 待发货 (PENDING)" << std::endl;
            std::cout << "2. 已发货 (SHIPPED)" << std::endl;
            std::cout << "3. 已签收 (DELIVERED)" << std::endl;
            std::cout << "请选择 (1-3): ";
            
            int statusChoice;
            std::cin >> statusChoice;
            
            if (std::cin.fail()) {
                clearInputBuffer();
                std::cout << "无效输入！" << std::endl;
                continue;
            }
            
            OrderStatus newStatus;
            switch (statusChoice) {
                case 1: newStatus = OrderStatus::PENDING; break;
                case 2: newStatus = OrderStatus::SHIPPED; break;
                case 3: newStatus = OrderStatus::DELIVERED; break;
                default:
                    std::cout << "无效选择！" << std::endl;
                    continue;
            }
            
            if (orderManager->updateOrderStatus(orderId, newStatus)) {
                std::cout << "状态更新成功！" << std::endl;
            } else {
                std::cout << "状态更新失败！" << std::endl;
            }
        } else {
            std::cout << "无效选择！" << std::endl;
        }
    }
}

/**
 * @brief 促销管理流程（管理员功能）
 * @param promotionManager 促销管理器
 */
void managePromotionsProcess(PromotionManager* promotionManager, ItemManager* itemManager) {
    while (true) {
        std::cout << "\n===== 促销管理 =====" << std::endl;
        std::cout << "1. 查看所有促销活动" << std::endl;
        std::cout << "2. 查看有效促销活动" << std::endl;
        std::cout << "3. 添加折扣促销" << std::endl;
        std::cout << "4. 添加满减促销" << std::endl;
        std::cout << "5. 修改促销信息" << std::endl;
        std::cout << "6. 启用/禁用促销" << std::endl;
        std::cout << "7. 删除促销活动" << std::endl;
        std::cout << "0. 返回上级菜单" << std::endl;
        std::cout << "======================" << std::endl;
        std::cout << "请选择: ";
        
        int choice;
        std::cin >> choice;
        
        if (std::cin.fail()) {
            clearInputBuffer();
            std::cout << "无效输入！" << std::endl;
            continue;
        }
        
        if (choice == 0) {
            break;
        } else if (choice == 1) {
            // 查看所有促销活动
            promotionManager->displayAllPromotions();
        } else if (choice == 2) {
            // 查看有效促销活动
            promotionManager->displayActivePromotions();
        } else if (choice == 3) {
            // 添加折扣促销
            std::cout << "\n===== 添加折扣促销 =====" << std::endl;
            std::cout << "请输入促销名称: ";
            std::string name;
            std::cin.ignore();
            std::getline(std::cin, name);
            
            std::cout << "请输入目标商品ID（输入-1表示全场折扣）: ";
            std::string itemId;
            std::getline(std::cin, itemId);
            
            // 空输入转换为-1
            if (itemId.empty()) {
                itemId = "-1";
            }
            
            // 如果不是全场促销，检查商品是否存在
            if (itemId != "-1" && !itemManager->findItemById(itemId)) {
                std::cout << "商品ID不存在！" << std::endl;
                continue;
            }
            
            std::cout << "请输入折扣率（如0.8表示8折）: ";
            double rate;
            std::cin >> rate;
            
            if (std::cin.fail() || rate <= 0 || rate >= 1) {
                clearInputBuffer();
                std::cout << "无效的折扣率！" << std::endl;
                continue;
            }
            
            std::cout << "请输入有效天数: ";
            int days;
            std::cin >> days;
            
            if (std::cin.fail() || days <= 0) {
                clearInputBuffer();
                std::cout << "无效的天数！" << std::endl;
                continue;
            }
            
            time_t now = time(nullptr);
            time_t endTime = now + (days * 24 * 60 * 60);
            
            std::string promotionId = promotionManager->generatePromotionId();
            auto promotion = std::make_shared<Promotion>(
                promotionId, name, true, now, endTime, itemId, rate
            );
            
            if (promotionManager->addPromotion(promotion)) {
                std::cout << "折扣促销添加成功！促销ID: " << promotionId << std::endl;
            } else {
                std::cout << "折扣促销添加失败！" << std::endl;
            }
        } else if (choice == 4) {
            // 添加满减促销
            std::cout << "\n===== 添加满减促销 =====" << std::endl;
            std::cout << "请输入促销名称: ";
            std::string name;
            std::cin.ignore();
            std::getline(std::cin, name);
            
            std::cout << "请输入满减门槛金额: ";
            double threshold;
            std::cin >> threshold;
            
            if (std::cin.fail() || threshold <= 0) {
                clearInputBuffer();
                std::cout << "无效的金额！" << std::endl;
                continue;
            }
            
            std::cout << "请输入减免金额: ";
            double reduction;
            std::cin >> reduction;
            
            if (std::cin.fail() || reduction <= 0 || reduction >= threshold) {
                clearInputBuffer();
                std::cout << "无效的减免金额！" << std::endl;
                continue;
            }
            
            std::cout << "请输入有效天数: ";
            int days;
            std::cin >> days;
            
            if (std::cin.fail() || days <= 0) {
                clearInputBuffer();
                std::cout << "无效的天数！" << std::endl;
                continue;
            }
            
            time_t now = time(nullptr);
            time_t endTime = now + (days * 24 * 60 * 60);
            
            std::string promotionId = promotionManager->generatePromotionId();
            auto promotion = std::make_shared<Promotion>(
                promotionId, name, true, now, endTime, threshold, reduction
            );
            
            if (promotionManager->addPromotion(promotion)) {
                std::cout << "满减促销添加成功！促销ID: " << promotionId << std::endl;
            } else {
                std::cout << "满减促销添加失败！" << std::endl;
            }
        } else if (choice == 5) {
            // 修改促销信息
            promotionManager->displayAllPromotions();
            std::cout << "\n请输入要修改的促销ID: ";
            std::string promotionId;
            std::cin >> promotionId;
            
            auto promotion = promotionManager->findPromotionById(promotionId);
            if (!promotion) {
                std::cout << "促销活动不存在！" << std::endl;
                continue;
            }
            
            // 显示当前促销信息
            std::cout << "\n当前促销信息：" << std::endl;
            std::cout << "ID: " << promotion->getPromotionId() << std::endl;
            std::cout << "名称: " << promotion->getPromotionName() << std::endl;
            std::cout << "类型: " << (promotion->getPromotionType() == PromotionType::DISCOUNT ? "折扣促销" : "满减促销") << std::endl;
            std::cout << "状态: " << (promotion->getIsActive() ? "启用" : "禁用") << std::endl;
            
            if (promotion->getPromotionType() == PromotionType::DISCOUNT) {
                std::cout << "目标商品: " << (promotion->getTargetItemId() == "-1" ? "全场" : promotion->getTargetItemId()) << std::endl;
                std::cout << "折扣率: " << promotion->getDiscountRate() << " (" << promotion->getDisplayTag() << ")" << std::endl;
            } else {
                std::cout << "门槛金额: " << promotion->getThresholdAmount() << std::endl;
                std::cout << "减免金额: " << promotion->getReductionAmount() << std::endl;
            }
            
            // 修改菜单
            bool modifying = true;
            while (modifying) {
                std::cout << "\n请选择要修改的项：" << std::endl;
                std::cout << "1. 修改名称" << std::endl;
                std::cout << "2. 修改有效期" << std::endl;
                if (promotion->getPromotionType() == PromotionType::DISCOUNT) {
                    std::cout << "3. 修改折扣率" << std::endl;
                    std::cout << "4. 修改目标商品" << std::endl;
                } else {
                    std::cout << "3. 修改门槛金额" << std::endl;
                    std::cout << "4. 修改减免金额" << std::endl;
                }
                std::cout << "0. 完成修改" << std::endl;
                std::cout << "请选择: ";
                
                int modChoice;
                std::cin >> modChoice;
                
                if (std::cin.fail()) {
                    clearInputBuffer();
                    std::cout << "无效输入！" << std::endl;
                    continue;
                }
                
                if (modChoice == 0) {
                    modifying = false;
                    break;
                } else if (modChoice == 1) {
                    // 修改名称
                    std::cout << "请输入新名称: ";
                    std::string newName;
                    std::cin.ignore();
                    std::getline(std::cin, newName);
                    
                    if (promotionManager->updatePromotionName(promotionId, newName)) {
                        std::cout << "名称修改成功！" << std::endl;
                    } else {
                        std::cout << "名称修改失败！" << std::endl;
                    }
                } else if (modChoice == 2) {
                    // 修改有效期
                    std::cout << "请输入新的有效天数: ";
                    int days;
                    std::cin >> days;
                    
                    if (std::cin.fail() || days <= 0) {
                        clearInputBuffer();
                        std::cout << "无效的天数！" << std::endl;
                        continue;
                    }
                    
                    time_t now = time(nullptr);
                    time_t endTime = now + (days * 24 * 60 * 60);
                    
                    if (promotionManager->updatePromotionTime(promotionId, now, endTime)) {
                        std::cout << "有效期修改成功！" << std::endl;
                    } else {
                        std::cout << "有效期修改失败！" << std::endl;
                    }
                } else if (modChoice == 3) {
                    if (promotion->getPromotionType() == PromotionType::DISCOUNT) {
                        // 修改折扣率
                        std::cout << "请输入新的折扣率（如0.8表示8折）: ";
                        double newRate;
                        std::cin >> newRate;
                        
                        if (std::cin.fail()) {
                            clearInputBuffer();
                            std::cout << "无效输入！" << std::endl;
                            continue;
                        }
                        
                        if (promotionManager->updateDiscountRate(promotionId, newRate)) {
                            std::cout << "折扣率修改成功！" << std::endl;
                        } else {
                            std::cout << "折扣率修改失败！" << std::endl;
                        }
                    } else {
                        // 修改门槛金额
                        std::cout << "请输入新的门槛金额: ";
                        double newThreshold;
                        std::cin >> newThreshold;
                        
                        if (std::cin.fail()) {
                            clearInputBuffer();
                            std::cout << "无效输入！" << std::endl;
                            continue;
                        }
                        
                        if (promotionManager->updateFullReductionThreshold(promotionId, newThreshold)) {
                            std::cout << "门槛金额修改成功！" << std::endl;
                        } else {
                            std::cout << "门槛金额修改失败！" << std::endl;
                        }
                    }
                } else if (modChoice == 4) {
                    if (promotion->getPromotionType() == PromotionType::DISCOUNT) {
                        // 修改目标商品
                        std::cout << "请输入新的目标商品ID（输入-1表示全场）: ";
                        std::string newItemId;
                        std::cin.ignore();
                        std::getline(std::cin, newItemId);
                        
                        // 空输入转换为-1
                        if (newItemId.empty()) {
                            newItemId = "-1";
                        }
                        
                        // 如果不是全场促销，检查商品是否存在
                        if (newItemId != "-1" && !itemManager->findItemById(newItemId)) {
                            std::cout << "商品ID不存在！" << std::endl;
                            continue;
                        }
                        
                        if (promotionManager->updateDiscountTargetItem(promotionId, newItemId)) {
                            std::cout << "目标商品修改成功！" << std::endl;
                        } else {
                            std::cout << "目标商品修改失败！" << std::endl;
                        }
                    } else {
                        // 修改减免金额
                        std::cout << "请输入新的减免金额: ";
                        double newReduction;
                        std::cin >> newReduction;
                        
                        if (std::cin.fail()) {
                            clearInputBuffer();
                            std::cout << "无效输入！" << std::endl;
                            continue;
                        }
                        
                        if (promotionManager->updateFullReductionAmount(promotionId, newReduction)) {
                            std::cout << "减免金额修改成功！" << std::endl;
                        } else {
                            std::cout << "减免金额修改失败！" << std::endl;
                        }
                    }
                } else {
                    std::cout << "无效选择！" << std::endl;
                }
            }
        } else if (choice == 6) {
            // 启用/禁用促销
            promotionManager->displayAllPromotions();
            std::cout << "\n请输入要修改的促销ID: ";
            std::string promotionId;
            std::cin >> promotionId;
            
            auto promotion = promotionManager->findPromotionById(promotionId);
            if (!promotion) {
                std::cout << "促销活动不存在！" << std::endl;
                continue;
            }
            
            bool currentStatus = promotion->getIsActive();
            
            if (promotionManager->setPromotionActive(promotionId, !currentStatus)) {
                std::cout << "促销状态已" << (!currentStatus ? "启用" : "禁用") << "！" << std::endl;
            } else {
                std::cout << "状态修改失败！" << std::endl;
            }
        } else if (choice == 7) {
            // 删除促销活动
            promotionManager->displayAllPromotions();
            std::cout << "\n请输入要删除的促销ID: ";
            std::string promotionId;
            std::cin >> promotionId;
            
            std::cout << "确认删除促销活动？(y/n): ";
            char confirm;
            std::cin >> confirm;
            
            if (confirm == 'y' || confirm == 'Y') {
                if (promotionManager->deletePromotion(promotionId)) {
                    std::cout << "促销活动已删除！" << std::endl;
                } else {
                    std::cout << "删除失败！" << std::endl;
                }
            } else {
                std::cout << "已取消操作。" << std::endl;
            }
        } else {
            std::cout << "无效选择！" << std::endl;
        }
    }
}

/**
 * @brief 显示购物车菜单
 */
void showShoppingCartMenu() {
    std::cout << "\n===== 购物车管理 =====" << std::endl;
    std::cout << "1. 添加商品到购物车" << std::endl;
    std::cout << "2. 查看购物车" << std::endl;
    std::cout << "3. 修改商品数量" << std::endl;
    std::cout << "4. 删除购物车中的商品" << std::endl;
    std::cout << "5. 清空购物车" << std::endl;
    std::cout << "6. 结算" << std::endl;
    std::cout << "0. 返回上级菜单" << std::endl;
    std::cout << "======================" << std::endl;
    std::cout << "请选择: ";
}

/**
 * @brief 购物车管理流程
 * @param cartManager 购物车管理器
 * @param itemManager 商品管理器
 * @param orderManager 订单管理器
 * @param username 当前用户名
 * @param customer 当前用户对象
 * @param promotionManager 促销管理器（可选）
 */
void shoppingCartProcess(ShoppingCartManager* cartManager, 
                         ItemManager* itemManager,
                         OrderManager* orderManager,
                         const std::string& username,
                         std::shared_ptr<Customer> customer,
                         PromotionManager* promotionManager = nullptr) {
    // 获取用户的购物车
    auto cart = cartManager->getCart(username, customer);
    
    bool inCartMenu = true;
    while (inCartMenu) {
        showShoppingCartMenu();
        int choice;
        std::cin >> choice;
        
        if (std::cin.fail()) {
            clearInputBuffer();
            std::cout << "无效输入，请输入数字。" << std::endl;
            continue;
        }
        
        switch (choice) {
            case 1: {
                // 添加商品到购物车
                std::cout << "\n===== 添加商品到购物车 =====" << std::endl;
                
                // 显示所有商品
                itemManager->displayAllItems();
                
                std::cout << "\n请输入要添加的商品ID: ";
                std::string itemId;
                std::cin >> itemId;
                
                auto item = itemManager->findItemById(itemId);
                if (!item) {
                    std::cout << "商品不存在！" << std::endl;
                    break;
                }
                
                std::cout << "请输入购买数量: ";
                int quantity;
                std::cin >> quantity;
                
                if (std::cin.fail()) {
                    clearInputBuffer();
                    std::cout << "数量输入无效！" << std::endl;
                    break;
                }
                
                cart->addItem(item, quantity);
                // 保存购物车数据
                cartManager->saveToFile();
                break;
            }
            
            case 2: {
                // 查看购物车
                cart->displayCart();
                break;
            }
            
            case 3: {
                // 修改商品数量
                if (cart->isEmpty()) {
                    std::cout << "购物车是空的！" << std::endl;
                    break;
                }
                
                cart->displayCart();
                
                std::cout << "\n请输入要修改的商品ID: ";
                std::string itemId;
                std::cin >> itemId;
                
                std::cout << "请输入新的数量: ";
                int newQuantity;
                std::cin >> newQuantity;
                
                if (std::cin.fail()) {
                    clearInputBuffer();
                    std::cout << "数量输入无效！" << std::endl;
                    break;
                }
                
                cart->updateItemQuantity(itemId, newQuantity);
                // 保存购物车数据
                cartManager->saveToFile();
                break;
            }
            
            case 4: {
                // 删除购物车中的商品
                if (cart->isEmpty()) {
                    std::cout << "购物车是空的！" << std::endl;
                    break;
                }
                
                cart->displayCart();
                
                std::cout << "\n请选择删除方式：" << std::endl;
                std::cout << "1. 删除单个商品" << std::endl;
                std::cout << "2. 删除多个商品" << std::endl;
                std::cout << "请选择: ";
                
                int deleteChoice;
                std::cin >> deleteChoice;
                
                if (std::cin.fail()) {
                    clearInputBuffer();
                    std::cout << "无效输入！" << std::endl;
                    break;
                }
                
                if (deleteChoice == 1) {
                    std::cout << "请输入要删除的商品ID: ";
                    std::string itemId;
                    std::cin >> itemId;
                    cart->removeItem(itemId);
                } else if (deleteChoice == 2) {
                    std::cout << "请输入要删除的商品ID（用空格分隔）: ";
                    std::cin.ignore();
                    std::string line;
                    std::getline(std::cin, line);
                    
                    std::vector<std::string> itemIds;
                    std::stringstream ss(line);
                    std::string itemId;
                    while (ss >> itemId) {
                        itemIds.push_back(itemId);
                    }
                    
                    int removedCount = cart->removeMultipleItems(itemIds);
                    std::cout << "成功删除 " << removedCount << " 个商品。" << std::endl;
                } else {
                    std::cout << "无效选择！" << std::endl;
                }
                
                // 保存购物车数据
                cartManager->saveToFile();
                break;
            }
            
            case 5: {
                // 清空购物车
                std::cout << "确认清空购物车？(y/n): ";
                char confirm;
                std::cin >> confirm;
                
                if (confirm == 'y' || confirm == 'Y') {
                    cart->clear();
                    // 保存购物车数据
                    cartManager->saveToFile();
                } else {
                    std::cout << "已取消操作。" << std::endl;
                }
                break;
            }

            case 6: {
                // 结算
                if (cart->isEmpty()) {
                    std::cout << "购物车为空！" << std::endl;
                    break;
                }
                
                // 展示促销预览并确认订单
                if (!confirmOrderWithPromotion(cart->getCartItems(), promotionManager)) {
                    std::cout << "已取消结算。" << std::endl;
                    break;
                }
                
                std::cout << "请输入收货地址: ";
                std::string address;
                std::cin.ignore();
                std::getline(std::cin, address);

                auto order = orderManager->createOrder(username, cart->getCartItems(), address);
                if (order) {
                    cart->clear();
                    cartManager->saveToFile();
                    orderManager->saveToFile();
                } else {
                    std::cout << "订单创建失败！" << std::endl;
                }
                break;
            }
            
            case 0:
                // 返回上级菜单
                inCartMenu = false;
                break;
                
            default:
                std::cout << "无效选择，请重新输入。" << std::endl;
                break;
        }
    }
}

/**
 * @brief 搜索商品流程（顾客功能）
 * @param itemSearcher 商品搜索器
 * @param itemManager 商品管理器（可选）
 * @param orderManager 订单管理器（可选）
 * @param loginSystem 登录系统（可选）
 * @param promotionManager 促销管理器（可选）
 */
void searchItemProcess(ItemSearcher* itemSearcher, ItemManager* itemManager = nullptr, OrderManager* orderManager = nullptr, LoginSystem* loginSystem = nullptr, PromotionManager* promotionManager = nullptr) {
    std::string keyword;
    
    std::cout << "\n===== 搜索商品 =====" << std::endl;
    std::cout << "请选择搜索方式：" << std::endl;
    std::cout << "1. 按商品名称搜索" << std::endl;
    std::cout << "2. 按商品类别搜索" << std::endl;
    std::cout << "3. 综合搜索（名称+类别）" << std::endl;
    std::cout << "4. 按商品价格区间" << std::endl;
    std::cout << "请选择 (1-4): ";
    
    int choice;
    std::cin >> choice;
    
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "无效输入！" << std::endl;
        return;
    }
    
    SearchType searchType;
    switch (choice) {
        case 1:
            searchType = SearchType::BY_NAME;
            std::cout << "\n按商品名称搜索" << std::endl;
            break;
        case 2:
            searchType = SearchType::BY_CATEGORY;
            std::cout << "\n按商品类别搜索" << std::endl;
            break;
        case 3:
            searchType = SearchType::ALL;
            std::cout << "\n综合搜索" << std::endl;
            break;
        case 4:
            searchType = SearchType::BY_PRICE;
            std::cout << "\n价格区间搜索" << std::endl;
            break;
        default:
            std::cout << "无效选择！" << std::endl;
            return;
    }
    
    if (searchType != SearchType::BY_PRICE) {
        std::cout << "请输入搜索关键字: ";
        std::cin.ignore();
        std::getline(std::cin, keyword);
        
        if (keyword.empty()) {
            std::cout << "关键字不能为空！" << std::endl;
            return;
        }
    } else {
        std::cout << "请输入搜索价格区间(<最低价>-<最高价>): ";
        std::cin.ignore();
        std::getline(std::cin, keyword);
        if (keyword.empty()) {
            std::cout << "价格区间不能为空！" << std::endl;
            return;
        }
    }

    // 执行搜索（先精确后模糊）
    auto results = itemSearcher->search(keyword, searchType);
    
    // 显示搜索结果
    itemSearcher->displaySearchResults(results, true);  // 显示相似度

    if (itemManager && orderManager && loginSystem) {
        processPurchaseInput(itemManager, orderManager, loginSystem, promotionManager);
    }
}

/**
 * @brief 主函数
 */
int main() {
    // 初始化配置
    Config* config = Config::getInstance();
    if (!config->loadConfig("res/config.yaml")) {
        std::cerr << "配置文件加载失败，使用默认配置。" << std::endl;
    }
    
    // 初始化用户管理器
    UserManager userManager(config->getUsersFilePath());
    userManager.loadFromFile();
    
    // 初始化商品管理器（使用shared_ptr以便在购物车管理器中共享）
    auto itemManagerPtr = std::make_shared<ItemManager>(config->getItemsFilePath());
    itemManagerPtr->loadFromFile();
    
    // 为了兼容性，创建一个引用
    ItemManager& itemManager = *itemManagerPtr;
    
    // 初始化商品搜索器
    ItemSearcher itemSearcher(itemManagerPtr.get());
    
    // 初始化购物车管理器
    ShoppingCartManager cartManager(config->getShoppingCartFilePath(), itemManagerPtr);
    cartManager.loadFromFile();

    // 初始化订单管理器
    OrderManager orderManager(config->getOrdersFilePath(), itemManagerPtr);
    orderManager.loadFromFile();
    if (config->isAutoUpdateEnabled()) {
        orderManager.enableAutoUpdate(config->getPendingToShippedSeconds(), config->getShippedToDeliveredSeconds());
    }
    
    // 初始化促销管理器
    PromotionManager promotionManager(config->getPromotionsFilePath());
    promotionManager.loadFromFile();
    
    // 初始化登录系统
    LoginSystem loginSystem(&userManager, config);
    
    // 主循环
    bool running = true;
    while (running) {
        // 根据登录状态显示不同菜单
        if (!loginSystem.isLoggedIn()) {
            // 未登录状态：显示主菜单
            showMainMenu();
            int choice;
            std::cin >> choice;
            
            // 检查输入是否有效
            if (std::cin.fail()) {
                clearInputBuffer();
                std::cout << "无效输入，请输入数字。" << std::endl;
                continue;
            }
            
            switch (choice) {
                case 1:
                    // 顾客注册
                    registerProcess(&loginSystem);
                    break;
                    
                case 2:
                    // 顾客登录
                    loginProcess(&loginSystem, false);
                    break;
                    
                case 3:
                    // 管理员登录
                    loginProcess(&loginSystem, true);
                    break;
                    
                case 4:
                    // 搜索商品
                    searchItemProcess(&itemSearcher, &itemManager, &orderManager, &loginSystem, &promotionManager);
                    break;
                case 5:
                    // 查看所有商品
                    viewItems(&itemManager, &orderManager, &loginSystem, &promotionManager);
                    break;
                    
                case 0:
                    // 退出系统
                    std::cout << "感谢使用购物系统，再见！" << std::endl;
                    running = false;
                    break;
                    
                default:
                    std::cout << "无效选择，请重新输入。" << std::endl;
                    break;
            }
        } else if (loginSystem.getCurrentUserRole() == UserRole::CUSTOMER) {
            // 顾客已登录：显示顾客菜单
            showCustomerMenu();
            int choice;
            std::cin >> choice;
            
            if (std::cin.fail()) {
                clearInputBuffer();
                std::cout << "无效输入，请输入数字。" << std::endl;
                continue;
            }
            
            switch (choice) {
                case 1:
                    // 查看商品信息
                    viewItems(&itemManager, &orderManager, &loginSystem, &promotionManager);
                    break;
                    
                case 2:
                    // 搜索商品
                    searchItemProcess(&itemSearcher, &itemManager, &orderManager, &loginSystem, &promotionManager);
                    break;
                    
                case 3: {
                    // 我的购物车
                    auto user = loginSystem.getCurrentUser();
                    if (user) {
                        std::string username = user->getUsername();
                        auto customer = std::dynamic_pointer_cast<Customer>(user);
                        shoppingCartProcess(&cartManager, &itemManager, &orderManager, username, customer, &promotionManager);
                    }
                    break;
                }
                    
                case 4: {
                    // 我的订单
                    auto user = loginSystem.getCurrentUser();
                    if (user) {
                        std::string username = user->getUsername();
                        orderManager.displayUserOrders(username);
                        
                        while (true) {
                            std::cout << "\n1. 查看订单详情" << std::endl;
                            std::cout << "0. 返回" << std::endl;
                            std::cout << "请选择: ";
                            
                            int detailChoice;
                            std::cin >> detailChoice;
                            
                            if (std::cin.fail()) {
                                clearInputBuffer();
                                std::cout << "无效输入！" << std::endl;
                                continue;
                            }
                            
                            if (detailChoice == 0) {
                                break;
                            } else if (detailChoice == 1) {
                                std::cout << "请输入订单ID: ";
                                std::string orderId;
                                std::cin >> orderId;
                                
                                auto order = orderManager.findOrderById(orderId);
                                if (order && order->getUserId() == username) {
                                    order->displayOrderInfo();
                                } else {
                                    std::cout << "未找到该订单或无权查看！" << std::endl;
                                }
                            } else {
                                std::cout << "无效选择！" << std::endl;
                            }
                        }
                    }
                    break;
                }

                case 5:
                    // 修改密码
                    changePasswordProcess(&loginSystem);
                    break;
                    
                case 6:
                    // 登出
                    loginSystem.logout();
                    break;
                    
                default:
                    std::cout << "无效选择，请重新输入。" << std::endl;
                    break;
            }
        } else if (loginSystem.getCurrentUserRole() == UserRole::ADMIN) {
            // 管理员已登录：显示管理员菜单
            showAdminMenu();
            int choice;
            std::cin >> choice;
            
            if (std::cin.fail()) {
                clearInputBuffer();
                std::cout << "无效输入，请输入数字。" << std::endl;
                continue;
            }
            
            switch (choice) {
                case 1:
                    // 查看所有顾客信息
                    viewAllCustomers(&userManager);
                    break;
                    
                case 2:
                    // 查看所有商品信息
                    viewItems(&itemManager, nullptr, nullptr, &promotionManager);
                    break;
                    
                case 3:
                    // 添加商品
                    addItemProcess(&itemManager);
                    break;
                    
                case 4:
                    // 修改商品
                    modifyItemProcess(&itemManager);
                    break;
                    
                case 5:
                    // 删除商品
                    deleteItemProcess(&itemManager);
                    break;

                case 6:
                    // 订单管理
                    manageOrdersProcess(&orderManager);
                    break;
                    
                case 7:
                    // 促销管理
                    managePromotionsProcess(&promotionManager, &itemManager);
                    break;
                    
                case 8:
                    // 登出
                    loginSystem.logout();
                    break;
                    
                default:
                    std::cout << "无效选择，请重新输入。" << std::endl;
                    break;
            }
        }
    }
    
    return 0;
}
