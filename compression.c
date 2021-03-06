#include "compression.h"
#include <stdlib.h>

// Initialise un elemNode à partir d'un noeud et le renvoie
elemNode* creerElemNoeud(node* n) {
    elemNode* nv = (elemNode*) malloc(sizeof(elemNode));
    if (nv == NULL) {
        fprintf(stderr, "Erreur : allocation memoire\n");
        exit(1);
    }

    nv->noeud = n;
    nv->suiv = NULL;

    return nv;
}

// Initialise un elemCara à partir d'un caractère et le renvoie
elemCara* creerElemCara(char c) {
    elemCara* nv = (elemCara*) malloc(sizeof(elemCara));
    if (nv == NULL) {
        fprintf(stderr, "Erreur : allocation memoire\n");
        exit(1);
    }

    nv->cara = c;
    nv->apparition = 1; // La fonction sera appelée lorsque l'élément aura déjà été trouvé une fois
    nv->ecritDansTable = 0;
    nv->suiv = NULL;

    return nv;
}

// Ajoute une apparition à un caractère s'il est déjà dans la liste chaînée, sinon crée un élément pour le nouveau caractère à la fin de la liste
void ajoutApparition(elemCara* e, char c) {
    // On sait que e n'est pas NULL car on a déjà initialisé notre liste au premier élément

    while(e->suiv && e->cara != c) e = e->suiv;

    if (e->cara == c) e->apparition++;
    else e->suiv = creerElemCara(c);
}

// Trie la liste d'elemCara par nombre d'apparitions et renvoie le premier élément de la liste
elemCara* triApparition(elemCara* e) {
    if (e == NULL || e->suiv == NULL) return e; // Inutile de trier une liste vide ou à un seul élément

    elemCara* actuel = e;
    while(actuel && actuel->suiv) {
        elemCara* min = actuel->suiv;
        elemCara* actuel2 = actuel->suiv->suiv;
        while(actuel2) {
            if (actuel2->apparition < min->apparition) min = actuel2;
            actuel2 = actuel2->suiv;
        }

        if (min->apparition < actuel->apparition) {
            char tempCara = actuel->cara;
            int tempApparition = actuel->apparition;
            actuel->cara = min->cara;
            actuel->apparition = min->apparition;
            min->cara = tempCara;
            min->apparition = tempApparition;
        }

        actuel = actuel->suiv;
    }

    return e;
}

// Trie la liste d'elemNode par valeur de la somme et renvoie le premier élément de la liste
elemNode* triApparitionElemNode(elemNode* e) {
    if (e == NULL || e->suiv == NULL) return e; // Inutile de trier une liste vide ou à un seul élément

    elemNode* actuel = e;
    while(actuel && actuel->suiv) {
        elemNode* min = actuel->suiv;
        elemNode* actuel2 = actuel->suiv->suiv;
        while(actuel2) {
            if (actuel2->noeud->somme < min->noeud->somme) min = actuel2;
            actuel2 = actuel2->suiv;
        }

        if (min->noeud->somme < actuel->noeud->somme) {
            node* tempNode = actuel->noeud;
            actuel->noeud = min->noeud;
            min->noeud = tempNode;
        }

        actuel = actuel->suiv;
    }

    return e;
}

// Initialise un noeud associé à un elemCara (donc une feuille de l'arbre) et le renvoie
node* creerNoeud(elemCara* e) {
    node* nv = (node*) malloc(sizeof(node));
    if (nv == NULL) {
        fprintf(stderr, "Erreur : allocation memoire\n");
        exit(1);
    }

    nv->elmt = e;
    nv->fg = NULL;
    nv->fd = NULL;
    nv->somme = -1;

    return nv;
}

// Initialise un noeud associé à une somme (donc un noeud interne de l'arbre) et le renvoie
node* creerNoeudSomme(node* fg, node* fd) {
    node* nv = (node*) malloc(sizeof(node));
    if (nv == NULL) {
        fprintf(stderr, "Erreur : allocation memoire\n");
        exit(1);
    }

    nv->fg = fg;
    nv->fd = fd;
    nv->somme = (fg->somme == -1 ? fg->elmt->apparition : fg->somme) + (fd->somme == -1 ? fd->elmt->apparition : fd->somme);

    return nv;
}

