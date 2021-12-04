#include "functions.h"

// Fonctions générales
elemNode* creerElemNoeud(node* n) {
    elemNode* nv = (elemNode*) malloc(sizeof(elemNode));
    if (nv == NULL) {
        fprintf(stderr, "ERREUR ALLOCATION MEMOIRE");
        exit(1);
    }

    nv->noeud = n;
    nv->suiv = NULL;

    return nv;
}

elemCara* creerElemCara(char c) {
    elemCara* nv = (elemCara*) malloc(sizeof(elemCara));
    if (nv == NULL) {
        fprintf(stderr, "ERREUR ALLOCATION MEMOIRE");
        exit(1);
    }

    nv->cara = c;
    nv->iter = 1; // la fonction sera appelée lorsque l'élément aura déjà été trouvé une fois
    nv->ecritDansTable = 0;
    nv->suiv = NULL;

    return nv;
}

// Cette fonction ajoute une itération à un caractère s'il est déjà dans la liste chaînée,
// sinon, elle crée un élément pour le nouveau caractère à la fin de la liste.
void ajoutIter(elemCara* e, char c) {
    // on sait que e n'est pas NULL car on a déjà initialisé notre liste au premier élément

    while(e->suiv && e->cara != c) e = e->suiv;

    if (e->cara == c) e->iter++;
    else e->suiv = creerElemCara(c);
}

//DEBUG
void afficherListe(elemCara* e) {
    while(e) {
        printf("%c - %d iterations\n", e->cara, e->iter);
        e = e->suiv;
    }
}

// Cette fonction trie une liste par nombre d'itérations et renvoie le premier élément de la liste
elemCara* triIter(elemCara* e) {
    if (e == NULL || e->suiv == NULL) return e; // inutile de trier une liste vide ou à un seul élément

    elemCara* actuel = e;
    while(actuel && actuel->suiv) {
        elemCara* min = actuel->suiv;
        elemCara* actuel2 = actuel->suiv->suiv;
        while(actuel2) {
            if (actuel2->iter < min->iter) min = actuel2;
            actuel2 = actuel2->suiv;
        }

        if (min->iter < actuel->iter) {
            char tempCara = actuel->cara;
            int tempIter = actuel->iter;
            actuel->cara = min->cara;
            actuel->iter = min->iter;
            min->cara = tempCara;
            min->iter = tempIter;
        }

        actuel = actuel->suiv;
    }

    return e;
}

