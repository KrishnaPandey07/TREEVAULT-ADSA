const API_BASE = (window.location.port === '8000' || !window.location.port || (window.location.hostname !== 'localhost' && window.location.hostname !== '127.0.0.1'))
    ? '/api'
    : 'http://localhost:8000/api';

// ---- Navigation ----

function goTo(id) {
    document.querySelectorAll('.sec').forEach(s => s.classList.remove('visible'));
    document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));

    const sec = document.getElementById('sec-' + id);
    if (sec) sec.classList.add('visible');

    const tab = document.querySelector(`.tab[data-tab="${id}"]`);
    if (tab) tab.classList.add('active');

    window.scrollTo({ top: 0, behavior: 'smooth' });
}

function runDemoInteractive(type) {
    goTo('play');
    setTimeout(() => {
        const map = { LL: 'demo-btn', RR: 'demo-rr-btn', LR: 'demo-lr-btn', RL: 'demo-rl-btn' };
        const btn = document.getElementById(map[type]);
        if (btn) btn.click();
    }, 300);
}

// ---- API ----

async function fetchAPI(endpoint, options = {}) {
    try {
        const res = await fetch(API_BASE + endpoint, options);
        if (!res.ok) {
            const err = await res.json().catch(() => ({}));
            throw new Error(err.detail || 'Something went wrong');
        }
        return await res.json();
    } catch (e) {
        showToast(e.message, 'error');
        throw e;
    }
}

const addStudent   = (id, name, marks) => fetchAPI('/students', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify({ id, name, marks }) });
const getAllStudents = () => fetchAPI('/students');
const searchStudent = (id) => fetchAPI('/students/' + id);
const deleteStudent = (id) => fetchAPI('/students/' + id, { method: 'DELETE' });
const getTree       = () => fetchAPI('/tree');
const getStats      = () => fetchAPI('/tree/stats');
const getTraversal  = (order) => fetchAPI('/tree/traversal/' + order);
const clearAll      = () => fetchAPI('/clear', { method: 'DELETE' });

// ---- Toast ----

function showToast(msg, type = 'info') {
    const el = document.getElementById('toast');
    el.textContent = msg;
    el.className = 'toast toast-' + type;
    clearTimeout(el._timer);
    el._timer = setTimeout(() => el.classList.add('hidden'), 3500);
}

// ---- Render Table ----

function renderTable(students) {
    const tbody = document.getElementById('students-tbody');
    const empty = document.getElementById('empty-message');
    const table = document.getElementById('students-table');
    const chip  = document.getElementById('student-count');

    tbody.innerHTML = '';
    chip.textContent = students.length;

    if (!students.length) {
        empty.style.display = 'block';
        table.style.display = 'none';
    } else {
        empty.style.display = 'none';
        table.style.display = 'table';
        students.forEach(s => {
            const tr = document.createElement('tr');
            tr.innerHTML = `<td><strong>${s.id}</strong></td><td>${s.name}</td><td>${s.marks}</td><td><button class="btn-del" onclick="handleDelete(${s.id})">Delete</button></td>`;
            tbody.appendChild(tr);
        });
    }
}

// ---- Render Stats ----

function renderStats(st) {
    document.getElementById('stat-count').textContent = st.count;
    document.getElementById('stat-height').textContent = st.height;
    document.getElementById('stat-balanced').textContent = st.is_balanced ? 'Yes' : 'No';
    document.getElementById('stat-balanced').style.color = st.is_balanced ? '#00B894' : '#D63031';
    document.getElementById('stat-rotations').textContent = st.total_rotations || 0;
}

// ---- Search Result ----

function showSearchResult(data) {
    const el = document.getElementById('search-result');
    el.classList.remove('hidden');
    let h = `<strong style="color: var(--green);">Found!</strong><br>ID: ${data.student.id} · Name: ${data.student.name} · Marks: ${data.student.marks}`;
    if (data.search_path && data.search_path.length) {
        h += `<br><strong>Path taken:</strong> <code style="color: var(--accent);">${data.search_path.join(' → ')}</code>`;
    }
    el.innerHTML = h;
}

