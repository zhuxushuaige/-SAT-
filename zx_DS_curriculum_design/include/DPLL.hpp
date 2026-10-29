#pragma once
#include "def.hpp"
#include "mystl.hpp"
#include <climits> // 用于INT_MAX
#include <cstring> // 用于memset

SATStatus InitClauseStack(ClauseStack &CS);
SATStatus InitLiteralStack(LiteralStack &LS);
SATStatus CSPush(ClauseStack &CS, LinkClause p);
SATStatus LSPush(LiteralStack &LS, LinkLiteral p);
SATStatus CSPop(ClauseStack &CS, LinkClause &p);
SATStatus LSPop(LiteralStack &LS, LinkLiteral &p);
int LiteralNumofClause(LinkClause cc);
int FastFindSingleClause(LinkClause C);
int JudgeClauseMapSituation(LinkClause C);
void DelClauseAndLiteral(LinkClause headpC, ClauseStack &CS, LiteralStack &LS, int num);
void RecoveryClauseAndLiteral(ClauseStack &CS, LiteralStack &LS, int num);
int Optimized_GetV_MOMs(LinkClause headpC);
int AdvancedUnitPropagation(LinkClause C, ClauseStack &CS, LiteralStack &LS, MyVectorInt &propagatedVars);
int GetV5_OptimizedJW(LinkClause C);
int GetV6_DynamicHeuristic(LinkClause C);
void UpdateConflictStats(int var);
bool ShouldRestart(int conflicts, int restarts);
SATStatus DPLL(LinkClause C, int strategy);
void saveRes(char filename[30], SATStatus res, int time);
LinkLiteral isNumInClause(int num, LinkClause pC);
int GetV1(LinkClause C);
int GetV3(void);
int GetV4(void);
int PureLiteralElimination(LinkClause C, ClauseStack &CS, LiteralStack &LS);

SATStatus InitClauseStack(ClauseStack &CS)
{
    CS.base = (LinkClause *)malloc(STACK_INIT_SIZE * sizeof(LinkClause));
    if (!CS.base)
        return ERROR;
    CS.top = CS.base;
    CS.stacksize = STACK_INIT_SIZE;
    return OK;
}

SATStatus InitLiteralStack(LiteralStack &LS)
{
    LS.base = (LinkLiteral *)malloc(STACK_INIT_SIZE * sizeof(LinkLiteral));
    if (!LS.base)
        return ERROR;
    LS.top = LS.base;
    LS.stacksize = STACK_INIT_SIZE;
    return OK;
}

SATStatus CSPush(ClauseStack &CS, LinkClause p)
{
    if (CS.top - CS.base >= CS.stacksize)
    {
        CS.base = (LinkClause *)realloc(CS.base, (CS.stacksize + STACKINCREMENT) * sizeof(LinkClause));
        if (!CS.base)
            return ERROR;
        CS.top = CS.base + CS.stacksize;
        CS.stacksize += STACKINCREMENT;
    }
    *CS.top++ = p;
    return OK;
}

SATStatus LSPush(LiteralStack &LS, LinkLiteral p)
{
    if (LS.top - LS.base >= LS.stacksize)
    {
        LS.base = (LinkLiteral *)realloc(LS.base, (LS.stacksize + STACKINCREMENT) * sizeof(LinkLiteral));
        if (!LS.base)
            return ERROR;
        LS.top = LS.base + LS.stacksize;
        LS.stacksize += STACKINCREMENT;
    }
    *LS.top++ = p;
    return OK;
}

SATStatus CSPop(ClauseStack &CS, LinkClause &p)
{
    if (CS.top == CS.base)
        return ERROR;
    p = *--CS.top;
    return OK;
}

SATStatus LSPop(LiteralStack &LS, LinkLiteral &p)
{
    if (LS.top == LS.base)
        return ERROR;
    p = *--LS.top;
    return OK;
}

int LiteralNumofClause(LinkClause cc)
{
    if (cc->state)
    {
        int n = 0;
        LinkLiteral p = cc->headpL->next;
        while (p)
        {
            if (p->state)
            {
                n++;
            }
            p = p->next;
        }
        return n;
    }
    return -1;
}

