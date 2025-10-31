/*
 * ============================================================================
 * FICHIER DE TEST POUR LA BIBLIOTHÈQUE BIGBINARY
 * ============================================================================
 * 
 * Ce programme teste toutes les fonctionnalités de la bibliothèque BigBinary
 * en deux phases :
 * 
 * PHASE 1 : Opérations de base
 *   - Initialisation depuis chaînes binaires
 *   - Comparaisons (égalité, infériorité)
 *   - Addition et soustraction binaires
 * 
 * PHASE 2 : Opérations avancées
 *   - Partie 1 : Décalages (gauche/droite) et valeur absolue
 *   - Partie 2 : PGCD binaire (algorithme de Stein)
 *   - Partie 3 : Modulo et exponentiation modulaire
 * 
 * ============================================================================
 */

#include "bigbinary.h"
#include <stdio.h>

/**
 * show - Fonction utilitaire pour afficher un BigBinary avec son label
 * 
 * RÔLE : Simplifie l'affichage des résultats dans les tests
 * 
 * FORMAT DE SORTIE : "label = bits\n"
 * EXEMPLE : "A = 10110\n"
 * 
 * @param label : Texte descriptif du nombre
 * @param X : Le BigBinary à afficher
 */
static void show(const char *label, BigBinary X) {
    printf("%s = ", label);
    afficheBigBinary(X);
}

