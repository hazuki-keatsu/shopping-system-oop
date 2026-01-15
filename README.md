# 购物系统

## 项目简介

这是西安电子科技大学2024级软件工程专业《面向对象程序设计》课程的期末项目——模拟购物系统。

这个系统是基于C++开发的功能完整的购物系统，提供了用户管理、登录认证、商品管理和智能搜索等功能。系统支持管理员和顾客两种角色，管理员可以管理商品和用户信息，顾客可以注册、登录、搜索和浏览商品。

## 功能特性

### 1. 登录功能
- **管理员登录**：通过配置文件预设的账号密码登录
- **顾客登录**：使用注册的账号密码登录
- **顾客注册**：首次使用需要注册，设置用户名、密码和手机号

### 2. 用户管理
- **保存顾客信息**：将注册信息保存到CSV文件
- **修改密码**：顾客可以修改自己的密码
- **查看用户**：管理员可以查看所有顾客信息

### 3. 商品管理（管理员功能）新增
- **查看商品**：表格化显示所有商品信息
- **添加商品**：添加新商品，支持自动生成ID
- **修改商品**：逐字段修改商品信息，实时显示更新
- **删除商品**：删除指定商品，带确认提示
- **双重索引**：使用vector和map<类别>维护商品数据
- **动态表头**：支持自定义CSV表头字段

### 4. 商品搜索（顾客功能）新增
- **精确搜索**：按商品名称、类别精确匹配
- **模糊搜索**：使用Levenshtein编辑距离算法
  - 计算字符串相似度
  - 支持关键字部分匹配
  - 搜索商品名称和描述
  - 结果按相似度排序
- **智能策略**：先精确搜索，无结果自动切换模糊搜索
- **结果展示**：表格化显示，支持相似度百分比

### 5. 促销管理（管理员和顾客功能）
- **折扣促销**
  - 支持针对特定商品或全场折扣
  - 设置折扣率（如8折、9折）
  - 商品列表自动显示折扣标签
- **满减促销**
  - 设置满减门槛和减免金额
  - 支持多档满减叠加
  - 自动计算优惠总额
- **促销叠加**
  - 折扣优先计算，再计算满减
  - 订单预览展示详细优惠明细
  - 显示原价、折扣价、满减后价格
- **促销管理**（管理员功能）
  - 添加/删除/启用/禁用促销活动
  - 设置促销有效期
  - 查看所有或有效的促销活动
- **购物体验优化**
  - 下单前展示促销预览和确认
  - 显示节省金额和优惠详情
  - 用户可选择确认或取消下单

### 6. 顾客购买数据统计分析最新
- **数据统计功能**
  - 按商品类别统计购买金额和频度
  - 按具体商品统计购买信息
  - 统计总购买金额、购买数量、购买频度
- **报告生成**
  - 自动生成CSV格式统计报告
  - 报告按用户名和日期命名
  - 包含类别统计和商品详细统计两部分
- **控制台显示**
  - 支持在控制台直接查看统计数据
  - 数据按购买金额排序显示
  - 商品统计显示前10名
- **应用场景**
  - 顾客查看自己的消费情况
  - 分析购买偏好和习惯
  - 便于做出购买决策

## 技术架构

### 设计原则
- **面向对象设计**：采用OOP思想，实现高内聚、低耦合
- **模块化架构**：分为配置管理、用户管理、登录系统等独立模块
- **智能指针**：使用`std::shared_ptr`管理对象生命周期
- **代码文件**：头文件和源文件分离，便于维护
- **注释风格**：使用Doxygen风格注释
- **构建工具**：使用CMake进行跨平台构建

### 项目结构
```
shopping/
├── CMakeLists.txt                  # CMake构建配置
├── DevDoc/                         # 开发文档目录
├── README.md                       # 项目说明文档
├── ChangeLog.md                    # 变更日志
├── run.ps1                         # 运行脚本
├── build.ps1                       # 构建脚本
├── output_all_code.ps1             # 导出所有代码脚本
├── Include/                        # 头文件目录
│   ├── DependencyInterfaces.h      # 依赖接口
│   ├── Config.h                    # 配置管理类
│   ├── Login/
│   │   └── LoginSystem.h           # 登录系统类
│   ├── UserManage/
│   │   ├── User.h                  # 用户基类和子类
│   │   └── UserManager.h           # 用户管理器
│   ├── ItemManage/                 # 商品管理模块
│   │   ├── Item.h                  # 商品类
│   │   ├── ItemManager.h           # 商品管理器
│   │   └── ItemSearcher.h          # 商品搜索器
│   ├── ShoppingCart/               # 购物车模块
│   │   ├── ShoppingCart.h          # 购物车类
│   │   └── ShoppingCartManager.h   # 购物车管理器
│   ├── Order/                      # 订单模块
│   │   ├── Order.h                 # 订单类
│   │   ├── OrderManager.h          # 订单管理器
│   │   └── OrderException.h        # 订单异常类
│   ├── Promotion/                  # 促销管理模块
│   │   ├── Promotion.h             # 促销活动类
│   │   └── PromotionManager.h      # 促销管理器
│   └── Services/                   # 服务模块
│       └── CustomerReportService.h # 顾客购买数据统计服务
├── Src/                            # 源文件目录
│   ├── Config.cpp
│   ├── Login/
│   │   └── LoginSystem.cpp
│   ├── Main/
│   │   └── main.cpp                # 主程序入口
│   ├── UserManage/
│   │   ├── User.cpp
│   │   └── UserManager.cpp
│   ├── ItemManage/
│   │   ├── Item.cpp
│   │   ├── ItemManager.cpp
│   │   └── ItemSearcher.cpp
│   ├── ShoppingCart/
│   │   ├── ShoppingCart.cpp
│   │   └── ShoppingCartManager.cpp
│   ├── Order/
│   │   ├── Order.cpp
│   │   └── OrderManager.cpp
│   ├── Promotion/                  # 促销管理实现
│   │   ├── Promotion.cpp
│   │   └── PromotionManager.cpp
│   └── Services/                   # 服务模块实现
│       └── CustomerReportService.cpp # 顾客购买数据统计服务实现
├── res/                            # 资源文件目录
│   ├── config.yaml                 # 系统配置文件
│   └── data/                       # 数据文件目录
│       ├── users.csv               # 用户数据文件
│       ├── items.csv               # 商品数据文件
│       ├── shopping_cart.csv       # 购物车数据文件
│       ├── orders.csv              # 订单数据文件
│       └── promotions.csv          # 促销数据文件
└── bin/                            # 二进制文件夹
```

## 编译和运行

### 环境要求
- **操作系统**：Windows
- **编译器**：MinGW（支持C++17）
- **构建工具**：CMake 3.15+

### 编译步骤

```powershell
# 构建
.\build.ps1

# 运行
.\run.ps1
```

## 配置说明

### config.yaml
```yaml
# 管理员账户信息
admin:
  username: admin
  password: admin123

# 数据文件路径
data_files:
  users: res/data/users.csv
  items: res/data/items.csv
  shopping_cart: res/data/shopping_cart.csv
  orders: res/data/orders.csv
  promotions: res/data/promotions.csv  # 促销数据文件

# 订单自动化配置
order_settings:
  auto_update: false
  pending_to_shipped_seconds: 10
  shipped_to_delivered_seconds: 20
```

## 作者

[Hazuki Keatsu](https://github.com/hazuki-keatsu)