int FastFindSingleClause(LinkClause C)
{
    if (!C) {
        return 0;
    }
    
    LinkClause p = C->next;
    while (p)
    {
        if (!p) {
            break;
        }
        
        int literalCount = LiteralNumofClause(p);
        if (literalCount == 1)
        {
            if (!p->headpL) {
                p = p->next;
                continue;
            }
            
            LinkLiteral q = p->headpL->next;
            while (q)
            {
                if (q->state == 1)
                {
                    return q->num;
                }
                q = q->next;
            }
        }
        p = p->next;
    }
    return 0;
}

int JudgeClauseMapSituation(LinkClause C)
{
    int x = 0;
    LinkClause p = C->next;
    while (p)
    {
        if (LiteralNumofClause(p) == 0)//存在空子句
        {
            return 1;
        }
        if (LiteralNumofClause(p) >= 1)//说明还要分支
        {
            x = 1;
        }
        p = p->next;
    }
    if (x)
    {
        return 2;
    }
    return 0;
}

// 计算所有最小子句中的最大出现次数的文字
int Optimized_GetV_MOMs(LinkClause headpC)
{
    int minSize = INT_MAX;               // 初始化为最大值
    MyMapIntInt literalFrequency; // 存储每个文字在最小子句中的出现次数

    // 遍历所有子句，寻找最小子句并统计出现频率
    LinkClause pC = headpC->next;
    while (pC)
    {
        if (pC->state == 1) // 子句有效
        {
            int clauseSize = LiteralNumofClause(pC);
            if (clauseSize > 0)
            {
                if (clauseSize < minSize)
                {
                    minSize = clauseSize;     // 找到更小的子句，更新最小子句大小
                    literalFrequency.clear(); // 清空频率统计
                }
                if (clauseSize == minSize)
                {
                    // 在当前最小子句中统计文字出现频率
                    LinkLiteral pL = pC->headpL->next;
                    while (pL)
                    {
                        if (pL->state == 1)
                        {
                            literalFrequency[pL->num]++;
                        }
                        pL = pL->next;
                    }
                }
            }
        }
        pC = pC->next;
    }

    // 寻找在最小子句中出现次数最多的文字
    int selectedLiteral = 0;
    int maxFrequency = -1;
    const int* keys = literalFrequency.keys();
    const int* values = literalFrequency.values();
    for (size_t i = 0; i < literalFrequency.size(); i++)
    {
        if (values[i] > maxFrequency)
        {
            maxFrequency = values[i];
            selectedLiteral = keys[i];
        }
    }

    // 返回变量编号的绝对值，让DPLL自己决定赋值方向
    return abs(selectedLiteral);
}

int GetV1(LinkClause C)
{
    LinkClause p = C->next;
    while (p)
    {
        if (p->state == 1)
        {
            LinkLiteral q = p->headpL->next;
            while (q)
            {
                if (q->state == 1)
                {
                    return abs(q->num);  // 返回变量编号的绝对值
                }
                q = q->next;
            }
        }
        p = p->next;
    }
    return 0;
}

int GetV3(void)  // 选择出现次数最多的变量
{
    int maxNum = 1; // 从1开始，避免访问LiteralsInfo[0]
    int maxCount = -1;

    // 获取当前CNF的实际变量数目外部变量n
    extern int n;

    for (int i = 1; i <= n; i++)
    {
        if (LiteralsInfo[i].state == 1)
        {
            if (LiteralsInfo[i].count > maxCount)
            {
                maxCount = LiteralsInfo[i].count;
                maxNum = i;
            }
        }
    }

    // 确保返回的是有效的变量
    if (maxCount == -1) {
        return 0; // 没有找到有效变量
    }
    
    return maxNum;
}

int GetV4(void) // 选择第一个有效变量
{
    extern int n;  // 确保使用全局n
    for (int i = 1; i <= n; i++) // 检查所有变量
    {
        if (LiteralsInfo[i].state == 1)
        {
            return i;
        }
    }
    return 0; // 没有找到有效变量
}

