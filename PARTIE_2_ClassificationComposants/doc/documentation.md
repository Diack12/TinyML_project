# Documentation Complète - Classification de Composants avec Edge Impulse et Arduino

## Table des matières
1. [Câblage de la caméra OV7675](#câblage-de-la-caméra-ov7675)
2. [Installation des librairies Edge Impulse](#installation-des-librairies-edge-impulse)
3. [Configuration Node-RED](#configuration-node-red)
4. [Justification des choix techniques](#justification-des-choix-techniques)

---

## Câblage de la caméra OV7675

### Composants nécessaires
- **Arduino Nano 33 BLE**
- **Caméra OV7675** (capteur d'image)
- Fils de connexion (Dupont)
- Breadboard (optionnel)

### Schéma de câblage

La caméra OV7675 communique avec l'Arduino Nano 33 BLE via l'interface **SCCB** (Serial Camera Control Bus), une variante du protocole I²C.

#### Pins de connexion

| Caméra OV7675 | Arduino Nano 33 BLE | Description |
|---------------|-------------------|-------------|
| **GND** | GND | Masse |
| **3V3** | 3V3 | Alimentation 3.3V |
| **SDA** | A4 (SDA) | Données I²C (contrôle) |
| **SCL** | A5 (SCL) | Horloge I²C (contrôle) |
| **VSYNC** | D2 | Synchronisation verticale (interruption) |
| **HREF** | D3 | Synchronisation horizontale |
| **PCLK** | D4 | Horloge pixel |
| **D0-D7** | D5-D12 | Bus de données 8 bits |

### Connexions détaillées

```
┌─────────────────────────────────────────────────────────────┐
│                      OV7675 Camera                          │
├─────────────────────────────────────────────────────────────┤
│ Pin 1 (GND)      ──────────────→ Arduino GND               │
│ Pin 2 (3V3)      ──────────────→ Arduino 3V3               │
│ Pin 3 (SDA)      ──────────────→ Arduino A4 (SDA)         │
│ Pin 4 (SCL)      ──────────────→ Arduino A5 (SCL)         │
│ Pin 5 (VSYNC)    ──────────────→ Arduino D2                │
│ Pin 6 (HREF)     ──────────────→ Arduino D3                │
│ Pin 7 (PCLK)     ──────────────→ Arduino D4                │
│ Pin 8-15 (D0-D7) ──────────────→ Arduino D5-D12           │
└─────────────────────────────────────────────────────────────┘
```

### Notes importantes
- **Alimentation** : La caméra nécessite une alimentation stable en 3.3V (max 100mA)
- **Résistances pull-up** : Les lignes SDA/SCL possèdent généralement des pull-up internes sur l'Arduino
- **Timing critique** : Les signaux VSYNC, HREF et PCLK doivent être connectés pour une synchronisation correcte
- **Longueur des câbles** : Garder les connexions courtes (< 10 cm) pour éviter les interférences

---

## Installation des librairies Edge Impulse

### Étape 1 : Téléchargement depuis Edge Impulse

#### Option A : Téléchargement manuel
1. Accéder au projet Edge Impulse
   > Le lien du projet est disponible dans [`../1-EdgeImpulse/link_to_edge_impulse.md`](../1-EdgeImpulse/link_to_edge_impulse.md)
2. Aller à **Deployment** → **Arduino Library**
3. Sélectionner **Arduino Nano 33 BLE Sense**
4. Cocher **Enable EON Compiler** (pour optimiser le modèle)
5. Cliquer **Build** → Télécharger le fichier `.zip`

#### Option B : Utiliser le fichier fourni dans le projet (recommandé)
Le fichier ZIP de la librairie Edge Impulse est fourni dans le dossier du projet :
```
arduino_camera_classification/
  └── [librairie_edge_impulse].zip
```
Vous pouvez directement utiliser ce fichier pour installer la librairie.

### Étape 2 : Installation dans Arduino IDE

1. Ouvrir **Arduino IDE**
2. Aller à **Sketch** → **Include Library** → **Add .ZIP Library...**
3. Sélectionner le fichier `.zip` :
   - Si vous utilisez le fichier fourni : naviguez vers `arduino_camera_classification/[librairie].zip`
   - Si vous avez téléchargé manuellement : sélectionnez le fichier téléchargé
4. Cliquer **Open** → L'IDE ajoute automatiquement la librairie
5. Attendre le message de confirmation "Library installed"

### Étape 3 : Vérification de l'installation

Vérifier que la librairie est disponible dans **Sketch** → **Include Library** (elle doit apparaître dans la liste).

### Étape 4 : Important - Résoudre le conflit de classe OV7675

⚠️ **IMPORTANT** : Vous devez corriger ce conflit **SEULEMENT si vous utilisez l'exemple de la librairie Edge Impulse**.

**Si vous utilisez le fichier fourni `arduino_camera_classification.ino` du projet, ce problème est déjà résolu et vous pouvez ignorer cette section.**

#### Problème

Un conflit de nom existe entre la classe OV7675 de la librairie Edge Impulse et l'enum OV7675 de la librairie Arduino_OV767X.

#### Solution : Renommer la classe OV7675 en OV7675_

Appliquez cette solution uniquement si vous utilisez l'exemple de la librairie Edge Impulse :

1. Ouvrir Arduino IDE
2. Aller à **File** → **Examples** → **composants_classification_inferencing** → **nano_ble33_sense** → **nano_ble33_sense_camera**
   > (Remplacer "composants_classification_inferencing" par le nom de votre librairie Edge Impulse)

3. Dans le sketch ouvert, utiliser **Ctrl+H** (Find and Replace) ou **Ctrl+F** (Find) pour chercher tous les emplacements

4. Chercher la déclaration de la classe OV7675 :
   ```cpp
   // Avant
   class OV7675 : public ov767x {
   
   // Après
   class OV7675_ : public ov767x {
   ```

5. Remplacer toutes les occurrences de `OV7675` par `OV7675_`, y compris :
   ```cpp
   // Exemple de variables
   OV7675 Cam;         →  OV7675_ Cam;
   ```
   > Utiliser **Replace All** pour remplacer automatiquement toutes les occurrences

6. Compiler et télécharger le sketch modifié

Voir la section **[Problème rencontré et résolution](#problème-rencontré-et-résolution)** pour plus de détails.

---

## Utilisation du sketch Arduino fourni

### Solution recommandée

Le fichier **`arduino_camera_classification.ino`** fourni dans le dossier `arduino_camera_classification/` contient déjà :
- ✅ La correction du conflit OV7675 (renommée en OV7675_)
- ✅ L'intégration complète du modèle Edge Impulse
- ✅ La configuration optimale de la caméra OV7675
- ✅ L'envoi des résultats via port série (compatible Node-RED)

### Installation et compilation

1. Ouvrir Arduino IDE
2. **File** → **Open** → Naviguer vers `arduino_camera_classification/arduino_camera_classification.ino`
3. Sélectionner la carte : **Tools** → **Board** → **Arduino Nano 33 BLE**
4. Sélectionner le port : **Tools** → **Port** → COM X (votre Arduino)
5. Cliquer **Upload** (→ Flèche) pour compiler et télécharger

### Format de sortie

Le sketch envoie les résultats au port série en JSON :
```
EI: {"winner": "Led", "prob": 0.95, "Led": 0.95, "Resistor": 0.02, "PushButton": 0.02, "Background": 0.01}
```

Ce format est directement compatible avec le flow Node-RED fourni.

### Description du problème

Lors de l'intégration de la librairie Edge Impulse téléchargée, une **collision de classe** s'est produite :

- **Source du conflit** : La classe `OV7675` définie dans l'exemple Edge Impulse hériterait de `ov767x` (de la librairie Arduino_OV767X)
- **Le problème** : La librairie Arduino_OV767X définit un **enum du même nom `OV7675`**, causant un conflit de nom
- **Symptôme** : Erreur de compilation indiquant une redéfinition de symbole

### Solution appliquée

**Renommer la classe `OV7675` en `OV7675_`** dans le fichier d'en-tête de la librairie Edge Impulse.

#### Étapes de résolution

1. Accéder au répertoire de la librairie Edge Impulse :
   ```
   Documents/Arduino/libraries/[votre_librairie]/src/
   ```

2. Ouvrir le fichier d'inference .ino

3. Remplacer :
   ```cpp
   class OV7675 : public ov767x {
   ```
   par :
   ```cpp
   class OV7675_ : public ov767x {
   ```

4. Mettre à jour toutes les références à la classe dans le fichier:
   ```cpp
   // Avant
   OV7675 camera;
   
   // Après
   OV7675_ camera;
   ```

5. Recompiler l'exemple

---

## Choix technique : Résolution des images 64×64

### Problème initial

- **Modèle initial** : Images d'entraînement en **96×96 pixels**
- **Conséquence** : Le modèle compilé était **trop volumineux** pour tenir en mémoire sur l'Arduino Nano 33 BLE
- **Erreur** : Le modèle ne se chargeait pas, causant l'échec du déploiement

### Solution

Réduire la taille des images à **64×64 pixels** pour :

#### Avantages

| Aspect | 96×96 | 64×64 | Réduction |
|--------|-------|-------|-----------|
| **Taille en RAM** | ~27 KB (x3 canaux) | ~12 KB (x3 canaux) | -56% |
| **Taille en mémoire** | Trop volumineux | ✓ Compatible | - |
| **Précision** | Potentiellement meilleure | Acceptable pour TinyML | Compromis |

#### Impact sur le modèle

- Le modèle 64×64 **se charge et s'exécute correctement** sur l'Arduino
- La précision reste **satisfaisante** (probabilités > 95% pour la plupart des prédictions)
- Les caractéristiques des composants sont **toujours bien visibles** à cette résolution

#### Justification technique

Pour TinyML sur systèmes embarqués :
- **Limite de mémoire** : L'Arduino Nano 33 BLE dispose de **256 KB de RAM**
- **Besoins du système** : Stack, buffers série, variables globales consomment de l'espace
- **Capacité utile** : ~150-180 KB disponibles pour le modèle et les buffers d'inférence
- **64×64 optimale** : Taille idéale pour un bon compromis entre performance et consommation mémoire

---

## Configuration Node-RED

### Prérequis

- **Node-RED** installé sur votre machine
- **Arduino Nano 33 BLE** connectée en USB
- **Librairie Arduino_OV767X** installée dans Arduino IDE
- **Edge Impulse model** téléchargée et compilée

### Installation de Node-RED

```bash
# Installation globale (recommandée)
npm install -g node-red

# Lancer Node-RED
node-red
```

Accès à l'interface : `http://localhost:1880`

### Installation du Dashboard

1. Dans Node-RED, cliquer sur le menu ☰ (haut-droit)
2. Sélectionner **Manage palette**
3. Onglet **Install**
4. Rechercher `node-red-dashboard`
5. Cliquer **Install**
6. Redémarrer Node-RED

### Importation du Flow

Le flow complet est disponible dans le fichier [`flows.json`](../3-NodeRED/flows.json).

#### Méthode 1 : Import via l'interface

1. Ouvrir Node-RED : `http://localhost:1880`
2. Menu ☰ → **Import**
3. Sélectionner **Clipboard**
4. Copier-coller le contenu de `flows.json`
5. Cliquer **Import**
6. Cliquer **Deploy** (bouton rouge haut-droit)

#### Méthode 2 : Remplacement direct

1. Arrêter Node-RED
2. Localisez le fichier `flows.json` dans le répertoire Node-RED :
   - **Linux/Mac** : `~/.node-red/flows.json`
   - **Windows** : `%USERPROFILE%\.node-red\flows.json`
3. Remplacer le fichier par celui du projet
4. Redémarrer Node-RED

### Configuration du port série

1. Dans Node-RED, double-cliquer sur le nœud **Serial In**
2. Configurer les paramètres :
   - **Port** : Sélectionner le port de l'Arduino (ex : COM7)
   - **Baud rate** : **115200**
   - **Data bits** : 8
   - **Stop bits** : 1
   - **Parity** : None
3. Cliquer **Done**
4. Cliquer **Deploy**

### Lancer le Dashboard

Une fois le flow déployé :

- **Éditeur Node-RED** : `http://localhost:1880`
- **Dashboard UI** : `http://localhost:1880/ui`

Le dashboard affiche en temps réel :
- Compteurs de détection pour chaque classe (Led, Resistor, PushButton, Background)
- Classe détectée ("Winner")
- Probabilité de prédiction (jauge 0-1)
- Bouton "Reset" pour réinitialiser les compteurs

### Dépannage Node-RED

| Problème | Solution |
|----------|----------|
| Port série non disponible | Vérifier la connexion USB et installer le driver Arduino |
| Dashboard vide | Vérifier que `node-red-dashboard` est installé |
| Compteurs ne s'incrémentent pas | Vérifier le format JSON reçu du port série |
| "Permission denied" sur le port | Relancer Node-RED ou redémarrer l'Arduino |

---

## Flux complet du système

```
┌──────────────────┐
│  Arduino Nano    │
│  33 BLE Sense    │
├──────────────────┤
│ OV7675 Camera    │  Capture 64×64
│ Edge Impulse     │  Inférence
│ Model (EON)      │  Format JSON
└────────┬─────────┘
         │ Serial 115200 baud
         │ Format: "EI: {...}"
         ↓
┌──────────────────────┐
│    Node-RED          │
├──────────────────────┤
│ Serial In → Filter   │  Parse JSON
│ → Function 1         │  Compteurs
│ → UI Widgets         │  Affichage
└────────┬─────────────┘
         │
         ↓
┌──────────────────────┐
│   Web Browser        │
├──────────────────────┤
│ Dashboard UI         │
│ http://localhost:88 │
│ 1880/ui             │
└──────────────────────┘
```

---

## Résumé

Ce projet intègre avec succès :
- ✅ Capteur d'image OV7675 sur Arduino Nano 33 BLE
- ✅ Modèle Edge Impulse optimisé (64×64, EON Compiler)
- ✅ Inférence embarquée sans latence réseau
- ✅ Visualisation temps réel via Node-RED Dashboard
- ✅ Résolution des conflits de librairie lors de l'intégration
