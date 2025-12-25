const API_URL = ""; // Relative to server
let gameId = null;
let currentState = null;
let lastRoll = 0;
let currentPlayerIdx = 0;

// Config
const ROWS = 15;
const COLS = 15;

const UI = {
    board: document.getElementById('ludo-board'),
    rollBtn: document.getElementById('roll-button'),
    resetBtn: document.getElementById('reset-button'),
    dice: document.getElementById('dice'),
    playerName: document.querySelector('.player-name'),
    playerIndicator: document.querySelector('.player-indicator .dot'),
    gameLog: document.getElementById('game-log'),
    winModal: document.getElementById('win-modal'),
    winnerName: document.getElementById('winner-name')
};

function addLog(msg) {
    const entry = document.createElement('div');
    entry.className = 'log-entry';
    entry.textContent = `> ${msg}`;
    UI.gameLog.appendChild(entry);
    UI.gameLog.scrollTop = UI.gameLog.scrollHeight;
}

// Initialize Board Cells
function createBoard() {
    UI.board.innerHTML = '';

    // Create base overlays
    for (let i = 0; i < 4; i++) {
        const base = document.createElement('div');
        base.className = `base base-${i}`;
        const inner = document.createElement('div');
        inner.className = 'base-inner';
        for (let j = 0; j < 4; j++) {
            const spot = document.createElement('div');
            spot.className = 'base-spot';
            inner.appendChild(spot);
        }
        base.appendChild(inner);
        UI.board.appendChild(base);
    }

    // Create center
    const center = document.createElement('div');
    center.className = 'center-home';
    center.innerHTML = '<div class="center-inner"></div>';
    UI.board.appendChild(center);

    // Create 15x15 cells
    for (let r = 0; r < ROWS; r++) {
        for (let c = 0; c < COLS; c++) {
            // Skip bases and center (already handled by z-index or explicit overlay)
            // But for ludo track, we need cells
            const cell = document.createElement('div');
            cell.className = 'cell';
            cell.dataset.r = r;
            cell.dataset.c = c;

            // Highlight specialty cells
            if (isHomeStretch(r, c)) cell.dataset.stretch = getStretchOwner(r, c);
            if (isStartSpot(r, c)) cell.classList.add(`start-${getStartOwner(r, c)}`);

            UI.board.appendChild(cell);
        }
    }
}

function isHomeStretch(r, c) {
    if (r === 7 && c >= 1 && c <= 5) return true;
    if (c === 7 && r >= 1 && r <= 5) return true;
    if (r === 7 && c >= 9 && c <= 13) return true;
    if (c === 7 && r >= 9 && r <= 13) return true;
    return false;
}

function getStretchOwner(r, c) {
    if (r === 7 && c <= 5) return 0; // Green
    if (c === 7 && r <= 5) return 1; // Red
    if (r === 7 && c >= 9) return 2; // Blue
    if (c === 7 && r >= 9) return 3; // Yellow
    return null;
}

function isStartSpot(r, c) {
    const starts = [[6, 1], [1, 8], [8, 13], [13, 6]];
    return starts.some(s => s[0] === r && s[1] === c);
}

function getStartOwner(r, c) {
    const starts = [[6, 1], [1, 8], [8, 13], [13, 6]];
    return starts.findIndex(s => s[0] === r && s[1] === c);
}

// Game Logic Communication
async function createGame() {
    const res = await fetch('/api/v1/game/create', { method: 'POST' });
    const json = await res.json();
    gameId = json.data.gameId;
    addLog(`Engine ready. Session: ${gameId}`);
    updateState();
}

async function updateState() {
    if (!gameId) return;
    const res = await fetch(`/api/v1/game/${gameId}/state`);
    const json = await res.json();
    if (json.status === 'success') {
        renderGame(json.data);
    }
}

function renderGame(data) {
    currentState = data.state;
    currentPlayerIdx = data.current_turn;
    lastRoll = data.last_roll;

    // Update Turn Info
    const p = data.players[currentPlayerIdx];
    UI.playerName.textContent = p.name;
    UI.playerIndicator.style.background = getHexColor(currentPlayerIdx);

    // Update Dice
    UI.dice.textContent = lastRoll || '?';
    UI.rollBtn.disabled = (currentState !== 1); // 1 = WAITING_FOR_ROLL

    // Remove old pieces
    document.querySelectorAll('.piece').forEach(p => p.remove());

    // Render Pieces
    data.players.forEach((player, pIdx) => {
        player.pieces.forEach((piece, pcIdx) => {
            const el = document.createElement('div');
            el.className = `piece p${pIdx}`;

            // If movable, add class and listener
            if (currentState === 2 && pIdx === currentPlayerIdx) {
                // Simplified movable check: if it's our turn and we rolled
                // In a real app, we'd check backend hasPossibleMoves
                el.classList.add('movable');
                el.onclick = () => makeMove(pcIdx);
            }

            // Position piece
            // Find cell at row/col
            const cell = document.querySelector(`.cell[data-r="${piece.row}"][data-c="${piece.col}"]`);
            if (cell) {
                // If multiple pieces in same cell, offset them slightly
                const existing = cell.querySelectorAll('.piece').length;
                if (existing > 0) {
                    el.style.transform = `translate(${-50 + existing * 10}%, ${-50 + existing * 10}%) scale(0.8)`;
                }
                cell.appendChild(el);
            } else if (piece.progress === -1) {
                // In base
                const baseSpots = document.querySelectorAll(`.base-${pIdx} .base-spot`);
                baseSpots[pcIdx].appendChild(el);
            }
        });
    });

    // Check Winner
    if (data.winner !== -1) {
        UI.winnerName.textContent = data.players[data.winner].name;
        UI.winModal.classList.add('active');
    }
}

async function rollDice() {
    UI.dice.classList.add('rolling');
    UI.rollBtn.disabled = true;

    try {
        const res = await fetch(`/api/v1/game/${gameId}/roll`, {
            method: 'POST',
            body: JSON.stringify({ playerId: currentPlayerIdx })
        });
        const json = await res.json();

        setTimeout(() => {
            UI.dice.classList.remove('rolling');
            if (json.status === 'success') {
                addLog(`Player ${currentPlayerIdx} rolled a ${json.data.roll}`);
                updateState();
            } else {
                addLog(`Roll failed: ${json.message}`);
                UI.rollBtn.disabled = false;
            }
        }, 600);
    } catch (e) {
        UI.dice.classList.remove('rolling');
        UI.rollBtn.disabled = false;
    }
}

async function makeMove(pieceId) {
    const res = await fetch(`/api/v1/game/${gameId}/move`, {
        method: 'POST',
        body: JSON.stringify({ playerId: currentPlayerIdx, pieceId: pieceId })
    });
    const json = await res.json();
    if (json.status === 'success') {
        updateState();
    } else {
        addLog(`Invalid move: ${json.message}`);
    }
}

function getHexColor(idx) {
    return ['#10b981', '#ef4444', '#3b82f6', '#f59e0b'][idx];
}

UI.rollBtn.onclick = rollDice;
UI.resetBtn.onclick = async () => {
    await fetch(`/api/v1/game/${gameId}/reset`, { method: 'POST' });
    updateState();
};

createBoard();
createGame();

// Poll state occasionally for multiplayer synchronized feel
setInterval(updateState, 2000);
