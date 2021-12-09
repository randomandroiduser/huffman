#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "compression.h"
#include "decompression.h"

int main(int argc, char* argv[]) {
    if (!argv[1]) {
        fprintf(stderr, "Erreur : Option manquante. Pour plus d'aide, tapez :\n./huffman -h\n");
        exit(1);
    }

    if (strcmp(argv[1], "-h") == 0) {
        printf("Pour utiliser ce programme, tapez :\n./huffman <-c | -d> <fichier1> <fichier2> [fichierTable]\n- L'option -c permet de compresser le fichier1 et creer un fichier2 compresse\n- L'option -d permet de decompresser le fichier1 a l'aide de fichierTable, pour creer un fichier2 decompresse");
        return 0;
    } else if (strcmp(argv[1], "-c") != 0 && strcmp(argv[1], "-d") != 0) {
        fprintf(stderr, "Erreur : Option invalide. Pour plus d'aide, tapez :\n./huffman -h\n");
        exit(1);
    } else if (argc <= 4) {
        if (strcmp(argv[1], "-d") == 0) {
            fprintf(stderr, "Erreur : argument(s) manquant(s). Tapez :\n./huffman -d nomDuFichierEntree nomDuFichierSortie nomDeLaTable\n");
            exit(1);
        } else if (argc <= 3) {
            fprintf(stderr, "Erreur : argument(s) manquant(s). Tapez :\n./huffman -c nomDuFichierEntree nomDuFichierSortie\n");
            exit(1);
        }
    }

    if (strcmp(argv[1], "-c") == 0) {
        clock_t debut = clock();
        printf("Compression de %s en cours...\n", argv[2]);

        FILE* fluxFichierEntree = fopen(argv[2], "r"); //r pour read
        if (fluxFichierEntree == NULL) {
            fprintf(stderr, "Erreur : le fichier n'existe pas.\n");
            exit(1);
        }

        char caractere = fgetc(fluxFichierEntree);
        elemCara* listeCaracteres = creerElemCara(caractere); //initialisation au premier caractère
        caractere = fgetc(fluxFichierEntree); //et passage au suivant

        while (caractere != EOF) {
            ajoutApparition(listeCaracteres, caractere);
            caractere = fgetc(fluxFichierEntree); //passage au caractère suivant chaque fois qu'on a fini de traiter l'actuel
        }

        listeCaracteres = triApparition(listeCaracteres);
        node* arbre = construireArbre(listeCaracteres);
        assignerNouveauCode(arbre);

        rewind(fluxFichierEntree); // retour au début du fichier d'entrée car on va maintenant le reparcourir pour traiter chaque caractère

        FILE* fluxFichierSortie = fopen(argv[3], "wb"); //wb pour write dans un fichier binaire
        if (fluxFichierSortie == NULL) {
            fprintf(stderr, "Erreur : impossible de creer le fichier sortie.\n");
            fclose(fluxFichierEntree);
            exit(1);
        }

        char* nomFichierTable = (char*) malloc(sizeof(char) * strlen(argv[2]) + 9);
        strcpy(nomFichierTable, argv[2]);
        strcat(nomFichierTable, "Table.txt");
        FILE* fluxFichierTable = fopen(nomFichierTable, "w"); //w pour write
        if (fluxFichierTable == NULL) {
            fprintf(stderr, "Erreur : impossible de creer le fichier table.\n");
            fclose(fluxFichierEntree);
            fclose(fluxFichierSortie);
            exit(1);
        }

        ecrireTexteCompresseEtTable(fluxFichierEntree, fluxFichierSortie, fluxFichierTable, listeCaracteres);
        
        free(nomFichierTable);
        freeElemCara(listeCaracteres);
        freeArbre(arbre);

        double tailleFichierEntree = ((double)ftell(fluxFichierEntree)) / 1000; //ftell renvoie la position dans le flux de lecture. Nous étions à la fin donc il renverra la taille du fichier
        double tailleFichierSortie = ((double)ftell(fluxFichierSortie)) / 1000;
        printf("Termine (%.4lfs).\n%.2lfko compresses en %.2lfko (%.1lf%%).\n", (double)(clock() - debut)/CLOCKS_PER_SEC, tailleFichierEntree, tailleFichierSortie, tailleFichierSortie*100/tailleFichierEntree);
        
        fclose(fluxFichierEntree);
        fclose(fluxFichierSortie);
        fclose(fluxFichierTable);
        return 0;
    }

    if (strcmp(argv[1], "-d") == 0) {
        clock_t debut = clock();
        printf("Decompression de %s en cours...\n", argv[2]);

        FILE* fluxFichierTable = fopen(argv[4], "r");
        if (fluxFichierTable == NULL) {
            fprintf(stderr, "Erreur : le fichier table n'existe pas.\n");
            exit(1);
        }

        int nbBitsVides = 0;
        elemCaraTable* listeElemCaraTable = traiterTableCodage(fluxFichierTable, &nbBitsVides);
        listeElemCaraTable = triElemCaraTab(listeElemCaraTable);

        FILE* fluxFichierEntree = fopen(argv[2], "rb"); // rb pour read un fichier binaire
        if (fluxFichierEntree == NULL) {
            fprintf(stderr, "Erreur : le fichier entree n'existe pas.\n");
            fclose(fluxFichierTable);
            exit(1);
        }

        FILE* fluxFichierSortie = fopen(argv[3], "w"); //w pour write
        if (fluxFichierSortie == NULL) {
            fprintf(stderr, "Erreur : impossible de creer le fichier sortie.\n");
            fclose(fluxFichierTable);
            fclose(fluxFichierEntree);
            exit(1);
        }

        int nbCara = nbCaracteres(fluxFichierEntree);
        int caractere = fgetc(fluxFichierEntree);
        int nbCaraActuel = 0;
        int accumulateurBits = 0;
        int compteurAccBits = 0;

        while (caractere != EOF) {
            nbCaraActuel++;
            for (int i = 0; i < 8; i++) {
                if (nbCara == nbCaraActuel && i == 8 - nbBitsVides) break; // on ne veut pas écrire les bits vides sur le dernier octet
                accumulateurBits |= (caractere >> (8 - (i + 1))) & 1;
                compteurAccBits++;
                testAccumulateurEtEcriture(&accumulateurBits, &compteurAccBits, listeElemCaraTable, fluxFichierSortie);
                accumulateurBits <<= 1;
            }

            caractere = fgetc(fluxFichierEntree);
        }

        freeElemCaraTable(listeElemCaraTable);

        fclose(fluxFichierTable);
        fclose(fluxFichierEntree);
        fclose(fluxFichierSortie);

        printf("Termine (%.4lfs).\n", (double)(clock() - debut)/CLOCKS_PER_SEC);
        return 0;
    }

    return 1;
}