/*
  ============================================================================
  CLASSIFICATION DE GESTES AVEC TENSORFLOW LITE
  ============================================================================
  
  Ce programme utilise l'IMU intégré de l'Arduino Nano 33 BLE Sense pour :
  1. Détecter un mouvement significatif (seuil d'accélération)
  2. Capturer 100 échantillons de données (accélération + gyroscope)
  3. Normaliser les données (comme pendant l'entraînement)
  4. Utiliser un modèle TensorFlow Lite pour classifier la vibration
  5. Afficher les probabilités pour chaque vibration
  
  FONCTIONNEMENT :
  - Le modèle a été entraîné dans le notebook Jupyter
  - Il a été converti en TensorFlow Lite et intégré dans model.h
  - Le programme utilise ce modèle pour prédire quelle vibration a été effectué
  
  MATÉRIEL REQUIS :
  - Arduino Nano 33 BLE ou Arduino Nano 33 BLE Sense
  - Bibliothèque TensorFlow Lite for Microcontrollers installée
  
  Par Faye Papa Djidiack
  
  Code dans le domaine public.
*/

// ============================================================================
// INCLUSIONS
// ============================================================================

// Bibliothèque pour l'IMU (accéléromètre + gyroscope)
#include <Arduino_LSM9DS1.h>

// Bibliothèques TensorFlow Lite for Microcontrollers
#include <TensorFlowLite.h>
#include <tensorflow/lite/micro/all_ops_resolver.h>  // Résolveur pour toutes les opérations
#include <tensorflow/lite/micro/micro_error_reporter.h>  // Gestionnaire d'erreurs
#include <tensorflow/lite/micro/micro_interpreter.h>  // Interpréteur pour exécuter le modèle
#include <tensorflow/lite/schema/schema_generated.h>  // Schéma du modèle
#include <tensorflow/lite/version.h>  // Version de TensorFlow Lite

// Fichier header contenant le modèle entraîné
// Ce fichier a été généré par le notebook Jupyter
#include "model.h"

// ============================================================================
// CONSTANTES DE CONFIGURATION
// ============================================================================
 float mean_[]={1.13949365e+00 ,2.21857781e-01, 1.38953988e+02, 3.74931971e+01,7.02365349e+02, 4.54910670e+05};

 float std_[]={3.01818009e-01, 4.20867090e-01, 8.21934847e+01, 5.60657371e+01, 1.23179281e+03, 8.25262541e+05};
// Nombre d'échantillons à capturer par Vibration
// 100échantillons = environ 2 secondes à 50 Hz
// Cette valeur DOIT correspondre à SAMPLES_PER_VIBRATION dans le notebook Python
const int numSamples = 100;

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

// Compteur d'échantillons lus depuis la derniere Vibration détecté
// Initialisé à 0 au debut 
int samplesRead = 0;

// ============================================================================
// VARIABLES TENSORFLOW LITE
// ============================================================================

// Gestionnaire d'erreurs pour TensorFlow Lite
// Utilisé pour rapporter les erreurs pendant l'exécution du modèle
tflite::MicroErrorReporter tflErrorReporter;

// Résolveur d'opérations TensorFlow Lite
// Contient toutes les opérations disponibles (add, mul, relu, softmax, etc.)
// Note : On pourrait n'inclure que les opérations nécessaires pour réduire
//        la taille du code compilé
tflite::AllOpsResolver tflOpsResolver;

// Pointeur vers le modèle TensorFlow Lite
// Le modèle est chargé depuis model.h (tableau de bytes)
const tflite::Model* tflModel = nullptr;

// Interpréteur TensorFlow Lite
// C'est l'objet qui exécute le modèle sur les données d'entrée
tflite::MicroInterpreter* tflInterpreter = nullptr;

// Tenseur d'entrée du modèle
// Contient les données normalisées (100 échantillons × 6 valeurs = 600 valeurs)
TfLiteTensor* tflInputTensor = nullptr;

