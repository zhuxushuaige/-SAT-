#pragma once
#include "cnfparser.hpp"
#include "Percent_sudoku.hpp"
#include "DPLL.hpp"

void IntroductionOfX_sudoku();
void IntroductionOfX_SAT();
void IntroductionOfPercent_sudoku();
void CreateArrAndAnsPercent(int arr[][10], int ans[][10]);
void str_replace(char *str1, const char *str2, const char *str3);
void saveRes(char filename[30], SATStatus res, int time);

/****
 * 遍历一下邻接表,显示子句和文字
 * parmas:LinkClause headpC
 * return:void
 ***/
void showClauseMap(LinkClause headpC)
{
    if (!headpC) {
        printf("ERROR: headpC is NULL\n");
        return;
    }
    
    printf("Clause map loaded successfully\n");
}

/****
 * 遍历一下LiteralsInfo
 * parmas:LinkLInfo LiteralsInfo
 * return:void
 ***/
void showLiteralsInfo(LinkLInfo LiteralsInfo)
{
    if (!LiteralsInfo) {
        printf("ERROR: LiteralsInfo is NULL\n");
        return;
    }
    
    printf("Variables: %d loaded successfully\n", n);
}

/***********************
 * param: int(根据标记来判断选择哪种菜单)
 * return: void
 * res: 根据标记来判断选择哪种菜单
 ***********************/
void menu(int choice)
{
    switch (choice)
    {
    case 0:
    {
        printf("-------------------------------------------\n");
        printf("|             Initial Interface           |\n");
        printf("|              1.Percent-Sudoku           |\n");
        printf("|              2.SAT                      |\n");
        printf("|              0.Exit                     |\n");
        printf("-------------------------------------------\n");
        break;
    }
    case 1:
    {
        printf("----------------------------------------------\n");
        printf("|                MENU of X_Sudoku            |\n");
        printf("|           1. Introduction                  |\n");
        printf("|           2. X_Sudoku Game                 |\n");
        printf("|           3. trans X_Sudoku into CnfFile   |\n");
        printf("|           4. Cnfparser and DPLL solve      |\n");
        printf("|           0. return to Initial Interface   |\n");
        printf("----------------------------------------------\n");
        break;
    }
    case 2:
    {
        printf("-------------------------------------------------\n");
        printf("|               MENU of Percent-Sudoku          |\n");
        printf("|           1. Introduction                     |\n");
        printf("|           2. Percent-Sudoku Game              |\n");
        printf("|           3. trans Percent-Sudoku into CnfFile|\n");
        printf("|           4. Cnfparser and DPLL solve         |\n");
        printf("|           0. return to Initial Interface      |\n");
        printf("-------------------------------------------------\n");
        break;
    }
    case 3:
    {
        printf("----------------------------------------------\n");
        printf("|               MENU of SAT                  |\n");
        printf("|           1. Introduction                  |\n");
        printf("|           2. Cnfparser and DPLL solve      |\n");
        printf("|           0. return to Initial Interface   |\n");
        printf("----------------------------------------------\n");
        break;
    }
    default:
        break;
    }
}

