# 购物系统

## 项目简介

这是一个基于C++开发的简单购物系统，提供了用户管理、登录认证等基础功能。系统支持管理员和顾客两种角色，管理员可以查看用户信息，顾客可以注册、登录、查看商品信息和修改密码。

## 功能特性

### 1. 登录功能
- **管理员登录**：通过配置文件预设的账号密码登录
- **顾客登录**：使用注册的账号密码登录
- **顾客注册**：首次使用需要注册，设置用户名、密码和手机号

### 2. 用户管理
- **保存顾客信息**：将注册信息保存到CSV文件
- **修改密码**：顾客可以修改自己的密码
- **查看用户**：管理员可以查看所有顾客信息

### 3. 商品浏览
- **未登录查看**：顾客无需登录即可查看商品信息
- **登录后查看**：登录后可以浏览商品信息（为购买功能预留接口）

## 技术架构

### 设计原则
- **面向对象设计**：采用OOP思想，实现高内聚、低耦合
- **模块化架构**：分为配置管理、用户管理、登录系统等独立模块
- **单例模式**：配置管理器采用单例模式
- **智能指针**：使用`std::shared_ptr`管理对象生命周期

### 项目结构
```
shopping/
├── CMakeLists.txt          # CMake构建配置
├── config.yaml             # 系统配置文件
├── dev.md                  # 开发需求文档
├── README.md               # 项目说明文档
├── ChangeLog.md            # 变更日志
├── Include/                # 头文件目录
│   ├── Config.h            # 配置管理类
│   ├── Login/
│   │   └── LoginSystem.h   # 登录系统类
│   └── UserManage/
│       ├── User.h          # 用户基类和子类
│       └── UserManager.h   # 用户管理器
├── Src/                    # 源文件目录
│   ├── Config.cpp
│   ├── Login/
│   │   └── LoginSystem.cpp
│   ├── Main/
│   │   └── main.cpp        # 主程序入口
│   └── UserManage/
│       ├── User.cpp
│       └── UserManager.cpp
├── data/                   # 数据文件目录
│   ├── users.csv           # 用户数据文件
│   └── items.csv           # 商品数据文件
└── bin/                    # 编译输出目录
```

### 核心模块

#### 1. Config（配置管理）
- 负责读取和解析`config.yaml`配置文件
- 提供管理员账号密码
- 提供数据文件路径
- 采用单例模式确保全局唯一配置

#### 2. UserManage（用户管理）
- **User**：用户基类，定义通用属性和方法
- **Customer**：顾客类，继承自User
- **Admin**：管理员类，继承自User
- **UserManager**：用户管理器，负责CSV文件读写和用户增删改查

#### 3. Login（登录系统）
- **LoginSystem**：登录系统类
  - 处理管理员和顾客的登录验证
  - 顾客注册功能
  - 密码修改功能
  - 会话管理

#### 4. Main（主程序）
- 提供控制台交互界面
- 整合各模块功能
- 根据用户角色显示不同菜单

## 编译和运行

### 环境要求
- **操作系统**：Windows
- **编译器**：MinGW（支持C++17）
- **构建工具**：CMake 3.15+

### 编译步骤

1. 创建构建目录：
```powershell
mkdir build
cd build
```

2. 生成构建文件：
```powershell
cmake -G "MinGW Makefiles" ..
```

3. 编译项目：
```powershell
mingw32-make
```

4. 运行程序：
```powershell
cd ..\bin
.\ShoppingSystem.exe
```

### 快速编译脚本
也可以使用以下命令一键编译：
```powershell
# 清理旧的构建文件
if (Test-Path build) { Remove-Item -Recurse -Force build }
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
cd ..
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
  users: data/users.csv
  items: data/items.csv
```

### users.csv
```csv
username,password,phone
```

### items.csv
```csv
item_id,item_name,price,stock
1,Apple iPhone 15,5999.00,50
2,Samsung Galaxy S24,4999.00,30
```

## 使用说明

### 1. 启动程序
运行`ShoppingSystem.exe`后，会显示主菜单：

```
========== 购物系统 ==========
1. 顾客注册
2. 顾客登录
3. 管理员登录
4. 查看商品信息（无需登录）
0. 退出系统
=============================
```

### 2. 顾客注册
选择选项1，输入用户名、密码和手机号即可注册。

### 3. 顾客登录
选择选项2，使用注册的账号密码登录后，可以：
- 查看商品信息
- 修改密码
- 登出

### 4. 管理员登录
选择选项3，使用配置文件中的管理员账号登录后，可以：
- 查看所有顾客信息
- 登出

默认管理员账号：
- 用户名：`admin`
- 密码：`admin123`

## 代码规范

### 注释规范
- 所有类和函数都有完整的Doxygen风格注释
- 注释量约占代码量的1/3
- 注释包含功能描述、参数说明、返回值说明等

### 命名规范
- **类名**：大驼峰命名法（如`LoginSystem`）
- **函数名**：小驼峰命名法（如`loadConfig`）
- **变量名**：小驼峰命名法（如`currentUser`）
- **常量名**：大写字母+下划线（如需要）

### 文件组织
- 头文件使用`.h`扩展名
- 源文件使用`.cpp`扩展名
- 头文件包含类声明和接口
- 源文件包含具体实现

## 扩展建议

### 未来可添加的功能
1. **购物车功能**：顾客可以将商品加入购物车
2. **订单管理**：记录购买历史和订单信息
3. **库存管理**：管理员可以添加、修改、删除商品
4. **数据加密**：对密码进行加密存储
5. **日志系统**：记录系统操作日志
6. **权限细化**：更详细的权限管理

## 常见问题

### Q: 编译时找不到头文件？
A: 确保CMakeLists.txt中的`include_directories`设置正确。

### Q: 运行时找不到配置文件？
A: 确保在bin目录下运行程序，且config.yaml文件已复制到该目录。

### Q: 无法保存用户数据？
A: 检查data目录是否存在，以及程序是否有写入权限。

## 许可证

本项目仅供学习和研究使用。

## 作者

Shopping System Development Team

## 更新日志

详见 [ChangeLog.md](ChangeLog.md)