// 策略5：优化的Jeroslow-Wang启发式 - 简化实现，专注小子句
int GetV5_OptimizedJW(LinkClause C)
{
    extern int n;
    static double jwScore[MAX_VAR_NUM + 1];
    memset(jwScore, 0, sizeof(jwScore));
    
    LinkClause pC = C->next;
    while (pC) {
        if (pC->state == 1) {
            int clauseSize = LiteralNumofClause(pC);
            if (clauseSize > 0 && clauseSize <= 6) { // 只考虑小子句，提高效率
                double weight = 1.0 / (1 << clauseSize);
                
                 // 统计子句中每个文字的得分
                LinkLiteral pL = pC->headpL->next;
                while (pL) {
                    if (pL->state == 1) {
                        int var = abs(pL->num);
                        if (var <= n) {
                            // 正负文字不同权重，正文字权重稍高
                            double bias = (pL->num > 0) ? 1.1 : 1.0;
                            jwScore[var] += weight * bias;
                        }
                    }
                    pL = pL->next;
                }
            }
        }
        pC = pC->next;
    }
    
    // 选择得分最高的变量
    int bestVar = 1;
    double maxScore = -1;
    for (int i = 1; i <= n; i++) {
        if (LiteralsInfo[i].state == 1 && jwScore[i] > maxScore) {
            maxScore = jwScore[i];
            bestVar = i;
        }
    }
    
    return bestVar;
}

    // 策略6：动态组合启发式 - 结合多种因素的智能选择
    static int conflictHistory[MAX_VAR_NUM + 1] = {0};
    static int totalConflicts = 0;

    int GetV6_DynamicHeuristic(LinkClause C)
    {
        extern int n;
        static double combinedScore[MAX_VAR_NUM + 1];
        memset(combinedScore, 0, sizeof(combinedScore));
        
        // 因子1：基于出现频率(权重15%)
        for (int i = 1; i <= n; i++) {
            if (LiteralsInfo[i].state == 1) {
                combinedScore[i] += 0.15 * LiteralsInfo[i].count;
            }
        }
        
        // 因子2：基于小子句偏好 (权重70%)
        LinkClause pC = C->next;
        while (pC) {
            if (pC->state == 1) {
                int clauseSize = LiteralNumofClause(pC);
                if (clauseSize > 0 && clauseSize <= 4) { // 专注于小子句
                    double bonus = 6.0 - clauseSize; // 子句越小奖励越高
                    
                    LinkLiteral pL = pC->headpL->next;
                    while (pL) {
                        if (pL->state == 1) {
                            int var = abs(pL->num);
                            if (var <= n) {
                                combinedScore[var] += 0.7 * bonus;
                            }
                        }
                        pL = pL->next;
                    }
                }
            }
            pC = pC->next;
        }

        // 因子3：基于冲突历史 (权重15%)
        if (totalConflicts > 0) {
            for (int i = 1; i <= n; i++) {
                if (LiteralsInfo[i].state == 1) {
                    double conflictRatio = (double)conflictHistory[i] / totalConflicts;
                    combinedScore[i] += 0.15 * conflictRatio * 30; // 放大系数
                }
            }
        }
        
        // 选择得分最高的变量
        int bestVar = 1;
        double maxScore = -1;
        for (int i = 1; i <= n; i++) {
            if (LiteralsInfo[i].state == 1 && combinedScore[i] > maxScore) {
                maxScore = combinedScore[i];
                bestVar = i;
            }
        }
        
        return bestVar;
    }

// 更新冲突统计
void UpdateConflictStats(int var) {
    if (var > 0 && var <= MAX_VAR_NUM) {
        conflictHistory[var]++;
        totalConflicts++;
        
        // 500次冲突后适度衰减历史数据
        if (totalConflicts % 500 == 0) {
            for (int i = 1; i <= MAX_VAR_NUM; i++) {
                conflictHistory[i] = (conflictHistory[i] * 4) / 5; // 衰减20%
            }
            totalConflicts = (totalConflicts * 4) / 5;
        }
    }
}

