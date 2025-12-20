#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import json
import os
from http. cookies import SimpleCookie
from pathlib import Path

# Récupérer le cookie
cookie_string = os.environ. get("HTTP_COOKIE", "")
cookie = SimpleCookie()
cookie.load(cookie_string)

username = ""

# Essayer plusieurs noms de cookies
if "nicelife_user" in cookie:
    username = cookie["nicelife_user"]. value
elif "username" in cookie:
    username = cookie["username"]. value

# Si pas de cookie → rediriger vers login
if not username:
    print("Status: 303 See Other")
    print("Location: /login. py")
    print()
    exit()

# Créer le dossier data
data_dir = Path("data")
data_dir.mkdir(exist_ok=True)

# Chemin du fichier
tasks_file = data_dir / f"{username}_tasks.json"

# Charger les tâches
if tasks_file.exists():
    with open(tasks_file, "r", encoding="utf-8") as f:
        tasks = json.load(f)
else:
    tasks = []

# Générer le HTML
print("Content-Type: text/html; charset=utf-8")
print()

# Générer les tâches
tasks_html = ""
for task in tasks:
    checked = "checked" if task["done"] else ""
    done_class = "task-done" if task["done"] else ""

    tasks_html += f"""
    <div class="task-item {done_class}">
        <form method="POST" action="/toggle-task.py" class="task-form">
            <input type="hidden" name="username" value="{username}">
            <input type="hidden" name="task_id" value="{task['id']}">
            <input
                type="checkbox"
                name="done"
                class="task-checkbox"
                {checked}
                onchange="this.form.submit()"
            >
        </form>

        <span class="task-title">{task['title']}</span>

        <form method="POST" action="/delete-task.py" class="task-delete-form">
            <input type="hidden" name="username" value="{username}">
            <input type="hidden" name="task_id" value="{task['id']}">
            <button type="submit" class="btn-delete">🗑️</button>
        </form>
    </div>
    """

if not tasks:
    tasks_html = "<p class='no-tasks'>✨ Aucune tâche pour le moment.  Ajoutez-en une !</p>"

html = f"""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Dashboard - NiceLife 🌳</title>
    <link rel="stylesheet" href="/nicelife-style.css">
    <style>
        . dashboard-container {{
            min-height: 100vh;
            padding: 6rem 0 2rem 0;
            position: relative;
            z-index: 10;
        }}
        .dashboard-box {{
            max-width: 700px;
            margin: 0 auto;
            background: #111111;
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 16px;
            padding: 3rem;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.7);
        }}
        .dashboard-header {{
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 2rem;
            padding-bottom: 1.5rem;
            border-bottom: 1px solid rgba(255, 255, 255, 0.1);
        }}
        .welcome-text {{
            font-size: 1.8rem;
            font-weight: 700;
            color: #FFFFFF;
        }}
        .username-highlight {{
            color: #2DD881;
        }}
        .btn-logout {{
            background: transparent;
            color: #B0B0B0;
            border: 1px solid rgba(255, 255, 255, 0.1);
            padding: 0. 5rem 1rem;
            border-radius: 8px;
            cursor: pointer;
            transition: all 0.3s ease;
            text-decoration: none;
        }}
        .btn-logout:hover {{
            background: rgba(255, 69, 58, 0.1);
            border-color: rgba(255, 69, 58, 0. 3);
            color: #FF453A;
        }}
        .tasks-section {{
            margin-bottom: 2rem;
        }}
        .section-subtitle {{
            font-size: 1.2rem;
            color: #B0B0B0;
            margin-bottom: 1. 5rem;
        }}
        .task-item {{
            display: flex;
            align-items: center;
            gap: 1rem;
            padding: 1rem;
            background: #0A0A0A;
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 8px;
            margin-bottom: 0.75rem;
            transition: all 0.3s ease;
        }}
        . task-item:hover {{
            border-color: rgba(45, 216, 129, 0.3);
            background: rgba(45, 216, 129, 0.05);
        }}
        .task-form {{
            display: flex;
        }}
        .task-checkbox {{
            width: 20px;
            height: 20px;
            cursor: pointer;
            accent-color: #2DD881;
        }}
        .task-title {{
            flex: 1;
            color: #FFFFFF;
        }}
        .task-done . task-title {{
            text-decoration: line-through;
            color: #666666;
        }}
        .btn-delete {{
            background: transparent;
            border: none;
            font-size: 1.2rem;
            cursor: pointer;
            opacity: 0.5;
            transition: all 0.3s ease;
        }}
        .btn-delete:hover {{
            opacity: 1;
            transform: scale(1.2);
        }}
        .no-tasks {{
            text-align: center;
            color: #B0B0B0;
            padding: 3rem;
            font-style: italic;
        }}
        .add-task-form {{
            display: flex;
            gap: 0.75rem;
            margin-top: 1.5rem;
        }}
        .task-input {{
            flex: 1;
            padding: 0.9rem;
            background: #0A0A0A;
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 8px;
            color: #FFFFFF;
            font-size: 1rem;
        }}
        .task-input:focus {{
            outline: none;
            border-color: #2DD881;
            box-shadow: 0 0 0 3px rgba(45, 216, 129, 0.1);
        }}
        .btn-add {{
            background: #2DD881;
            color: #000000;
            border: none;
            padding: 0.9rem 1.5rem;
            border-radius: 8px;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s ease;
        }}
        .btn-add:hover {{
            background: #5FE9A0;
            transform: translateY(-2px);
            box-shadow: 0 0 20px rgba(45, 216, 129, 0.4);
        }}
    </style>
</head>
<body>
    <div class="particles">
        <span></span>
        <span></span>
        <span></span>
    </div>
    <div class="glow glow-1"></div>
    <div class="glow glow-2"></div>

    <nav class="navbar">
        <div class="container">
            <div class="logo">
                <span class="logo-icon">🌳</span>
                <span class="logo-text">NiceLife</span>
            </div>
            <div class="nav-links">
                <a href="/logout.py" class="btn-nav">Se déconnecter</a>
            </div>
        </div>
    </nav>

    <div class="dashboard-container">
        <div class="container">
            <div class="dashboard-box">
                <div class="dashboard-header">
                    <h1 class="welcome-text">
                        Bonjour <span class="username-highlight">{username}</span> ! 🌳
                    </h1>
                </div>

                <div class="tasks-section">
                    <h2 class="section-subtitle">Vos tâches du jour</h2>
                    {tasks_html}
                </div>

                <form method="POST" action="/add-task.py" class="add-task-form">
                    <input type="hidden" name="username" value="{username}">
                    <input
                        type="text"
                        name="title"
                        class="task-input"
                        placeholder="Nouvelle tâche..."
                        required
                        autofocus
                    >
                    <button type="submit" class="btn-add">Ajouter</button>
                </form>
            </div>
        </div>
    </div>
</body>
</html>"""

print(html)
