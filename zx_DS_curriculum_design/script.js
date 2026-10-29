let sudokuBoard = [];
let solution = [];
let selectedCell = null;
let timer = null;
let seconds = 0;
let errors = 0;
let wasmModule = null;
let isAnswerShown = false;

// 初始化WebAssembly模块（如果可用）
async function initializeWASM() {
    try {
        // 这里假设WebAssembly模块已编译并可用
        // wasmModule = await loadWASMModule();
        console.log('WebAssembly模块加载成功');
    } catch (error) {
        console.log('WebAssembly不可用，使用JavaScript实现');
    }
}

function startGame(difficulty) {
    console.log('开始游戏，难度：', difficulty);
    document.getElementById('difficulty-selection').style.display = 'none';
    document.getElementById('game-board').style.display = 'block';
    document.getElementById('game-result').style.display = 'none';

    // 移除所有已存在的返回菜单按钮
    removeAllReturnButtons();
    
    // 重置答案显示状态
    resetAnswerState();

    generateSudoku(difficulty).then(() => {
        console.log('数独生成成功');
        createGrid();
        createNumberPad();
        startTimer();
        errors = 0;
        updateErrorDisplay();
    }).catch(error => {
        console.error('Error generating Sudoku:', error);
        alert('生成数独时出错，请重试。');
        returnToMenu();
    });
}

function generateSudoku(difficulty) {
    return new Promise((resolve, reject) => {
        console.log('开始生成数独，难度：', difficulty);
        
        // 尝试从后端API获取数独
        fetch('/api/generate-sudoku', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ difficulty })
        })
        .then(response => response.json())
        .then(data => {
            console.log('API响应：', data);
            if (data.success) {
                sudokuBoard = data.puzzle;
                solution = data.solution;
                console.log('从API获取数独成功');
                console.log('题目：', sudokuBoard);
                console.log('答案：', solution);
                resolve();
            } else {
                throw new Error(data.error);
            }
        })
        .catch(error => {
            console.error('API调用失败，使用JavaScript生成:', error);
            // 如果API调用失败，使用JavaScript备用方案
            generateSudokuJS(difficulty).then(() => {
                console.log('JavaScript生成数独成功');
                console.log('题目：', sudokuBoard);
                console.log('答案：', solution);
                resolve();
            }).catch(reject);
        });
    });
}

function generateSudokuJS(difficulty) {
    // JavaScript实现的数独生成
    return new Promise((resolve) => {
        setTimeout(() => {
            console.log('使用JavaScript生成随机百分号数独...');
            const difficultyLevel = difficulty === 'easy' ? 1 : (difficulty === 'medium' ? 2 : 3);
            
            // 生成完整的数独解决方案
            console.log('生成完整解决方案...');
            solution = generateFullSudoku();
            console.log('完整解决方案生成完成');
            
            // 根据难度创建题目
            sudokuBoard = JSON.parse(JSON.stringify(solution)); // 深拷贝
            
            // 根据难度移除数字
            let cellsToRemove;
            if (difficulty === 'easy') cellsToRemove = 35;
            else if (difficulty === 'medium') cellsToRemove = 45;
            else cellsToRemove = 55;
            
            console.log(`开始移除${cellsToRemove}个格子...`);
            removeCells(sudokuBoard, cellsToRemove);
            
            // 统计实际移除的格子数
            let actualRemoved = 0;
            for (let i = 0; i < 9; i++) {
                for (let j = 0; j < 9; j++) {
                    if (sudokuBoard[i][j] === 0) actualRemoved++;
                }
            }
            console.log(`实际移除了${actualRemoved}个格子`);
            
            resolve();
        }, 100);
    });
}

