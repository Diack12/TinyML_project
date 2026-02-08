# PARTIE 2 : Classification de Composants Électroniques avec TinyML

## Vue d'ensemble

Cette partie du projet **TinyML** démontre une application pratique de l'intelligence artificielle embarquée pour la classification automatique de composants électroniques. Le système utilise une **caméra OV7675** connectée à un **Arduino Nano 33 BLE** pour capturer des images en temps réel, un modèle de machine learning entraîné sur **Edge Impulse** pour la classification, et un **dashboard Node-RED** pour la visualisation des résultats.

### Objectifs pédagogiques
- Comprendre l'intégration de capteurs de vision dans les systèmes embarqués
- Maîtriser le workflow Edge Impulse pour le développement de modèles TinyML
- Implémenter une interface utilisateur temps réel avec Node-RED
- Optimiser les performances pour les contraintes matérielles des microcontrôleurs

### Composants classifiés
Le système reconnaît automatiquement :
- **LED** (diode électroluminescente)
- **Résistance** (résistor)
- **Bouton poussoir** (push button)
- **Fond/Arrière-plan** (background)

---

## Architecture du système

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Caméra        │    │   Arduino Nano   │    │   Node-RED      │
│   OV7675        │───▶│   33 BLE         │───▶│   Dashboard     │
│                 │    │   + Edge Impulse │    │                 │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                              │
                              ▼
                       ┌──────────────────┐
                       │   Edge Impulse   │
                       │   Studio         │
                       │   (Entraînement) │
                       └──────────────────┘
```

### Flux de données
1. **Acquisition** : La caméra capture des images 160x120 pixels
2. **Prétraitement** : Conversion en niveaux de gris et redimensionnement
3. **Inférence** : Classification par le modèle TinyML embarqué
4. **Transmission** : Envoi des résultats via port série
5. **Visualisation** : Affichage temps réel sur le dashboard Node-RED

---

## Structure du projet

```
PARTIE_2_ClassificationComposants/
├── 1-EdgeImpulse/              # Configuration Edge Impulse
│   ├── instruction.md          # Guide de création du projet
│   ├── link_to_edge_impulse.md # Lien vers le projet Edge Impulse
│   └── images/                 # Captures d'écran
├── 2-ArduinoCamera/            # Code Arduino avec caméra
│   └── arduino_camera_classification/
│       └── arduino_camera_classification.ino
|   └──  ei-composants-classification-arduino-1.0.6.zip   #Librairie Edge Impulse
├── 3-NodeRED/                  # Dashboard de visualisation
│   ├── images/                 # Captures d'ecran dashboard
│   ├── dashboard.md            # Configuration du dashboard
│   └── flows.json              # Définition du flow Node-RED
└── doc/                        # Documentation technique
    └── documentation.md        # Guide complet détaillé
```

---

## Prérequis matériels

### Matériel requis
- **Arduino Nano 33 BLE** (avec caméra OV7675)
- **Ordinateur** avec port USB

### Logiciels requis
- **Arduino IDE** (version 1.8.19 ou supérieure)
- **Node-RED** (avec module dashboard)
- **Navigateur web** (Chrome/Firefox recommandé)
- **Edge Impulse CLI** ( pour déploiement avancé)

---

## Guide d'installation et d'utilisation

### Étape 1 : Configuration Edge Impulse
1. Suivre les instructions dans [`1-EdgeImpulse/instruction.md`](1-EdgeImpulse/instruction.md)
2. Créer un compte Edge Impulse si nécessaire
3. Importer le projet via le lien fourni
4. Collecter des données d'entraînement pour chaque classe de composant

### Étape 2 : Programmation Arduino
1. Installer les librairies Edge Impulse (voir [`doc/documentation.md`](doc/documentation.md))
2. Ouvrir le sketch [`2-ArduinoCamera/arduino_camera_classification/arduino_camera_classification.ino`](2-ArduinoCamera/arduino_camera_classification/arduino_camera_classification.ino)
3. Téléverser sur l'Arduino Nano 33 BLE
4. Vérifier la connexion série (baudrate 115200)

### Étape 3 : Configuration Node-RED
1. Installer Node-RED et le module dashboard
2. Importer le flow depuis [`3-NodeRED/flows.json`](3-NodeRED/flows.json)
3. Configurer le port série dans le nœud "Serial In"
4. Accéder au dashboard via `http://localhost:1880/ui`

### Étape 4 : Test du système
1. Placer un composant devant la caméra
2. Observer la classification en temps réel sur le dashboard
3. Vérifier les compteurs et la probabilité de détection

---

## Performances attendues

### Métriques du modèle
- **Précision** : > 90% sur les classes cibles
- **Latence d'inférence** : < 200ms sur Arduino Nano 33 BLE
- **Taille du modèle** : ~50-100KB (optimisé avec EON Compiler)

### Contraintes techniques
- **Résolution d'image** : 160x120 pixels (QVGA)
- **Format couleur** : Niveaux de gris (conversion automatique)
- **Fréquence d'acquisition** : 5-10 FPS
- **Communication** : Port série à 115200 bauds

---

## Dépannage

### Problèmes courants
- **Caméra non détectée** : Vérifier le câblage (pins SDA/SCL)
- **Modèle non chargé** : Régénérer la librairie Edge Impulse
- **Dashboard vide** : Vérifier le port série dans Node-RED
- **Erreurs de compilation** : Mettre à jour Arduino IDE et les librairies

### Ressources supplémentaires
- [Documentation Edge Impulse](https://docs.edgeimpulse.com/)
- [Guide Arduino Nano 33 BLE](https://www.arduino.cc/en/Guide/NANO33BLE)
- [Tutoriels Node-RED](https://nodered.org/docs/)

---

## Extensions possibles

### Améliorations suggérées
- Ajout de classes supplémentaires (condensateurs, transistors)
- Implémentation de la détection en temps réel avec feedback audio
- Intégration avec MQTT pour communication cloud
- Optimisation énergétique pour fonctionnement sur batterie

### Applications pratiques
- Tri automatique de composants électroniques
- Contrôle qualité en ligne de production
- Systèmes éducatifs pour l'apprentissage de l'électronique
- Applications IoT industrielles

---

*Pour une documentation technique détaillée, consultez [`doc/documentation.md`](doc/documentation.md)*
