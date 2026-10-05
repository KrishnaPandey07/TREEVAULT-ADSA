# 🌲 TreeVault — Student Record Management Using Self-Balancing AVL Tree

> **"Organising Knowledge, One Balanced Tree at a Time."**

TreeVault is a modern, high-performance full-stack web application designed for ADSA (Advanced Data Structures and Algorithms). It manages student records using a **real, hand-coded AVL Tree backend** with live SVG telemetry, interactive rotation demos, logarithmic complexity guarantees, and SQLite persistence.

## ✨ Highlights

- **Real Backend AVL Tree**: Insertions, searches, and deletions are executed strictly within a manual Python AVL Tree data structure with height calculation, balance factor checks (`-1 ≤ BF ≤ +1`), and automatic LL/RR/LR/RL rotations.
- **Modern Developer Studio UI**: Sleek dark theme inspired by Linear and Supabase, with an SVG tree graph emblem and high-contrast telemetry.
- **Interactive Rotation Playground**: 1-click demos for LL (Right Rotation), RR (Left Rotation), LR (Left-Right Rotation), and RL (Right-Left Rotation).
- **Out-of-the-Box One-Click Random Student**: Instant testing with pre-generated student profiles and random marks.
- **Real-Time SVG Telemetry**: Click any node to inspect height, balance factor, children keys, and invariant satisfaction.
- **Search Path Highlighting**: Visualizes the binary search path `O(log n)` from the root to the target student.
- **SQLite Persistence**: Automatically preserves student records across server restarts without substituting database queries for tree logic.

## 🏗️ System Architecture

```
                USER
                  │
                  ▼
        ┌──────────────────┐
        │   VIDYAVRIKSHA   │
        │    FRONTEND      │
        │  (HTML/CSS/JS)   │
        └────────┬─────────┘
                 │
             REST API
                 │
                 ▼
        ┌──────────────────┐
        │     FASTAPI      │
        │     BACKEND      │
        └────────┬─────────┘
                 │
                 ▼
        ┌──────────────────┐
        │    AVL TREE      │
        │                  │
        │ Insert / Search  │
        │ Delete / Balance │
        │ Rotations        │
        └────────┬─────────┘
                 │
                 ▼
        ┌──────────────────┐
        │      SQLITE      │
        │   PERSISTENCE    │
        └──────────────────┘
```

## 🛠️ Technology Stack

| Layer | Technology |
|-------|------------|
| Frontend | HTML5, CSS3, Vanilla JavaScript |
| Backend | Python, FastAPI |
| Data Structure | AVL Tree (hand-implemented) |
| Persistence | SQLite |
| Validation | Pydantic v2 |

## 🌲 AVL Tree Explanation

An AVL Tree is a self-balancing Binary Search Tree where the difference in heights of left and right subtrees (the **balance factor**) of every node is at most 1.

### Balance Factor
```
BF(node) = height(left subtree) - height(right subtree)
```
Valid range: **-1 ≤ BF ≤ 1**

### Rotation Cases

| Case | Condition | Rotation |
|------|-----------|----------|
| LL | BF > 1, key < left child | Right Rotation |
| RR | BF < -1, key > right child | Left Rotation |
| LR | BF > 1, key > left child | Left + Right Rotation |
| RL | BF < -1, key < right child | Right + Left Rotation |

### Complexity

| Operation | Time Complexity |
|-----------|----------------|
| Search | O(log n) |
| Insert | O(log n) |
| Delete | O(log n) |
| Space | O(n) |

## 📡 API Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| POST | `/api/students` | Add a new student |
| GET | `/api/students` | Get all students (sorted) |
| GET | `/api/students/{id}` | Search for a student |
| DELETE | `/api/students/{id}` | Delete a student |
| GET | `/api/tree` | Get complete tree structure |
| GET | `/api/tree/traversal/{order}` | Get traversal (inorder/preorder/postorder) |
| GET | `/api/tree/stats` | Get tree statistics |
| DELETE | `/api/clear` | Clear all records |

## 🚀 Installation

### Prerequisites
- Python 3.9+
- A modern web browser

### Clone the Repository
```bash
git clone <repository-url>
cd VidyaVriksha
```

### Backend Setup
```bash
cd backend
python -m venv venv

# Windows
venv\Scripts\activate

# macOS/Linux
source venv/bin/activate

pip install -r requirements.txt
```

### Environment Configuration
Copy the example environment file:
```bash
cp .env.example .env
```
Edit `.env` if you need to change defaults.

## ▶️ Running the Application

### Start the Backend
```bash
cd backend
uvicorn main:app --reload --port 8000
```
The API will be available at `http://localhost:8000`.
API documentation: `http://localhost:8000/docs`

### Start the Frontend
Open `frontend/index.html` in your browser, or use a local server:
```bash
# Using Python
cd frontend
python -m http.server 5500

# Or use VS Code Live Server extension
```
The frontend will be available at `http://localhost:5500`.

## 🧪 Testing

### Run AVL Tree & API Tests
```bash
cd backend
pytest -v
```
This runs the 37 automated tests covering:
- Unit tests for the AVL tree (`test_avl.py`): insertions, deletions, all 4 rotation cases (LL, RR, LR, RL), heights, balance factors, duplicate handling, traversals, and invariants.
- Integration tests for FastAPI & persistence (`test_api.py`): all REST endpoints, SQLite data persistence, and tree rebuilding upon restart.

### Manual Testing Checklist
- [ ] Add a student and verify it appears in the table and tree
- [ ] Add duplicate ID and verify error message
- [ ] Search existing student and verify highlighting
- [ ] Search non-existing student and verify "not found" message
- [ ] Delete a student and verify removal from table and tree
- [ ] Test LL rotation: Insert 30, 20, 10
- [ ] Test RR rotation: Insert 10, 20, 30
- [ ] Test LR rotation: Insert 30, 10, 20
- [ ] Test RL rotation: Insert 10, 30, 20
- [ ] Click "Try Demo" and observe rotations
- [ ] Click "Clear All" and verify everything resets
- [ ] Stop and restart backend, verify data persists

## 📦 Deployment

### Production Notes
1. Set `FRONTEND_URL` in `.env` to your production frontend URL
2. Use a production ASGI server:
   ```bash
   uvicorn main:app --host 0.0.0.0 --port 8000 --workers 1
   ```
   Note: Use only 1 worker since the AVL tree is stored in memory.
3. Update the `API_BASE` in `app.js` to point to your production backend URL

## 📁 Project Structure
```
VidyaVriksha/
├── backend/
│   ├── main.py           # FastAPI application and routes
│   ├── avl_tree.py       # AVL Tree implementation
│   ├── database.py       # SQLite persistence
│   ├── schemas.py        # Pydantic validation schemas
│   ├── test_avl.py       # AVL Tree automated unit tests
│   ├── test_api.py       # API integration & persistence tests
│   └── requirements.txt  # Python dependencies
├── frontend/
│   ├── index.html        # Main HTML page
│   ├── style.css         # Styles and design
│   └── app.js            # JavaScript logic and tree visualization
├── .env.example          # Environment variable template
└── README.md             # This file
```

## 📄 License

This project is created for academic purposes as an ADSA mini-project.

---

*VidyaVriksha — where ancient wisdom meets modern algorithms.* 🌳
