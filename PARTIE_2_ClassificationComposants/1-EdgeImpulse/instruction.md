#               Etapes réalisées sur Edge Impulse

## Acquisition de données

Pour constituer la base de données destinée au traitement d’images sur Edge Impulse, j’ai procédé de la manière suivante :
- J’ai préparé un dossier principal contenant cinq sous‑dossiers, chacun correspondant à une classe et regroupant les images associées.

- Les images ont été enregistrées au format PNG, un format pris en charge nativement par Edge Impulse.

- Dans l’interface Edge Impulse, j’ai utilisé la fonction Upload data. Pour    chaque classe, j’ai sélectionné le sous‑dossier correspondant, laissé l’option de répartition automatique entre training et testing, puis renseigné le label approprié (ou laissé Edge Impulse l’inférer à partir du nom des fichiers).

## Architecture Pipeline

Pour la définition de l’architecture du pipeline, j’ai configuré les éléments suivants :

- **Bloc de prétraitement :** j’ai sélectionné le bloc Image, adapté au traitement d’images et permettant le redimensionnement ainsi que la normalisation des données visuelles.

- **Bloc d’apprentissage automatique :** j’ai choisi le bloc Classification, qui convient à la reconnaissance de catégories d’images.

- **Extraction de caractéristiques :** j’ai opté pour une profondeur de couleur en niveau de gris (grayscale). Ce choix permet d’éviter que le modèle s’appuie sur la couleur du composant pour déterminer sa classe, et l’oblige à se concentrer uniquement sur la forme, la texture et la structure visuelle.

## Entraînement du modèle

- **Pour la phase de d'entrainement**, j’ai conservé l’ensemble des paramètres par défaut proposés par Edge Impulse. La seule modification apportée concerne le nombre d’epochs (cycles d’entraînement), que j’ai fixé à 30 afin de permettre au modèle d’apprendre de manière plus approfondie tout en évitant un temps d’entraînement excessif.