#include <stdio.h>

typedef struct elementCaraTable {
    char cara;
    int tailleVar;
    int nbBitsTailleVar; // le nombre de bits de la tailleVar (pour par exemple différencier 0000 et 0)
    struct elementCaraTable* suiv;
} elemCaraTable;

elemCaraTable* creerElemCaraTable(char, int, int);
elemCaraTable* traiterTableCodage(FILE*, int*);
elemCaraTable* triElemCaraTab(elemCaraTable*);
void testAccumulateurEtEcriture(int*, int*, elemCaraTable*, FILE*);