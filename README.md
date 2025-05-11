# forum-messagerie-udp
Communication réseau avec les sockets UDP en C

# 🌟 Forum de Messagerie UDP

> _Un forum de messagerie léger utilisant le protocole UDP, permettant aux clients de publier et lire des messages._

---

## 🧭 Table des matières

- [📸 Démo](#-démo)
- [🚀 Fonctionnalités principales](#-fonctionnalités-principales)
- [📦 Technologies utilisées](#-technologies-utilisées)
- [Structure du projet](#-Structure du projet)
- [🛠️ Installation](#️-installation)
- [▶️ Utilisation](#️-utilisation)
- [🧪 Tests](#-tests)
- [📁 Structure du projet](#-structure-du-projet)
- [📄 Licence](#-licence)
- [👤 Auteur](#-auteur)
- [🙏 Remerciements](#-remerciements)

---

## 📸 Démo

![Demo](./screenshots/demo.gif)

---

## 🚀 Fonctionnalités principales

- 📝 Publication de messages avec commande `PUSH`
- 📜 Lecture de messages avec commande `PULL`
- 📦 Gestion des messages via une pile LIFO
- 🔄 Prise en charge de plusieurs clients

---

## 📦 Technologies utilisées

- **Langage principal** : C

---
## Structure du projet

Le projet est divisé en deux parties :
- **Serveur** : Gère l'écoute des clients, le stockage des messages et la réponse aux requêtes.
- **Client** : Permet à l'utilisateur de se connecter au serveur, d'envoyer des messages ou de lire les messages existants.

## 🛠️ Installation

```bash
# 1. Cloner le repo
git clone https://github.com/tonpseudo/nom-du-projet.git
cd nom-du-projet

# 2. Créer un environnement virtuel (optionnel mais recommandé)
python3 -m venv venv
source venv/bin/activate  # ou .\venv\Scripts\activate sous Windows

# 3. Installer les dépendances
pip install -r requirements.txt

