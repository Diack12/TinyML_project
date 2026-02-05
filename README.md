# TinyML Project : Intelligence Artificielle Embarquée

## Présentation générale du projet

Ce projet explore les applications pratiques de l'**Intelligence Artificielle Embarquée (TinyML)** sur microcontrôleurs Arduino. Il démontre comment déployer des modèles de machine learning optimisés sur des appareils à ressources limitées pour des tâches de classification en temps réel.

Le projet est structuré en deux parties complémentaires qui illustrent différents aspects du TinyML :
- **Classification de vibrations** (PARTIE 1)
- **Classification de composants électroniques** (PARTIE 2)

---

## Objectifs pédagogiques

### Comprendre les fondamentaux du TinyML
- **Optimisation des modèles** : Réduction de la taille et de la complexité pour les microcontrôleurs
- **Acquisition de données** : Collecte et prétraitement de données depuis capteurs
- **Déploiement embarqué** : Intégration de modèles ML sur Arduino
- **Interfaces utilisateur** : Visualisation temps réel des résultats

### Maîtriser les technologies clés
- **Arduino Nano 33 BLE** : Plateforme de développement embarquée
- **Edge Impulse** : Plateforme de développement TinyML
- **Node-RED** : Outil de programmation visuelle pour IoT
- **Capteurs variés** : Accéléromètre, caméra, communication série

---

## Architecture technique

### PARTIE 1 : Classification de Vibrations
```
    IMU──▶ Arduino ──▶ Notebook TensorFlow ──▶ Modèle TFLite ──▶ Classification
     │             │               │                      │              │
Accéléro-gyro    Acquisition   Entraînement          Conversion         Secouer
                 temps réel    Keras/TensorFlow      .tflite         No_vibration 
                                                                      Telephone
```

**Technologies utilisées :**
- Capteur IMU intégré (Arduino Nano 33 BLE)
- Collecte de données par script Python
- Entraînement dans un notebook Jupyter avec Keras/TensorFlow
- Conversion en TensorFlow Lite pour déploiement embarqué

### PARTIE 2 : Classification de Composants Électroniques
```
Caméra OV7675 ──▶ Arduino ──▶ Edge Impulse ──▶ Modèle TinyML ──▶ Node-RED Dashboard
      │                │             │                    │              │
   Acquisition      Prétraitement   Entraînement        Classification   Visualisation
   d'images         (64x64px)     du modèle           LED/Résistor/   temps reél
                                                       Bouton/Fond
```

**Technologies utilisées :**
- Caméra OV7675 (résolution QVGA)
- Prétraitement d'images embarqué
- Interface utilisateur avec Node-RED
- Communication série pour les résultats

---

## Structure du projet

```
TinyML_project/
├── README.md                           # Présentation générale (ce fichier)
├── PARTIE_1_ClassificationVibrations/  # Classification de vibrations
│   ├── README.md                       # Guide détaillé partie 1
|   ├── doc/                            # Documentation technique partie 1
│   ├── 1-DataGeneration_Arduino/       # Collecte de données
│   ├── 2-Training/                     # Entraînement du modèle
│   └── 3-Inference_Arduino/            # Déploiement embarqué
├── PARTIE_2_ClassificationComposants/  # Classification de composants
│   ├── README.md                       # Guide détaillé partie 2
|   ├── doc/                            # Documentation technique partie 2
│   ├── 1-EdgeImpulse/                  # Configuration Edge Impulse
│   ├── 2-ArduinoCamera/                # Code Arduino avec caméra
│   └── 3-NodeRED/                      # Dashboard de visualisation

```

---

## Prérequis matériels et logiciels

### Matériel requis
- **Arduino Nano 33 BLE** (1 ou 2 unités selon les parties)
- **Capteurs additionnels** :
  - Caméra OV7675 (pour partie 2)
- **Ordinateur** avec port USB

### Logiciels requis
- **Arduino IDE** (≥ 1.8.19)
- **Python 3.x** (avec TensorFlow/Keras pour l'entraînement)
- **Jupyter Notebook** (pour l'entraînement du modèle Partie 1)
- **Node-RED** (pour visualisation Partie 2)
- **Navigateur web** (Chrome/Firefox)
- **Edge Impulse** (compte gratuit requis pour Partie 2 uniquement)

---

## Démarrage rapide

### Pour débuter avec la classification de vibrations (PARTIE 1)
1. Consulter [`PARTIE_1_ClassificationVibrations/README.md`](PARTIE_1_ClassificationVibrations/README.md)
2. Générer des données d'entraînement avec Arduino
3. Entraîner le modèle dans le notebook Jupyter avec TensorFlow
4. Convertir en TFLite et déployer sur Arduino pour inférence temps réel

### Pour débuter avec la classification de composants (PARTIE 2)
1. Consulter [`PARTIE_2_ClassificationComposants/README.md`](PARTIE_2_ClassificationComposants/README.md)
2. Configurer le projet Edge Impulse
3. Connecter la caméra OV7675 à l'Arduino
4. Programmer l'Arduino et lancer le dashboard Node-RED

---

## Performances et contraintes

### Métriques typiques
- **Précision de classification** : 85-95% selon la qualité des données
- **Latence d'inférence** : 50-500ms (selon complexité du modèle)
- **Consommation énergétique** : Compatible avec batteries LiPo
- **Taille des modèles** : 10-200KB (optimisés pour microcontrôleurs)

### Limitations techniques
- **Puissance de calcul** : Cortex-M4F @ 64MHz, 256KB RAM
- **Mémoire flash** : 1MB disponible pour le programme + modèle
- **Précision numérique** : Int8/Int16 pour optimiser les performances
- **Interfaces** : Port série, I2C, SPI disponibles

---

## Applications et extensions

### Cas d'usage industriels
- **Maintenance prédictive** : Détection de vibrations anormales
- **Contrôle qualité** : Inspection visuelle automatique
- **Tri de composants** : Classification en ligne de production
- **Surveillance environnementale** : Détection d'événements

### Possibilités d'extension
- **Multi-capteurs** : Fusion de données IMU + audio
- **Edge computing** : Traitement distribué sur plusieurs nœuds
- **Communication cloud** : Envoi de données vers serveurs distants
- **Apprentissage continu** : Mise à jour des modèles en conditions réelles

---

## Ressources et documentation

### Documentation par partie
- [PARTIE 1 : Vibrations](PARTIE_1_ClassificationVibrations/README.md)
- [PARTIE 2 : Composants](PARTIE_2_ClassificationComposants/README.md)

### Liens utiles
- [Edge Impulse Documentation](https://docs.edgeimpulse.com/)
- [Arduino Nano 33 BLE Guide](https://www.arduino.cc/en/Guide/NANO33BLE)
- [Node-RED Getting Started](https://nodered.org/docs/getting-started/)
- [TinyML Book](https://tinymlbook.com/) (ressource théorique)

### Support et dépannage
- Vérifier les connexions matérielles
- Consulter les logs série Arduino
- Tester les exemples Edge Impulse
- Utiliser les forums Arduino/Node-RED

---

## Licence et contribution

Ce projet est développé dans un cadre éducatif pour l'enseignement de l'IA embarquée. Les contributions sont les bienvenues sous forme de pull requests ou d'issues sur le dépôt Git.

**Technologies utilisées :** Arduino, Edge Impulse, TensorFlow Lite, Node-RED, Python

---

*Projet réalisé dans le cadre du cours "IA Embarquée" - Université Sorbonne Paris Nord - SUP GALILEE*