// 高级单元传播：使用队列避免重复扫描
int AdvancedUnitPropagation(LinkClause C, ClauseStack &CS, LiteralStack &LS, MyVectorInt &propagatedVars)
{
    MyQueueInt unitQueue;
    MyUnorderedSetInt processed;

    // 初始扫描找出所有单元子句
    LinkClause pC = C->next;
    while (pC) {
        if (pC->state == 1 && LiteralNumofClause(pC) == 1) {
            LinkLiteral pL = pC->headpL->next;
            while (pL) {
                if (pL->state == 1) {
                    unitQueue.push(pL->num);
                    break;
                }
                pL = pL->next;
            }
        }
        pC = pC->next;
    }
    
    // 处理单元传播队列
    while (!unitQueue.empty()) {
        int literal = unitQueue.front();
        unitQueue.pop();
        
        if (processed.count(literal) || LiteralsInfo[abs(literal)].state == 0) {
            continue;
        }
        
        processed.insert(literal);
        propagatedVars.push_back(abs(literal));
        
        // 执行传播，把相关字句删去
        DelClauseAndLiteral(C, CS, LS, literal);

        // 检查新产生的单元子句
        pC = C->next;
        while (pC) {
            if (pC->state == 1 && LiteralNumofClause(pC) == 1) {
                LinkLiteral pL = pC->headpL->next;
                while (pL) {
                    if (pL->state == 1 && !processed.count(pL->num)) {
                        unitQueue.push(pL->num);
                        break;
                    }
                    pL = pL->next;
                }
            } else if (pC->state == 1 && LiteralNumofClause(pC) == 0) {
                return 1; // 发现空子句，冲突
            }
            pC = pC->next;
        }
    }
    
    return 0; // 成功完成单元传播
}

// 重启条件判断
bool ShouldRestart(int conflicts, int restarts) {
    // 简单的重启策略：每256次冲突后重启
    // 随着重启次数增加，重启阈值递增
    int threshold = 256 * (1 << restarts);  // 256, 512, 1024, 2048, ...
    return conflicts >= threshold;
}

// 纯文字消除优化
int PureLiteralElimination(LinkClause C, ClauseStack &CS, LiteralStack &LS) {
    extern int n;
    MyUnorderedSetInt positiveLiterals, negativeLiterals;
    
    // 收集所有出现的文字
    LinkClause pC = C->next;
    while (pC) {
        if (pC->state == 1) {
            LinkLiteral pL = pC->headpL->next;
            while (pL) {
                if (pL->state == 1) {
                    if (pL->num > 0) {
                        positiveLiterals.insert(pL->num);
                    } else {
                        negativeLiterals.insert(-pL->num);
                    }
                }
                pL = pL->next;
            }
        }
        pC = pC->next;
    }
    
    // 找到纯文字并消除
    int eliminated = 0;
    for (int i = 1; i <= n; i++) {
        if (LiteralsInfo[i].state == 1) {
            bool hasPositive = positiveLiterals.count(i);
            bool hasNegative = negativeLiterals.count(i);
            
            if (hasPositive && !hasNegative) {
                // 纯正文字
                DelClauseAndLiteral(C, CS, LS, i);
                eliminated++;
            } else if (hasNegative && !hasPositive) {
                // 纯负文字
                DelClauseAndLiteral(C, CS, LS, -i);
                eliminated++;
            }
        }
    }
    
    return eliminated;
}

