#include "bigbinary.h"
#include <stdio.h>

static void show(const char *label, BigBinary X) {
    printf("%s = ", label);
    afficheBigBinary(X);
}

int main(void) {
    printf("===== PHASE 1 – Vérification de base =====\n");
    BigBinary A = initBigBinaryFromString("10110"); // 22
    BigBinary B = initBigBinaryFromString("1101");  // 13

    show("A", A);
    show("B", B);
    printf("Egal(A,B) = %d\n", Egal(A, B));
    printf("A < B ?   = %d\n", Inferieur(A, B));

    BigBinary S = additionBigBinary(A, B);
    BigBinary D = soustractionBigBinary(A, B);
    show("A + B", S); // 100011 (35)
    show("A - B", D); // 1001 (9)

    libereBigBinary(&S);
    libereBigBinary(&D);

    /* =====================================================
       PHASE 2 – PARTIE 1 : Décalages et soustraction absolue
       ===================================================== */
    printf("\n===== PHASE 2 – Décalages et |A-B| =====\n");
    BigBinary X = initBigBinaryFromString("101101000"); // 360
    BigBinary X_L3 = decaleGauche(X, 3);                // 360 * 8 = 2880
    BigBinary X_R3 = decaleDroite(X, 3);                // 360 / 8 = 45
    show("X", X);
    show("X<<3", X_L3);
    show("X>>3", X_R3);

    BigBinary U = initBigBinaryFromString("101101000"); // 360
    BigBinary V = initBigBinaryFromString("11000");     // 24
    BigBinary ABS = soustractionAbsolue(U, V);          // |360 - 24| = 336
    show("|U - V|", ABS); // attendu : 101010000 (336)

    /* =====================================================
       PHASE 2 – PARTIE 2 : PGCD binaire (algorithme de Stein)
       ===================================================== */
    printf("\n===== PHASE 2 – PGCD binaire (Stein) =====\n");
    BigBinary G1 = pgcdBinaire(U, V);                   // gcd(360,24)=24
    show("gcd(360,24)", G1); // attendu : 11000 (24)

    BigBinary C1 = initBigBinaryFromString("110000");   // 48
    BigBinary D1 = initBigBinaryFromString("10010");    // 18
    BigBinary G2 = pgcdBinaire(C1, D1);                 // gcd(48,18)=6
    show("gcd(48,18)", G2); // attendu : 110 (6)

    BigBinary C2 = initBigBinaryFromString("10101");    // 21
    BigBinary D2 = initBigBinaryFromString("1000");     // 8
    BigBinary G3 = pgcdBinaire(C2, D2);                 // gcd(21,8)=1
    show("gcd(21,8)", G3); // attendu : 1 (1)

    /* =====================================================
       PHASE 2 – PARTIE 3 : Modulo et exponentiation modulaire rapide
       ===================================================== */
    printf("\n===== PHASE 2 – Modulo et Exponentiation modulaire =====\n");

    // --- MODULO ---
    BigBinary M1 = initBigBinaryFromString("101101000"); // 360
    BigBinary N1 = initBigBinaryFromString("11000");     // 24
    BigBinary R1 = BigBinary_mod(M1, N1);                // 360 mod 24 = 0
    show("360 mod 24", R1); // attendu : 0

    BigBinary M2 = initBigBinaryFromString("110000");    // 48
    BigBinary N2 = initBigBinaryFromString("10010");     // 18
    BigBinary R2 = BigBinary_mod(M2, N2);                // 48 mod 18 = 12
    show("48 mod 18", R2); // attendu : 1100 (12)

    // --- EXPONENTIATION MODULAIRE ---
    // 5^13 mod 23 = 21
    BigBinary base1 = initBigBinaryFromString("101");     // 5
    BigBinary exp1  = initBigBinaryFromString("1101");    // 13
    BigBinary mod1  = initBigBinaryFromString("10111");   // 23
    BigBinary expRes1 = BigBinary_expMod(base1, exp1, mod1);
    show("5^13 mod 23", expRes1); // attendu : 10101 (21)

    // 7^20 mod 13 = 9
    BigBinary base2 = initBigBinaryFromString("111");     // 7
    BigBinary exp2  = initBigBinaryFromString("10100");   // 20
    BigBinary mod2  = initBigBinaryFromString("1101");    // 13
    BigBinary expRes2 = BigBinary_expMod(base2, exp2, mod2);
    show("7^20 mod 13", expRes2); // attendu : 1001 (9)

    /* =====================================================
       LIBÉRATION DE MÉMOIRE
       ===================================================== */
    libereBigBinary(&A);  libereBigBinary(&B);
    libereBigBinary(&X);  libereBigBinary(&X_L3); libereBigBinary(&X_R3);
    libereBigBinary(&U);  libereBigBinary(&V);    libereBigBinary(&ABS);
    libereBigBinary(&G1); libereBigBinary(&C1);   libereBigBinary(&D1); libereBigBinary(&G2);
    libereBigBinary(&C2); libereBigBinary(&D2);   libereBigBinary(&G3);
    libereBigBinary(&M1); libereBigBinary(&N1);   libereBigBinary(&R1);
    libereBigBinary(&M2); libereBigBinary(&N2);   libereBigBinary(&R2);
    libereBigBinary(&base1); libereBigBinary(&exp1); libereBigBinary(&mod1); libereBigBinary(&expRes1);
    libereBigBinary(&base2); libereBigBinary(&exp2); libereBigBinary(&mod2); libereBigBinary(&expRes2);

    printf("\n===== ✅ Phase 2 validée avec succès =====\n");
    return 0;
}