// ---- Traversals ----

async function showTraversal(order) {
    const el = document.getElementById('traversal-result');
    try {
        const data = await getTraversal(order);
        el.classList.remove('hidden');
        const label = order.charAt(0).toUpperCase() + order.slice(1);
        if (data.students && data.students.length) {
            el.innerHTML = `<strong>${label}:</strong> ${data.students.map(s => s.id + ' (' + s.name + ')').join(' → ')}`;
        } else {
            el.innerHTML = `<strong>${label}:</strong> (empty)`;
        }
    } catch (e) {}
}

// ---- Tree Rendering ----

let nodeMap = new Map();

function inspectNode(id) {
    const n = nodeMap.get(id);
    const el = document.getElementById('node-inspector');
    if (!n || !el) return;

    let status = 'Perfectly balanced';
    let color = 'color: #00B894;';
    if (n.balance_factor === 1 || n.balance_factor === -1) {
        status = 'Slightly tilted (still valid)';
        color = 'color: #E17055;';
    }

    el.classList.remove('hidden');
    el.innerHTML = `
        <div class="inspector-title">Student #${n.id} — ${n.name}</div>
        <div class="inspector-grid">
            <div class="inspector-box"><span class="box-label">ID</span><span class="box-value">${n.id}</span></div>
            <div class="inspector-box"><span class="box-label">Name</span><span class="box-value">${n.name}</span></div>
            <div class="inspector-box"><span class="box-label">Marks</span><span class="box-value">${n.marks}</span></div>
            <div class="inspector-box"><span class="box-label">Height</span><span class="box-value">${n.height}</span></div>
            <div class="inspector-box"><span class="box-label">Balance</span><span class="box-value" style="${color}">${n.balance_factor}</span></div>
            <div class="inspector-box"><span class="box-label">Left</span><span class="box-value">${n.left ? n.left.id : '—'}</span></div>
            <div class="inspector-box"><span class="box-label">Right</span><span class="box-value">${n.right ? n.right.id : '—'}</span></div>
        </div>
        <div class="inspector-note"><span style="${color} font-weight: 700;">${status}</span> — Balance = Left height minus Right height. Needs to be -1, 0, or +1.</div>
    `;
    el.scrollIntoView({ behavior: 'smooth', block: 'nearest' });
}

const NW = 116, NH = 64, LH = 92, NG = 18;

function renderTree(tree, highlights = []) {
    const svg = document.getElementById('tree-svg');
    const emptyEl = document.getElementById('tree-empty');
    nodeMap.clear();

    if (!tree) {
        svg.innerHTML = '';
        svg.style.display = 'none';
        emptyEl.style.display = 'block';
        const insp = document.getElementById('node-inspector');
        if (insp) insp.classList.add('hidden');
        return;
    }

    emptyEl.style.display = 'none';
    svg.style.display = 'block';

    const pos = new Map();
    let idx = 0;
    const PAD = 20;

    function layout(node, depth) {
        if (!node) return;
        nodeMap.set(node.id, node);
        layout(node.left, depth + 1);
        pos.set(node.id, {
            x: PAD + idx * (NW + NG) + NW / 2,
            y: PAD + depth * LH + NH / 2
        });
        idx++;
        layout(node.right, depth + 1);
    }

    layout(tree, 0);

    function edges(node) {
        if (!node) return '';
        let s = '';
        const p = pos.get(node.id);
        if (node.left) {
            const c = pos.get(node.left.id);
            s += `<line x1="${p.x}" y1="${p.y + NH/2}" x2="${c.x}" y2="${c.y - NH/2}" class="tree-edge"/>`;
            s += edges(node.left);
        }
        if (node.right) {
            const c = pos.get(node.right.id);
            s += `<line x1="${p.x}" y1="${p.y + NH/2}" x2="${c.x}" y2="${c.y - NH/2}" class="tree-edge"/>`;
            s += edges(node.right);
        }
        return s;
    }

    function nodes(node) {
        if (!node) return '';
        const p = pos.get(node.id);
        const hl = highlights.includes(node.id);
        const cls = hl ? 'node-highlight' : '';
        const bfCol = node.balance_factor === 0 ? '#00B894' : '#E17055';
        const bfTxt = node.balance_factor === 0 ? 'balanced' : 'BF: ' + node.balance_factor;

        let s = `<g class="tree-node-group" onclick="inspectNode(${node.id})" style="cursor:pointer">`;
        s += `<rect x="${p.x-NW/2}" y="${p.y-NH/2}" width="${NW}" height="${NH}" class="tree-node-rect ${cls}"/>`;
        s += `<text x="${p.x}" y="${p.y-12}" class="node-id">${node.id}</text>`;
        s += `<text x="${p.x}" y="${p.y+5}" class="node-name">${node.name}</text>`;
        s += `<text x="${p.x}" y="${p.y+20}" class="node-bf" fill="${bfCol}">${bfTxt}</text>`;
        s += `</g>`;
        s += nodes(node.left);
        s += nodes(node.right);
        return s;
    }

    const maxX = Math.max(...[...pos.values()].map(p => p.x)) + NW / 2 + PAD;
    const maxY = Math.max(...[...pos.values()].map(p => p.y)) + NH / 2 + PAD;
    svg.setAttribute('width', maxX);
    svg.setAttribute('height', maxY);
    svg.setAttribute('viewBox', `0 0 ${maxX} ${maxY}`);
    svg.innerHTML = edges(tree) + nodes(tree);
}

