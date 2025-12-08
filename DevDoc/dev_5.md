系统支持商品的各种促销优惠活动。若顾客购买的商品参与促销活动，在计算订单总额时需按促销价计算。

促销活动都有有效期的限制；所有的促销活动的相关信息均由管理员进行设置。

1. 限时折扣。给商品设置折扣活动（如8折、9折等），不同的商品可以设置不同的折扣。
2. 满减活动。订单总额满足限定额度给予一定的满减，如满300减50等。
3. 叠加促销活动。折扣与满减可以叠加，如果商品参与叠加促销，优先计算折扣再计算满减。
4. 若商品参与促销活动，在商品列表中需显示相应促销标签（折扣或是满减）。

---

基本要求：
1. 符合面向对象的开发设计规范，实现高内聚、低耦合特征
2. 注释量要有代码量的1/3
3. 项目结构符合cpp的最佳实践
4. 开发完成之后更新Readme文件、ChangeLog文件和构建脚本
5. 保持项目原有的代码风格

实现提示：
在这一轮开发中，您需要维护两个类：Promotion类和PromotionManager类，他们的位置是`Src/Promotion/Promotion.cpp`、`Include/Promotion/Promotion.h`和`Src/Promotion/PromotionManager.cpp`、`Include/Promotion/PromotionManager.h`

## 实现提示

### 一、核心类设计

#### 1. Promotion 类（促销活动类）

**职责**：表示单个促销活动，存储促销的基本信息和规则。

**核心数据成员**：
- `promotionId`：促销活动的唯一标识符（字符串）
- `promotionName`：促销名称（如"电子产品8折"、"全场满300减50"）
- `promotionType`：促销类型枚举（`DISCOUNT` 表示折扣，`FULL_REDUCTION` 表示满减）
- `isActive`：促销是否启用（布尔值）
- `startTime` 和 `endTime`：促销有效期的开始和结束时间（使用 `time_t` 类型）

**限时折扣特有数据**：
- `targetItemId`：目标商品ID（如果为空表示全场折扣，否则表示针对特定商品）
- `discountRate`：折扣率（如 0.8 表示8折，0.9 表示9折）

**满减活动特有数据**：
- `thresholdAmount`：满减门槛金额（如满300元）
- `reductionAmount`：减免金额（如减50元）

**核心方法**：
- `isValid()`：检查促销活动是否在有效期内且已启用
- `isApplicableToItem(itemId)`：检查某个商品是否适用该促销（仅折扣促销使用）
- `calculateDiscountForItem(originalPrice)`：计算商品折扣后的价格（仅折扣促销使用）
- `calculateReduction(totalAmount)`：计算满减优惠金额（仅满减促销使用）
- `getDisplayTag()`：返回促销标签文本（如"8折"、"满300减50"）

#### 2. PromotionManager 类（促销管理器类）

**职责**：管理所有促销活动，负责促销的加载、查询和计算。

**核心数据成员**：
- `promotions`：存储所有促销活动的向量（`std::vector<std::shared_ptr<Promotion>>`）
- `filePath`：促销数据文件路径（CSV格式）

**核心方法**：
- `loadFromFile()`：从CSV文件加载促销数据
- `saveToFile()`：保存促销数据到CSV文件
- `addPromotion(promotion)`：添加新的促销活动（管理员功能）
- `deletePromotion(promotionId)`：删除促销活动（管理员功能）
- `updatePromotion(promotion)`：更新促销活动（管理员功能）
- `getActiveDiscountForItem(itemId)`：获取某个商品当前有效的折扣促销
- `getActiveFullReductions()`：获取当前所有有效的满减促销
- `calculatePromotionResult(items)`：核心计算方法，计算一组商品的促销优惠结果

### 二、数据存储设计

#### promotions.csv 文件格式

```
promotion_id,promotion_name,promotion_type,is_active,start_time,end_time,target_item_id,discount_rate,threshold_amount,reduction_amount
PROMO001,电子产品8折,DISCOUNT,1,1733616000,1735689600,ITEM001,0.8,,
PROMO002,全场9折,DISCOUNT,1,1733616000,1735689600,,0.9,,
PROMO003,满300减50,FULL_REDUCTION,1,1733616000,1735689600,,,300.0,50.0
PROMO004,满500减100,FULL_REDUCTION,1,1733616000,1735689600,,,500.0,100.0
```

**字段说明**：
- 折扣促销：填写 `target_item_id`（可为空）和 `discount_rate`，其他字段留空
- 满减促销：填写 `threshold_amount` 和 `reduction_amount`，其他字段留空

### 三、核心功能实现流程

#### 1. 商品列表显示促销标签

**位置**：`ItemManager::displayAllItems()` 方法中