SATStatus display(void)
{
    int choice = 0, branch = 0;
    while (true)
    {
        system("cls");
        menu(0);
        printf("Please input 0 or 1 or 2:");
        scanf("%d", &choice);
        if (choice == 0)
        {
            printf("Exit success!\n");
            return 0;
        }
        else if (choice == 1)
        {
            while (true)
            {
                system("cls");
                menu(2); // 显示百分号数独菜单
                printf("Please input :");
                scanf("%d", &branch);
                if (branch == 0)
                {
                    break;
                }
                else if (branch == 1)
                {
                    IntroductionOfPercent_sudoku();
                    system("pause");
                }
                else if (branch == 2)
                {
                    CreateArrAndAnsPercent(arr, ans);
                    int x, y, value;
                    int errorcount = 0;
                    while (true)
                    {
                        printf("\nNow percent-sudoku board's situation is as follows:");
                        printPercentSudoku(arr);
                        printf("\nAtten: ErrorCount is %d\n", errorcount);
                        printf("input 0 0 0 you will exit this game directly\n");
                        printf("input 0 0 1 you will see the ans of this game\n");
                        printf("please input x y and value:");
                        scanf("%d %d %d", &x, &y, &value);
                        if (x == 0 && y == 0 && value == 0)
                        {
                            break;
                        }
                        else if (x == 0 && y == 0 && value == 1)
                        {
                            printf("\nThe answer of this percent-sudoku game is as follows:");
                            printPercentSudoku(ans);
                            system("pause");
                            system("cls");
                            continue;
                        }
                        else if (x >= 1 && x <= 9 && y >= 1 && y <= 9 && value >= 1 && value <= 9)
                        {
                            if (arr[x][y] != 0)
                            {
                                printf("This position has already been filled!\n");
                                system("pause");
                                system("cls");
                                continue;
                            }
                            
                            if (value == ans[x][y]) 
                            {
                                arr[x][y] = value;
                                holes--;
                                printf("Correct move!\n");
                                system("pause");
                                system("cls");
                            }
                            else
                            {
                                errorcount++;
                                printf("Invalid move! This violates percent-sudoku rules. Try again!\n");
                                system("pause");
                                system("cls");
                            }
                        }
                        else
                        {
                            errorcount++;
                            printf("Unreasonable input.Please input again:\n");
                            system("pause");
                            system("cls");
                        }
                        if (errorcount == 3)
                        {
                            system("cls");
                            printf("Error 3 times.Game failure\n");
                            system("pause");
                            break;
                        }
                        if (holes == 0)
                        {
                            system("cls");
                            printf("WIN!");
                            system("pause");
                            break;
                        }
                    }
                }
                else if (branch == 3)
                {
                    CreateArrAndAnsPercent(arr, ans);
                    char PercentSudokuCnfFileName[30];
                    printf("Please input the percent-sudoku cnfFile name(don't include .cnf): ");
                    scanf("%s", PercentSudokuCnfFileName);
                    strcat(PercentSudokuCnfFileName, ".cnf");
                    printf("generate percent-sudoku_board and its cnfFile now......\n");
                    CreatePercentSudokuCNF(arr, PercentSudokuCnfFileName);
                    system("pause");
                    int Seechoice;
                    while (true)
                    {
                        printf("\nMENU of SeeChoice\n1.See the percent-sudoku_board\n2.See the cnf file\n0.return\n");
                        printf("Please input:");
                        scanf("%d", &Seechoice);
                        if (Seechoice == 1)
                        {
                            printPercentSudoku(arr);
                            system("pause");
                        }
                        else if (Seechoice == 2)
                        {
                            FILE *fp;
                            fp = fopen(PercentSudokuCnfFileName, "r");
                            if (fp == NULL)
                            {
                                printf("Open file error!\n");
                                system("pause");
                                break;
                            }
                            char CnfFlieLineStr[50];
                            while (fgets(CnfFlieLineStr, 51, fp))
                            {
                                printf("%s", CnfFlieLineStr);
                            }
                        }
                        else if (Seechoice == 0)
                        {
                            break;
                        }
                        else
                        {
                            printf("Wrong input!Input again\n");
                        }
                    }
                }
                else if (branch == 4)
                {
                    char filename[30];
                    printf("Please input the cnfFile name(don't include .cnf): ");
                    scanf("%s", filename);
                    strcat(filename, ".cnf");
                    
                    printf("Loading CNF file: %s\n", filename);
                    headpC = InitClauseMap(filename);
                    if (headpC)
                    {
                        printf("Cnfparser module work normally\n");
                        printf("Variables: %d, Clauses: %d\n", n, m);
                    }
                    else
                    {
                        printf("ERROR: InitClauseMap failed!\n");
                        system("pause");
                        continue;
                    }
                    
                    printf("\nDPLL module is working now...\n");
                    printf("This may take some time for large problems.\n");
                    
                    clock_t begin, end;
                    begin = clock();
                    SATStatus res = DPLL(headpC, 2); // 使用策略2
                    end = clock();
                    
                    str_replace(filename, ".cnf", ".res");
                    saveRes(filename, res, end - begin);
                    
                    int cost_time = end - begin;
                    if (res == TRUE) {
                        printf("\nSAT problem solved successfully!\n");
                        printf("Time cost: %d ms\n", cost_time);
                        
                        // 只有当求解成功且是数独问题时才显示数独结果
                        if (n == 729) { // 9x9x9 = 729变量表示数独
                            int DPLLans[10][10];
                            GetShuduValue(DPLLans);
                            printf("\nThe DPLL result is as follows:");
                            printPercentSudoku(DPLLans);
                            printf("\nThe actual result is as follows:");
                            printPercentSudoku(ans);
                        }
                    } else { //数独必定有解
                        printf("\nSAT problem is unsatisfiable.\n");
                        printf("Time cost: %d ms\n", cost_time);
                    }
                    system("pause");
                }
                else
                {
                    printf("INPUT ERROR!Please input again\n");
                    system("pause");
                }
            }
        }
        else if (choice == 2)
        {
            while (true)
            {
                system("cls");
                menu(3);
                printf("Please input :");
                scanf("%d", &branch);
                if (branch == 0)
                {
                    break;
                }
                else if (branch == 1)
                {
                    IntroductionOfX_SAT();
                    system("pause");
                }
                else if (branch == 2)
                {
                    char filename[30];
                    printf("Please input the cnfFile name(don't include .cnf): ");
                    scanf("%s", filename);
                    strcat(filename, ".cnf");
                    headpC = InitClauseMap(filename);
                    if (headpC)
                    {
                        printf("Cnfparser module work normally!\n");
                        printf("Variables: %d, Clauses: %d\n", n, m);
                    }
                    system("pause");
                    int strategy;
                    printf("Strategy 1:choose the first v\n");
                    printf("Strategy 2:choose the most v in shortest clause\n");
                    printf("Strategy 3:choose the most v\n");
                    printf("Strategy 4:choose the first v(in LiteralsInfo)\n");
                    printf("Strategy 5:choose the small sentence v\n");
                    printf("Strategy 6:choose the smart v\n");
                    printf("Plaese input strategy(1 or 2 or 3 or 4 or 5 or 6):");
                    scanf("%d", &strategy);
                    printf("DPLL module is working now\n");
                    clock_t begin, end;
                    begin = clock();
                    SATStatus res = DPLL(headpC, strategy);
                    end = clock();
                    str_replace(filename, ".cnf", ".res");
                    int cost_time = end - begin;
                    saveRes(filename, res, cost_time);
                    // 本地CLOCKS_PER_SEC 是 1000，则 1 个时钟计时单元对应 0.001 秒（即 1 毫秒），即end-begin的单位是毫秒
                    if (res)
                    {
                        printf("This SAT problem is sovleable\n");
                        printf("Time cost is %d ms\n", cost_time);
                    }
                    else
                    {
                        printf("This SAT problem is unsolveable\n");
                        printf("Time cost is %d ms\n", cost_time);
                    }
                    system("pause");
                }
                else
                {
                    printf("INPUT ERROR!Please input again\n");
                    system("pause");
                }
            }
        }
        else
        {
            printf("INPUT ERROR!Please input again\n");
            system("pause");
        }
    }
    return 0;
}