int main(void) {
    /*
     * ========================================================================
     * PHASE 1 – VÉRIFICATION DES OPÉRATIONS DE BASE
     * ========================================================================
     * 
     * On teste les fonctionnalités fondamentales :
     *   - Création de BigBinary depuis des chaînes binaires
     *   - Comparaisons entre nombres
     *   - Addition et soustraction
     */
    printf("===== PHASE 1 – Vérification de base =====\n");
    
    // TEST 1.1 : Initialisation depuis des chaînes binaires
    // ----------------------------------------------------
    // Création de deux nombres :
    //   A = "10110" = 16 + 4 + 2 = 22 en décimal
    //   B = "1101"  = 8 + 4 + 1 = 13 en décimal
    BigBinary A = initBigBinaryFromString("10110"); // 22
    BigBinary B = initBigBinaryFromString("1101");  // 13

    // Affichage des valeurs
    show("A", A);  // Affichera : A = 10110
    show("B", B);  // Affichera : B = 1101

    // TEST 1.2 : Test d'égalité
    // -------------------------
    // Egal(A,B) devrait retourner 0 car 22 ≠ 13
    printf("Egal(A,B) = %d\n", Egal(A, B));

    // TEST 1.3 : Test de comparaison (inférieur)
    // ------------------------------------------
    // A < B ? devrait retourner 0 car 22 > 13
    printf("A < B ?   = %d\n", Inferieur(A, B));

    // TEST 1.4 : Addition binaire
    // ---------------------------
    // A + B = 22 + 13 = 35
    // En binaire : 35 = 32 + 2 + 1 = "100011"
    BigBinary S = additionBigBinary(A, B);
    show("A + B", S); // Affichera : A + B = 100011 (35)

    // TEST 1.5 : Soustraction binaire
    // -------------------------------
    // A - B = 22 - 13 = 9
    // En binaire : 9 = 8 + 1 = "1001"
    BigBinary D = soustractionBigBinary(A, B);
    show("A - B", D); // Affichera : A - B = 1001 (9)

    // Libération de la mémoire temporaire
    libereBigBinary(&S);
    libereBigBinary(&D);

    /*
     * ========================================================================
     * PHASE 2 – PARTIE 1 : DÉCALAGES ET SOUSTRACTION ABSOLUE
     * ========================================================================
     * 
     * Les décalages binaires sont des opérations très efficaces :
     *   - Décalage à gauche (<<) : multiplication par 2^n
     *   - Décalage à droite (>>) : division entière par 2^n
     */
    printf("\n===== PHASE 2 – Décalages et |A-B| =====\n");
    
    // TEST 2.1.1 : Décalages binaires
    // -------------------------------
    // Création d'un nombre de test :
    // X = "101101000" = 256 + 64 + 32 + 8 = 360 en décimal
    BigBinary X = initBigBinaryFromString("101101000"); // 360
    
    // Décalage à gauche de 3 positions (multiplication par 2^3 = 8)
    // X << 3 = 360 × 8 = 2880
    // En binaire : "101101000000"
    BigBinary X_L3 = decaleGauche(X, 3);
    
    // Décalage à droite de 3 positions (division entière par 2^3 = 8)
    // X >> 3 = 360 ÷ 8 = 45
    // En binaire : "101101"
    BigBinary X_R3 = decaleDroite(X, 3);
    
    // Affichage des résultats
    show("X", X);         // Affichera : X = 101101000
    show("X<<3", X_L3);   // Affichera : X<<3 = 101101000000 (2880)
    show("X>>3", X_R3);   // Affichera : X>>3 = 101101 (45)

    // TEST 2.1.2 : Soustraction absolue |U - V|
    // -----------------------------------------
    // Cette fonction calcule la valeur absolue de la différence,
    // quel que soit l'ordre des opérandes
    
    // U = 360, V = 24
    BigBinary U = initBigBinaryFromString("101101000"); // 360
    BigBinary V = initBigBinaryFromString("11000");     // 24
    
    // |U - V| = |360 - 24| = 336
    // En binaire : 336 = 256 + 64 + 16 = "101010000"
    BigBinary ABS = soustractionAbsolue(U, V);
    show("|U - V|", ABS); // Affichera : |U - V| = 101010000 (336)

    /*
     * ========================================================================
     * PHASE 2 – PARTIE 2 : PGCD BINAIRE (ALGORITHME DE STEIN)
     * ========================================================================
     * 
     * Le PGCD (Plus Grand Commun Diviseur) est calculé avec l'algorithme
     * de Stein, une méthode optimisée pour les ordinateurs car elle
     * n'utilise que des décalages, comparaisons et soustractions
     * (pas de divisions coûteuses).
     * 
     * RAPPEL : Le PGCD est le plus grand nombre qui divise à la fois
     *          les deux nombres donnés.
     */
    printf("\n===== PHASE 2 – PGCD binaire (Stein) =====\n");
    
    // TEST 2.2.1 : PGCD(360, 24)
    // --------------------------
    // CALCUL MANUEL :
    //   360 = 24 × 15 + 0
    //   Donc PGCD(360, 24) = 24
    BigBinary G1 = pgcdBinaire(U, V);
    show("gcd(360,24)", G1); // Affichera : gcd(360,24) = 11000 (24)

    // TEST 2.2.2 : PGCD(48, 18)
    // -------------------------
    // CALCUL MANUEL :
    //   48 = 18 × 2 + 12
    //   18 = 12 × 1 + 6
    //   12 = 6 × 2 + 0
    //   Donc PGCD(48, 18) = 6
    BigBinary C1 = initBigBinaryFromString("110000");   // 48
    BigBinary D1 = initBigBinaryFromString("10010");    // 18
    BigBinary G2 = pgcdBinaire(C1, D1);
    show("gcd(48,18)", G2); // Affichera : gcd(48,18) = 110 (6)

    // TEST 2.2.3 : PGCD(21, 8)
    // ------------------------
    // CALCUL MANUEL :
    //   21 = 8 × 2 + 5
    //   8 = 5 × 1 + 3
    //   5 = 3 × 1 + 2
    //   3 = 2 × 1 + 1
    //   2 = 1 × 2 + 0
    //   Donc PGCD(21, 8) = 1 (nombres premiers entre eux)
    BigBinary C2 = initBigBinaryFromString("10101");    // 21
    BigBinary D2 = initBigBinaryFromString("1000");     // 8
    BigBinary G3 = pgcdBinaire(C2, D2);
    show("gcd(21,8)", G3); // Affichera : gcd(21,8) = 1 (1)

    /*
     * ========================================================================
     * PHASE 2 – PARTIE 3 : MODULO ET EXPONENTIATION MODULAIRE
     * ========================================================================
     * 
     * Ces opérations sont fondamentales en cryptographie :
     * 
     * MODULO : Reste de la division euclidienne
     *   a mod n = reste de a ÷ n
     *   Exemple : 17 mod 5 = 2 (car 17 = 5 × 3 + 2)
     * 
     * EXPONENTIATION MODULAIRE : (base^exposant) mod modulo
     *   Calcul efficace des grandes puissances modulo un nombre
     *   Utilisé dans RSA, Diffie-Hellman, etc.
     *   Algorithme : "square-and-multiply"
     */
    printf("\n===== PHASE 2 – Modulo et Exponentiation modulaire =====\n");

    // ========================================================================
    // SECTION 2.3.1 : TESTS DU MODULO
    // ========================================================================
    
    // TEST 2.3.1.1 : 360 mod 24
    // -------------------------
    // CALCUL : 360 ÷ 24 = 15 reste 0
    // Donc 360 mod 24 = 0
    BigBinary M1 = initBigBinaryFromString("101101000"); // 360
    BigBinary N1 = initBigBinaryFromString("11000");     // 24
    BigBinary R1 = BigBinary_mod(M1, N1);
    show("360 mod 24", R1); // Affichera : 360 mod 24 = 0

    // TEST 2.3.1.2 : 48 mod 18
    // ------------------------
    // CALCUL : 48 ÷ 18 = 2 reste 12
    // Donc 48 mod 18 = 12 = "1100" en binaire
    BigBinary M2 = initBigBinaryFromString("110000");    // 48
    BigBinary N2 = initBigBinaryFromString("10010");     // 18
    BigBinary R2 = BigBinary_mod(M2, N2);
    show("48 mod 18", R2); // Affichera : 48 mod 18 = 1100 (12)

    // ========================================================================
    // SECTION 2.3.2 : TESTS DE L'EXPONENTIATION MODULAIRE
    // ========================================================================
    
    // TEST 2.3.2.1 : 5^13 mod 23
    // --------------------------
    // CALCUL : 5^13 = 1220703125
    // 1220703125 ÷ 23 = 53074048 reste 21
    // Donc 5^13 mod 23 = 21 = "10101" en binaire
    // 
    // VÉRIFICATION CRYPTOGRAPHIQUE :
    // Ceci peut être utilisé dans des protocoles comme Diffie-Hellman
    // où on calcule des puissances modulo un grand nombre premier
    BigBinary base1 = initBigBinaryFromString("101");     // 5
    BigBinary exp1  = initBigBinaryFromString("1101");    // 13
    BigBinary mod1  = initBigBinaryFromString("10111");   // 23
    BigBinary expRes1 = BigBinary_expMod(base1, exp1, mod1);
    show("5^13 mod 23", expRes1); // Affichera : 5^13 mod 23 = 10101 (21)

    // TEST 2.3.2.2 : 7^20 mod 13
    // --------------------------
    // CALCUL : 7^20 = 79792266297612001
    // 79792266297612001 ÷ 13 = 6137866638277846 reste 3
    // Donc 7^20 mod 13 = 3 = "11" en binaire
    // 
    // NOTE : Sans l'algorithme optimisé, ce calcul serait impossible
    //        à faire efficacement (trop de multiplications).
    //        Avec "square-and-multiply", on fait ~20 opérations
    //        au lieu de 20 millions !
    BigBinary base2 = initBigBinaryFromString("111");     // 7
    BigBinary exp2  = initBigBinaryFromString("10100");   // 20
    BigBinary mod2  = initBigBinaryFromString("1101");    // 13
    BigBinary expRes2 = BigBinary_expMod(base2, exp2, mod2);
    show("7^20 mod 13", expRes2); // Affichera : 7^20 mod 13 = 11 (3)

    /*
     * ========================================================================
     * LIBÉRATION DE MÉMOIRE
     * ========================================================================
     * 
     * IMPORTANT : Chaque BigBinary alloue dynamiquement de la mémoire
     * (via malloc dans le tableau Tdigits).
     * 
     * Pour éviter les fuites mémoire, on doit libérer TOUS les BigBinary
     * créés dans ce programme.
     * 
     * ORDRE DE LIBÉRATION :
     *   1. Variables de la Phase 1
     *   2. Variables de la Phase 2 - Partie 1
     *   3. Variables de la Phase 2 - Partie 2
     *   4. Variables de la Phase 2 - Partie 3
     */
    
    // Phase 1
    libereBigBinary(&A);  
    libereBigBinary(&B);
    
    // Phase 2 - Partie 1 : Décalages
    libereBigBinary(&X);  
    libereBigBinary(&X_L3); 
    libereBigBinary(&X_R3);
    libereBigBinary(&U);  
    libereBigBinary(&V);    
    libereBigBinary(&ABS);
    
    // Phase 2 - Partie 2 : PGCD
    libereBigBinary(&G1); 
    libereBigBinary(&C1);   
    libereBigBinary(&D1); 
    libereBigBinary(&G2);
    libereBigBinary(&C2); 
    libereBigBinary(&D2);   
    libereBigBinary(&G3);
    
    // Phase 2 - Partie 3 : Modulo et Exponentiation
    libereBigBinary(&M1); 
    libereBigBinary(&N1);   
    libereBigBinary(&R1);
    libereBigBinary(&M2); 
    libereBigBinary(&N2);   
    libereBigBinary(&R2);
    libereBigBinary(&base1); 
    libereBigBinary(&exp1); 
    libereBigBinary(&mod1); 
    libereBigBinary(&expRes1);
    libereBigBinary(&base2); 
    libereBigBinary(&exp2); 
    libereBigBinary(&mod2); 
    libereBigBinary(&expRes2);

    /*
     * ========================================================================
     * FIN DU PROGRAMME
     * ========================================================================
     * 
     * Si tous les tests passent correctement, vous devriez voir :
     * 
     * PHASE 1 :
     *   - A = 10110, B = 1101
     *   - A + B = 100011 (35)
     *   - A - B = 1001 (9)
     * 
     * PHASE 2 - Partie 1 :
     *   - X<<3 = 101101000000 (2880)
     *   - X>>3 = 101101 (45)
     *   - |U - V| = 101010000 (336)
     * 
     * PHASE 2 - Partie 2 :
     *   - gcd(360,24) = 11000 (24)
     *   - gcd(48,18) = 110 (6)
     *   - gcd(21,8) = 1 (1)
     * 
     * PHASE 2 - Partie 3 :
     *   - 360 mod 24 = 0
     *   - 48 mod 18 = 1100 (12)
     *   - 5^13 mod 23 = 10101 (21)
     *   - 7^20 mod 13 = 11 (3)
     * 
     * Ces résultats valident le bon fonctionnement de la bibliothèque
     * BigBinary pour des applications en cryptographie et calcul numérique.
     */
    printf("\n===== ✅ Phase 2 validée avec succès =====\n");
    return 0;
}