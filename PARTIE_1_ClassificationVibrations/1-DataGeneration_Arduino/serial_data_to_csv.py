"""
============================================================================
CAPTURE DE DONNÉES SÉRIE VERS FICHIER CSV
============================================================================

Ce script Python lit les données envoyées par l'Arduino via le port série
et les sauvegarde dans un fichier CSV pour l'entraînement du modèle.

FONCTIONNEMENT :
1. Se connecte au port série de l'Arduino
2. Lit les données au format CSV (aX,aY,aZ,gX,gY,gZ)
3. Sauvegarde les données dans un fichier CSV
4. Compte automatiquement les vibrations capturées (50 échantillons = 1 vibration)

UTILISATION :
1. Assurez-vous que l'Arduino est connecté et que le sketch generate_data_to_train.ino
   est téléversé et fonctionne
2. Modifiez SERIAL_PORT avec le port COM de votre Arduino (ex: 'COM3', 'COM7')
3. Exécutez le script : python serial_data_to_csv.py
4. Effectuez vos vibrations  - le script capturera automatiquement,  pause de 3 secondes entre chaque vibration
5. Appuyez sur Ctrl+C pour arrêter la capture
6. Renommez le fichier output.csv selon la vibration (<vibration_name>.csv) et placez-le dans
   le dossier PARTIE_1_ClassificationVibrations/Training/dataset pour l'entraînement

PRÉREQUIS :
- Python 3.x installé
- Bibliothèque pyserial installée : pip install pyserial
- Arduino connecté et sketch generate_data_to_train.ino téléversé

AUTEUR : Script original modifié pour TinyML
"""

# ============================================================================
# IMPORTS
# ============================================================================

import serial  # Bibliothèque pour la communication série avec l'Arduino
import csv     # Bibliothèque pour la manipulation des fichiers CSV
import time    # Bibliothèque pour les pauses et temporisations

# ============================================================================
# CONFIGURATION
# ============================================================================

# Port série de l'Arduino
# Sur Windows : 'COM3', 'COM7', etc. (vérifiez dans le Gestionnaire de périphériques)
# Sur Linux/Mac : '/dev/ttyUSB0', '/dev/ttyACM0', etc.
# IMPORTANT : Modifiez cette valeur selon votre système !
SERIAL_PORT = 'COM7'  # ⚠️ À MODIFIER selon votre port série

# Vitesse de communication série (baud rate)
# DOIT correspondre à la vitesse configurée dans le sketch Arduino (Serial.begin)
BAUD_RATE = 9600

# Nombre d'échantillons par vibration
# 100 échantillons = environ 2 seconde à 50 Hz
# Cette valeur DOIT correspondre à numSamples dans data_generation.ino
numSamples = 100

# Compteur du nombre de vibrations capturées
# S'incrémente automatiquement chaque fois qu'une vibration complète est capturée
Number_of_vibrations = 0

# ============================================================================
# CAPTURE DES DONNÉES
# ============================================================================