// 从完整数独中移除指定数量的数字，确保唯一解
function removeCells(board, count) {
    const cells = [];
    for (let i = 0; i < 9; i++) {
        for (let j = 0; j < 9; j++) {
            cells.push([i, j]);
        }
    }
    
    // 随机打乱
    shuffleArray(cells);
    
    let removedCount = 0;
    let attempts = 0;
    const maxAttempts = count * 3; // 避免无限循环
    
    // 逐个尝试移除格子，确保仍有唯一解
    for (let i = 0; i < cells.length && removedCount < count && attempts < maxAttempts; i++) {
        attempts++;
        const [row, col] = cells[i % cells.length];
        
        if (board[row][col] !== 0) {
            const temp = board[row][col];
            board[row][col] = 0;
            
            // 检查是否仍有唯一解（简化版检查）
            if (hasUniqueSolutionSimple(board)) {
                removedCount++;
            } else {
                board[row][col] = temp; // 还原
            }
        }
    }
    
    // 如果未能移除足够的格子，使用简单移除策略
    if (removedCount < count * 0.8) {
        console.log('唯一解验证失败，使用简单移除策略');
        const remainingCells = [];
        for (let i = 0; i < 9; i++) {
            for (let j = 0; j < 9; j++) {
                if (board[i][j] !== 0) {
                    remainingCells.push([i, j]);
                }
            }
        }
        shuffleArray(remainingCells);
        
        const simpleRemoveCount = Math.min(count - removedCount, remainingCells.length);
        for (let i = 0; i < simpleRemoveCount; i++) {
            const [row, col] = remainingCells[i];
            board[row][col] = 0;
        }
    }
}

// 简化的唯一解检查（性能考虑）
function hasUniqueSolutionSimple(board) {
    const testBoard = JSON.parse(JSON.stringify(board));
    let solutionCount = 0;
    
    function countSolutions(row, col) {
        if (solutionCount > 1) return false; // 早期退出
        
        if (row === 9) {
            solutionCount++;
            return solutionCount <= 1;
        }
        if (col === 9) {
            return countSolutions(row + 1, 0);
        }
        if (testBoard[row][col] !== 0) {
            return countSolutions(row, col + 1);
        }
        
        for (let num = 1; num <= 9; num++) {
            if (isValidPlacement(testBoard, row, col, num)) {
                testBoard[row][col] = num;
                if (!countSolutions(row, col + 1)) {
                    testBoard[row][col] = 0;
                    return false;
                }
                testBoard[row][col] = 0;
            }
        }
        return true;
    }
    
    countSolutions(0, 0);
    return solutionCount === 1;
}

function generateFullSudoku() {
    // 生成一个符合X数独约束的完整解决方案
    let board = [
        [0,0,0,0,0,0,0,0,0],
        [0,0,0,0,0,0,0,0,0],
        [0,0,0,0,0,0,0,0,0],
        [0,0,0,0,0,0,0,0,0],
        [0,0,0,0,0,0,0,0,0],
        [0,0,0,0,0,0,0,0,0],
        [0,0,0,0,0,0,0,0,0],
        [0,0,0,0,0,0,0,0,0],
        [0,0,0,0,0,0,0,0,0]
    ];
    
    // 使用随机回溯算法生成有效的X数独
    if (solveXSudokuRandom(board)) {
        return board;
    }
    
    // 如果生成失败，返回一个预设的有效X数独（但先随机变换）
    let fallbackBoard = [
        [5, 3, 4, 6, 7, 8, 9, 1, 2],
        [6, 7, 2, 1, 9, 5, 3, 4, 8],
        [1, 9, 8, 3, 4, 2, 5, 6, 7],
        [8, 5, 9, 7, 6, 1, 4, 2, 3],
        [4, 2, 6, 8, 5, 3, 7, 9, 1],
        [7, 1, 3, 9, 2, 4, 8, 5, 6],
        [9, 6, 1, 5, 3, 7, 2, 8, 4],
        [2, 8, 7, 4, 1, 9, 6, 3, 5],
        [3, 4, 5, 2, 8, 6, 1, 7, 9]
    ];
    
    // 随机变换以增加多样性
    return randomizeSudoku(fallbackBoard);
}

// 随机回溯算法
function solveXSudokuRandom(board) {
    for (let row = 0; row < 9; row++) {
        for (let col = 0; col < 9; col++) {
            if (board[row][col] === 0) {
                // 创建随机数字序列
                const numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9];
                shuffleArray(numbers);
                
                for (let num of numbers) {
                    if (isValidPlacement(board, row, col, num)) {
                        board[row][col] = num;
                        if (solveXSudokuRandom(board)) {
                            return true;
                        }
                        board[row][col] = 0;
                    }
                }
                return false;
            }
        }
    }
    return true;
}

// Fisher-Yates 洗牌算法
function shuffleArray(array) {
    for (let i = array.length - 1; i > 0; i--) {
        const j = Math.floor(Math.random() * (i + 1));
        [array[i], array[j]] = [array[j], array[i]];
    }
}

