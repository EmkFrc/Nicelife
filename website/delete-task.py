#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import json
import cgi
from pathlib import Path

# Récupérer les données
form = cgi.FieldStorage()
username = form.getvalue("username", "").strip()
task_id = form.getvalue("task_id", ""). strip()

if username and task_id:
    task_id = int(task_id)

    # Charger les tâches
    data_dir = Path("data")
    tasks_file = data_dir / f"{username}_tasks.json"

    if tasks_file.exists():
        with open(tasks_file, "r", encoding="utf-8") as f:
            tasks = json.load(f)

        # Supprimer la tâche
        tasks = [task for task in tasks if task["id"] != task_id]

        # Sauvegarder
        with open(tasks_file, "w", encoding="utf-8") as f:
            json.dump(tasks, f, indent=4, ensure_ascii=False)

# Rediriger
print("Status: 303 See Other")
print("Location: /dashboard.py")
print()
