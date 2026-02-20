#!/usr/bin/env python3

import os
import sys
from http.cookies import SimpleCookie


try:
    with open("/tmp/dashboard_debug. log", "w") as log:
        log.write("=== DASHBOARD STARTED ===\n")
        log.write(f"REQUEST_METHOD: {os.environ.get('REQUEST_METHOD', 'NONE')}\n")
        log.write(f"HTTP_COOKIE: {os.environ.get('HTTP_COOKIE', 'NONE')}\n")
        log.write(f"SCRIPT_NAME: {os.environ. get('SCRIPT_NAME', 'NONE')}\n")

        # Lire cookie
        cookie_string = os.environ.get("HTTP_COOKIE", "")
        log.write(f"Cookie string: [{cookie_string}]\n")

        cookie = SimpleCookie()
        cookie.load(cookie_string)

        log.write(f"Cookie parsed: {cookie}\n")

        username = ""
        if "username" in cookie:
            username = cookie["username"].value
            log.write(f"Username found: [{username}]\n")
        else:
            log.write("No username in cookie\n")

        log.write("=== GENERATING HTML ===\n")
except Exception as e:
    username = ""



print("Content-Type: text/html; charset=utf-8")
print()


cookie_string = os.environ.get("HTTP_COOKIE", "")
cookie = SimpleCookie()
cookie.load(cookie_string)

username = ""
if "username" in cookie:
    username = cookie["username"].value


if username:

    print(f"""<! DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Dashboard - {username}</title>
    <style>
        body {{
            background: #000;
            color:  #fff;
            font-family: Arial;
            padding: 50px;
            text-align: center;
        }}
        .box {{
            background: #1a1a1a;
            border: 2px solid #2DD881;
            padding: 40px;
            border-radius: 10px;
            max-width: 600px;
            margin: 0 auto;
        }}
        h1 {{
            color:  #2DD881;
            font-size: 2.5rem;
        }}
        .username {{
            font-size: 2rem;
            color: #5FE9A0;
            margin:  20px 0;
        }}
        .info {{
            background: rgba(45, 216, 129, 0.1);
            padding: 20px;
            border-radius: 5px;
            margin: 20px 0;
        }}
        a {{
            display: inline-block;
            background: #ff4444;
            color: #fff;
            padding: 15px 40px;
            text-decoration:  none;
            border-radius:  5px;
            margin:  10px;
            font-weight: bold;
        }}
        a:hover {{
            background: #ff6666;
        }}
        .debug {{
            margin-top: 30px;
            padding: 20px;
            background: #0a0a0a;
            border-radius: 5px;
            font-family: monospace;
            font-size: 0.9rem;
            text-align: left;
        }}
    </style>
</head>
<body>
    <div class="box">
        <h1>✅ Dashboard</h1>
        <div class="username">Bienvenue {username} !</div>

        <div class="info">
            <p>🍪 Cookie actif</p>
            <p>✓ Session valide</p>
            <p>✓ Requête GET traitée</p>
        </div>

        <a href="/">🏠 Accueil</a>
        <a href="/logout.py">🚪 Déconnexion</a>

        <div class="debug">
            <strong>Debug Info:</strong><br>
            HTTP_COOKIE = {cookie_string}<br>
            username = {username}<br>
            REQUEST_METHOD = {os.environ.get('REQUEST_METHOD', 'N/A')}<br>
            <br>
            Logs:  /tmp/dashboard_debug.log
        </div>
    </div>
</body>
</html>""")

else:

    print("""<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Non connecté</title>
    <style>
        body {
            background: #1a0000;
            color: #fff;
            font-family: Arial;
            padding: 50px;
            text-align: center;
        }
        . box {
            background: rgba(255, 69, 58, 0.1);
            border: 2px solid #ff4444;
            padding: 40px;
            border-radius: 10px;
            max-width: 500px;
            margin: 0 auto;
        }
        h1 {
            color:  #ff4444;
            font-size: 3rem;
        }
        a {
            display: inline-block;
            background: #2DD881;
            color: #000;
            padding: 15px 40px;
            text-decoration: none;
            border-radius: 5px;
            margin-top: 20px;
            font-weight: bold;
        }
        a:hover {
            background: #5FE9A0;
        }
        .debug {
            margin-top: 30px;
            padding: 20px;
            background: rgba(0, 0, 0, 0.5);
            border-radius: 5px;
            font-family: monospace;
            font-size: 0.9rem;
        }
    </style>
</head>
<body>
    <div class="box">
        <h1>🔒</h1>
        <h2>Non connecté</h2>
        <p>Aucun cookie trouvé</p>
        <p>Vous devez vous connecter d'abord</p>

        <a href="/login.py">Se connecter →</a>

        <div class="debug">
            <strong>Debug: </strong><br>
            HTTP_COOKIE = """ + cookie_string + """<br>
            Cookie vide ou invalide<br>
            <br>
            Logs: /tmp/dashboard_debug.log
        </div>
    </div>
</body>
</html>""")

try:
    with open("/tmp/dashboard_debug.log", "a") as log:
        log. write("=== HTML SENT ===\n")
        log.write(f"Username in response: [{username}]\n")
except:
    pass
