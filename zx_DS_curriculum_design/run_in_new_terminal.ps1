param($file = "src\web_main.cpp")
$projectRoot = "d:\SAT_AND_DPLL\zx_DS_curriculum_design"
$webRoot = "d:\SAT_AND_DPLL\zx_DS_curriculum_design"

# 设置到web根目录进行编译
Set-Location $webRoot
Write-Host "Compiling web_main.cpp..." -ForegroundColor Yellow
g++ $file -o "web_main.exe" -I./include
if ($LASTEXITCODE -eq 0) {
    Write-Host "Compilation successful!" -ForegroundColor Green
    Write-Host "Starting both console application and web server..." -ForegroundColor Yellow
    
    # 从项目根目录运行控制台程序
    Start-Process powershell -ArgumentList "-NoExit", "-Command", "[Console]::OutputEncoding = [System.Text.Encoding]::UTF8; [Console]::InputEncoding = [System.Text.Encoding]::UTF8; Set-Location '$projectRoot'; Write-Host '=== Console Application ===' -ForegroundColor Cyan; Write-Host 'Console Application Started - Choose option 1 for Percent-Sudoku' -ForegroundColor Green; Write-Host 'CNF files are located in this directory for option 4' -ForegroundColor Yellow; & '.\web_main.exe'"
    
    # 启动Web服务器（从正确的目录）
    Start-Process powershell -ArgumentList "-NoExit", "-Command", "Set-Location '$webRoot'; Write-Host '=== Web Server ===' -ForegroundColor Cyan; Write-Host 'Starting Web Server...' -ForegroundColor Green; if (!(Test-Path 'node_modules')) { Write-Host 'Installing npm dependencies...' -ForegroundColor Yellow; npm install }; Write-Host 'Web server starting at http://localhost:8080' -ForegroundColor Green; Write-Host 'Browser will open automatically...' -ForegroundColor White; Start-Sleep -Seconds 2; Start-Process 'http://localhost:8080'; node server.js"
    
    Write-Host "Both applications are starting..." -ForegroundColor Cyan
    Write-Host "- Console app: New PowerShell window (for SAT solving)" -ForegroundColor White
    Write-Host "- Web app: http://localhost:8080 (will open in browser)" -ForegroundColor White
}
