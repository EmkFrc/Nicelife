#!/bin/bash

BASE="http://localhost:6969"
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

echo "======================================"
echo "Tests DELETE - Validation complète"
echo "======================================"

# ← FONCTION MANQUANTE, À REMETTRE ICI
function test_case() {
    local name="$1"
    local expected="$2"
    shift 2

    echo ""
    echo "Test: $name"
    echo "Attendu: $expected"

    result=$(curl -s -o /dev/null -w "%{http_code}" --max-time 5 "$@")

    if [ "$result" = "$expected" ]; then
        echo "✅ PASS ($result)"
    else
        echo "❌ FAIL (got $result, expected $expected)"
    fi
}

# CGI
test_case "CGI logout" "200" -X DELETE -H "Cookie: username=alice" "$BASE/logout.py"

# Routing
test_case "Bon fichier (501)" "501" -X DELETE -H "Cookie: username=alice" "$BASE/uploads/alice_test1.jpg"
test_case "Sans /uploads/ (404)" "404" -X DELETE -H "Cookie: username=alice" "$BASE/files/alice_photo.jpg"

# Path traversal
test_case "Path traversal . ." "404" -X DELETE -H "Cookie: username=alice" "$BASE/uploads/../../../etc/passwd"
test_case "Path traversal //" "403" -X DELETE -H "Cookie: username=alice" "$BASE/uploads/alice//photo.jpg"

# Extensions
test_case "Extension .jpg (OK)" "501" -X DELETE -H "Cookie: username=alice" "$BASE/uploads/alice_photo3.jpg"
test_case "Extension .png (OK)" "501" -X DELETE -H "Cookie: username=alice" "$BASE/uploads/alice_image.png"
test_case "Extension .json (NON)" "403" -X DELETE -H "Cookie: username=alice" "$BASE/uploads/alice_data.json"
test_case "Extension .py (NON)" "403" -X DELETE -H "Cookie: username=alice" "$BASE/uploads/alice_script.py"
test_case "Sans extension" "403" -X DELETE -H "Cookie: username=alice" "$BASE/uploads/alice_fichier"

# Propriété
test_case "Bon owner" "501" -X DELETE -H "Cookie: username=alice" "$BASE/uploads/alice_photo4.jpg"
test_case "Mauvais owner" "403" -X DELETE -H "Cookie: username=alice" "$BASE/uploads/bob_secret.pdf"
test_case "Username similaire" "403" -X DELETE -H "Cookie: username=alice" "$BASE/uploads/alicia_photo.jpg"
test_case "Sans préfixe" "403" -X DELETE -H "Cookie: username=alice" "$BASE/uploads/photo.jpg"

# Authentification
test_case "Sans cookie" "403" -X DELETE "$BASE/uploads/alice_photo7.jpg"
test_case "Cookie vide" "403" -X DELETE -H "Cookie: username=" "$BASE/uploads/alice_photo8.jpg"
test_case "Cookie multiple" "501" -X DELETE -H "Cookie: session=xyz; username=alice; theme=dark" "$BASE/uploads/alice_photo9.jpg"

echo ""
echo "======================================"
echo "Tests terminés"
echo "======================================"