// Construit l'arbre de compression pour appliquer l'algorithme de Huffman ensuite et renvoie la racine
node* construireArbre(elemCara* e) {
    if (e == NULL) return NULL;
    if (e->suiv == NULL) return creerNoeud(e);
    
    node* noeud = creerNoeudSomme(creerNoeud(e), creerNoeud(e->suiv));
    elemCara* actuel = e->suiv->suiv;
    elemNode* dernierNoeudSomme = creerElemNoeud(noeud); // Tiendra compte des noeuds somme créés qui ne sont pas encore des fils pour les ajouter à l'arbre
    while (actuel || (dernierNoeudSomme && dernierNoeudSomme->suiv)) {
        if (!actuel) {// Si actuel == NULL et qu'on est tout de même dans la boule, c'est qu'il y a deux sommes à additionner : on le traite à part
            node* nvNoeud = creerNoeudSomme(dernierNoeudSomme->noeud, dernierNoeudSomme->suiv->noeud);
            elemNode* nvElemNoeud = creerElemNoeud(nvNoeud);
            nvElemNoeud->suiv = dernierNoeudSomme->suiv->suiv;

            free(dernierNoeudSomme->suiv);
            elemNode* temp = dernierNoeudSomme;
            dernierNoeudSomme = nvElemNoeud;
            free(temp);
        } else if (dernierNoeudSomme->noeud->somme < actuel->apparition) {// Le prochain noeud à créer a un noeud de somme en fils gauche
            if (dernierNoeudSomme->suiv && dernierNoeudSomme->suiv->noeud->somme < actuel->apparition) {// Le prochain noeud à créer a deux noeuds de somme en fils
                node* nvNoeud = creerNoeudSomme(dernierNoeudSomme->noeud, dernierNoeudSomme->suiv->noeud);
                elemNode* nvElemNoeud = creerElemNoeud(nvNoeud);
                nvElemNoeud->suiv = dernierNoeudSomme->suiv->suiv;

                free(dernierNoeudSomme->suiv);
                elemNode* temp = dernierNoeudSomme;
                dernierNoeudSomme = nvElemNoeud;
                free(temp);
            } else {// Sinon le prochain noeud à créer a exactement une somme comme fils (et c'est le fils gauche)
                node* nvNoeud = creerNoeudSomme(dernierNoeudSomme->noeud, creerNoeud(actuel));
                elemNode* nvElemNoeud = creerElemNoeud(nvNoeud);
                nvElemNoeud->suiv = dernierNoeudSomme->suiv;

                elemNode* temp = dernierNoeudSomme;
                dernierNoeudSomme = nvElemNoeud;
                free(temp);

                actuel = actuel->suiv;
            }
        } else if (actuel->suiv && dernierNoeudSomme->noeud->somme > actuel->suiv->apparition) {// Le prochain noeud à créer est une somme de deux feuilles
            node* nvNoeud = creerNoeudSomme(creerNoeud(actuel), creerNoeud(actuel->suiv));
            elemNode* nvElemNoeud = creerElemNoeud(nvNoeud);

            elemNode* dernierNoeudSommeActuel = dernierNoeudSomme;
            while(dernierNoeudSommeActuel->suiv) dernierNoeudSommeActuel = dernierNoeudSommeActuel->suiv;
            dernierNoeudSommeActuel->suiv = nvElemNoeud;

            actuel = actuel->suiv->suiv;
        } else {// Le cas restant
            node* nvNoeud = creerNoeudSomme(creerNoeud(actuel), dernierNoeudSomme->noeud);
            elemNode* nvElemNoeud = creerElemNoeud(nvNoeud);
            nvElemNoeud->suiv = dernierNoeudSomme->suiv;

            elemNode* temp = dernierNoeudSomme;
            dernierNoeudSomme = nvElemNoeud;
            free(temp);

            actuel = actuel->suiv;
        }

        dernierNoeudSomme = triApparitionElemNode(dernierNoeudSomme);
    }

    return dernierNoeudSomme->noeud;
}

