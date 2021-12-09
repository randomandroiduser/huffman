#include "decompression.h"
#include <stdlib.h>

elemCaraTable* creerElemCaraTable(char c, int nvCode, int nbBitsNvCode) {
    elemCaraTable* nv = (elemCaraTable*) malloc(sizeof(elemCaraTable));
    if (nv == NULL) {
        fprintf(stderr, "Erreur : allocation memoire\n");
        exit(1);
    }

    nv->cara = c;
    nv->nvCode = nvCode;
    nv->nbBitsNvCode = nbBitsNvCode;
    nv->suiv = NULL;

    return nv;
}

// Renvoie le nombre de caractères du fichier d'un flux de lecture et réinitilise son pointeur au premier caractère
int nbCaracteres(FILE* fluxFichier) {
    int nbCara;
    for (nbCara = 0; fgetc(fluxFichier) != EOF; ++nbCara); // Calcul du nombre de caractères
    rewind(fluxFichier);

    return nbCara;
}

elemCaraTable* traiterTableCodage(FILE* fluxFichierTable, int* nbBitsVides) {
    (*nbBitsVides) = 0;
    int nbCara = nbCaracteres(fluxFichierTable);
    int nbCaraActuel = 1;
    char caraActuel = fgetc(fluxFichierTable);
    char caractere = caraActuel;
    int nvCode = 0;
    int nbBitsNvCode = 0;

    while (caraActuel != ',') {
        nbCaraActuel++;
        caraActuel = fgetc(fluxFichierTable);
        if (caraActuel != ',') {
            nvCode |= caraActuel - 48;
            nvCode <<= 1;
            nbBitsNvCode++;
        }
    }
    nvCode >>= 1; // on enlève le <<= 1 de trop

    elemCaraTable* liste = creerElemCaraTable(caractere, nvCode, nbBitsNvCode);
    elemCaraTable* actuel = liste;
    nbCaraActuel++;
    caraActuel = fgetc(fluxFichierTable); // on s'est arrêté à la virgule donc on passe au caractère suivant

    int ignorerVerification = 0; // on utilise une variable qui ignore la vérification du premier caractère suivant une virgule, de cette manière si ce dit caractère est une virgule il sera tout de même pris en compte
    while (caraActuel != EOF) {
        if (nbCaraActuel == nbCara) { // le cas où la fin du fichier table contient un entier correspondant au nombre de bits vides à la fin de la décompression
            (*nbBitsVides) = caraActuel - 48;
            break;
        }
        
        caractere = caraActuel;
        nvCode = 0;
        nbBitsNvCode = 0;
        while (caraActuel != ',' || ignorerVerification) {
            if (ignorerVerification) ignorerVerification = 0;
            nbCaraActuel++;
            caraActuel = fgetc(fluxFichierTable);
            if (caraActuel != ',') {
                nvCode |= caraActuel - 48;
                nvCode <<= 1;
                nbBitsNvCode++;
            }
        }
        nvCode >>= 1;
        ignorerVerification = 1;

        elemCaraTable* nv = creerElemCaraTable(caractere, nvCode, nbBitsNvCode);
        actuel->suiv = nv;
        actuel = actuel->suiv;

        nbCaraActuel++;
        caraActuel = fgetc(fluxFichierTable);
    }

    return liste;
}

// tri croissant par nbBitsNvCode pour placer les éléments qui sont écrits sur peu de bits (les + utilisés) en premier, et les éléments qui sont écrits sur beaucoup de bits (les - utilisés) en dernier
elemCaraTable* triElemCaraTab(elemCaraTable* e) {
    if (e == NULL || e->suiv == NULL) return e; // inutile de trier une liste vide ou à un seul élément

    elemCaraTable* actuel = e;
    while (actuel && actuel->suiv) {
        elemCaraTable* min = actuel->suiv;
        elemCaraTable* actuel2 = actuel->suiv->suiv;
        while (actuel2) {
            if (actuel2->nbBitsNvCode < min->nbBitsNvCode) min = actuel2;
            actuel2 = actuel2->suiv;
        }

        if (min->nbBitsNvCode < actuel->nbBitsNvCode) {
            char tempCara = actuel->cara;
            int tempNvCode = actuel->nvCode;
            int tempNbBitsNvCode = actuel->nbBitsNvCode;

            actuel->cara = min->cara;
            actuel->nvCode = min->nvCode;
            actuel->nbBitsNvCode = min->nbBitsNvCode;
            min->cara = tempCara;
            min->nvCode = tempNvCode;
            min->nbBitsNvCode = tempNbBitsNvCode;
        }

        actuel = actuel->suiv;
    }

    return e;
}

void testAccumulateurEtEcriture(int* accBits, int* comptAccBits, elemCaraTable* liste, FILE* fluxFichierSortie) {
    while (liste) {
        if (*comptAccBits == liste->nbBitsNvCode && *accBits == liste->nvCode) {
            fprintf(fluxFichierSortie, "%c", liste->cara);
            (*accBits) = 0;
            (*comptAccBits) = 0;
            break;
        }

        liste = liste->suiv;
    }
}

void freeElemCaraTable(elemCaraTable* liste) {
    while (liste) {
        elemCaraTable* temp = liste;
        liste = liste->suiv;
        free(temp);
    }
}