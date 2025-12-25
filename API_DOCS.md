# Ludo Game API Documentation (v1)

Base URL: `http://localhost:8080/api/v1/game`

## Response Format
All responses follow a standard envelope:
```json
{
  "status": "success" | "error",
  "data": { ... },    // Only on success
  "message": "..."    // Only on error
}
```

---

## Endpoints

### 1. Get Game State
Retrieves the current snapshot of the board, players, and turn info.

- **URL**: `/state`
- **Method**: `GET`
- **Response**:
```json
{
  "status": "success",
  "data": {
    "current_turn": 0,
    "last_roll": 6,
    "waiting_for_move": true,
    "winner": -1,
    "players": [
      {
        "id": 0,
        "color": "Green",
        "pieces": [
            { "row": 6, "col": 1, "active": false, "home": false }
        ]
      },
      ...
    ]
  }
}
```

### 2. Roll Dice
Initiates a dice roll for the current player.

- **URL**: `/roll`
- **Method**: `POST`
- **Body**: `{ "playerId": 0 }`
- **Response**:
```json
{
  "status": "success",
  "data": { "roll": 5 }
}
```

### 3. Move Piece
Moves a specific piece based on the last roll.

- **URL**: `/move`
- **Method**: `POST`
- **Body**: `{ "playerId": 0, "pieceId": 2 }`
- **Response**:
```json
{
  "status": "success",
  "data": { "moved": true }
}
```

### 4. Reset Game
Resets the board to the initial state.

- **URL**: `/reset`
- **Method**: `POST`
- **Response**:
```json
{ "status": "success" }
```