function updateRotation(rotations) {
    const el = document.getElementById('rotation-info');
    if (!el) return;
    if (rotations && rotations.length) {
        el.textContent = 'Tree rebalanced: ' + rotations.join(', ');
        el.classList.remove('hidden');
    } else {
        el.classList.add('hidden');
    }
}

// ---- Refresh Everything ----

async function refresh(highlights = []) {
    try {
        const [students, tree, stats] = await Promise.all([getAllStudents(), getTree(), getStats()]);
        renderTable(students);
        renderTree(tree, highlights);
        renderStats(stats);
    } catch (e) {}
}

// ---- Event Handlers ----

document.getElementById('add-form').addEventListener('submit', async (e) => {
    e.preventDefault();
    const id = parseInt(document.getElementById('student-id').value);
    const name = document.getElementById('student-name').value.trim();
    const marks = parseFloat(document.getElementById('student-marks').value);

    if (!id || id <= 0) return showToast('Enter a valid ID', 'error');
    if (!name) return showToast('Enter a name', 'error');
    if (isNaN(marks) || marks < 0 || marks > 100) return showToast('Marks should be 0–100', 'error');

    try {
        const r = await addStudent(id, name, marks);
        let msg = `Added ${name} (#${id})`;
        let type = 'success';
        if (r.rotations && r.rotations.length) {
            msg += ' — tree rebalanced!';
            type = 'rotation';
        }
        showToast(msg, type);
        updateRotation(r.rotations);
        document.getElementById('add-form').reset();
        await refresh([id]);
    } catch (e) {}
});

document.getElementById('search-btn').addEventListener('click', async () => {
    const id = parseInt(document.getElementById('search-id').value);
    const el = document.getElementById('search-result');
    if (!id || id <= 0) return showToast('Enter a valid ID', 'error');

    try {
        const r = await searchStudent(id);
        showSearchResult(r);
        showToast(`Found student #${id}`, 'success');
        await refresh(r.search_path || [id]);
    } catch (e) {
        el.classList.remove('hidden');
        el.innerHTML = '<strong style="color: var(--red);">Not found.</strong>';
        await refresh();
    }
});

async function handleDelete(id) {
    if (!confirm('Delete student #' + id + '?')) return;
    try {
        const r = await deleteStudent(id);
        let msg = `Deleted #${id}`;
        if (r.rotations && r.rotations.length) msg += ' — tree rebalanced!';
        showToast(msg, r.rotations && r.rotations.length ? 'rotation' : 'success');
        updateRotation(r.rotations);
        document.getElementById('search-result').classList.add('hidden');
        await refresh();
    } catch (e) {}
}