// Tenseur de sortie du modèle
// Contient les probabilités pour chaque geste (3 valeurs : [no_vibration, secouer_vibration, telephone_vibration])
TfLiteTensor* tflOutputTensor = nullptr;

// Tampon mémoire statique pour TensorFlow Lite
// Le modèle a besoin de mémoire pour stocker les valeurs intermédiaires
// 8 KB = 8192 bytes (peut nécessiter un ajustement selon le modèle)
constexpr int tensorArenaSize = 8 * 1024;

// Tableau de bytes aligné sur 16 bytes pour optimiser l'accès mémoire
// L'alignement est important pour les performances sur microcontrôleurs
byte tensorArena[tensorArenaSize] __attribute__((aligned(16)));

// ============================================================================
// CONFIGURATION DES GESTES
// ============================================================================

// Tableau des noms de gestes
// L'ordre DOIT correspondre à l'ordre utilisé pendant l'entraînement
// Index 0 = "no vibration" → correspond à [1, 0, 0] en one-hot encoding
// Index 1 = "secouer vibration" → correspond à [0, 1, 0] en one-hot encoding
// Index 2 = "telephone vibration" → correspond à  [0, 0, 1] en one-hot encoding
const char* VIBRATIONS[] = {
  "no vibration",  // pas de vibration
  "secouer vibration",    // secousse de haut en bas
  "telephone vibration" // vibration du telephone
};

// Calculer le nombre de gestes automatiquement
// sizeof(VIBRATIONS) = taille totale du tableau en bytes
// sizeof(VIBRATIONS[0]) = taille d'un élément (pointeur) en bytes
// Le résultat est le nombre d'éléments dans le tableau
#define NUM_GESTURES (sizeof(VIBRATIONS) / sizeof(VIBRATIONS[0]))

// ============================================================================
// FONCTION SETUP - EXÉCUTÉE UNE SEULE FOIS AU DÉMARRAGE
// ============================================================================
void setup() {
  // Initialiser la communication série à 9600 bauds
  Serial.begin(9600);
  
  // Attendre que le port série soit ouvert
  while (!Serial);

  // ========================================================================
  // INITIALISATION DE L'IMU
  // ========================================================================
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    // Si l'initialisation échoue, arrêter le programme
    while (1);
  }

  // Afficher les fréquences d'échantillonnage de l'IMU
  // Ces informations sont utiles pour comprendre la fréquence de capture
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  
  Serial.println();

  // ========================================================================
  // CHARGEMENT DU MODÈLE TENSORFLOW LITE
  // ========================================================================
  
  // Obtenir le modèle TensorFlow Lite depuis le tableau de bytes dans model.h
  // Le tableau 'model' est défini dans model.h (généré par le notebook)
  tflModel = tflite::GetModel(model);
  
  // Vérifier que la version du schéma correspond
  // Si la version ne correspond pas, le modèle pourrait ne pas fonctionner
  if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema mismatch!");
    Serial.print("Model version: ");
    Serial.println(tflModel->version());
    Serial.print("Required version: ");
    Serial.println(TFLITE_SCHEMA_VERSION);
    while (1);  // Arrêter le programme
  }

  // ========================================================================
  // CRÉATION DE L'INTERPRÉTEUR TENSORFLOW LITE
  // ========================================================================
  
  // Créer un interpréteur pour exécuter le modèle
  // Paramètres :
  //   - tflModel : le modèle à exécuter
  //   - tflOpsResolver : résolveur d'opérations (contient les fonctions mathématiques)
  //   - tensorArena : tampon mémoire pour les calculs intermédiaires
  //   - tensorArenaSize : taille du tampon mémoire
  //   - &tflErrorReporter : gestionnaire d'erreurs
  tflInterpreter = new tflite::MicroInterpreter(
    tflModel, 
    tflOpsResolver, 
    tensorArena, 
    tensorArenaSize, 
    &tflErrorReporter
  );

  // Allouer la mémoire pour les tenseurs d'entrée et de sortie
  // Cette étape prépare la mémoire nécessaire pour les calculs
  TfLiteStatus allocStatus = tflInterpreter->AllocateTensors();
