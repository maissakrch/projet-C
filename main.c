#include "bigbinary.h"

int main() {
    BigBinary A = initBigBinaryFromString("10110"); // 22
    BigBinary B = initBigBinaryFromString("1101");  // 13

    printf("A = "); afficheBigBinary(A);
    printf("B = "); afficheBigBinary(B);

    printf("\nÉgal(A,B) = %d\n", Egal(A, B));
    printf("A < B ? %d\n", Inferieur(A, B));

    BigBinary S = additionBigBinary(A, B);
    printf("\nA + B = ");
    afficheBigBinary(S);

    BigBinary D = soustractionBigBinary(A, B);
    printf("A - B = ");
    afficheBigBinary(D);

    libereBigBinary(&A);
    libereBigBinary(&B);
    libereBigBinary(&S);
    libereBigBinary(&D);

    return 0;
}