document.getElementById('clear-btn').addEventListener('click', async () => {
    if (!confirm('Remove all students?')) return;
    try {
        await clearAll();
        showToast('All cleared', 'success');
        updateRotation([]);
        document.getElementById('traversal-result').classList.add('hidden');
        document.getElementById('search-result').classList.add('hidden');
        await refresh();
    } catch (e) {}
});

// ---- Rotation Demos ----

async function runDemo(label, data, desc) {
    const btns = document.querySelectorAll('.btn-pill, .btn-main, .btn-try');
    btns.forEach(b => b.disabled = true);

    try { await clearAll(); await refresh(); } catch (e) {}

    for (const s of data) {
        try {
            const r = await addStudent(s.id, s.name, s.marks);
            let msg = `Added ${s.name} (#${s.id})`;
            let type = 'success';
            if (r.rotations && r.rotations.length) { msg += ' — rotated!'; type = 'rotation'; }
            showToast(msg, type);
            updateRotation(r.rotations);
            await refresh([s.id]);
            await new Promise(r => setTimeout(r, 1100));
        } catch (e) {}
    }

    showToast(label + ' done — ' + desc, 'rotation');
    btns.forEach(b => b.disabled = false);
}

document.getElementById('demo-btn').addEventListener('click', () => {
    runDemo('LL Demo', [
        { id: 30, name: 'Alex', marks: 88 },
        { id: 20, name: 'Maya', marks: 94 },
        { id: 10, name: 'Liam', marks: 79 }
    ], 'Right rotation fixed the imbalance');
});

document.getElementById('demo-rr-btn').addEventListener('click', () => {
    runDemo('RR Demo', [
        { id: 10, name: 'Elena', marks: 85 },
        { id: 20, name: 'Lucas', marks: 91 },
        { id: 30, name: 'Zoe', marks: 78 }
    ], 'Left rotation fixed the imbalance');
});

document.getElementById('demo-lr-btn').addEventListener('click', () => {
    runDemo('LR Demo', [
        { id: 30, name: 'Aria', marks: 86 },
        { id: 10, name: 'Noah', marks: 92 },
        { id: 20, name: 'Sam', marks: 81 }
    ], 'Double rotation fixed the zigzag');
});

document.getElementById('demo-rl-btn').addEventListener('click', () => {
    runDemo('RL Demo', [
        { id: 10, name: 'Chloe', marks: 84 },
        { id: 30, name: 'Kai', marks: 90 },
        { id: 20, name: 'Leo', marks: 77 }
    ], 'Double rotation fixed the zigzag');
});

// ---- Random Student ----

document.getElementById('random-btn').addEventListener('click', async () => {
    const names = ['Alex', 'Maya', 'Liam', 'Elena', 'Zoe', 'Lucas', 'Aria', 'Noah', 'Chloe', 'Kai', 'Leo', 'Sophia', 'Julian', 'Emma', 'Nathan', 'Mia'];

    const used = new Set();
    document.querySelectorAll('#students-tbody tr').forEach(r => {
        const td = r.querySelector('td strong');
        if (td) used.add(parseInt(td.textContent));
    });

    let id = Math.floor(Math.random() * 900) + 100;
    let tries = 0;
    while (used.has(id) && tries < 100) { id = Math.floor(Math.random() * 900) + 100; tries++; }

    const name = names[Math.floor(Math.random() * names.length)];
    const marks = Math.floor(Math.random() * 35) + 65;

    try {
        const r = await addStudent(id, name, marks);
        let msg = `Added ${name} (#${id})`;
        if (r.rotations && r.rotations.length) msg += ' — tree rebalanced!';
        showToast(msg, r.rotations && r.rotations.length ? 'rotation' : 'success');
        updateRotation(r.rotations);
        await refresh([id]);
    } catch (e) {}
});

// ---- Init ----

document.addEventListener('DOMContentLoaded', () => refresh());
