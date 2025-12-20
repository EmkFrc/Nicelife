#!/usr/bin/env python3

import os
from http.cookies import SimpleCookie

# Récupérer la méthode HTTP
request_method = os.environ.get("REQUEST_METHOD", "GET")

# Supprimer le cookie
cookie = SimpleCookie()
cookie["username"] = ""
cookie["username"]["path"] = "/"
cookie["username"]["max-age"] = 0

if request_method == "DELETE":
    # Réponse pour requête DELETE (AJAX)
    print("Content-Type: application/json; charset=utf-8")
    print(cookie.output())
    print()
    print('{"status": "success", "message": "Cookie deleted"}')
else:
    # Réponse pour requête GET (lien direct)
    print("Content-Type: text/html; charset=utf-8")
    print(cookie.output())
    print()

    html = """<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta http-equiv="refresh" content="2;url=/">
    <title>Déconnexion</title>
    <style>
        body {
            background: #000;
            color: #fff;
            font-family: Arial;
            text-align: center;
            padding: 100px;
        }
        .box {
            background: rgba(255, 69, 58, 0.2);
            border: 2px solid #ff4444;
            padding: 40px;
            border-radius: 10px;
            display: inline-block;
        }
        .spinner {
            border: 4px solid rgba(255, 255, 255, 0.3);
            border-top: 4px solid #fff;
            border-radius: 50%;
            width: 40px;
            height: 40px;
            animation: spin 1s linear infinite;
            margin: 20px auto;
        }
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
    </style>
</head>
<body>
    <div class="box">
        <h1>👋 Déconnexion</h1>
        <p>Cookie supprimé avec succès</p>
        <div class="spinner"></div>
        <p>Redirection vers l'accueil dans 2 secondes...</p>
    </div>
</body>
</html>"""

    print(html)
