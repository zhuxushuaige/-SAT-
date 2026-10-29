#pragma once
#include "def.hpp"
#include "cnfparser.hpp"
#include <direct.h>  // for _getcwd

//全局变量
extern int arr[10][10];
extern int ans[10][10];
extern int difficluty_choice, holes;

void printPercentSudoku(int arr[][10]); // 打印百分号数独棋盘（带阴影窗口显示）
int isSafePercent(int row, int col, int num); // 检查数字在百分号数独中的合法性
int fillPercentSudoku(int row, int col);      // 生成完整百分号数独棋盘
bool solveAndCountPercent(int row, int col);  // 检查百分号数独解的唯一性
bool hasUniqueSolutionPercent();
void createPercentPuzzle(int holes);                            // 挖洞生成百分号数独游戏
void CreatePercentSudokuCNF(int arr[][10], char sudokuName[30]); // 创建百分号数独的cnf公式并存入文件

void GetShuduValue(int arr[][10])
{
    int cnt = 1, i, j;
    for (i = 1; i <= 9; i++)
        for (j = 1; j <= 9; j++)
        {
            while (LiteralsInfo[cnt].value == 0)
                cnt++;
            arr[i][j] = cnt - (i - 1) * 81 - (j - 1) * 9;
            cnt++;
        }
}

// 打印百分号数独棋盘（带阴影窗口显示）
void printPercentSudoku(int arr[][10])
{
    printf("百分号数独（Percent-Sudoku）:\n");
    printf("撇对角线: /, 窗口: 上方[2-4,2-4], 下方[6-8,6-8]\n\n");
    
    for (int i = 0; i <= 9; i++)
    {
        for (int j = 0; j <= 9; j++)
        {
            if (j == 0 && i >= 1)
                printf("|");
            if (i >= 1 && j >= 1)
            {
                // 检查是否在窗口区域
                bool isWindow = ((i >= 2 && i <= 4 && j >= 2 && j <= 4) || 
                               (i >= 6 && i <= 8 && j >= 6 && j <= 8));
                
                if (arr[i][j] >= 1 && arr[i][j] <= 9)
                {
                    if (isWindow)
                        printf("[%d]|", arr[i][j]); // 用[]标记窗口区域
                    else
                        printf(" %d |", arr[i][j]);
                }
                else
                {
                    if (isWindow)
                        printf("[ ]|"); // 空的窗口格子
                    else
                        printf("   |");
                }
            }
        }
        printf("\n-------------------------------------\n");
    }
    printf("说明: [n] 表示窗口约束区域\n");
}

// 检查数字在百分号数独中的合法性
int isSafePercent(int row, int col, int num)
{
    // 检查行和列
    for (int x = 1; x <= 9; x++)
    {
        if (arr[row][x] == num || arr[x][col] == num)
        {
            return 0;
        }
    }
    
    // 检查3x3子宫格
    int startRow = row - (row - 1) % 3;
    int startCol = col - (col - 1) % 3;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (arr[i + startRow][j + startCol] == num)
            {
                return 0;
            }
        }
    }
    
    // 检查撇对角线（右上到左下）
    if (row + col == 10)
    {
        for (int i = 1; i <= 9; i++)
        {
            if (arr[i][10 - i] == num)
            {
                return 0;
            }
        }
    }
    
    // 检查上方窗口约束 (2-4, 2-4)
    if (row >= 2 && row <= 4 && col >= 2 && col <= 4)
    {
        for (int i = 2; i <= 4; i++)
        {
            for (int j = 2; j <= 4; j++)
            {
                if (arr[i][j] == num)
                {
                    return 0;
                }
            }
        }
    }
    
    // 检查下方窗口约束 (6-8, 6-8)
    if (row >= 6 && row <= 8 && col >= 6 && col <= 8)
    {
        for (int i = 6; i <= 8; i++)
        {
            for (int j = 6; j <= 8; j++)
            {
                if (arr[i][j] == num)
                {
                    return 0;
                }
            }
        }
    }
    
    return 1;
}

// 生成完整百分号数独棋盘
int fillPercentSudoku(int row, int col)
{
    if (row == 10)
    {
        return 1; // 成功填充
    }
    if (col == 10)
    {
        return fillPercentSudoku(row + 1, 1);
    }
    if (arr[row][col] != 0)
    {
        return fillPercentSudoku(row, col + 1);
    }
    
    // 随机生成1到9的数字
    int numList[9];
    for (int i = 0; i < 9; i++)
    {
        numList[i] = i + 1;
    }
    // 洗牌把numList[0]到numList[8]（也就是1到9）打乱
     for (int i = 0; i < 9; i++)
    {
        int j = i + rand() % (9 - i);  // 从i到8中随机选择一个索引
        int temp = numList[i];         // 临时保存当前元素
        numList[i] = numList[j];       // 交换元素
        numList[j] = temp;             // 完成交换
    }
    for (int i = 0; i < 9; i++)
    {
        int num = numList[i];
        if (isSafePercent(row, col, num))
        {
            arr[row][col] = num;
            if (fillPercentSudoku(row, col + 1))
            {
                return 1;
            }
            arr[row][col] = 0; // 回溯
        }
    }
    return 0;
}