// 随机变换数独（保持有效性）
function randomizeSudoku(board) {
    let newBoard = JSON.parse(JSON.stringify(board));
    
    // 随机交换相同区块内的行
    for (let block = 0; block < 3; block++) {
        let rows = [block * 3, block * 3 + 1, block * 3 + 2];
        shuffleArray(rows);
        let tempBoard = JSON.parse(JSON.stringify(newBoard));
        for (let i = 0; i < 3; i++) {
            newBoard[block * 3 + i] = tempBoard[rows[i]];
        }
    }
    
    // 随机交换相同区块内的列
    for (let block = 0; block < 3; block++) {
        let cols = [block * 3, block * 3 + 1, block * 3 + 2];
        shuffleArray(cols);
        let tempBoard = JSON.parse(JSON.stringify(newBoard));
        for (let i = 0; i < 9; i++) {
            for (let j = 0; j < 3; j++) {
                newBoard[i][block * 3 + j] = tempBoard[i][cols[j]];
            }
        }
    }
    
    return newBoard;
}

function solveXSudoku(board) {
    for (let row = 0; row < 9; row++) {
        for (let col = 0; col < 9; col++) {
            if (board[row][col] === 0) {
                for (let num = 1; num <= 9; num++) {
                    if (isValidPlacement(board, row, col, num)) {
                        board[row][col] = num;
                        if (solveXSudoku(board)) {
                            return true;
                        }
                        board[row][col] = 0;
                    }
                }
                return false;
            }
        }
    }
    return true;
}

function digHoles(fullSudoku, holes) {
    let board = JSON.parse(JSON.stringify(fullSudoku)); // 深拷贝
    let positions = [];
    for (let i = 0; i < 9; i++) {
        for (let j = 0; j < 9; j++) {
            positions.push([i, j]);
        }
    }

    // 随机挖洞
    for (let i = 0; i < holes; i++) {
        if (positions.length === 0) break;
        let index = Math.floor(Math.random() * positions.length);
        let [row, col] = positions.splice(index, 1)[0];
        board[row][col] = 0;
    }

    return board;
}

function createGrid() {
    console.log('开始创建网格');
    const grid = document.getElementById('sudoku-grid');
    grid.innerHTML = '';
    for (let i = 0; i < 9; i++) {
        for (let j = 0; j < 9; j++) {
            const cell = document.createElement('div');
            cell.className = 'cell';
            
            // 只添加撇对角线样式 - 百分号数独特有（移除主对角线）
            if (i + j === 8) {
                cell.classList.add('anti-diagonal');
            }
            
            // 添加窗口区域样式 - 百分号数独特有
            // 上方窗口：位置[1-3][1-3]
            if ((i >= 1 && i <= 3) && (j >= 1 && j <= 3)) {
                cell.classList.add('upper-window');
            }
            // 下方窗口：位置[5-7][5-7]
            if ((i >= 5 && i <= 7) && (j >= 5 && j <= 7)) {
                cell.classList.add('lower-window');
            }
            
            // 设置格子内容
            if (sudokuBoard[i][j] !== 0) {
                cell.textContent = sudokuBoard[i][j];
                cell.classList.add('fixed');
            } else {
                cell.addEventListener('click', () => selectCell(cell, i, j));
            }
            
            grid.appendChild(cell);
        }
    }
    console.log('网格创建完成，撇对角线和窗口区域使用背景高亮显示');

    // 添加返回菜单按钮
    addReturnButton();
}

function addReturnButton() {
    // 检查是否已存在返回按钮，如果存在则不添加新的
    if (!document.getElementById('return-button')) {
        const returnButton = document.createElement('button');
        returnButton.id = 'return-button';
        returnButton.textContent = '返回菜单';
        returnButton.onclick = returnToMenu;
        document.querySelector('.container').appendChild(returnButton);
    }
}

function removeAllReturnButtons() {
    const returnButtons = document.querySelectorAll('#return-button');
    returnButtons.forEach(button => button.remove());

    // 也移除可能存在于 .container 中的按钮
    const containerButtons = document.querySelectorAll('.container > #return-button');
    containerButtons.forEach(button => button.remove());
}

function createNumberPad() {
    const numberPad = document.getElementById('number-pad');
    numberPad.innerHTML = '';
    for (let i = 1; i <= 9; i++) {
        const button = document.createElement('button');
        button.textContent = i;
        button.addEventListener('click', () => {
            console.log('数字按钮被点击:', i);
            fillNumber(i);
        });
        numberPad.appendChild(button);
    }
    console.log('数字面板创建完成');
}

