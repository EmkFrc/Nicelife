# WebServ

status code

prendre le site nicelife

y ajouter upload

gerer les infos de request / response

Je veux apprendre [NOTION/CONCEPT] pour mon projet [NOM DU PROJET/CONTEXTE].

CONTRAINTES IMPORTANTES :
- Ne me donne PAS de code complet, seulement des explications
- Guide-moi étape par étape
- À chaque étape, attends ma confirmation avant de passer à la suivante
- Pose-moi des questions de design/choix quand c'est pertinent
- Donne-moi des exemples concrets avec mon contexte
- Explique les pièges et erreurs courantes
- Propose-moi des tests pour valider chaque étape

STRUCTURE SOUHAITÉE :

1. ANALYSE DE L'EXISTANT
   - Comprendre ce qui existe déjà dans mon code
   - Identifier les points d'entrée
   - Tracer le flux actuel

2. PLAN GÉNÉRAL
   - Vue d'ensemble de l'architecture
   - Découpage en phases/blocs logiques
   - Diagramme ou schéma du flux

3. DÉCISIONS DE DESIGN
   - Me poser des questions sur les choix à faire
   - Expliquer les avantages/inconvénients de chaque option
   - M'aider à choisir selon mon contexte

4. PLAN DÉTAILLÉ PAR BLOC
   - Pour chaque bloc :
     * Objectif clair
     * Sous-étapes numérotées
     * Questions à me poser pour valider ma compréhension
     * Exemples de flux complet (cas nominal + cas d'erreur)
     * Pièges à éviter
     * Tests à faire

5. CHECKLIST FINALE
   - Liste de tous les points à implémenter
   - Critères de validation
   - Tests à effectuer

ATTENDS MA VALIDATION à chaque grande étape avant de continuer.

Commence par me poser des questions sur mon contexte si besoin.

////////////////////////////////////////////////////////////////////////////////////////////////////

Pour un concept plus théorique :
Code

Je veux comprendre [CONCEPT THÉORIQUE] et comment l'appliquer dans [CONTEXTE].

Au lieu de code, donne-moi :
- Des analogies concrètes
- Des schémas/diagrammes
- Des exemples de la vie réelle
- Les erreurs de compréhension courantes
- Comment vérifier que j'ai bien compris

Pose-moi des questions de validation à chaque étape.
////////////////////////////////////////////////////////////////////////////////////////////////////
Pour du débogage guidé :
Code

J'ai un problème avec [FONCTIONNALITÉ] dans mon projet [CONTEXTE].

Symptôme : [DESCRIPTION DU PROBLÈME]

Guide-moi pour diagnostiquer le problème :
1. Quelles sont les causes possibles ?
2. Pour chaque cause, comment la vérifier ?
3. Comment ajouter des logs de debug ?
4. Quels tests faire ?

Ne me donne pas la solution directement, aide-moi à la trouver.
////////////////////////////////////////////////////////////////////////////////////////////////////
Pour de l'optimisation :
Code

Je veux optimiser [ASPECT] de mon projet [CONTEXTE].

Actuellement :  [DESCRIPTION DE L'IMPLÉMENTATION ACTUELLE]

Guide-moi pour :
1. Identifier les goulots d'étranglement
2. Comprendre les différentes approches d'optimisation
3. Choisir la meilleure selon mes contraintes
4. Implémenter étape par étape
5. Mesurer les améliorations

Explique les trade-offs à chaque choix.
////////////////////////////////////////////////////////////////////////////////////////////////////
📚 PROMPT POUR CONCEPTS LARGES
Code

Je veux une ROADMAP complète pour implémenter [FONCTIONNALITÉ LARGE]
dans mon projet [CONTEXTE].

Découpe ça en phases progressives :
- Phase 1 : MVP minimal (juste pour tester)
- Phase 2 :  Fonctionnalité de base
- Phase 3 : Sécurité
- Phase 4 :  Gestion d'erreurs
- Phase 5 : Optimisation
- Phase 6 : Edge cases

Pour chaque phase :
- Objectif clair
- Ce qui doit fonctionner à la fin
- Tests de validation
- Questions de design

Attends ma validation entre chaque phase.
////////////////////////////////////////////////////////////////////////////////////////////////////
🎨 PERSONNALISATION

Tu peux ajouter des contraintes spécifiques :
Code

CONTRAINTES SUPPLÉMENTAIRES :
- Mon niveau :  [débutant/intermédiaire/avancé]
- Langage : [C++/Python/JavaScript...]
- Environnement : [Linux/Mac/Windows]
- Restrictions : [pas de bibliothèques externes/C++98/etc.]
- Deadline : [projet école/production/personnel]
- Focus : [plus sur la sécurité/performance/simplicité]
///////////////////////////////////////////////////////////////////////////////////////////////////
✅ PROMPT DE VALIDATION

Pour vérifier que tu as bien compris une étape :
Code

Je viens de terminer [ÉTAPE/BLOC].

Vérifie ma compréhension :
1. Pose-moi 3-5 questions sur ce que je viens d'implémenter
2. Donne-moi des cas de test à vérifier
3. Demande-moi d'expliquer avec mes mots comment ça fonctionne
4. Identifie les points que je n'ai peut-être pas compris

Ne passe à l'étape suivante que si mes réponses sont correctes.
///////////////////////////////////////////////////////////////////////////////////////////////////
🚀 EXEMPLE COMPLET D'UTILISATION
Code

Je veux apprendre la gestion du chunked transfer encoding pour mon serveur web C++.

CONTRAINTES IMPORTANTES :
- Ne me donne PAS de code complet, seulement des explications
- Guide-moi étape par étape
- À chaque étape, attends ma confirmation avant de passer à la suivante
- Pose-moi des questions de design/choix quand c'est pertinent
- Donne-moi des exemples concrets avec des requêtes HTTP réelles
- Explique les pièges et erreurs courantes
- Propose-moi des tests pour valider chaque étape

STRUCTURE SOUHAITÉE :

1. ANALYSE DE L'EXISTANT
   - Comment mon serveur gère actuellement les requêtes POST
   - Où est parsé le Content-Length
   - Comment est stocké le body

2. PLAN GÉNÉRAL
   - Qu'est-ce que le chunked encoding et pourquoi ça existe
   - Différence avec Content-Length
   - Vue d'ensemble du parsing

3. DÉCISIONS DE DESIGN
   - Est-ce que je dois supporter chunked pour GET/POST/DELETE ?
   - Comment gérer les gros fichiers en chunks ?
   - Où stocker les chunks pendant la réception ?

4. PLAN DÉTAILLÉ PAR BLOC
   - Détection du Transfer-Encoding:  chunked
   - Parsing d'un chunk (taille en hexa + data + CRLF)
   - Reconstruction du body complet
   - Gestion du chunk final (size = 0)
   - Gestion des trailers (optionnel)

5. CHECKLIST FINALE
   - Tests avec curl --data-binary
   - Tests avec de gros fichiers
   - Tests de chunks malformés

CONTEXTE ADDITIONNEL :
- Serveur en C++ avec poll()
- Parsing actuel dans Connection::recvData()
- Buffer :  std::string _read_buf

Attends ma validation à chaque grande étape avant de continuer.