if (allocStatus != kTfLiteOk) {
  Serial.println("AllocateTensors() failed!");
  while (1);
}

  // ========================================================================
  // OBTENIR LES POINTEURS VERS LES TENSEURS
  // ========================================================================
  
  // Obtenir le pointeur vers le tenseur d'entrée (index 0)
  // C'est ici qu'on mettra les données normalisées
  tflInputTensor = tflInterpreter->input(0);
  
  // Obtenir le pointeur vers le tenseur de sortie (index 0)
  // C'est ici qu'on récupérera les probabilités de chaque geste
  tflOutputTensor = tflInterpreter->output(0);
  
  // Afficher des informations sur les tenseurs (optionnel, pour debug)
  Serial.println("Model loaded successfully!");
  Serial.print("Input tensor size: ");
  Serial.println(tflInputTensor->bytes);
  Serial.print("Output tensor size: ");
  Serial.println(tflOutputTensor->bytes);
  Serial.println();
}

// ============================================================================
// FONCTION LOOP - EXÉCUTÉE EN BOUCLE CONTINUE
// ============================================================================
void loop() {
  // Variables pour stocker les valeurs brutes de l'IMU
  float aX, aY, aZ;  // Accélération sur les axes X, Y, Z (en G)
  float gX, gY, gZ;  // Vitesse angulaire (gyroscope) sur les axes X, Y, Z (en deg/s)
  float sum_a = 0.0,sum_a2 = 0.0,sum_g = 0.0,sum_g2 = 0.0;
  
  samplesRead=0;
  int t=3;
  while( t>0)
  {
    Serial.print("Capture de données dans:");
    Serial.print(t);
    Serial.println("secoondes");
    delay(1000);
    t--;
  }
  // ========================================================================
  // PHASE: CAPTURER ET TRAITER LES DONNÉES DE LA VIBRATION
  // ========================================================================
  // Cette boucle capture numSamples échantillons et les normalise
  while (samplesRead < numSamples) {
    // Vérifier si de nouvelles données sont disponibles pour les deux capteurs
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
      // Lire les données brutes de l'IMU
      IMU.readAcceleration(aX, aY, aZ);
      IMU.readGyroscope(gX, gY, gZ);
      
      // ====================================================================
      // NORMALISATION DES DONNÉES
      // ====================================================================
      // IMPORTANT : Cette normalisation DOIT être identique à celle utilisée
      // pendant l'entraînement dans le notebook Python !
      //
      // Formule de normalisation : (valeur - min) / (max - min)
      // 
      // Pour l'accélération :
      //   - Plage : -4G à +4G
      //   - Normalisation : (valeur + 4) / 8
      //   - Résultat : 0.0 à 1.0
      //
      // Pour le gyroscope :
      //   - Plage : -2000 à +2000 deg/s
      //   - Normalisation : (valeur + 2000) / 4000
      //   - Résultat : 0.0 à 1.0
      //
      // Format d'entrée du modèle :
      //   - 119 échantillons × 6 valeurs = 714 valeurs au total
      //   - Ordre : [aX, aY, aZ, gX, gY, gZ] pour chaque échantillon
      
      // Calculer l'index dans le tableau d'entrée
      // samplesRead * 6 = position de départ pour cet échantillon
      // + 0, 1, 2, 3, 4, 5 = position de chaque valeur dans l'échantillon
      
      //Calcul des magnitudes
      float a_mag = sqrt((aX*aX)+(aY*aY)+(aZ*aZ));
      float g_mag= sqrt((gX*gX)+(gY*gY)+(gZ*gZ));
      
      sum_a+=a_mag;
      sum_a2+=a_mag*a_mag;
      sum_g+=g_mag;
      sum_g2+=g_mag*g_mag;

      // Incrémenter le compteur d'échantillons
      samplesRead++;

      // ====================================================================
      // EXÉCUTER LE MODÈLE (INFÉRENCE)
      // ====================================================================
      // Si on a capturé tous les échantillons nécessaires
      if (samplesRead == numSamples) {
        // Exécuter le modèle TensorFlow Lite
        // Cette fonction prend les données d'entrée (tflInputTensor)
        // et calcule les probabilités de sortie (tflOutputTensor)
        float mean_a = sum_a / numSamples;
        float var_a  = sum_a2 / numSamples - mean_a*mean_a;
        float rms_a  = sqrt(sum_a2 / numSamples);
        float ene_a  = sum_a2;
        float mean_g = sum_g / numSamples;
        float var_g  = sum_g2 / numSamples - mean_g*mean_g;
        float rms_g  = sqrt(sum_g2 / numSamples);
        float ene_g  = sum_g2;

        tflInputTensor->data.f[ 0] = (rms_a - mean_[0])/std_[0];
        tflInputTensor->data.f[ 1] = (var_a - mean_[1])/std_[1];
        tflInputTensor->data.f[ 2] = (ene_a - mean_[2])/std_[2];
        tflInputTensor->data.f[ 3] = (rms_g - mean_[3])/std_[3];
        tflInputTensor->data.f[ 4] = (var_g - mean_[4])/std_[4];
        tflInputTensor->data.f[ 5] = (ene_g - mean_[5])/std_[5];
        TfLiteStatus invokeStatus = tflInterpreter->Invoke();
        
        
        // Vérifier si l'exécution a réussi
        if (invokeStatus != kTfLiteOk) {
          Serial.println("Invoke failed!");
          Serial.print("Status code: ");
          Serial.println(invokeStatus);
          while (1);  // Arrêter le programme en cas d'erreur
          return;
        }

        // ====================================================================
        // AFFICHER LES RÉSULTATS
        // ====================================================================
        // Le tenseur de sortie contient les probabilités pour chaque geste
        // Exemple : [0.95, 0.05] signifie 95% punch, 5% flex
        
        // Parcourir tous les gestes
        for (int i = 0; i < NUM_GESTURES; i++) {
          // Afficher le nom du geste
          Serial.print(VIBRATIONS[i]);
          Serial.print(": ");
          
          // Afficher la probabilité avec 6 décimales
          // tflOutputTensor->data.f[i] contient la probabilité du geste i
          Serial.println(tflOutputTensor->data.f[i], 6);
        }
        
        // Ligne vide pour séparer les résultats
        Serial.println();
        
        // Après l'affichage, samplesRead == numSamples, donc on retourne
        // à la phase 1 pour attendre le prochain geste
      }
    }
  }
}