void IntroductionOfX_sudoku()
{
    printf("This interface is about Percent_Sudoku (2 is a separate part, 3 and 4 together are a part)\n");
    printf("Option 2: You can enter Percent_Sudoku game interface, there are three difficulty modes (easy, medium and hard) to play\n");
    printf("          Enter x, y, and value, and you can fill in the value in the (x, y) position until you win the game.\n");
    printf("          I provided two special inputs:0 0 0 to exit the game directly,0 0 1 to check the answer during the game.\n");
    printf("          Enjoy this game!\n");
    printf("Option 3: Similar to 2, you can generate a Percent_Sudoku board in difficulty mode,\n");
    printf("          program will generate the cnf file corresponding to the board.\n");
    printf("          You can choose to view the board (1) or file (2) in the SeeChoice interface.\n");
    printf("Option 4: You can input the cnf file name(generated by Option3), and program will parse the file and solve it using DPLL algorithm.\n");
    printf("          The result will be saved in a file with the same name as the res file.\n");
    printf("          And the Percent_Sudoku answer will be presented in 9*9 board too\n");
    printf("Option 0: You can go back to the Initial Interface\n");
}

void IntroductionOfX_SAT()
{
    printf("This interface is about SAT\n");
    printf("Option 2: You can input the cnf of any type of SAT problem,and program will solve it using DPLL algorithm.\n");
    printf("          The program will tell you whether the SAT problem is sovleable.\n");
    printf("          The result will be saved in a file with the same name as the res file.\n");
    printf("Option 0: You can go back to the Initial Interface\n");
}

void str_replace(char *str1, const char *str2, const char *str3)
{
    char result[300];
    int i = 0, j = 0;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    int len3 = strlen(str3);
    int found = 0;
    while (i < len1)
    {
        //&str1[i]指的是以str[i]位置开头的指针
        if (strncmp(&str1[i], str2, len2) == 0)
        {
            strcpy(&result[j], str3);
            j += len3;
            i += len2;
            found = 1;
        }
        else
        {
            result[j++] = str1[i++];
        }
    }
    result[j] = '\0';
    if (!found)
    {
        printf("Can't find the replaced string!\n");
        return;
    }
    strcpy(str1, result);
}

void IntroductionOfPercent_sudoku()
{
    printf("This interface is about Percent-Sudoku (百分号数独)\n");
    printf("Percent-Sudoku is a variant of Sudoku with additional constraints:\n");
    printf("1. Standard Sudoku rules (rows, columns, 3x3 boxes)\n");
    printf("2. Anti-diagonal constraint (top-right to bottom-left) - 撇对角线\n");
    printf("3. Two window constraints: [2-4,2-4] and [6-8,6-8] regions\n\n");
    printf("Option 2: You can enter Percent-Sudoku game interface with three difficulty modes\n");
    printf("          Enter x, y, and value to fill the position until you win.\n");
    printf("          Special inputs: 0 0 0 to exit, 0 0 1 to see the answer.\n");
    printf("Option 3: Generate a Percent-Sudoku board and convert it to CNF format.\n");
    printf("          You can view the board or CNF file in the SeeChoice interface.\n");
    printf("Option 4: Input a percent-sudoku CNF file name for DPLL algorithm solving.\n");
    printf("          Results will be saved and displayed in board format.\n");
    printf("Option 0: Return to the Initial Interface\n");
}