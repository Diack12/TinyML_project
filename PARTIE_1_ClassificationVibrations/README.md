# PARTIE 1 : Classification de Vibrations

## Présentation générale

Cette partie du projet implémente un système de classification de vibrations en temps réel sur Arduino Nano 33 BLE Sense utilisant TensorFlow Lite.

Le système détecte et classe les vibrations en trois catégories :
- **no_vibration** : Aucune vibration détectée
- **secouer_vibration** : Vibration produite en secouant physiquement la carte Arduino
- **telephone_vibration** : Vibration provenant d'un téléphone mobile (signal généralement plus faible)

## Fonctionnement

Le projet suit un pipeline en trois étapes :

### 1. Collecte de données (Étape 1-DataGeneration_Arduino)
- Arduino Nano 33 BLE Sense collecte les données brutes de l'IMU (accéléromètre + gyroscope)
- Capture 50 échantillons par vibration à une fréquence d'échantillonnage de 50 Hz
- Les données sont transmises via connexion série et sauvegardées en fichiers CSV

### 2. Entraînement du modèle (Étape 2-Training)
- Notebook Python Jupyter traite les données CSV
- Entraîne un classificateur de réseau de neurones Keras
- Convertit le modèle entraîné au format TensorFlow Lite
- Génère un fichier header C++ pour intégration dans Arduino

### 3. Inférence en temps réel (Étape 3-Inference_Arduino)
- Le sketch Arduino charge le modèle TFLite
- Capture les nouveaux échantillons de vibration en temps réel
- Classifie les vibrations en utilisant le modèle embarqué
- Affiche les résultats de classification avec scores de confiance

## Structure du projet

```
PARTIE_1_ClassificationVibrations/
├── 1-DataGeneration_Arduino/        # Sketch Arduino & lecteur série Python
├── 2-Training/                      # Notebook Jupyter & modèles entraînés
│   ├── dataset/                     # Fichiers CSV pour données d'entraînement
│   ├── models/                      # Sortie du modèle TFLite
│   └── notebook/                    # Script d'entraînement & paramètres de normalisation
├── 3-Inference_Arduino/             # Sketch Arduino d'inférence
└── doc/                             # Documentation détaillée du projet
```

## Démarrage rapide

1. **Collecter les données d'entraînement** : Exécutez le sketch Arduino pour générer les échantillons de vibration
2. **Entraîner le modèle** : Exécutez le notebook Jupyter pour entraîner et convertir
3. **Déployer sur Arduino** : Chargez le sketch d'inférence avec le modèle généré