int solutionCount = 0;

// 检查百分号数独解的唯一性
bool solveAndCountPercent(int row, int col)
{
    if (row == 10)
    {
        solutionCount++;
        return solutionCount == 1;
    }
    if (col == 10)
    {
        return solveAndCountPercent(row + 1, 1);
    }
    if (arr[row][col] != 0)
    {
        return solveAndCountPercent(row, col + 1);
    }
    for (int num = 1; num <= 9; num++)
    {
        if (isSafePercent(row, col, num))
        {
            arr[row][col] = num;
            if (!solveAndCountPercent(row, col + 1))
            {
                arr[row][col] = 0;
                return false;
            }
            arr[row][col] = 0;
        }
    }
    return true;
}

bool hasUniqueSolutionPercent()
{
    solutionCount = 0;
    solveAndCountPercent(1, 1);
    return solutionCount == 1;
}

// 挖洞生成百分号数独游戏 - 加上了唯一性检验
void createPercentPuzzle(int holes)
{
    int count = holes;
    while (count > 0)
    {
        int row = 1 + rand() % 9;
        int col = 1 + rand() % 9;
        if (arr[row][col] != 0)
        {
            int temp = arr[row][col];
            arr[row][col] = 0;
            if(hasUniqueSolutionPercent())
            {
                count--;
            }
            else
            {
                arr[row][col] = temp; // 不唯一则还原
            }
        }
    }
}

/***********************
 * 创建百分号数独的cnf公式并存入文件
 ***********************/
