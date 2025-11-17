# 购物系统运行脚本
# 使用方法: .\run.ps1

Write-Host "========== 启动购物系统 ==========" -ForegroundColor Cyan

# 检查可执行文件是否存在
if (-not (Test-Path "bin\ShoppingSystem.exe")) {
    Write-Host "错误：找不到可执行文件！" -ForegroundColor Red
    Write-Host "请先运行构建脚本: .\build.ps1" -ForegroundColor Yellow
    exit 1
}

# 进入bin目录
Set-Location bin

# 运行程序
Write-Host "启动程序..." -ForegroundColor Green
.\ShoppingSystem.exe

# 返回根目录
Set-Location ..