**实现步骤**：
1. 遍历每个商品时，调用 `PromotionManager::getActiveDiscountForItem(itemId)`
2. 如果返回有效的折扣促销，在商品名称后面添加标签（如 `[8折]`）
3. 在商品列表末尾显示当前有效的满减活动（如 `【当前满减活动：满300减50】`）

#### 2. 订单创建前的促销计算与展示

**位置**：`main.cpp` 中的 `processPurchaseInput()` 和购物车结算流程

**实现步骤**：

**步骤1：创建确认订单函数**
```cpp
bool confirmOrderWithPromotion(
    const std::vector<std::pair<std::shared_ptr<Item>, int>>& items,
    PromotionManager* promotionManager
)
```

**步骤2：计算促销**
- 调用 `PromotionManager::calculatePromotionResult(items)` 获取促销结果
- 促销结果包含：原价、折扣后价格、满减后最终价格、应用的促销列表

**步骤3：展示订单预览**
```
========== 订单预览 ==========
商品明细：
  苹果手机 x1 = ¥5999.00 [8折]
  笔记本电脑 x1 = ¥4500.00
--------------------------------
商品原价：¥10499.00
折扣优惠：-¥1199.80（苹果手机8折）
小计：¥9299.20
满减优惠：-¥50.00（满300减50）
==============================
实付金额：¥9249.20 【已省¥1249.80】
==============================
```

**步骤4：用户确认**
- 提示"是否确认下单？(y/n)"
- 用户输入 y 继续创建订单，输入 n 取消

#### 3. 促销计算的核心逻辑

**`PromotionManager::calculatePromotionResult()` 方法实现**：

**第一步：计算折扣**
1. 遍历购物车中的每个商品
2. 查询该商品是否有有效的折扣促销
3. 如果有，计算折扣价 = 原价 × 折扣率
4. 累加所有商品的折扣后价格，得到折扣小计
5. 记录应用的折扣促销信息（用于显示）

**第二步：计算满减**
1. 使用折扣后的小计金额作为基数
2. 遍历所有有效的满减促销
3. 检查金额是否达到门槛（如 ≥300）
4. 如果达到，应用减免金额
5. 可以支持多档满减叠加（如满300减50，满500再减100）
6. 记录应用的满减促销信息（用于显示）

**第三步：返回结果**
- 封装成 `PromotionResult` 结构体
- 包含：原价、折扣后价格、最终价格、总优惠金额、应用的促销列表

### 四、管理员功能集成

#### 在管理员菜单中添加"促销管理"选项

**位置**：`main.cpp` 的 `showAdminMenu()` 和管理员菜单的 switch-case

**实现流程**：

**1. 添加菜单选项**
- 在管理员菜单中添加"8. 促销管理"

**2. 实现促销管理流程函数**
```cpp
void managePromotionsProcess(PromotionManager* promotionManager)
```

**3. 子菜单功能**：
- 查看所有促销活动（`displayAllPromotions()`）
- 添加折扣促销（输入：商品ID/留空表示全场、折扣率、有效期）
- 添加满减促销（输入：门槛金额、减免金额、有效期）
- 修改促销活动（选择促销ID，修改参数）
- 删除促销活动（输入促销ID确认删除）
- 启用/禁用促销（切换 `isActive` 状态）

### 五、配置文件更新

**在 `res/config.yaml` 中添加**：
```yaml
# 促销数据文件路径
data_files:
  promotions: res/data/promotions.csv
```

**在 `Config` 类中添加**：
- `getPromotionsFilePath()` 方法
- 解析 `promotions` 配置项

### 六、主函数初始化流程

**在 `main()` 函数中**：

1. 在初始化 `OrderManager` 之前，先初始化 `PromotionManager`
```cpp
PromotionManager promotionManager(config->getPromotionsFilePath());
promotionManager.loadFromFile();
```

2. 将 `promotionManager` 传递给需要使用的函数：
   - `viewItems()` - 显示商品时展示促销标签
   - `processPurchaseInput()` - 结算时计算促销
   - `shoppingCartProcess()` - 购物车结算时计算促销

### 七、关键技术点

1. **时间有效性检查**：使用 `time(nullptr)` 获取当前时间，与促销的 `startTime` 和 `endTime` 比较
2. **优先级处理**：折扣优先于满减，先计算折扣价，再用折扣后的总价判断满减
3. **多档满减**：支持同时应用多个满减（如满300减50 + 满500减100 = 总共减150）
4. **空值处理**：折扣的 `targetItemId` 为空表示全场，满减不需要指定商品
5. **CSV解析**：处理空字段（使用 `trim()` 和空字符串判断）
6. **内存管理**：使用 `shared_ptr` 管理 `Promotion` 对象

---

感谢 Claude Sonnet 4.5 的辛勤工作！！！

---

- [x] 直接创建订单的时候没有展示减免信息
- [x] 管理员没有修改优惠的方法
- [x] 由于csv文件没有使用占位符导致的促销信息读取问题
