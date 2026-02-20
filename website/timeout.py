#!/usr/bin/env python3
import time
import sys

# Entêtes CGI (nécessaires pour la réponse HTTP)
print("Content-Type: text/html")
print()  # Ligne vide pour séparer les entêtes du corps

# Attendre très longtemps (par exemple 120 secondes = 2 minutes)
# Vous pouvez ajuster ce délai selon le timeout configuré dans votre serveur
time.sleep(120)

# Ce message ne sera probablement jamais envoyé si le timeout est déclenché
print("<html>")
print("<head><title>Réponse tardive</title></head>")
print("<body>")
print("<h1>Si vous voyez ce message, le timeout n'a pas fonctionné!</h1>")
print("<p>Le script a attendu 120 secondes avant de répondre.</p>")
print("</body>")
print("</html>")

sys.stdout.flush()
