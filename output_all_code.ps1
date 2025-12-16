# 导出所有 .h 和 .cpp 文件到单个文本文件

$outputFile = "all_code.txt"
$encoding = [System.Text.Encoding]::UTF8

# 清空输出文件（如果存在）
if (Test-Path $outputFile) {
    Remove-Item $outputFile
}

# 获取所有代码文件（只从 Src 和 Include 文件夹）
$files = Get-ChildItem -Path Src, Include -Recurse -Include *.h, *.cpp | Sort-Object FullName

$fileCount = $files.Count
Write-Host "找到 $fileCount 个代码文件，正在导出..." -ForegroundColor Cyan

# 导出文件
$files | ForEach-Object -Begin { $i = 0 } {
    $i++
    $percentage = [math]::Round($i / $fileCount * 100, 1)
    Write-Progress -Activity "导出代码文件" -Status "$($_.Name)" -PercentComplete $percentage
    
    # 文件头分隔符
    $header = "`n$('='*80)`n文件: $($_.FullName)`n路径: $($_.DirectoryName)`n$('='*80)`n"
    Add-Content -Path $outputFile -Value $header -Encoding $encoding
    
    # 文件内容
    Get-Content $_.FullName -Encoding UTF8 | Add-Content -Path $outputFile -Encoding $encoding
    
    # 文件尾分隔符
    $footer = "`n"
    Add-Content -Path $outputFile -Value $footer -Encoding $encoding
}

Write-Progress -Activity "导出代码文件" -Completed

# 获取输出文件信息
$fileInfo = Get-Item $outputFile
$sizeKB = [math]::Round($fileInfo.Length / 1KB, 2)
$sizeMB = [math]::Round($fileInfo.Length / 1MB, 2)

Write-Host "`n✅ 导出完成！" -ForegroundColor Green
Write-Host "文件路径: $(Get-Location)\$outputFile"
Write-Host "文件大小: $sizeKB KB ($sizeMB MB)"
Write-Host "代码文件数: $fileCount"
