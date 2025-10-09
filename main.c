#include "bigbinary.h"

int main() {
    // Tests de base
    BigBinary A = initBigBinaryFromString("10110"); // 22
    BigBinary B = initBigBinaryFromString("1101");  // 13

    printf("A = "); afficheBigBinary(A);
    printf("B = "); afficheBigBinary(B);

    printf("\nEgal(A,B) = %d\n", Egal(A, B));
    printf("A < B ? %d\n", Inferieur(A, B));

    BigBinary S = additionBigBinary(A, B);
    printf("\nA + B = ");
    afficheBigBinary(S);

    BigBinary D = soustractionBigBinary(A, B);
    printf("A - B = ");
    afficheBigBinary(D);

    // Test normalisation
    printf("\n--- Tests de normalisation ---\n");
    BigBinary C = initBigBinaryFromString("0011");
    BigBinary E = initBigBinaryFromString("11");  // Renommé de D en E
    printf("C (0011) = "); afficheBigBinary(C);
    printf("E (11)   = "); afficheBigBinary(E);
    printf("0011 == 11 ? %d\n", Egal(C, E)); // Devrait afficher 1

    // Test zéro
    printf("\n--- Tests du zero ---\n");
    BigBinary Z1 = initBigBinaryFromString("0000");
    BigBinary Z2 = initBigBinary();
    printf("Z1 (0000) = "); afficheBigBinary(Z1);
    printf("Z2 (init) = "); afficheBigBinary(Z2);
    printf("0000 == 0 ? %d\n", Egal(Z1, Z2)); // Devrait afficher 1

    // Libération mémoire
    libereBigBinary(&A);
    libereBigBinary(&B);
    libereBigBinary(&S);
    libereBigBinary(&D);
    libereBigBinary(&C);
    libereBigBinary(&E);
    libereBigBinary(&Z1);
    libereBigBinary(&Z2);

    return 0;
}