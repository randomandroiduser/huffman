#include <stdio.h>

typedef struct elementCara { // élément qui contient caractère:itérations
    char cara; //caractère
    int iter; //nombres d'iterations
    int tailleVar; //la nouvelle taille
    int nbBitsTailleVar; // le nombre de bits de la tailleVar, on en aura besoin pour écrire la tailleVar correctement
    int ecritDansTable; //si le caractère a déjà été associé à sa taille variable dans le fichier table de sortie
    struct elementCara* suiv;
} elemCara;

typedef struct noeud {
    elemCara* elmt;
    struct noeud* fg;
    struct noeud* fd;
    int somme; //entier qui permettra de déterminer si le noeud représente un caractère (dans ce cas il vaudra -1)
    //ou est juste une somme de deux noeuds (dans ce cas il vaudra la somme des itérations des deux noeuds)
} node;

typedef struct elementNoeud { // élément qui contient des noeuds de l'arbre à construire, indépendamment de noeuds somme ou noeuds caractère:itérations
    node* noeud;
    struct elementNoeud* suiv;
} elemNode;

elemNode* creerElemNoeud(node*);
elemCara* creerElemCara(char);

void ajoutIter(elemCara*, char);
elemCara* triIter(elemCara*);
elemNode* triIterElemNode(elemNode*);

node* creerNoeud(elemCara*);
node* creerNoeudSomme(node*, node*);

node* construireArbre(elemCara*);
void assignerTailleVariableRec(node*, int*, int*);
void assignerTailleVariable(node*);

void ecrireTexteCompresseEtTable(FILE*, FILE*, FILE*, elemCara*);