elemNode* triIterElemNode(elemNode* e) {
    if (e == NULL || e->suiv == NULL) return e; // inutile de trier une liste vide ou à un seul élément

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

node* creerNoeud(elemCara* e) { // dans la pratique, les noeuds qui ne sont pas des sommes sont les feuilles de notre arbre
    node* nv = (node*) malloc(sizeof(node));
    if (nv == NULL) {
        fprintf(stderr, "ERREUR ALLOCATION MEMOIRE");
        exit(1);
    }

    nv->elmt = e;
    nv->fg = NULL;
    nv->fd = NULL;
    nv->somme = -1;

    return nv;
}

node* creerNoeudSomme(node* fg, node* fd) {
    node* nv = (node*) malloc(sizeof(node));
    if (nv == NULL) {
        fprintf(stderr, "ERREUR ALLOCATION MEMOIRE");
        exit(1);
    }

    nv->fg = fg;
    nv->fd = fd;
    nv->somme = (fg->somme == -1 ? fg->elmt->iter : fg->somme) + (fd->somme == -1 ? fd->elmt->iter : fd->somme);

    return nv;
}

node* construireArbre(elemCara* e) {
    if (e == NULL) return NULL;
    if (e->suiv == NULL) return creerNoeud(e);
    
    node* noeud = creerNoeudSomme(creerNoeud(e), creerNoeud(e->suiv));
    elemCara* actuel = e->suiv->suiv;
    elemNode* dernierNoeudSomme = creerElemNoeud(noeud); // tiendra compte des noeuds somme créés qui ne sont pas encore des fils pour les ajouter à l'arbre
    while (actuel || (dernierNoeudSomme && dernierNoeudSomme->suiv)) {
        if (!actuel) {// si actuel == NULL et qu'on est tout de même dans la boule, c'est qu'il y a deux sommes à additionner : on le traite à part
            node* nvNoeud = creerNoeudSomme(dernierNoeudSomme->noeud, dernierNoeudSomme->suiv->noeud);
            elemNode* nvElemNoeud = creerElemNoeud(nvNoeud);
            nvElemNoeud->suiv = dernierNoeudSomme->suiv->suiv;

            free(dernierNoeudSomme->suiv);
            elemNode* temp = dernierNoeudSomme;
            dernierNoeudSomme = nvElemNoeud;
            free(temp);
        } else if (dernierNoeudSomme->noeud->somme < actuel->iter) {// le prochain noeud à créer a un noeud de somme en fils gauche
            if (dernierNoeudSomme->suiv && dernierNoeudSomme->suiv->noeud->somme < actuel->iter) {// le prochain noeud à créer a deux noeuds de somme en fils
                node* nvNoeud = creerNoeudSomme(dernierNoeudSomme->noeud, dernierNoeudSomme->suiv->noeud);
                elemNode* nvElemNoeud = creerElemNoeud(nvNoeud);
                nvElemNoeud->suiv = dernierNoeudSomme->suiv->suiv;

                free(dernierNoeudSomme->suiv);
                elemNode* temp = dernierNoeudSomme;
                dernierNoeudSomme = nvElemNoeud;
                free(temp);
            } else {// sinon le prochain noeud à créer a exactement une somme comme fils (et c'est le fils gauche)
                node* nvNoeud = creerNoeudSomme(dernierNoeudSomme->noeud, creerNoeud(actuel));
                elemNode* nvElemNoeud = creerElemNoeud(nvNoeud);
                nvElemNoeud->suiv = dernierNoeudSomme->suiv;

                elemNode* temp = dernierNoeudSomme;
                dernierNoeudSomme = nvElemNoeud;
                free(temp);

                actuel = actuel->suiv;
            }
        } else if (actuel->suiv && dernierNoeudSomme->noeud->somme > actuel->suiv->iter) {// le prochain noeud à créer est une somme de deux feuilles
            node* nvNoeud = creerNoeudSomme(creerNoeud(actuel), creerNoeud(actuel->suiv));
            elemNode* nvElemNoeud = creerElemNoeud(nvNoeud);

            elemNode* dernierNoeudSommeActuel = dernierNoeudSomme;
            while(dernierNoeudSommeActuel->suiv) dernierNoeudSommeActuel = dernierNoeudSommeActuel->suiv;
            dernierNoeudSommeActuel->suiv = nvElemNoeud;

            actuel = actuel->suiv->suiv;
        } else {// le cas restant
            node* nvNoeud = creerNoeudSomme(creerNoeud(actuel), dernierNoeudSomme->noeud);
            elemNode* nvElemNoeud = creerElemNoeud(nvNoeud);
            nvElemNoeud->suiv = dernierNoeudSomme->suiv;

            elemNode* temp = dernierNoeudSomme;
            dernierNoeudSomme = nvElemNoeud;
            free(temp);

            actuel = actuel->suiv;
        }

        dernierNoeudSomme = triIterElemNode(dernierNoeudSomme);
    }

    return dernierNoeudSomme->noeud;
}

void assignerTailleVariableRec(node* arbre, int* bits, int* profondeur) {
    if (arbre) {
        (*profondeur)++;
        if (arbre->somme == -1) {
            arbre->elmt->tailleVar = *bits; // on a atteint une feuille, on donne la taille variable alors obtenue
            arbre->elmt->nbBitsTailleVar = *profondeur;
        } else { // on a atteint un noeud somme, décalons les bits de la taille variable vers la gauche (et ceci ajoute un 0 à la fin par défaut)
            (*bits) <<= 1;
            if (arbre->fg) assignerTailleVariableRec(arbre->fg, bits, profondeur); // si c'est un fils gauche, on réitère juste
            if (arbre->fd) {
                (*bits) |= 1; // si c'est un fils droit, il faut d'abord modifier le dernier bit en 1
                assignerTailleVariableRec(arbre->fd, bits, profondeur);
            }
            (*bits) >>= 1; // quand on remonte dans l'arbre, on redécale vers la droite d'un bit
        }
        (*profondeur)--;
    }
}

void assignerTailleVariable(node* arbre) {
    if (arbre) {
        if (arbre->somme == -1) { // fichier qui ne contient qu'un même caractère
            arbre->elmt->tailleVar = 0;
            arbre->elmt->nbBitsTailleVar = 1;
        } else {
            int prof;
            if (arbre->fg) {
                int tailleVar = 0;
                prof = 0;
                assignerTailleVariableRec(arbre->fg, &tailleVar, &prof);
            }
            if (arbre->fd) {
                int tailleVar = 1;
                prof = 0;
                assignerTailleVariableRec(arbre->fd, &tailleVar, &prof);
            }
        }
    }
}

void ecrireTexteCompresseEtTable(FILE* fluxFichierEntree, FILE* fluxFichierSortie, FILE* fluxFichierTable, elemCara* listeCaracteres) {
    int accumulateurBits = 0; // on écrira les bits par paquet de 8 pour remplir les octets
    int compteurAccBits = 0; // on comptera jusqu'à 8 avant de repasser le compteur à 0 et d'écrire une fois

    char caractere = fgetc(fluxFichierEntree);

    while (caractere != EOF) {
        elemCara* actuel = listeCaracteres;
        while (actuel) {
            if (caractere == actuel->cara) {// on peut comparer car deux char (donc deux entiers dans la table ASCII)
                if (!actuel->ecritDansTable) fprintf(fluxFichierTable, "%c", actuel->cara);

                for (int i = 0; i < actuel->nbBitsTailleVar; i++) {
                    int bitActuel = (actuel->tailleVar >> (actuel->nbBitsTailleVar - (i + 1))) & 1;

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

    if (compteurAccBits != 0) {
        accumulateurBits <<= 8 - compteurAccBits;
        accumulateurBits |= (1 << (8 - compteurAccBits - 1));
        fwrite(&accumulateurBits, 1, 1, fluxFichierSortie); // si le dernier octet à écrire n'a pas été complètement rempli, on l'écrit tout de même
    }
}

elemCaraTable* creerElemCaraTable(char c, int tailleVar, int nbBitsTailleVar) {
    elemCaraTable* nv = (elemCaraTable*) malloc(sizeof(elemCaraTable));
    if (nv == NULL) {
        fprintf(stderr, "ERREUR ALLOCATION MEMOIRE");
        exit(1);
    }

    nv->cara = c;
    nv->tailleVar = tailleVar;
    nv->nbBitsTailleVar = nbBitsTailleVar;
    nv->suiv = NULL;

    return nv;
}

elemCaraTable* traiterTableCodage(FILE* fluxFichierTable) {
    char caraActuel = fgetc(fluxFichierTable);
    char caractere = caraActuel;
    int tailleVar = 0;
    int nbBitsTailleVar = 0;

    while (caraActuel != ',') {
        caraActuel = fgetc(fluxFichierTable);
        if (caraActuel != ',') {
            tailleVar |= caraActuel - 48;
            tailleVar <<= 1;
            nbBitsTailleVar++;
        }
    }
    tailleVar >>= 1; // on enlève le <<= 1 de trop

    elemCaraTable* liste = creerElemCaraTable(caractere, tailleVar, nbBitsTailleVar);
    elemCaraTable* actuel = liste;
    caraActuel = fgetc(fluxFichierTable); // on s'est arrêté à la virgule donc on passe au caractère suivant

    int ignorerVerification = 0; // on utilise une variable qui ignore la vérification du premier caractère suivant une virgule, de cette manière si ce dit caractère est une virgule il sera tout de même pris en compte
    while (caraActuel != EOF) {
        caractere = caraActuel;
        tailleVar = 0;
        nbBitsTailleVar = 0;
        while (caraActuel != ',' || ignorerVerification) {
            if (ignorerVerification) ignorerVerification = 0;
            caraActuel = fgetc(fluxFichierTable);
            if (caraActuel != ',') {
                tailleVar |= caraActuel - 48;
                tailleVar <<= 1;
                nbBitsTailleVar++;
            }
        }
        tailleVar >>= 1;
        ignorerVerification = 1;

        elemCaraTable* nv = creerElemCaraTable(caractere, tailleVar, nbBitsTailleVar);
        actuel->suiv = nv;
        actuel = actuel->suiv;

        caraActuel = fgetc(fluxFichierTable);
    }

    return liste;
}

// tri croissant par nbBitsTailleVar pour placer les éléments qui sont écrits sur peu de bits (les + utilisés) en premier, et les éléments qui sont écrits sur beaucoup de bits (les - utilisés) en dernier
elemCaraTable* triElemCaraTab(elemCaraTable* e) {
    if (e == NULL || e->suiv == NULL) return e; // inutile de trier une liste vide ou à un seul élément

    elemCaraTable* actuel = e;
    while (actuel && actuel->suiv) {
        elemCaraTable* min = actuel->suiv;
        elemCaraTable* actuel2 = actuel->suiv->suiv;
        while (actuel2) {
            if (actuel2->nbBitsTailleVar < min->nbBitsTailleVar) min = actuel2;
            actuel2 = actuel2->suiv;
        }

        if (min->nbBitsTailleVar < actuel->nbBitsTailleVar) {
            char tempCara = actuel->cara;
            int tempTailleVar = actuel->tailleVar;
            int tempNbBitsTailleVar = actuel->nbBitsTailleVar;

            actuel->cara = min->cara;
            actuel->tailleVar = min->tailleVar;
            actuel->nbBitsTailleVar = min->nbBitsTailleVar;
            min->cara = tempCara;
            min->tailleVar = tempTailleVar;
            min->nbBitsTailleVar = tempNbBitsTailleVar;
        }

        actuel = actuel->suiv;
    }

    return e;
}

//DEBUG
void afficherListeTab(elemCaraTable* e) {
    while(e) {
        printf("%c - %d\n", e->cara, e->tailleVar);
        e = e->suiv;
    }
}

void testAccumulateurEtEcriture(int* accBits, int* comptAccBits, elemCaraTable* liste, FILE* fluxFichierSortie) {
    while (liste) {
        if (*comptAccBits == liste->nbBitsTailleVar && *accBits == liste->tailleVar) {
            fprintf(fluxFichierSortie, "%c", liste->cara);
            (*accBits) = 0;
            (*comptAccBits) = 0;
            break;
        }

        liste = liste->suiv;
    }
}