# Ouvrir le port série avec gestion automatique de la fermeture (context manager)
# timeout=1 : attendre maximum 1 seconde pour lire une ligne
# Si aucune donnée n'arrive en 1 seconde, readline() retourne une chaîne vide
with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
    # Ouvrir le fichier CSV en mode écriture
    # 'w' = write mode (écrase le fichier s'il existe)
    # newline='' = nécessaire pour éviter les lignes vides supplémentaires sous Windows
    with open('output.csv', 'w', newline='') as csvfile:
        # Créer un writer CSV pour écrire les données formatées
        writer = csv.writer(csvfile)

        # Écrire l'en-tête du fichier CSV
        # Cette ligne définit les colonnes : accélération X,Y,Z et gyroscope X,Y,Z
        writer.writerow(['aX', 'aY', 'aZ', 'gX', 'gY', 'gZ'])

        # Message de démarrage
        print("Starting data collection... Press Ctrl+C to stop.")
        print(f"Listening on port {SERIAL_PORT} at {BAUD_RATE} baud...")
        print(f"Each gesture requires {numSamples} samples.")
        print()

        # Compteur d'échantillons collectés pour le geste en cours
        # S'incrémente à chaque ligne valide lue
        samples_collected = 0
        t=time.time()
        while time.time()-t<3:
                print(f"  Starting in {3 - int(time.time()-t)} seconds...", end='\r')
                time.sleep(1)

        try:
            # Boucle infinie pour lire continuellement les données
            # Le programme s'arrête uniquement avec Ctrl+C (KeyboardInterrupt)
            while True:
                # Lire une ligne depuis le port série
                # readline() : lit jusqu'à un caractère de nouvelle ligne (\n)
                # decode('utf-8') : convertit les bytes en chaîne de caractères
                # strip() : supprime les espaces et caractères de fin de ligne
                line = ser.readline().decode('utf-8').strip()

                # ====================================================================
                # TRAITEMENT DES LIGNES REÇUES
                # ====================================================================
                
                # Vérifier si la ligne n'est pas vide et n'est pas l'en-tête
                # L'en-tête est envoyé par l'Arduino au démarrage, on l'ignore
                if line and not line.startswith('aX,aY,aZ,gX,gY,gZ'):
                    # Séparer la ligne en valeurs individuelles
                    # split(',') : divise la chaîne à chaque virgule
                    # Résultat : ['aX', 'aY', 'aZ', 'gX', 'gY', 'gZ']
                    values = line.split(',')

                    # Vérifier qu'on a bien 6 valeurs (sécurité)
                    # Format attendu : aX,aY,aZ,gX,gY,gZ
                    if len(values) == 6:
                        # Écrire la ligne dans le fichier CSV
                        # writer.writerow() formate automatiquement les valeurs
                        writer.writerow(values)

                        # Incrémenter le compteur d'échantillons
                        samples_collected += 1

                        # ============================================================
                        # GESTE COMPLET DÉTECTÉ
                        # ============================================================
                        # Si on a collecté le nombre d'échantillons requis
                        if samples_collected == numSamples:
                            # Afficher un message de confirmation
                            print(f"✓ {numSamples} samples have been saved.")
                            print("  Data collection complete for one vibration.")
                            
                            # Réinitialiser le compteur pour le prochain geste
                            samples_collected = 0

                            # Incrémenter le compteur de vibrations
                            Number_of_vibrations += 1
                            
                            # Afficher le nombre total de vibrations capturées
                            print(f"  Total vibrations captured: {Number_of_vibrations}")
                            print()
                            print("  Ready for next vibration in 3 seconds... CTRL+C to stop.")
                            print()
                            # Pause de 3 secondes avant de capturer le prochain geste
                            t=time.time()
                            while time.time()-t<3:
                                print(f"  Starting in {3 - int(time.time()-t)} seconds...", end='\r')
                                time.sleep(1)
                            

                    else:
                        # Format de ligne incorrect (pas 6 valeurs)
                        # Afficher un avertissement mais continuer
                        print(f"⚠ Warning: Unexpected line format (expected 6 values, got {len(values)}): {line}")

                # Si la ligne est l'en-tête, l'ignorer silencieusement
                elif line.startswith('aX,aY,aZ,gX,gY,gZ'):
                    # L'en-tête est envoyé par l'Arduino au démarrage
                    # On l'ignore car on l'a déjà écrit dans le fichier CSV
                    pass  # Ne rien faire

        # ========================================================================
        # GESTION DE L'ARRÊT MANUEL
        # ========================================================================
        # KeyboardInterrupt est déclenché quand l'utilisateur appuie sur Ctrl+C
        except KeyboardInterrupt:
            print()
            print("=" * 60)
            print("Data collection stopped by user.")
            print(f"Total vibrations captured: {Number_of_vibrations}")
            print(f"Total samples saved: {samples_collected + (Number_of_vibrations * numSamples)}")
            print("=" * 60)

# ============================================================================
# MESSAGE FINAL
# ============================================================================
# Ce message s'affiche après la fermeture du port série et du fichier CSV
print()
print("=" * 60)
print("Data collection is complete.")
print(f"The data has been saved to 'output.csv'.")
print()
print("NEXT STEPS:")
print("1. Rename 'output.csv' to match your vibration (e.g., '<vibration_name>.csv')")
print("2. Place the CSV file in the 'PARTIE_1_ClassificationVibrations/Training/dataset' directory")
print("3. Use it in the notebook for training your model")
print("=" * 60)

"""
============================================================================
NOTES IMPORTANTES
============================================================================

1. PORT SÉRIE :
   - Sur Windows, trouvez le port COM dans le Gestionnaire de périphériques
   - Le port peut changer si vous débranchez/rebranchez l'Arduino
   - Si vous obtenez une erreur "Access Denied", fermez le moniteur série Arduino

2. FORMAT DES DONNÉES :
   - Format attendu : aX,aY,aZ,gX,gY,gZ (6 valeurs séparées par des virgules)
   - Les valeurs sont des nombres décimaux (float)
   - Chaque ligne = 1 échantillon

3. CAPTURE DE PLUSIEURS VIBRATIONS :
   - Le script capture automatiquement plusieurs vibrations
   - Chaque vibration = 100 échantillons
   - Les vibrations sont enregistrées consécutivement dans le même fichier
   - Pour séparer les vibrations, utilisez le script Python du notebook qui divise
     les données en enregistrements individuels

4. UTILISATION AVEC LE NOTEBOOK :
   - Le fichier CSV généré peut contenir plusieurs vibrations
   - Le notebook Python divise automatiquement les données en enregistrements
   - Chaque enregistrement de 100 échantillons = une vibration capturé

5. DÉBOGAGE :
   - Si aucune donnée n'arrive : vérifiez le port série et la vitesse
   - Si des lignes sont mal formatées : vérifiez le sketch Arduino
   - Si le fichier est vide : vérifiez que l'Arduino envoie bien des données
"""

