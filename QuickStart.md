# 快速开始指南

## 一、编译项目

### 方法1：使用构建脚本（推荐）
```powershell
.\build.ps1
```

### 方法2：手动编译
```powershell
# 1. 创建构建目录
mkdir build
cd build

# 2. 生成构建文件
cmake -G "MinGW Makefiles" ..

# 3. 编译
mingw32-make

# 4. 返回根目录
cd ..
```

## 二、运行程序

### 方法1：使用运行脚本（推荐）
```powershell
.\run.ps1
```

### 方法2：手动运行
```powershell
cd bin
.\ShoppingSystem.exe
```

## 三、使用示例

### 场景1：顾客首次使用

1. 启动程序后，选择 `1` 进行注册
2. 输入用户名、密码、手机号
3. 注册成功后，选择 `2` 登录
4. 登录后可以查看商品、修改密码等

**示例操作流程：**
```
========== 购物系统 ==========
1. 顾客注册
2. 顾客登录
3. 管理员登录
4. 查看商品信息（无需登录）
0. 退出系统
=============================
请选择: 1

===== 顾客注册 =====
请输入用户名: zhangsan
请输入密码: 123456
请输入手机号: 13800138000
注册成功！请使用新账号登录。

[然后选择2登录...]
```

### 场景2：顾客查看商品（无需登录）

1. 启动程序后，选择 `4` 查看商品信息
2. 无需登录即可浏览商品列表

### 场景3：管理员登录查看用户

1. 启动程序后，选择 `3` 管理员登录
2. 输入管理员账号：`admin`
3. 输入管理员密码：`admin123`
4. 登录后选择 `1` 查看所有顾客信息

**示例操作流程：**
```
请选择: 3

===== 管理员登录 =====
请输入用户名: admin
请输入密码: admin123
管理员登录成功！欢迎，admin

===== 管理员菜单 =====
1. 查看所有顾客信息
2. 登出
======================
请选择: 1

===== 所有顾客信息 =====
用户名          密码            手机号
----------------------------------------
zhangsan        123456          13800138000
lisi            654321          13900139000
========================
共 2 个顾客。
```

### 场景4：顾客修改密码

1. 以顾客身份登录
2. 在顾客菜单选择 `2` 修改密码
3. 输入旧密码、新密码、确认新密码
4. 修改成功

**示例操作流程：**
```
===== 顾客菜单 =====
1. 查看商品信息
2. 修改密码
3. 登出
=====================
请选择: 2

===== 修改密码 =====
请输入旧密码: 123456
请输入新密码: 888888
请确认新密码: 888888
密码修改成功！
```

## 四、配置文件说明

### config.yaml
位置：项目根目录

```yaml
# 管理员账户信息
admin:
  username: admin      # 可自定义管理员用户名
  password: admin123   # 可自定义管理员密码

# 数据文件路径
data_files:
  users: data/users.csv   # 用户数据文件
  items: data/items.csv   # 商品数据文件
```

修改配置文件后，需要重新编译或重启程序。

## 五、数据文件说明

### users.csv
位置：`data/users.csv`

格式：
```csv
username,password,phone
zhangsan,123456,13800138000
lisi,654321,13900139000
```

**注意：** 该文件在首次注册用户时自动创建，管理员账户不会存储在此文件中。

### items.csv
位置：`data/items.csv`

格式：
```csv
item_id,item_name,price,stock
1,Apple iPhone 15,5999.00,50
2,Samsung Galaxy S24,4999.00,30
3,Xiaomi 14 Pro,3999.00,100
```

可以手动编辑此文件来添加、修改商品信息。

## 六、常见问题

### Q1: 编译失败，提示找不到cmake
**解决方法：**
1. 确保已安装CMake：`cmake --version`
2. 如未安装，请从 [cmake.org](https://cmake.org/download/) 下载安装
3. 安装后重启命令行

### Q2: 编译失败，提示找不到mingw32-make
**解决方法：**
1. 确保已安装MinGW
2. 将MinGW的bin目录添加到系统PATH
3. 验证：`mingw32-make --version`

### Q3: 运行时提示"无法打开配置文件"
**解决方法：**
1. 确保在bin目录下运行程序
2. 确保config.yaml已复制到bin目录
3. 或使用提供的run.ps1脚本运行

### Q4: 找不到用户数据文件
**解决方法：**
- 这是正常的，程序首次运行时会自动创建
- 确保data目录存在
- 程序会在首次注册用户时创建users.csv

### Q5: 忘记管理员密码怎么办？
**解决方法：**
- 编辑项目根目录下的config.yaml文件
- 修改admin部分的password字段
- 重新启动程序

## 七、开发和调试

### 查看编译详细信息
```powershell
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make VERBOSE=1
```

### 清理编译产物
```powershell
Remove-Item -Recurse -Force build, bin
```

### 重新编译
```powershell
.\build.ps1
```

## 八、目录结构说明

```
shopping/
├── bin/                    # 可执行文件输出目录
│   ├── ShoppingSystem.exe  # 主程序
│   └── config.yaml         # 配置文件（自动复制）
├── build/                  # 构建临时文件目录
├── data/                   # 数据文件目录
│   ├── users.csv           # 用户数据
│   └── items.csv           # 商品数据
├── Include/                # 头文件目录
│   ├── Config.h
│   ├── Login/
│   └── UserManage/
├── Src/                    # 源文件目录
│   ├── Config.cpp
│   ├── Login/
│   ├── Main/
│   └── UserManage/
├── build.ps1               # 构建脚本
├── run.ps1                 # 运行脚本
├── CMakeLists.txt          # CMake配置
├── config.yaml             # 配置文件
├── README.md               # 项目说明
├── ChangeLog.md            # 变更日志
└── QuickStart.md           # 本文档
```

## 九、技术支持

如遇到问题：
1. 查看README.md了解详细信息
2. 查看ChangeLog.md了解版本历史
3. 检查代码注释了解具体实现

---

祝使用愉快！
