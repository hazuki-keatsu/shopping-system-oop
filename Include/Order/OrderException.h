/**
 * @file OrderException.h
 * @brief 订单异常类的定义
 * @author Hazuki Keatsu
 * @date 2025-12-01
 */

#ifndef ORDER_EXCEPTION_H
#define ORDER_EXCEPTION_H

#include <exception>
#include <string>

/**
 * @class OrderException
 * @brief 订单异常类，用于处理订单相关的异常情况
 * 
 * 主要用于处理：
 * 1. 商品库存不足
 * 2. 订单创建失败
 * 3. 订单状态转换异常
 */
class OrderException : public std::exception {
private:
    std::string message;  // 异常信息

public:
    /**
     * @brief 构造函数
     * @param msg 异常信息
     */
    explicit OrderException(const std::string& msg) : message(msg) {}
    
    /**
     * @brief 获取异常信息
     * @return 异常信息字符串
     */
    virtual const char* what() const noexcept override {
        return message.c_str();
    }
    
    /**
     * @brief 析构函数
     */
    virtual ~OrderException() {}
};

/**
 * @class InsufficientStockException
 * @brief 库存不足异常类
 */
class InsufficientStockException : public OrderException {
public:
    /**
     * @brief 构造函数
     * @param itemName 商品名称
     * @param requested 请求数量
     * @param available 可用数量
     */
    InsufficientStockException(const std::string& itemName, int requested, int available)
        : OrderException("库存不足！商品\"" + itemName + "\"请求数量：" + 
                        std::to_string(requested) + "，可用库存：" + 
                        std::to_string(available)) {}
};

#endif // ORDER_EXCEPTION_H