// Réalise l'affectation du nouveau code dans les elemCara des noeuds de l'arbre, de manière récursive
void assignerNouveauCodeRec(node* arbre, int* bits, int* profondeur) {
    if (arbre) {
        (*profondeur)++;
        if (arbre->somme == -1) {
            arbre->elmt->nvCode = *bits; // On a atteint une feuille, on donne le nouveau code alors obtenue
            arbre->elmt->nbBitsNvCode = *profondeur;
        } else { // On a atteint un noeud somme, décalons les bits du nouveau code vers la gauche (et ceci ajoute un 0 à la fin par défaut)
            (*bits) <<= 1;
            if (arbre->fg) assignerNouveauCodeRec(arbre->fg, bits, profondeur); // Si c'est un fils gauche, on réitère juste
            if (arbre->fd) {
                (*bits) |= 1; // Si c'est un fils droit, il faut d'abord modifier le dernier bit en 1
                assignerNouveauCodeRec(arbre->fd, bits, profondeur);
            }
            (*bits) >>= 1; // Quand on remonte dans l'arbre, on redécale vers la droite d'un bit
        }
        (*profondeur)--;
    }
}

// Initialise l'affectation du nouveau code dans les elemCara des noeuds de l'arbre et appelle assignerNouveauCodeRec pour le faire de manière récursive
void assignerNouveauCode(node* arbre) {
    if (arbre) {
        if (arbre->somme == -1) { // Fichier qui ne contient qu'un même caractère
            arbre->elmt->nvCode = 0;
            arbre->elmt->nbBitsNvCode = 1;
        } else {
            int prof;
            if (arbre->fg) {
                int nvCode = 0;
                prof = 0;
                assignerNouveauCodeRec(arbre->fg, &nvCode, &prof);
            }
            if (arbre->fd) {
                int nvCode = 1;
                prof = 0;
                assignerNouveauCodeRec(arbre->fd, &nvCode, &prof);
            }
        }
    }
}

// Lit le fichier texte d'entrée et écrit chaque caractère avec son nouveau code dans le fichier binaire de sortie
// Ecrit également dans le fichier table pour décompresser ensuite
void ecrireTexteCompresseEtTable(FILE* fluxFichierEntree, FILE* fluxFichierSortie, FILE* fluxFichierTable, elemCara* listeCaracteres) {
    int accumulateurBits = 0; // on écrira les bits par paquet de 8 pour remplir les octets
    int compteurAccBits = 0; // on comptera jusqu'à 8 avant de repasser le compteur à 0 et d'écrire une fois

    char caractere = fgetc(fluxFichierEntree);

    while (caractere != EOF) {
        elemCara* actuel = listeCaracteres;
        while (actuel) {
            if (caractere == actuel->cara) {// on peut comparer car deux char (donc deux entiers dans la table ASCII)
                if (!actuel->ecritDansTable) fprintf(fluxFichierTable, "%c", actuel->cara);

                for (int i = 0; i < actuel->nbBitsNvCode; i++) {
                    int bitActuel = (actuel->nvCode >> (actuel->nbBitsNvCode - (i + 1))) & 1;

                    if (compteurAccBits != 0) accumulateurBits <<= 1;
                    accumulateurBits |= bitActuel;
                    compteurAccBits++;

                    if (compteurAccBits == 8) {
                        fwrite(&accumulateurBits, 1, 1, fluxFichierSortie);
                        accumulateurBits = 0;
                        compteurAccBits = 0;
                    }

                    if (!actuel->ecritDansTable) fprintf(fluxFichierTable, "%c", bitActuel + 48); // écrire à quoi correspond le caractère en binaire si ce n'est pas déjà fait
                }

                if (!actuel->ecritDansTable) fprintf(fluxFichierTable, "%c", ',');

                actuel->ecritDansTable = 1;
                break;
            }

            actuel = actuel->suiv;
        }

        caractere = fgetc(fluxFichierEntree); //passage au caractère suivant
    }

    if (compteurAccBits != 0) { // si le dernier octet à écrire n'a pas été complètement rempli, on l'écrit tout de même
        int nbBitsVides = 8 - compteurAccBits;
        accumulateurBits <<= nbBitsVides;
        fprintf(fluxFichierTable, "%c", nbBitsVides + 48);
        fwrite(&accumulateurBits, 1, 1, fluxFichierSortie);
    }
}

// Libère l'espace mémoire alloué à la liste d'elemCara
void freeElemCara(elemCara* liste) {
    while (liste) {
        elemCara* temp = liste;
        liste = liste->suiv;
        free(temp);
    }
}

// Libère l'espace mémoire alloué à l'arbre
void freeArbre(node* arbre) {
    if (arbre->fg) freeArbre(arbre->fg);
    if (arbre->fd) freeArbre(arbre->fd);
    free(arbre);
}