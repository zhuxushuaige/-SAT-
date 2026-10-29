// 简单的Node.js服务器，用于调用C++程序
const express = require('express');
const { spawn } = require('child_process');
const path = require('path');
const fs = require('fs');

const app = express();
const port = 8080;

// 静态文件服务
app.use(express.static(__dirname));
app.use(express.json());

// 生成数独题目的API
app.post('/api/generate-sudoku', (req, res) => {
    const { difficulty } = req.body;
    
    // 将难度转换为数字
    const difficultyNum = difficulty === 'easy' ? 1 : (difficulty === 'medium' ? 2 : 3);
    
    // 调用C++程序生成数独
    const cppProcess = spawn('./web_main.exe', [difficultyNum.toString()], {
        cwd: __dirname,
        stdio: 'pipe'
    });
    
    let output = '';
    let error = '';
    
    cppProcess.stdout.on('data', (data) => {
        output += data.toString();
    });
    
    cppProcess.stderr.on('data', (data) => {
        error += data.toString();
    });
    
    cppProcess.on('close', (code) => {
        if (code === 0) {
            try {
                const sudokuData = parseSudokuOutput(output);
                res.json({
                    success: true,
                    puzzle: sudokuData.puzzle,
                    solution: sudokuData.solution
                });
            } catch (err) {
                console.error('Parse error:', err);
                res.status(500).json({
                    success: false,
                    error: 'Failed to parse sudoku data'
                });
            }
        } else {
            console.error('C++ process error:', error);
            res.status(500).json({
                success: false,
                error: error || 'C++ program failed'
            });
        }
    });
});

// 验证数独解答的API
app.post('/api/validate-sudoku', (req, res) => {
    const { board } = req.body;
    
    // 这里可以添加数独验证逻辑
    const isValid = validateSudokuBoard(board);
    
    res.json({
        success: true,
        isValid: isValid
    });
});

function parseSudokuOutput(output) {
    // 解析C++程序的输出，提取数独题目和解答
    // 这需要根据你的C++程序的实际输出格式来调整
    const lines = output.trim().split('\n');
    
    // 假设输出格式为：题目的9行，然后解答的9行
    const puzzle = [];
    const solution = [];
    
    for (let i = 0; i < 9; i++) {
        puzzle[i] = lines[i].split(' ').map(num => parseInt(num));
    }
    
    for (let i = 9; i < 18; i++) {
        solution[i - 9] = lines[i].split(' ').map(num => parseInt(num));
    }
    
    return { puzzle, solution };
}

function validateSudokuBoard(board) {
    // 简单的数独验证逻辑
    for (let i = 0; i < 9; i++) {
        for (let j = 0; j < 9; j++) {
            const num = board[i][j];
            if (num !== 0) {
                // 检查行
                for (let k = 0; k < 9; k++) {
                    if (k !== j && board[i][k] === num) return false;
                }
                
                // 检查列
                for (let k = 0; k < 9; k++) {
                    if (k !== i && board[k][j] === num) return false;
                }
                
                // 检查3x3宫格
                const boxRow = Math.floor(i / 3) * 3;
                const boxCol = Math.floor(j / 3) * 3;
                for (let r = boxRow; r < boxRow + 3; r++) {
                    for (let c = boxCol; c < boxCol + 3; c++) {
                        if ((r !== i || c !== j) && board[r][c] === num) return false;
                    }
                }
                
                // 检查对角线（X数独特有）
                if (i === j) { // 主对角线
                    for (let k = 0; k < 9; k++) {
                        if (k !== i && board[k][k] === num) return false;
                    }
                }
                if (i + j === 8) { // 副对角线
                    for (let k = 0; k < 9; k++) {
                        if (k !== i && board[k][8 - k] === num) return false;
                    }
                }
            }
        }
    }
    return true;
}

app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
});

module.exports = app;
