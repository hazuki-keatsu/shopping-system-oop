# 购物系统构建脚本
# 使用方法: .\build.ps1

Write-Host "========== 购物系统构建脚本 ==========" -ForegroundColor Cyan

# 清理旧的构建文件
if (Test-Path "build") {
    Write-Host "清理旧的构建目录..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force build
}

# 创建构建目录
Write-Host "创建构建目录..." -ForegroundColor Green
New-Item -ItemType Directory -Path "build" | Out-Null

# 进入构建目录
Set-Location build

# 运行CMake
Write-Host "运行CMake生成构建文件..." -ForegroundColor Green
cmake -G "MinGW Makefiles" ..

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake配置失败！" -ForegroundColor Red
    Set-Location ..
    exit 1
}

# 编译项目
Write-Host "编译项目..." -ForegroundColor Green
mingw32-make

if ($LASTEXITCODE -ne 0) {
    Write-Host "编译失败！" -ForegroundColor Red
    Set-Location ..
    exit 1
}

# 返回根目录
Set-Location ..

Write-Host "`n========== 构建成功！ ==========" -ForegroundColor Green
Write-Host "可执行文件位于: bin\ShoppingSystem.exe" -ForegroundColor Cyan
Write-Host "运行程序: cd bin; .\ShoppingSystem.exe" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Green
