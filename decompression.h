#include <stdio.h>

typedef struct elementCaraTable {
    char cara;
    int nvCode;
    int nbBitsNvCode; // Le nombre de bits du nouveau code (pour par exemple différencier 0000 et 0)
    struct elementCaraTable* suiv;
} elemCaraTable;

elemCaraTable* creerElemCaraTable(char, int, int);
int nbCaracteres(FILE*);
elemCaraTable* traiterTableCodage(FILE*, int*);
elemCaraTable* triElemCaraTab(elemCaraTable*);

void testAccumulateurEtEcriture(int*, int*, elemCaraTable*, FILE*);

void freeElemCaraTable(elemCaraTable*);