/*
  ============================================================================
  NOTES IMPORTANTES
  ============================================================================
  
  1. NORMALISATION :
     - La normalisation DOIT être identique à celle utilisée pendant l'entraînement
     - Si vous changez la normalisation dans le notebook, changez-la aussi ici
     - Les plages (-4 à +4 pour accélération, -2000 à +2000 pour gyroscope)
       doivent correspondre aux plages réelles de votre capteur
  
  2. FORMAT DES DONNÉES :
     - Le modèle attend 6 valeurs (6 features tiré)
     - Ordre : [aX, aY, aZ, gX, gY, gZ] répété 100 fois
     - Toutes les valeurs doivent être normalisées entre 0.0 et 1.0
  
  3. INTERPRÉTATION DES RÉSULTATS :
     - Les valeurs de sortie sont des probabilités (entre 0.0 et 1.0)
     - La somme des probabilités = 1.0 (grâce à softmax)
     - Le geste avec la probabilité la plus élevée est la prédiction
  
  4. PERFORMANCE :
     - L'exécution du modèle prend quelques millisecondes
     - La taille du modèle est limitée par la mémoire disponible
     - Le tampon tensorArena peut nécessiter un ajustement selon le modèle
  
  5. DÉBOGAGE :
     - Si le modèle ne fonctionne pas, vérifiez :
       * Que model.h est bien inclus
       * Que la normalisation est correcte
       * Que le nombre d'échantillons correspond
       * Que les versions de TensorFlow Lite sont compatibles
*/
