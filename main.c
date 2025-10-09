#include "bigbinary.h"
#include <stdio.h>

static void show(const char *label, BigBinary X) {
    printf("%s = ", label);
    afficheBigBinary(X);
}

int main(void) {
    /* ============================
       PHASE 1 – TESTS RAPIDES
       ============================ */
    BigBinary A = initBigBinaryFromString("10110"); // 22
    BigBinary B = initBigBinaryFromString("1101");  // 13

    show("A", A);
    show("B", B);

    printf("\n[Phase 1] Comparaisons\n");
    printf("Egal(A,B)    = %d (attendu 0)\n", Egal(A, B));
    printf("A < B ?      = %d (attendu 0)\n", Inferieur(A, B));

    BigBinary S = additionBigBinary(A, B);          // 22+13=35 -> 100011
    BigBinary D = soustractionBigBinary(A, B);      // 22-13=9  -> 1001
    printf("\n[Phase 1] Opérations\n");
    show("A + B", S); // attendu 100011
    show("A - B", D); // attendu 1001

    printf("\n[Phase 1] Normalisation & zéros\n");
    BigBinary C  = initBigBinaryFromString("0011");
    BigBinary E  = initBigBinaryFromString("11");
    BigBinary Z1 = initBigBinaryFromString("0000");
    BigBinary Z2 = initBigBinary();
    show("C (0011)", C);
    show("E (11)  ", E);
    printf("0011 == 11 ? %d (attendu 1)\n", Egal(C, E));
    show("Z1 (0000)", Z1);
    show("Z2 (init)", Z2);
    printf("0000 == 0 ? %d (attendu 1)\n", Egal(Z1, Z2));

    /* ============================
       PHASE 2 – DÉCALAGES
       ============================ */
    printf("\n============================\n");
    printf("PHASE 2 – Décalages\n");

    BigBinary X = initBigBinaryFromString("101101000"); // 360
    BigBinary X_L3 = decaleGauche(X, 3);                // 360 * 8  = 2880 -> 101101000000
    BigBinary X_R3 = decaleDroite(X, 3);                // 360 / 8  = 45   -> 101101

    show("X", X);
    show("X<<3", X_L3); // attendu 101101000000
    show("X>>3", X_R3); // attendu 101101

    // cas bords: shift par 0, par >= taille
    BigBinary X_L0 = decaleGauche(X, 0);
    BigBinary X_R100 = decaleDroite(X, 100);           // -> 0
    show("X<<0", X_L0);   // attendu X inchangé
    show("X>>100", X_R100); // attendu 0

    /* ============================
       PHASE 2 – |A - B|
       ============================ */
    printf("\n============================\n");
    printf("PHASE 2 – Soustraction absolue |A-B|\n");

    BigBinary U = initBigBinaryFromString("101101000"); // 360
    BigBinary V = initBigBinaryFromString("11000");     // 24
    BigBinary ABS1 = soustractionAbsolue(U, V);         // 336 -> 101010000
    BigBinary ABS2 = soustractionAbsolue(V, U);         // idem (symétrique)

    show("|U - V|", ABS1); // attendu 101010000
    show("|V - U|", ABS2); // attendu 101010000

    BigBinary EQ1 = initBigBinaryFromString("10101");   // 21
    BigBinary EQ2 = initBigBinaryFromString("10101");   // 21
    BigBinary ABS_EQ = soustractionAbsolue(EQ1, EQ2);   // 0
    show("|EQ1 - EQ2|", ABS_EQ); // attendu 0

    /* ============================
       PHASE 2 – PGCD binaire (Stein)
       ============================ */
    printf("\n============================\n");
    printf("PHASE 2 – PGCD binaire (Stein)\n");

    // Jeux de tests avec attentes commentées
    BigBinary G1 = pgcdBinaire(U, V);                   // gcd(360,24)=24 -> 11000
    show("gcd(360,24)", G1);                            // attendu 11000

    BigBinary C1 = initBigBinaryFromString("110000");   // 48
    BigBinary D1 = initBigBinaryFromString("10010");    // 18
    BigBinary G2 = pgcdBinaire(C1, D1);                 // gcd(48,18)=6 -> 110
    show("gcd(48,18)", G2);                             // attendu 110

    BigBinary C2 = initBigBinaryFromString("10101");    // 21
    BigBinary D2 = initBigBinaryFromString("1000");     // 8
    BigBinary G3 = pgcdBinaire(C2, D2);                 // gcd(21,8)=1 -> 1
    show("gcd(21,8)", G3);                              // attendu 1

    BigBinary C3 = initBigBinaryFromString("1101000");  // 104
    BigBinary D3 = initBigBinaryFromString("101000");   // 40
    BigBinary G4 = pgcdBinaire(C3, D3);                 // gcd(104,40)=8 -> 1000
    show("gcd(104,40)", G4);                            // attendu 1000

    // Cas avec zéro (règles: gcd(0,x)=x)
    BigBinary Z = initBigBinary();                      // 0
    BigBinary G5 = pgcdBinaire(Z, V);                   // gcd(0,24)=24
    show("gcd(0,24)", G5);                              // attendu 11000

    /* ============================
       LIBÉRATIONS
       ============================ */
    libereBigBinary(&A);  libereBigBinary(&B);
    libereBigBinary(&S);  libereBigBinary(&D);
    libereBigBinary(&C);  libereBigBinary(&E);
    libereBigBinary(&Z1); libereBigBinary(&Z2);

    libereBigBinary(&X); libereBigBinary(&X_L3);
    libereBigBinary(&X_R3); libereBigBinary(&X_L0);
    libereBigBinary(&X_R100);

    libereBigBinary(&U); libereBigBinary(&V);
    libereBigBinary(&ABS1); libereBigBinary(&ABS2);
    libereBigBinary(&EQ1); libereBigBinary(&EQ2); libereBigBinary(&ABS_EQ);

    libereBigBinary(&G1);
    libereBigBinary(&C1); libereBigBinary(&D1); libereBigBinary(&G2);
    libereBigBinary(&C2); libereBigBinary(&D2); libereBigBinary(&G3);
    libereBigBinary(&C3); libereBigBinary(&D3); libereBigBinary(&G4);
    libereBigBinary(&Z);  libereBigBinary(&G5);

    return 0;
}