void CreatePercentSudokuCNF(int arr[][10], char sudokuName[30])
{
    int i, j, cnt = 0, hole = 0;
    char holes[20] = "", des[30] = "p cnf 729 ";
    FILE *fp = fopen(sudokuName, "w");
    
    // 计算空格数
    for (i = 1; i <= 9; i++)
    {
        for (j = 1; j <= 9; j++)
        {
            if (arr[i][j] == 0)
            {
                hole++;
            }
        }
    }

    int totalClauses = 12717+81-hole;
    printf("Generating Percent-Sudoku CNF with %d holes, total clauses: %d\n", hole, totalClauses);
    itoa(totalClauses, holes, 10);
    strcat(des, holes);
    strcat(des, "\n");
    fputs(des, fp);
    
    // 生成基础数独约束（使用正确的编码公式）
    printf("Adding basic Sudoku constraints...\n");
    
    // 1. 每个格子有且仅有一个数字
    for (int i = 1; i <= 9; i++) {
        for (int j = 1; j <= 9; j++) {
            // 至少有一个数字
            for (int num = 1; num <= 9; num++) {
                fprintf(fp, "%d ", (i-1)*81 + (j-1)*9 + num);
            }
            fprintf(fp, "0\n");
            
            // 最多有一个数字
            for (int num1 = 1; num1 <= 8; num1++) {
                for (int num2 = num1 + 1; num2 <= 9; num2++) {
                    fprintf(fp, "-%d -%d 0\n", 
                           (i-1)*81 + (j-1)*9 + num1,
                           (i-1)*81 + (j-1)*9 + num2);
                }
            }
        }
    }
    
    // 2. 每行每个数字最多出现一次
    for (int i = 1; i <= 9; i++) {
        for (int num = 1; num <= 9; num++) {
            for (int j1 = 1; j1 <= 9; j1++) {
                for (int j2 = j1 + 1; j2 <= 9; j2++) {
                    fprintf(fp, "-%d -%d 0\n", 
                           (i-1)*81 + (j1-1)*9 + num,
                           (i-1)*81 + (j2-1)*9 + num);
                }
            }
        }
    }
    
    // 3. 每列每个数字最多出现一次
    for (int j = 1; j <= 9; j++) {
        for (int num = 1; num <= 9; num++) {
            for (int i1 = 1; i1 <= 9; i1++) {
                for (int i2 = i1 + 1; i2 <= 9; i2++) {
                    fprintf(fp, "-%d -%d 0\n", 
                           (i1-1)*81 + (j-1)*9 + num,
                           (i2-1)*81 + (j-1)*9 + num);
                }
            }
        }
    }
    
    // 4. 每个3x3九宫格每个数字最多出现一次
    for (int box_row = 0; box_row < 3; box_row++) {
        for (int box_col = 0; box_col < 3; box_col++) {
            for (int num = 1; num <= 9; num++) {
                for (int pos1 = 0; pos1 < 9; pos1++) {
                    for (int pos2 = pos1 + 1; pos2 < 9; pos2++) {
                        int i1 = box_row * 3 + pos1 / 3 + 1;
                        int j1 = box_col * 3 + pos1 % 3 + 1;
                        int i2 = box_row * 3 + pos2 / 3 + 1;
                        int j2 = box_col * 3 + pos2 % 3 + 1;
                        fprintf(fp, "-%d -%d 0\n", 
                               (i1-1)*81 + (j1-1)*9 + num,
                               (i2-1)*81 + (j2-1)*9 + num);
                    }
                }
            }
        }
    }
    
    // 添加撇对角线约束（右上到左下：19,28,37,46,55,64,73,82,91）
    int diagPositions[] = {19, 28, 37, 46, 55, 64, 73, 82, 91};
    
    // 每个数字在撇对角线上最多出现一次
    for (int num = 1; num <= 9; num++)
    {
        for (int pos1 = 0; pos1 < 9; pos1++)
        {
            for (int pos2 = pos1 + 1; pos2 < 9; pos2++)
            {
                int linearPos1 = diagPositions[pos1];
                int i1 = linearPos1 / 10;
                int j1 = linearPos1 % 10;
                int linearPos2 = diagPositions[pos2];
                int i2 = linearPos2 / 10;
                int j2 = linearPos2 % 10;
                fprintf(fp, "-%d -%d 0\n", 
                       (i1-1)*81 + (j1-1)*9 + num,
                       (i2-1)*81 + (j2-1)*9 + num);
            }
        }
    }
    
    // 添加上方窗口约束（22,23,24,32,33,34,42,43,44）
    int upperWindow[] = {22, 23, 24, 32, 33, 34, 42, 43, 44};
    
    // 每个数字在上方窗口最多出现一次
    for (int num = 1; num <= 9; num++)
    {
        for (int pos1 = 0; pos1 < 9; pos1++)
        {
            for (int pos2 = pos1 + 1; pos2 < 9; pos2++)
            {
                int linearPos1 = upperWindow[pos1];
                int i1 = linearPos1 / 10;
                int j1 = linearPos1 % 10;
                int linearPos2 = upperWindow[pos2];
                int i2 = linearPos2 / 10;
                int j2 = linearPos2 % 10;
                fprintf(fp, "-%d -%d 0\n", 
                       (i1-1)*81 + (j1-1)*9 + num,
                       (i2-1)*81 + (j2-1)*9 + num);
            }
        }
    }
    
    // 添加下方窗口约束（66,67,68,76,77,78,86,87,88）
    int lowerWindow[] = {66, 67, 68, 76, 77, 78, 86, 87, 88};
    
    // 每个数字在下方窗口最多出现一次
    for (int num = 1; num <= 9; num++)
    {
        for (int pos1 = 0; pos1 < 9; pos1++)
        {
            for (int pos2 = pos1 + 1; pos2 < 9; pos2++)
            {
                int linearPos1 = lowerWindow[pos1];
                int i1 = linearPos1 / 10;
                int j1 = linearPos1 % 10;
                int linearPos2 = lowerWindow[pos2];
                int i2 = linearPos2 / 10;
                int j2 = linearPos2 % 10;
                fprintf(fp, "-%d -%d 0\n", 
                       (i1-1)*81 + (j1-1)*9 + num,
                       (i2-1)*81 + (j2-1)*9 + num);
            }
        }
    }
    
    // 对于特定百分号数独的提示数
    printf("Adding given numbers constraints...\n");
    for (i = 1; i <= 9; i++)
    {
        for (j = 1; j <= 9; j++)
        {
            if (arr[i][j] >= 1)
            {
                char str[20] = "", numStr[10];
                int num = (i - 1) * 81 + (j - 1) * 9 + arr[i][j];
                itoa(num, numStr, 10);
                strcat(str, numStr);
                strcat(str, " 0\n");
                cnt++;
                fputs(str, fp);
            }
        }
    }
    
    fclose(fp);
    printf("百分号数独CNF文件生成完成！\n");
}

// 创建百分号数独游戏的完整函数
void CreateArrAndAnsPercent(int arr[][10], int ans[][10])
{
    srand(time(NULL));
    // 初始化棋盘
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            arr[i][j] = 0;
        }
    }
    // 生成完整的百分号数独棋盘
    fillPercentSudoku(1, 1);
    // 保存一下答案
    for (int i = 1; i <= 9; i++)
    {
        for (int j = 1; j <= 9; j++)
        {
            ans[i][j] = arr[i][j];
        }
    }
    printf("Please choose percent-sudoku difficulty:\n");
    printf("1.Easy\n2.Medium\n3.Difficult\n");
    while (true)
    {
        scanf("%d", &difficluty_choice);
        if (difficluty_choice == 1)
        {
            holes = rand() % 11 + 30;  // 30-40个空格
            break;
        }
        else if (difficluty_choice == 2)
        {
            holes = rand() % 11 + 40;  // 40-50个空格
            break;
        }
        else if (difficluty_choice == 3)
        {
            holes = rand() % 11 + 50;  // 50-60个空格
            break;
        }
        else
        {
            printf("Wrong Input!Please input again:");
        }
    }

    // 挖洞以创建游戏
    createPercentPuzzle(holes);
}