#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import json
import cgi
from pathlib import Path

# Récupérer les données du formulaire
form = cgi. FieldStorage()
username = form.getvalue("username", ""). strip()
title = form.getvalue("title", "").strip()

if username and title:
    # Chemin du fichier
    data_dir = Path("data")
    data_dir.mkdir(exist_ok=True)
    tasks_file = data_dir / f"{username}_tasks.json"

    # Charger les tâches existantes
    if tasks_file.exists():
        with open(tasks_file, "r", encoding="utf-8") as f:
            tasks = json.load(f)
    else:
        tasks = []

    # Trouver le prochain ID
    next_id = max([task["id"] for task in tasks], default=0) + 1

    # Ajouter la nouvelle tâche
    new_task = {
        "id": next_id,
        "title": title,
        "done": False
    }
    tasks.append(new_task)

    # Sauvegarder
    with open(tasks_file, "w", encoding="utf-8") as f:
        json. dump(tasks, f, indent=4, ensure_ascii=False)

# Rediriger vers le dashboard
print("Status: 303 See Other")
print("Location: /dashboard.py")
print()
