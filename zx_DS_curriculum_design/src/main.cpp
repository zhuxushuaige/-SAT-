#include "../include/def.hpp"
#include "../include/cnfparser.hpp"
#include "../include/display.hpp"
#include "../include/DPLL.hpp"
#include "../include/Percent_sudoku.hpp"
#include <windows.h>
#include <locale.h>

// 定义全局变量
int arr[10][10];
int ans[10][10];
int difficluty_choice, holes;

int main()
{
    // 设置控制台编码为UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // 设置C语言本地化环境
    setlocale(LC_ALL, "zh_CN.UTF-8");
    
    display();
    return 0;
}