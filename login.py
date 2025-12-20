#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import cgi
import os
from http.cookies import SimpleCookie

# Récupérer les données du formulaire
form = cgi. FieldStorage()
username = form.getvalue("username", ""). strip()

# Initialiser les variables
error_message = ""
cookie_header = ""

# Si un username est envoyé (soumission du formulaire)
if username:
    if len(username) >= 3:
        # Créer le cookie
        cookie = SimpleCookie()
        cookie["nicelife_user"] = username
        cookie["nicelife_user"]["path"] = "/"
        cookie["nicelife_user"]["max-age"] = 86400  # 24h
        cookie_header = cookie. output()

        # Redirection vers dashboard
        print("Status: 303 See Other")
        print(cookie_header)
        print("Location: /dashboard.py")
        print()
        exit()
    else:
        error_message = "⚠️ Le pseudo doit contenir au moins 3 caractères"

# Afficher le formulaire HTML
print("Content-Type: text/html; charset=utf-8")
print()

html = f"""<! DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Connexion - NiceLife 🌳</title>
    <link rel="stylesheet" href="/nicelife-style.css">
    <style>
        . login-container {{
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 2rem;
        }}
        .login-box {{
            background: #111111;
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 16px;
            padding: 3rem;
            max-width: 400px;
            width: 100%;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.7);
            position: relative;
            z-index: 10;
        }}
        .login-title {{
            text-align: center;
            font-size: 2rem;
            margin-bottom: 1rem;
            color: #FFFFFF;
        }}
        . login-subtitle {{
            text-align: center;
            color: #B0B0B0;
            margin-bottom: 2rem;
        }}
        .form-group {{
            margin-bottom: 1.5rem;
        }}
        .form-label {{
            display: block;
            margin-bottom: 0. 5rem;
            color: #B0B0B0;
            font-size: 0.9rem;
        }}
        .form-input {{
            width: 100%;
            padding: 0.9rem;
            background: #0A0A0A;
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 8px;
            color: #FFFFFF;
            font-size: 1rem;
            transition: all 0.3s ease;
        }}
        .form-input:focus {{
            outline: none;
            border-color: #2DD881;
            box-shadow: 0 0 0 3px rgba(45, 216, 129, 0.1);
        }}
        .btn-submit {{
            width: 100%;
            padding: 1rem;
            background: #2DD881;
            color: #000000;
            border: none;
            border-radius: 8px;
            font-weight: 600;
            font-size: 1. 1rem;
            cursor: pointer;
            transition: all 0.3s ease;
        }}
        .btn-submit:hover {{
            background: #5FE9A0;
            transform: translateY(-2px);
            box-shadow: 0 0 20px rgba(45, 216, 129, 0.4);
        }}
        .error-message {{
            background: rgba(255, 69, 58, 0.1);
            border: 1px solid rgba(255, 69, 58, 0.3);
            color: #FF453A;
            padding: 1rem;
            border-radius: 8px;
            margin-bottom: 1. 5rem;
            text-align: center;
        }}
        .back-link {{
            display: block;
            text-align: center;
            margin-top: 1. 5rem;
            color: #B0B0B0;
            transition: color 0.3s ease;
        }}
        .back-link:hover {{
            color: #2DD881;
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

    <div class="login-container">
        <div class="login-box">
            <h1 class="login-title">🌳 NiceLife</h1>
            <p class="login-subtitle">Connectez-vous pour continuer</p>

            {"<div class='error-message'>" + error_message + "</div>" if error_message else ""}

            <form method="POST" action="/login.py">
                <div class="form-group">
                    <label class="form-label" for="username">Votre pseudo</label>
                    <input
                        type="text"
                        id="username"
                        name="username"
                        class="form-input"
                        placeholder="Entrez votre pseudo"
                        value="{username}"
                        required
                        autofocus
                    >
                </div>

                <button type="submit" class="btn-submit">
                    Se connecter →
                </button>
            </form>

            <a href="/" class="back-link">← Retour à l'accueil</a>
        </div>
    </div>
</body>
</html>"""

print(html)