function selectCell(cell, row, col) {
    console.log('选中单元格:', row, col);
    // 移除所有单元格的 'selected' 类
    document.querySelectorAll('.cell').forEach(c => c.classList.remove('selected'));

    // 为选中的单元格添加 'selected' 类
    cell.classList.add('selected');

    // 更新 selectedCell 变量
    selectedCell = cell;
    console.log('selectedCell 已更新:', selectedCell);

    highlightRelatedCells(row, col);
}

function highlightRelatedCells(row, col) {
    const cells = document.querySelectorAll('.cell');
    cells.forEach((cell, index) => {
        const cellRow = Math.floor(index / 9);
        const cellCol = index % 9;
        
        // 清除之前的高亮，但保持对角线样式
        cell.classList.remove('highlighted');
        
        // 高亮同行、同列、同宫格
        let shouldHighlight = (cellRow === row || cellCol === col ||
            (Math.floor(cellRow / 3) === Math.floor(row / 3) &&
                Math.floor(cellCol / 3) === Math.floor(col / 3)));
        
        // 对于X数独，还要高亮对角线上的格子
        if (row === col && cellRow === cellCol) shouldHighlight = true; // 主对角线
        if (row + col === 8 && cellRow + cellCol === 8) shouldHighlight = true; // 副对角线
        
        if (shouldHighlight && !(cellRow === row && cellCol === col)) {
            // 不高亮选中的格子本身，只高亮相关格子
            cell.classList.add('highlighted');
        }
    });
}

function fillNumber(number) {
    console.log('尝试填入数字:', number);
    console.log('selectedCell:', selectedCell);
    if (selectedCell && !selectedCell.classList.contains('fixed')) {
        const row = Math.floor([...selectedCell.parentNode.children].indexOf(selectedCell) / 9);
        const col = [...selectedCell.parentNode.children].indexOf(selectedCell) % 9;
        
        if (isValidPlacement(sudokuBoard, row, col, number) && solution[row][col] === number) {
            selectedCell.textContent = number;
            selectedCell.classList.add('correct');
            sudokuBoard[row][col] = number;
            checkGameCompletion();
        } else {
            // 不填入错误的数字，只显示错误状态
            selectedCell.classList.add('incorrect');
            setTimeout(() => {
                selectedCell.classList.remove('incorrect');
            }, 500);
            errors++;
            updateErrorDisplay();
            if (errors >= 3) {
                endGame(false);
            }
        }
        console.log('数字已处理');
    } else {
        console.log('无法填入数字：未选中单元格或单元格为固定值');
    }
}

function updateErrorDisplay() {
    document.getElementById('errors').textContent = `错误: ${errors}/3`;
}

function checkGameCompletion() {
    const allCells = document.querySelectorAll('.cell');
    const isComplete = [...allCells].every(cell => cell.textContent !== '');
    if (isComplete) {
        endGame(true);
    }
}

function endGame(isWin) {
    clearInterval(timer);
    document.getElementById('game-board').style.display = 'none';

    // 移除所有已存在的返回菜单按钮
    removeAllReturnButtons();

    const gameResult = document.getElementById('game-result');
    gameResult.innerHTML = ''; // 清空之前的内容
    gameResult.style.display = 'block';

    const resultMessage = document.createElement('div');
    resultMessage.id = 'result-message';
    if (isWin) {
        resultMessage.textContent = `恭喜！你赢了！用时: ${formatTime(seconds)}`;
    } else {
        resultMessage.textContent = '游戏结束，错误次数超过3次。';
    }
    gameResult.appendChild(resultMessage);

    // 为结果界面添加返回菜单按钮
    const returnButton = document.createElement('button');
    returnButton.id = 'return-button';
    returnButton.textContent = '返回菜单';
    returnButton.onclick = returnToMenu;
    gameResult.appendChild(returnButton);
}

function startTimer() {
    if (timer) clearInterval(timer);
    seconds = 0;
    timer = setInterval(() => {
        seconds++;
        document.getElementById('timer').textContent = `时间: ${formatTime(seconds)}`;
    }, 1000);
}

function formatTime(totalSeconds) {
    const minutes = Math.floor(totalSeconds / 60);
    const seconds = totalSeconds % 60;
    return `${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}`;
}

