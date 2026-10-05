# 🚀 VidyaVriksha — Cloud Deployment Guide

This guide explains how to deploy **VidyaVriksha** online for free so anyone can access it from a web browser.

Because we configured FastAPI to serve both the REST API and the frontend UI together, you only need **one single free service**!

---

## 🌟 Option 1: Render.com (Recommended — 100% Free)

Render provides a free Web Service tier that can host the entire full-stack application.

### Step 1: Upload code to GitHub
1. Go to [GitHub.com](https://github.com) and create a new repository named `VidyaVriksha` (set to **Public**).
2. Upload the project folder `c:\Users\24f20\Desktop\VidyaVriksha` to GitHub:
   - Either drag and drop the files directly on the GitHub web page, OR
   - Install [GitHub Desktop](https://desktop.github.com/) to push with one click.

### Step 2: Deploy on Render
1. Go to [Render.com](https://render.com) and Sign Up / Log In (you can log in with GitHub).
2. Click **New +** in the top navigation and select **Web Service**.
3. Connect your `VidyaVriksha` GitHub repository.
4. Fill in the settings:
   - **Name**: `vidyavriksha` (or any name you like)
   - **Region**: Choose the closest region (e.g., *Singapore* or *Frankfurt*)
   - **Branch**: `main`
   - **Root Directory**: *(leave blank)*
   - **Runtime**: `Python 3`
   - **Build Command**:
     ```bash
     pip install -r backend/requirements.txt
     ```
   - **Start Command**:
     ```bash
     cd backend && uvicorn main:app --host 0.0.0.0 --port $PORT --workers 1
     ```
   - **Instance Type**: `Free`
5. Click **Create Web Service**.

Within 2 minutes, Render will build and deploy your application. You will get a live public URL like:
👉 **`https://vidyavriksha.onrender.com`**

---

## 🚂 Option 2: Railway.app

1. Go to [Railway.app](https://railway.app) and sign in with GitHub.
2. Click **New Project** → **Deploy from GitHub repo**.
3. Select your `VidyaVriksha` repository.
4. Railway will automatically detect the `Procfile` and deploy your app.
5. In project settings, click **Generate Domain** to get your live `.up.railway.app` URL.

---

## ⚡ Option 3: Separate Frontend on Vercel + Backend on Render

If you prefer hosting the static frontend on Vercel:

1. Deploy the backend on Render as shown in Option 1. Copy your backend URL (e.g. `https://vidyavriksha-api.onrender.com`).
2. Open `frontend/app.js` and set:
   ```javascript
   const API_BASE = 'https://vidyavriksha-api.onrender.com/api';
   ```
3. Set the environment variable on Render:
   ```
   FRONTEND_URL=https://your-frontend.vercel.app
   ```
4. Deploy the `frontend/` folder on Vercel (import repo and set Root Directory to `frontend`).

---

## 💻 Running Locally

To run locally anytime:
- **Windows**: Just double-click `run.bat` in the `VidyaVriksha` folder. It will launch the server and automatically open `http://localhost:8000` in your default browser!
- **Terminal**:
  ```powershell
  cd c:\Users\24f20\Desktop\VidyaVriksha\backend
  .\venv\Scripts\activate
  uvicorn main:app --port 8000 --reload
  ```
