#!/usr/bin/env python3

# Max-Age=0 supprime le cookie
print("Set-Cookie: session_id=; Path=/; Max-Age=0")
print("Set-Cookie: username=; Path=/; Max-Age=0")
print("Content-Type: text/html")
print()
print("<html>")
print("<head><title>Logout</title></head>")
print("<body>")
print("<h1>👋 Déconnexion réussie</h1>")
print("<p>Les cookies ont été supprimés.</p>")
print("<p><a href='/cgi-bin/login.py'>Se reconnecter</a></p>")
print("</body>")
print("</html>")