function returnToMenu() {
    if (timer) clearInterval(timer);
    document.getElementById('difficulty-selection').style.display = 'block';
    document.getElementById('game-board').style.display = 'none';
    document.getElementById('game-result').style.display = 'none';

    // 移除所有返回菜单按钮
    removeAllReturnButtons();

    // 重置游戏状态
    selectedCell = null;
    errors = 0;
    seconds = 0;
    resetAnswerState();
    updateErrorDisplay();
    document.getElementById('timer').textContent = '时间: 00:00';
}

function isValidPlacement(board, row, col, num) {
    // 检查行
    for (let x = 0; x < 9; x++) {
        if (board[row][x] == num) return false;
    }
    
    // 检查列
    for (let x = 0; x < 9; x++) {
        if (board[x][col] == num) return false;
    }
    
    // 检查3x3方格
    let startRow = Math.floor(row / 3) * 3;
    let startCol = Math.floor(col / 3) * 3;
    for (let i = 0; i < 3; i++) {
        for (let j = 0; j < 3; j++) {
            if (board[i + startRow][j + startCol] == num) return false;
        }
    }
    
    // 只检查撇对角线（反对角线）- 百分号数独特有
    if (row + col === 8) {
        for (let x = 0; x < 9; x++) {
            if (board[x][8 - x] == num) return false;
        }
    }
    
    // 检查上方窗口约束：位置[1-3][1-3]（对应数组[1][1]到[3][3]）
    const upperWindowPositions = [
        [1,1], [1,2], [1,3],
        [2,1], [2,2], [2,3],
        [3,1], [3,2], [3,3]
    ];
    if (upperWindowPositions.some(([r, c]) => r === row && c === col)) {
        for (const [r, c] of upperWindowPositions) {
            if (board[r][c] == num) return false;
        }
    }
    
    // 检查下方窗口约束：位置[5-7][5-7]（对应数组[5][5]到[7][7]）
    const lowerWindowPositions = [
        [5,5], [5,6], [5,7],
        [6,5], [6,6], [6,7],
        [7,5], [7,6], [7,7]
    ];
    if (lowerWindowPositions.some(([r, c]) => r === row && c === col)) {
        for (const [r, c] of lowerWindowPositions) {
            if (board[r][c] == num) return false;
        }
    }
    
    return true;
}

// 显示/隐藏答案功能
function toggleAnswer() {
    const button = document.getElementById('show-answer-btn');
    const cells = document.querySelectorAll('.cell');
    
    if (!isAnswerShown) {
        // 显示答案
        cells.forEach((cell, index) => {
            const row = Math.floor(index / 9);
            const col = index % 9;
            if (!cell.classList.contains('fixed')) {
                cell.textContent = solution[row][col];
                cell.classList.add('answer-shown');
                cell.style.color = '#ff6b6b';
                cell.style.fontStyle = 'italic';
            }
        });
        button.textContent = '隐藏答案';
        button.classList.add('active');
        isAnswerShown = true;
    } else {
        // 隐藏答案，恢复用户输入
        cells.forEach((cell, index) => {
            const row = Math.floor(index / 9);
            const col = index % 9;
            if (!cell.classList.contains('fixed')) {
                cell.textContent = sudokuBoard[row][col] === 0 ? '' : sudokuBoard[row][col];
                cell.classList.remove('answer-shown');
                cell.style.color = '';
                cell.style.fontStyle = '';
            }
        });
        button.textContent = '显示答案';
        button.classList.remove('active');
        isAnswerShown = false;
    }
}

// 提示功能
function getHint() {
    if (selectedCell && !selectedCell.classList.contains('fixed')) {
        const row = Math.floor([...selectedCell.parentNode.children].indexOf(selectedCell) / 9);
        const col = [...selectedCell.parentNode.children].indexOf(selectedCell) % 9;
        
        if (sudokuBoard[row][col] === 0) {
            const correctNumber = solution[row][col];
            selectedCell.textContent = correctNumber;
            selectedCell.classList.add('hint-given');
            selectedCell.style.color = '#28a745';
            selectedCell.style.fontWeight = 'bold';
            sudokuBoard[row][col] = correctNumber;
            
            checkGameCompletion();
        }
    } else {
        alert('请先选择一个空白格子来获取提示！');
    }
}

// 重置答案显示状态
function resetAnswerState() {
    isAnswerShown = false;
    const button = document.getElementById('show-answer-btn');
    if (button) {
        button.textContent = '显示答案';
        button.classList.remove('active');
    }
}
