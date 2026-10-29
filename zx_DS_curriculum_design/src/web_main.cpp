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

// Web接口专用的main函数
int main(int argc, char* argv[])
{
    // 设置控制台编码为UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // 设置C语言本地化环境
    setlocale(LC_ALL, "zh_CN.UTF-8");
    
    if (argc > 1) {
        // 如果有命令行参数，处理Web请求
        int difficulty = atoi(argv[1]);
        
        srand(time(NULL));
        
        // 初始化棋盘
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                arr[i][j] = 0;
                ans[i][j] = 0;
            }
        }
        
        // 生成完整的百分号数独棋盘
        if (fillPercentSudoku(1, 1)) {
            // 保存答案
            for (int i = 1; i <= 9; i++) {
                for (int j = 1; j <= 9; j++) {
                    ans[i][j] = arr[i][j];
                }
            }
            
            // 根据难度设置挖洞数量
            int holes;
            if (difficulty == 1) holes = rand() % 11 + 30;      // 简单：30-40个洞
            else if (difficulty == 2) holes = rand() % 11 + 40; // 中等：40-50个洞
            else holes = rand() % 11 + 50;                       // 困难：50-60个洞
            
            // 挖洞创建游戏
            createPercentPuzzle(holes);
            
            // 输出题目（9行）
            for (int i = 1; i <= 9; i++) {
                for (int j = 1; j <= 9; j++) {
                    printf("%d", arr[i][j]);
                    if (j < 9) printf(" ");
                }
                printf("\n");
            }
            
            // 输出答案（9行）
            for (int i = 1; i <= 9; i++) {
                for (int j = 1; j <= 9; j++) {
                    printf("%d", ans[i][j]);
                    if (j < 9) printf(" ");
                }
                printf("\n");
            }
        } else {
            // 如果生成失败，返回错误状态
            return 1;
        }
        
        return 0;
    } else {
        // 没有参数，运行原来的交互式程序
        display();
        return 0;
    }
}
