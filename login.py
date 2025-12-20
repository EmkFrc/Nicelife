#!/usr/bin/env python3

import cgi
import os
from http.cookies import SimpleCookie

method = os.environ.get("REQUEST_METHOD", "GET")



if method == "POST":
    # Traiter le POST
    form = cgi.FieldStorage()
    username = form. getvalue("username", "").strip()

    print("Content-Type: text/html; charset=utf-8")

    if username and len(username) >= 3:
        # Créer cookie
        cookie = SimpleCookie()
        cookie["username"] = username
        cookie["username"]["path"] = "/"
        cookie["username"]["max-age"] = 3600

        print(cookie.output())
        print()

        print(f"""
<html>
<head>
    <meta charset="UTF-8">
    <title>Success</title>
    <style>
        body {{
            font-family: Arial;
            background: #000;
            color: #fff;
            padding: 50px;
            text-align:  center;
        }}
        . box {{
            background: #1a1a1a;
            border: 2px solid #2DD881;
            padding: 40px;
            border-radius:  10px;
            max-width: 500px;
            margin: 0 auto;
        }}
        a {{
            display: inline-block;
            background: #2DD881;
            color: #000;
            padding: 15px 40px;
            text-decoration:  none;
            border-radius:  5px;
            margin-top: 20px;
            font-weight: bold;
        }}
    </style>
</head>
<body>
    <div class="box">
        <h1>✅ Connecté !</h1>
        <p>Bienvenue <strong>{username}</strong></p>
        <p>Cookie créé avec succès</p>
        <a href="/dashboard.py">Dashboard →</a>
    </div>
</body>
</html>
        """)
    else:
        print()
        print("""
<html>
<body style="background:#000;color:#fff;text-align:center;padding:50px;">
    <h1>❌ Erreur</h1>
    <p>Username doit avoir 3+ caractères</p>
    <a href="/login.py" style="color:#2DD881;">Réessayer</a>
</body>
</html>
        """)
else:
    # Afficher formulaire
    print("Content-Type: text/html; charset=utf-8")
    print()

    print("""
<!  DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Login</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background:  #000;
            color: #fff;
            padding: 50px;
            margin: 0;
        }
        .container {
            max-width: 400px;
            margin: 0 auto;
            background: #1a1a1a;
            padding: 40px;
            border-radius:  10px;
            border: 2px solid #2DD881;
        }
        h1 {
            color: #2DD881;
            text-align: center;
            margin-bottom: 30px;
        }
        label {
            display: block;
            margin-bottom: 10px;
            color: #ccc;
        }
        input[type="text"] {
            width: 100%;
            padding: 12px;
            font-size: 16px;
            background: #0a0a0a;
            border: 1px solid #2DD881;
            border-radius: 5px;
            color: #fff;
            box-sizing: border-box;
        }
        input[type="text"]:focus {
            outline:  none;
            border-color:  #5FE9A0;
        }
        button {
            width: 100%;
            padding:  15px;
            margin-top: 20px;
            background: #2DD881;
            color: #000;
            border: none;
            border-radius: 5px;
            font-size: 18px;
            font-weight: bold;
            cursor: pointer;
        }
        button:hover {
            background: #5FE9A0;
        }
        .info {
            margin-top: 20px;
            padding: 15px;
            background: rgba(45, 216, 129, 0.1);
            border-radius: 5px;
            font-size: 14px;
            color: #999;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🌳 Login</h1>

        <form method="POST" action="/login.py">
            <label for="username">Username</label>
            <input
                type="text"
                id="username"
                name="username"
                placeholder="Votre username"
                required
                minlength="3"
                autofocus
            >

            <button type="submit">Se connecter</button>
        </form>

        <div class="info">
            💡 Entrez au moins 3 caractères
        </div>
    </div>
</body>
</html>
    """)