SATStatus DPLL(LinkClause C, int strategy)
{
    if (!C) {
        return FALSE;
    }
    
    // 静态变量用于跟踪冲突和重启
    static int conflictCount = 0;
    static int restartCount = 0;

    // 预处理：移除永真子句
    static bool preprocessed = false;
    if (!preprocessed) {
        // 移除包含互补文字的子句
        LinkClause pC = C->next;
        while (pC) {
            if (pC->state == 1) {
                MyUnorderedSetInt literals;
                bool isTautology = false;
                LinkLiteral pL = pC->headpL->next;
                while (pL) {
                    if (pL->state == 1) {
                        int lit = pL->num;
                        if (literals.count(-lit)) {
                            isTautology = true;
                            break;
                        }
                        literals.insert(lit);
                    }
                    pL = pL->next;
                }
                if (isTautology) {
                    pC->state = 0; // 移除永真子句
                }
            }
            pC = pC->next;
        }
        preprocessed = true;
    }

    // 检查重启条件
    if (ShouldRestart(conflictCount, restartCount)) {
        restartCount++;
        conflictCount = 0;
    }
    
    ClauseStack Cs1, Cs2;
    LiteralStack Ls1, Ls2;
    if (InitClauseStack(Cs1) != OK || InitClauseStack(Cs2) != OK ||
        InitLiteralStack(Ls1) != OK || InitLiteralStack(Ls2) != OK) {
        return FALSE;
    }
    
    int num, res;
    
    // 使用高级单元传播
    MyVectorInt propagatedVars;
    int propagationResult = AdvancedUnitPropagation(C, Cs1, Ls1, propagatedVars);
    
    if (propagationResult == 1) {
        // 单元传播发现空子句，回溯，并赋值为false
        for (size_t i = 0; i < propagatedVars.size(); i++) {
            int var = propagatedVars[i];
            LiteralsInfo[var].state = 1;
            LiteralsInfo[var].value = 2;
        }
        LinkClause temp_clause;
        LinkLiteral temp_literal;
        while (CSPop(Cs1, temp_clause)) {
            temp_clause->state = 1;
            LinkLiteral pL1 = temp_clause->headpL->next;
            while (pL1) {
                if (pL1->state == 1) {
                    LiteralsInfo[abs(pL1->num)].count++;
                }
                pL1 = pL1->next;
            }
        }
        while (LSPop(Ls1, temp_literal)) {
            temp_literal->state = 1;
        }
        return FALSE;
    }

    // 应用纯文字消除（删除符号单一的文字）
    if (restartCount < 2) {
        PureLiteralElimination(C, Cs1, Ls1);
    }

    // 检查子句状态
    res = JudgeClauseMapSituation(C);
    if (res == 0) //子句为空
    {
        return TRUE;
    }
    else if (res == 1)// 存在空子句
    {
        // 回溯所有单元传播的变化并赋值为false
        LinkClause temp_clause;
        LinkLiteral temp_literal;

        // 恢复所有被单元传播赋值的变量状态
        for (size_t i = 0; i < propagatedVars.size(); i++) {
            int var = propagatedVars[i];
            LiteralsInfo[var].state = 1;
            LiteralsInfo[var].value = 2;
        }
        
        while (CSPop(Cs1, temp_clause)) {
            temp_clause->state = 1;
            LinkLiteral pL1 = temp_clause->headpL->next;
            while (pL1) {
                if (pL1->state == 1) {
                    LiteralsInfo[abs(pL1->num)].count++;
                }
                pL1 = pL1->next;
            }
        }
        while (LSPop(Ls1, temp_literal)) {
            temp_literal->state = 1;
        }
        return FALSE;
    }
    
    // 选择变量
    int v;
    if (strategy == 1) //选用第一个有效变量（按字句顺序）
    {
        v = GetV1(C);
    }
    else if (strategy == 2)
    {
        v = Optimized_GetV_MOMs(C);  // 使用优化的MOMS启发式（选择最小子句中出现次数最多的变量）
    }
    else if (strategy == 3)
    {
        v = GetV3(); //直接选择出现次数最多的变量
    }
    else if (strategy == 4)
    {
        v = GetV4(); //直接选择第一个有效变量（按变量编号顺序）
    }
    else if (strategy == 5)
    {
        v = GetV5_OptimizedJW(C);  // 使用优化的Jeroslow-Wang启发式（小子句，正负文字区分,用处不大）
    }
    else if (strategy == 6)
    {
        v = GetV6_DynamicHeuristic(C);  // 使用动态组合启发式（基于出现频率、小子句偏好和冲突历史的综合甲醛）
    }
    else
    {
        v = GetV1(C);  // 默认使用组合策略
    }
    
    if (v == 0) {
        // 没有找到可分支的变量，检查是否所有子句都被满足
        res = JudgeClauseMapSituation(C);
        if (res == 0) {
            return TRUE;  // 所有子句都满足
        } else {
            return FALSE; // 还有未满足的子句，但没有变量可选择
        }
    }

    // 尝试正赋值
    int ans;
    DelClauseAndLiteral(C, Cs2, Ls2, v); //分支试探性删除时用的是CS2和LS2
    ans = DPLL(C, strategy);
    if (ans == TRUE) {
        return TRUE;
    }
    
    // 回溯并尝试负赋值
    RecoveryClauseAndLiteral(Cs2, Ls2, v);
    DelClauseAndLiteral(C, Cs2, Ls2, -v);
    ans = DPLL(C, strategy);
    if (ans == FALSE)
    {
        RecoveryClauseAndLiteral(Cs2, Ls2, -v);

        // 冲突发生，更新统计信息
        UpdateConflictStats(v);
        
        // 回溯所有单元传播的变化
        LinkClause temp_clause;
        LinkLiteral temp_literal;

        // 恢复所有被单元传播赋值的变量状态
        for (size_t i = 0; i < propagatedVars.size(); i++) {
            int var = propagatedVars[i];
            LiteralsInfo[var].state = 1;
            LiteralsInfo[var].value = 2;
        }
        
        while (CSPop(Cs1, temp_clause)) {
            temp_clause->state = 1;
            LinkLiteral pL1 = temp_clause->headpL->next;
            while (pL1) {
                if (pL1->state == 1) {
                    LiteralsInfo[abs(pL1->num)].count++;
                }
                pL1 = pL1->next;
            }
        }
        while (LSPop(Ls1, temp_literal)) {
            temp_literal->state = 1;
        }
    }
    return ans;
}

