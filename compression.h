#include <stdio.h>

typedef struct elementCara { // Elément qui contient caractère:apparitions
    char cara; // Caractère
    int apparition; // Nombres d'apparitions
    int nvCode; // Le nouveau code binaire du caractère
    int nbBitsNvCode; // Le nombre de bits du nouveau code, on en aura besoin pour écrire nvCode correctement
    int ecritDansTable; // Si le caractère a déjà été associé à son nouveau code dans le fichier table de sortie
    struct elementCara* suiv;
} elemCara;

typedef struct noeud {
    elemCara* elmt;
    struct noeud* fg;
    struct noeud* fd;
    int somme; // Entier qui permettra de déterminer si le noeud représente un caractère (dans ce cas il vaudra -1)
    //ou est juste une somme de deux noeuds (dans ce cas il vaudra la somme des apparitions des deux noeuds)
} node;

typedef struct elementNoeud { // Elément qui contient des noeuds de l'arbre à construire, indépendamment de noeuds somme ou noeuds caractère:apparitions
    node* noeud;
    struct elementNoeud* suiv;
} elemNode;

elemNode* creerElemNoeud(node*);
elemCara* creerElemCara(char);

void ajoutApparition(elemCara*, char);
elemCara* triApparition(elemCara*);
elemNode* triApparitionElemNode(elemNode*);

node* creerNoeud(elemCara*);
node* creerNoeudSomme(node*, node*);

node* construireArbre(elemCara*);
void assignerNouveauCodeRec(node*, int*, int*);
void assignerNouveauCode(node*);

void ecrireTexteCompresseEtTable(FILE*, FILE*, FILE*, elemCara*);

void freeElemCara(elemCara*);
void freeArbre(node*);