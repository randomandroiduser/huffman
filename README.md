
<div align="center">
  <br />
  <p>
    <img src="https://i.imgur.com/ssOAmsW.png" width="546" alt="Projet Huffman" />
  </p>
</div>

## A propos

![Capture d'écran](https://i.imgur.com/Y9PprA0.jpg)

Le projet Huffman consiste à compresser des fichiers texte de la table ASCII étendue en fichiers binaire, puis à les décompresser dans le sens inverse. Ceci est réalisé à l'aide de l'algorithme de Huffman implémenté dans le langage C.

## Pour commencer

Avant de pouvoir compresser et décompresser des fichiers, il est nécessaire de suivre ces instructions.

### Prérequis

* gcc (ou un autre compilateur en C)
* Une machine sous Linux (l'application a initialement été conçue pour fonctionner sous Linux)

### Mise en place lors de la première utilisation

1. Ouvrez un terminal et placez-vous dans le répertoire dans lequel se trouvent le Makefile et les fichiers .c et .h
2. Compilez à l'aide du Makefile
   ```sh
   make
   ```
3. (Optionnel) Supprimez les fichiers .o
   ```sh
   make clean
   ```

La compression et la décompression sont maintenant prêtes à être utilisées !

## Usage

### Pour obtenir de l'aide

   ```sh
   ./huffman -h
   ```

### Pour compresser un fichier texte

1. Munissez-vous du fichier texte que vous souhaitez compresser (dans notre exemple `fichierTxt`)
2.  Compressez `fichierTexte` et récupérez la sortie dans un fichier binaire (dans notre exemple `fichierBin.bin`)
   ```sh
   ./huffman -c fichierTexte fichierBin.bin
   ```

### Pour décompresser un fichier binaire

1. Munissez-vous du fichier binaire et du fichier table précédemment obtenus (dans notre exemple `fichierBin` et `fichierTxtTable.txt`)
2.  Décompressez `fichierBin` et récupérez la sortie dans un fichier texte (dans notre exemple `fichierTxt2`)
   ```sh
   ./huffman -d fichierBin.bin fichierTxt2 fichierTxtTable.txt
   ```