void saveRes(char filename[30], SATStatus res, int time)
{
    char str[10];
    if (res == TRUE)
    {
        strcpy(str, "s 1\nv");
    }
    else
    {
        strcpy(str, "s 0\nv");
    }
    int i;
    FILE *fp = fopen(filename, "w");
    fputs(str, fp);
    for (i = 1; i <= n; i++)
    {
        if (LiteralsInfo[i].value == 1)
        {
            fprintf(fp, " %d", i);
        }
        else if (LiteralsInfo[i].value == 0)
        {
            fprintf(fp, " %d", -i);
        }
        else // value == 2 (未赋值，随意赋值)
        {
            fprintf(fp, " %d", -i);  // 默认赋假值
        }
    }
    fprintf(fp, "\nt %d", time);
    fclose(fp);
}

LinkLiteral isNumInClause(int num, LinkClause pC)
{
    LinkLiteral pL = pC->headpL->next;
    while (pL)
    {
        if (pL->state)
        {
            if (pL->num == num)
            {
                return pL;
            }
        }
        pL = pL->next;
    }
    return nullptr;
}

// 利用abs(num)号变元化简ClauseMap
// 将删去的子句、文字保存并维护LiteralsInfo
void DelClauseAndLiteral(LinkClause headpC, ClauseStack &CS, LiteralStack &LS, int num)
{
    extern int n;  // 使用全局变量n
    if (abs(num) > n || abs(num) <= 0) {
        return;  // 超出范围的变量直接返回
    }
    
    LiteralsInfo[abs(num)].state = 0;
    LiteralsInfo[abs(num)].value = (num > 0 ? 1 : 0);
    LinkClause pC = headpC->next;
    while (pC)
    {
        if (pC->state)
        {
            LinkLiteral flag1 = isNumInClause(num, pC);
            LinkLiteral flag2 = isNumInClause(-num, pC);
            if (flag1)
            {
                pC->state = 0;
                CSPush(CS, pC);
                LinkLiteral pL1 = pC->headpL->next;
                while (pL1)
                {
                    if (pL1->state == 1)
                    {
                        LiteralsInfo[abs(pL1->num)].count--;
                    }
                    pL1 = pL1->next;
                }
            }
            else if (flag2)
            {
                LSPush(LS, flag2);
                flag2->state = 0;
                // 不在这里减count，因为子句仍然活跃
            }
        }
        pC = pC->next;
    }
}

// 回溯
void RecoveryClauseAndLiteral(ClauseStack &CS, LiteralStack &LS, int num)
{
    extern int n;  // 使用全局变量n
    if (abs(num) > n || abs(num) <= 0) {
        return;  // 超出范围的变量直接返回
    }
    
    LiteralsInfo[abs(num)].state = 1;
    LiteralsInfo[abs(num)].value = 2;
    LinkClause pC;
    LinkLiteral pL;
    while (CSPop(CS, pC))
    {
        if (pC->state == 0)
        {
            LinkLiteral pL1 = pC->headpL->next;
            while (pL1)
            {
                if (pL1->state == 1)
                {
                    LiteralsInfo[abs(pL1->num)].count++;
                }
                pL1 = pL1->next;
            }
            pC->state = 1;
        }
    }
    while (LSPop(LS, pL))
    {
        pL->state = 1;
    }
}
