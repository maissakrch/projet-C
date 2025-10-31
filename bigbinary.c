#include "bigbinary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===========================================================
 *  Helpers internes
 * ===========================================================
 */

/*
 * normalizeBigBinary :
 *  — Cette fonction "nettoie" la représentation binaire.
 *  — Elle supprime les zéros inutiles au début (MSB = bit le plus à gauche)
 *  — Si la valeur vaut 0, elle force une représentation canonique :
 *        Taille = 1, Tdigits = {0}, Signe = 0
 */
static void normalizeBigBinary(BigBinary *A) {

    // Sécurité : si le pointeur est NULL → rien à faire
    if (A == NULL) return;

    // Cas où structure mal formée ou vide → on force à 0
    if (A->Tdigits == NULL || A->Taille <= 0) {

        // Libère (par sécurité) l'ancien tableau, même si NULL
        free(A->Tdigits);

        // On réalloue un seul entier = 0
        A->Tdigits = (int*)malloc(sizeof(int));
        A->Tdigits[0] = 0;
        A->Taille = 1;
        A->Signe = 0;
        return;
    }

    // On cherche le premier bit différent de 0 (pour enlever les zéros de tête)
    int i = 0;
    while (i < A->Taille - 1 && A->Tdigits[i] == 0)
        i++;

    // Si on arrive au dernier bit et qu’il vaut 0 → nombre = 0
    if (i == A->Taille - 1 && A->Tdigits[i] == 0) {

        // On libère et on force la représentation canonique
        free(A->Tdigits);
        A->Tdigits = (int*)malloc(sizeof(int));
        A->Tdigits[0] = 0;
        A->Taille = 1;
        A->Signe = 0;
        return;
    }

    // Si i > 0 → il y avait des zéros en trop au début
    // On compacte le tableau pour enlever ces zéros
    if (i > 0) {
        int newLen = A->Taille - i;

        // On réalloue un tableau plus petit
        int *nd = (int*)malloc(newLen * sizeof(int));

        // copie les bits non-nuls restants
        memcpy(nd, A->Tdigits + i, newLen * sizeof(int));

        // libère l'ancien tableau
        free(A->Tdigits);

        // met à jour la structure
        A->Tdigits = nd;
        A->Taille = newLen;
    }

    // Convention déjà gérée : si 0 → signe = positif
}


/* ===========================================================
 * Phase 1 — Base : création, affichage, égalité, comparaison
 * ===========================================================
 */

/*
 * initBigBinary :
 *  → initialise un BigBinary à zéro (0 canonique)
 */
BigBinary initBigBinary() {
    BigBinary A;

    // On alloue un seul bit
    A.Tdigits = (int*)malloc(sizeof(int));

    // Ce bit vaut 0
    A.Tdigits[0] = 0;

    // Taille = 1 (1 bit, c’est le 0)
    A.Taille = 1;

    // Signe = 0 = positif
    A.Signe  = 0;
    return A;
}

/*
 * initBigBinaryFromString :
 *  → Construire un BigBinary à partir d’une chaîne de caractères binaire
 *  → Ignore espaces, accepte un signe optionnel + ou -
 */
BigBinary initBigBinaryFromString(const char *str) {

    // Cas chaîne inexistante → renvoie 0
    if (str == NULL || strlen(str) == 0) {
        return initBigBinary();
    }

    int signe = 0; // 0 = positif
    int i0 = 0;    // index de parcours

    // On saute les blancs au début
    while (str[i0] == ' ' || str[i0] == '\t' || str[i0] == '\n')
        i0++;

    // On lit le signe éventuel
    if (str[i0] == '-') {
        signe = 1;   // note : dans la phase 1 on ignore le signe, mais on le stocke
        i0++;
    }
    else if (str[i0] == '+') {
        i0++;
    }

    // On compte combien de caractères utiles : uniquement '0' ou '1'
    int count = 0;
    for (int i = i0; str[i] != '\0'; ++i) {

        // Si 0 ou 1 → OK
        if (str[i] == '0' || str[i] == '1')
            count++;

        // On ignore les espaces
        else if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
            continue;

        // Si autre caractère → erreur
        else {
            fprintf(stderr, "Erreur: caractère invalide '%c' dans la chaîne binaire\n", str[i]);
            return initBigBinary();
        }
    }

    // Si aucun bit valide → renvoie 0
    if (count == 0) {
        return initBigBinary();
    }

    // On crée le BigBinary
    BigBinary A;
    A.Taille  = count;    // nombre de bits trouvés
    A.Signe   = signe;    // on stocke le signe
    A.Tdigits = (int*)malloc(A.Taille * sizeof(int));

    // On remplit le tableau avec les bits trouvés
    int k = 0;
    for (int i = i0; str[i] != '\0'; ++i) {
        if (str[i] == '0' || str[i] == '1') {
            A.Tdigits[k++] = (str[i] == '1') ? 1 : 0;
        }
    }

    // On normalise pour supprimer les éventuels zéros de tête
    normalizeBigBinary(&A);
    return A;
}

/*
 * afficheBigBinary :
 *  → affiche le nombre binaire (options : affiche le signe s'il est négatif)
 */
void afficheBigBinary(const BigBinary A) {

    // Si signe négatif → imprime '-'
    if (A.Signe)
        printf("-");

    // Parcourt tous les bits et les imprime
    for (int i = 0; i < A.Taille; ++i)
        printf("%d", A.Tdigits[i]);

    printf("\n");
}

/*
 * libereBigBinary :
 *  → libère la mémoire du tableau Tdigits
 *  → remet la structure à zéro
 */
void libereBigBinary(BigBinary *A) {

    // Si structure invalide → rien
    if (!A) return;

    // Si le tableau existe → on libère
    if (A->Tdigits)
        free(A->Tdigits);

    // On remet tout à zéro
    A->Tdigits = NULL;
    A->Taille  = 0;
    A->Signe   = 0;
}

/*
 * Egal :
 *  → Vérifie si deux BigBinary sont exactement identiques
 *  → même taille, même signe, mêmes bits
 */
int Egal(const BigBinary A, const BigBinary B) {

    // Si tailles ou signe différents → déjà faux
    if (A.Taille != B.Taille) return 0;
    if (A.Signe  != B.Signe)  return 0;

    // On compare chaque bit
    for (int i = 0; i < A.Taille; ++i) {
        if (A.Tdigits[i] != B.Tdigits[i])
            return 0;
    }
    return 1; // sinon égaux
}

/*
 * Inferieur :
 *  → Compare A < B (non signé)
 */
int Inferieur(const BigBinary A, const BigBinary B) {

    // Le plus court (moins de bits) est le plus petit
    if (A.Taille < B.Taille) return 1;
    if (A.Taille > B.Taille) return 0;

    // Même taille → on compare bit à bit depuis MSB
    for (int i = 0; i < A.Taille; ++i) {

        if (A.Tdigits[i] < B.Tdigits[i])
            return 1;

        if (A.Tdigits[i] > B.Tdigits[i])
            return 0;
    }

    // Si tous les bits identiques → A n’est pas strictement < B
    return 0;
}

/*
 * additionBigBinary :
 *  → Additionne A + B en binaire (MSB-first)
 */
BigBinary additionBigBinary(const BigBinary A, const BigBinary B) {

    // n = taille du plus grand des deux
    int n = (A.Taille > B.Taille) ? A.Taille : B.Taille;

    BigBinary R;

    // On ajoute 1 bit pour stocker une éventuelle retenue finale
    R.Taille  = n + 1;
    R.Signe   = 0;

    // calloc initialise à 0
    R.Tdigits = (int*)calloc(R.Taille, sizeof(int));

    int carry = 0; // stockage de la retenue

    // On additionne depuis la droite (LSB)
    for (int i = 0; i < n; ++i) {

        // On récupère les bits depuis la droite
        int abit = (A.Taille - 1 - i >= 0) ? A.Tdigits[A.Taille - 1 - i] : 0;
        int bbit = (B.Taille - 1 - i >= 0) ? B.Tdigits[B.Taille - 1 - i] : 0;

        // Somme du bit + éventuelle retenue
        int sum = abit + bbit + carry;

        // On stocke le bit de poids faible de la somme
        R.Tdigits[R.Taille - 1 - i] = sum & 1;

        // On calcule la nouvelle retenue (sum >> 1 = division par 2)
        carry = (sum >> 1);
    }

    // Le bit le plus à gauche = la dernière retenue
    R.Tdigits[0] = carry;

    // On normalise pour retirer un éventuel zéro en tête
    normalizeBigBinary(&R);
    return R;
}
/* ===========================================================
 *  Soustraction binaire
 * ===========================================================
 */

/*
 * soustractionBigBinary :
 *   → Calcule (A - B) en binaire
 *   → Hypothèse : A ≥ B (sinon on affiche une erreur et retourne 0)
 *   → Fonctionne en faisant une soustraction bit par bit
 *     avec emprunts (borrow) si nécessaire.
 */
BigBinary soustractionBigBinary(const BigBinary A, const BigBinary B) {

    // Si A < B → erreur (précondition non respectée)
    if (Inferieur(A, B)) {
        fprintf(stderr, "ERREUR: A < B dans soustractionBigBinary (précondition non respectée)\n");
        return initBigBinary(); // retourne 0
    }

    BigBinary R;

    // Le résultat ne peut pas être plus long que A
    R.Taille  = A.Taille;
    R.Signe   = 0;

    // calloc → initialise tout à 0
    R.Tdigits = (int*)calloc(R.Taille, sizeof(int));

    int borrow = 0; // flag d’emprunt

    /*
     * On parcourt de droite à gauche (LSB-first par indexage inverse)
     * Pour chaque bit :
     *   diff = bit(A) - bit(B) - borrow
     *   si diff < 0 → on emprunte : diff += 2 et borrow = 1
     *   sinon borrow = 0
     */
    for (int i = 0; i < A.Taille; ++i) {

        int abit = A.Tdigits[A.Taille - 1 - i];
        int bbit = (B.Taille - 1 - i >= 0) ? B.Tdigits[B.Taille - 1 - i] : 0;

        int diff = abit - bbit - borrow;

        // Si négatif → emprunt
        if (diff < 0) {
            diff += 2;
            borrow = 1;
        } else {
            borrow = 0;
        }

        // On stocke le bit final
        R.Tdigits[R.Taille - 1 - i] = diff;
    }

    // On normalise : supprime potentiels zéros MSB
    normalizeBigBinary(&R);
    return R;
}

/* ===========================================================
 *  Phase 2 — Helpers & opérations étendues
 * ===========================================================
 */

/*
 * estZero :
 *   → Renvoie 1 si A vaut exactement 0
 *   → Vérifie que tous les bits du tableau valent 0
 */
int estZero(const BigBinary A) {
    for (int i = 0; i < A.Taille; ++i)
        if (A.Tdigits[i] != 0) return 0;
    return 1; // tous les bits valent 0
}

/*
 * estPair :
 *   → Renvoie 1 si A est pair (dernier bit = LSB = 0)
 *   → Dernier bit = A.Tdigits[A.Taille - 1]
 */
int estPair(const BigBinary A) {

    // sécurité
    if (A.Taille <= 0 || A.Tdigits == NULL)
        return 1;

    // Pair si le dernier bit vaut 0
    return (A.Tdigits[A.Taille - 1] == 0);
}

/*
 * copieBigBinary :
 *   → copie profonde du BigBinary A dans une nouvelle structure
 */
BigBinary copieBigBinary(const BigBinary A) {
    BigBinary C;

    C.Taille  = A.Taille;
    C.Signe   = A.Signe;

    // allocation d'un nouveau tableau pour copier les bits
    C.Tdigits = (int*)malloc(C.Taille * sizeof(int));

    // copie brute
    memcpy(C.Tdigits, A.Tdigits, C.Taille * sizeof(int));
    return C;
}

/*
 * decaleGauche :
 *   → Décale A de n bits vers la gauche
 *   → équivaut à multiplier par 2^n
 *   → concrètement : on ajoute n zéros à droite (LSB)
 */
BigBinary decaleGauche(const BigBinary A, int n) {

    // si pas besoin de décaler → on copie juste A
    if (n <= 0 || estZero(A))
        return copieBigBinary(A);

    BigBinary R;

    // nouvelle taille = ancienne + n
    R.Taille  = A.Taille + n;
    R.Signe   = A.Signe;

    // allocation
    R.Tdigits = (int*)malloc(R.Taille * sizeof(int));

    // copie l'ancien contenu
    memcpy(R.Tdigits, A.Tdigits, A.Taille * sizeof(int));

    // ajoute n ZERO à la fin
    memset(R.Tdigits + A.Taille, 0, n * sizeof(int));

    // nettoie
    normalizeBigBinary(&R);
    return R;
}

/*
 * decaleDroite :
 *   → Décale A de n bits à droite
 *   → équivaut à diviser par 2^n (partie entière)
 *   → Concrètement : on supprime les n derniers bits
 */
BigBinary decaleDroite(const BigBinary A, int n) {

    // si aucun décalage → copie
    if (n <= 0)
        return copieBigBinary(A);

    // si on décale plus que Taille → résultat = 0
    if (n >= A.Taille) {
        BigBinary Z = initBigBinary();
        return Z;
    }

    BigBinary R;
    R.Taille  = A.Taille - n;
    R.Signe   = A.Signe;

    // On alloue le tableau plus petit
    R.Tdigits = (int*)malloc(R.Taille * sizeof(int));

    // On copie tout sauf les n derniers bits
    memcpy(R.Tdigits, A.Tdigits, R.Taille * sizeof(int));

    // On normalise
    normalizeBigBinary(&R);
    return R;
}

/*
 * soustractionAbsolue :
 *   → Calcule |A - B| (valeur absolue de la différence)
 *   → Si A < B → on fait (B - A)
 *     Sinon    → (A - B)
 */
BigBinary soustractionAbsolue(const BigBinary A, const BigBinary B) {

    // Si A < B → on retourne (B - A)
    if (Inferieur(A, B)) {
        return soustractionBigBinary(B, A);
    }
    // Sinon → retour (A - B)
    else {
        return soustractionBigBinary(A, B);
    }
}
/*
 * countTrailingZeros :
 *   → Compte le nombre de zéros successifs à la FIN du nombre binaire
 *     côté LSB (bit le moins significatif, donc à droite)
 *
 *   Exemple :
 *      A = 101000  → trailing zeros = 3
 *      A = 1111000 → trailing zeros = 3
 *      A = 0       → trailing zeros = taille
 *
 *   — Utilisé dans l’algorithme du PGCD (version binaire de Stein)
 *     pour retirer des facteurs 2.
 */
static int countTrailingZeros(const BigBinary A) {
    int c = 0;

    // On parcourt depuis la droite jusqu’à ce qu’on rencontre un 1
    for (int i = A.Taille - 1; i >= 0; --i) {

        if (A.Tdigits[i] == 0)
            c++;
        else
            break;
    }
    return c;
}

/*
 * rshift1 :
 *   → Petit alias pratique pour decaleDroite(A,1)
 */
static BigBinary rshift1(const BigBinary A) {
    return decaleDroite(A, 1);
}

/*
 * lshiftK :
 *   → Petit alias pour decaleGauche(A,k)
 */
static BigBinary lshiftK(const BigBinary A, int k) {
    return decaleGauche(A, k);
}


/* ===========================================================
 *  pgcdBinaire — Algorithme de Stein
 * ===========================================================
 *
 *  But : calculer le PGCD(A,B) sans divisions classiques
 *        → uniquement décalages et soustractions
 *
 *  Propriétés utilisées :
 *   - si A et B sont pairs :
 *        G = 2 * PGCD(A/2, B/2)
 *
 *   - si A pair et B impair :
 *        PGCD(A, B) = PGCD(A/2, B)
 *
 *   - si A impair et B impair :
 *        PGCD(A, B) = PGCD(|A-B|/2, min(A,B))
 *
 *  L’algorithme retire donc les facteurs 2 en commun,
 *  puis réduit par soustractions successives.
 */
BigBinary pgcdBinaire(const BigBinary A, const BigBinary B) {

    // On copie A et B pour travailler dessus (sans modifier les originaux)
    BigBinary X = copieBigBinary(A); normalizeBigBinary(&X);
    BigBinary Y = copieBigBinary(B); normalizeBigBinary(&Y);

    // Si A == 0 → PGCD(0,B) = B
    if (estZero(X)) {
        libereBigBinary(&X); // free copie A
        return Y;            // retourne copie de B
    }

    // Si B == 0 → PGCD(A,0) = A
    if (estZero(Y)) {
        libereBigBinary(&Y); // free copie B
        return X;            // retourne copie de A
    }

    // On compte les zéros de fin de X et Y (facteur commun 2^k)
    int kx = countTrailingZeros(X);
    int ky = countTrailingZeros(Y);

    // Le facteur commun est 2^(min(kx,ky))
    int k  = (kx < ky) ? kx : ky;

    BigBinary t;

    /*
     * On divise X par 2^kx → supprime les kx derniers bits
     * On divise Y par 2^ky
     * Cela retire explicitement les facteurs 2 de A et B.
     */
    t = decaleDroite(X, kx); libereBigBinary(&X); X = t;
    t = decaleDroite(Y, ky); libereBigBinary(&Y); Y = t;

    // On rend X impair en continuant de diviser par 2 tant qu’il est pair
    while (estPair(X)) {
        t = rshift1(X);
        libereBigBinary(&X);
        X = t;
    }

    /*
     * Boucle principale :
     *   On rend Y impair puis on remplace Y ← |Y - X|
     *   jusqu’à ce que Y = 0
     */
    while (!estZero(Y)) {

        // Rendre Y impair
        while (estPair(Y)) {
            t = rshift1(Y);
            libereBigBinary(&Y);
            Y = t;
        }

        /*
         * On s’assure X ≤ Y
         * Si X > Y → on swap X,Y
         */
        if (Inferieur(Y, X) == 0 && Egal(Y, X) == 0) {
            // Si ni (Y<X) ni (Y==X) → alors X>Y donc swap
            if (Inferieur(X, Y) == 0) {
                BigBinary tmp = X;
                X = Y;
                Y = tmp;
            }
        } else if (Inferieur(X, Y) == 0) {
            // X >= Y → swap pour garder X ≤ Y
            BigBinary tmp = X;
            X = Y;
            Y = tmp;
        }

        // Y = |Y - X|
        t = soustractionAbsolue(Y, X);
        libereBigBinary(&Y);
        Y = t;
    }

    /*
     * Leur PGCD "impair" est X
     * On doit réappliquer les 2^k qu’on avait retirés au début
     */
    BigBinary G = lshiftK(X, k);

    // On libère X et Y (Y == 0)
    libereBigBinary(&X);
    libereBigBinary(&Y);

    // Nettoyage
    normalizeBigBinary(&G);
    return G;
}

/* ===========================================================
 *  BigBinary_mod
 * ===========================================================
 *
 *  → Calcule A mod B
 *  → Hypothèse : B > 0
 *  → Méthode :
 *      On aligne B sur A via décalages (B << k)
 *      Puis on soustrait à chaque fois que le résultat est ≥ B<<k
 *
 *  C’est une version lente du modulo
 *  (≈ comme un long division mais uniquement via - et <<)
 */
BigBinary BigBinary_mod(const BigBinary A, const BigBinary B) {

    // Cas interdit → mod par zéro
    if (estZero(B)) {
        fprintf(stderr, "Erreur: modulo par zero\n");
        return initBigBinary();
    }

    // R = copie de A → on va réduire R
    BigBinary R = copieBigBinary(A);

    // Si A < B → le reste = A
    if (Inferieur(R, B))
        return R;

    // décalage max possible = différence de tailles
    int maxShift = R.Taille - B.Taille;

    /*
     * On descend k depuis maxShift jusqu’à 0
     * À chaque k :
     *    - On calcule B << k
     *    - Si R ≥ (B << k) → R = R - (B << k)
     */
    for (int k = maxShift; k >= 0; --k) {

        BigBinary Bk = decaleGauche(B, k);

        // Si R >= B<<k
        if (!Inferieur(R, Bk)) {
            BigBinary tmp = soustractionBigBinary(R, Bk);
            libereBigBinary(&R);
            R = tmp;
        }

        // Libère B<<k
        libereBigBinary(&Bk);

        // Si R == 0 → inutile de continuer
        if (estZero(R))
            break;
    }

    // R est normalisé par soustraction
    return R;
}
/* ===========================================================
 *  Addition modulaire
 * ===========================================================
 *
 * add_mod(X, Y, mod)
 *   → Retourne (X + Y) mod mod
 *   → On fait simplement :
 *       s = X + Y
 *       r = s mod mod
 *   → Puis on libère la mémoire intermédiaire
 */
static BigBinary add_mod(const BigBinary X, const BigBinary Y, const BigBinary mod) {

    // Addition binaire X + Y
    BigBinary s = additionBigBinary(X, Y);

    // On réduit modulo mod
    BigBinary r = BigBinary_mod(s, mod);

    // Libère la somme intermédiaire
    libereBigBinary(&s);
    return r;
}

/* ===========================================================
 *  Décalage gauche modulaire
 * ===========================================================
 *
 * lshift1_mod(X, mod)
 *   → Retourne (X << 1) mod mod
 *   → Décalage gauche = multiplication par 2
 */
static BigBinary lshift1_mod(const BigBinary X, const BigBinary mod) {

    // Décalage gauche → multiplie par 2
    BigBinary d = decaleGauche(X, 1);

    // Réduction modulo mod
    BigBinary r = BigBinary_mod(d, mod);

    // On libère le résultat du décalage non réduit
    libereBigBinary(&d);

    return r;
}

/* ===========================================================
 *  Multiplication modulaire
 * ===========================================================
 *
 * BigBinary_mul_mod(X, Y, mod)
 *
 * But :
 *   → Calcule (X * Y) mod mod
 *
 * Méthode :
 *   → Algorithme similaire à "multiplication par décomposition binaire"
 *
 *     On écrit :
 *         Y = y0 + 2*y1 + 4*y2 + ...
 *
 *     Concrètement :
 *         res = 0
 *         tant que Y > 0 :
 *            si Y est impair
 *                res = (res + X) mod mod
 *            X = (X << 1) mod mod
 *            Y = Y >> 1
 *
 *   → C’est l’équivalent binaire de la multiplication scolaire
 *     mais en ne faisant que :
 *        +, <<, >>, mod
 *
 *   → Très utile pour gros entiers et opérations cryptographiques
 */
static BigBinary BigBinary_mul_mod(const BigBinary X, const BigBinary Y, const BigBinary mod) {

    // a = X mod mod
    BigBinary a = BigBinary_mod(X, mod);

    // b = copie de Y
    BigBinary b = copieBigBinary(Y);

    // res = 0
    BigBinary res = initBigBinary();

    /*
     * Tant que b != 0
     * On va analyser b bit par bit (par shifting)
     */
    while (!estZero(b)) {

        // Si b impair → on ajoute a au résultat
        if (!estPair(b)) {
            BigBinary tmp = add_mod(res, a, mod);

            // remplace res = tmp
            libereBigBinary(&res);
            res = tmp;
        }

        // a = (a * 2) mod mod = a << 1 mod mod
        BigBinary a2 = lshift1_mod(a, mod);
        libereBigBinary(&a);
        a = a2;

        // b >>= 1 (division entière par 2)
        BigBinary b2 = decaleDroite(b, 1);
        libereBigBinary(&b);
        b = b2;
    }

    // Nettoyage
    libereBigBinary(&a);
    libereBigBinary(&b);

    return res;
}


/* ===========================================================
 *  Conversion BigBinary → uint64
 * ===========================================================
 *
 * to_u64(E, &out):
 *   → Convertit un BigBinary en entier 64 bits (unsigned long long)
 *
 *   ⚠ Limite :
 *      - Si plus de 64 bits → retourne 0 (échec)
 *
 *   → Utilisée pour l’exposant de expMod
 */
static int to_u64(const BigBinary E, unsigned long long *out) {

    // Trop grand → conversion impossible
    if (E.Taille > 64)
        return 0;

    unsigned long long v = 0ULL;

    // On parcourt les bits en MSB-first
    for (int i = 0; i < E.Taille; ++i) {

        // on décale v pour faire de la place
        v = (v << 1)
            // puis on ajoute le bit actuel
          | (unsigned long long)(E.Tdigits[i] ? 1 : 0);
    }

    *out = v;
    return 1;
}


/* ===========================================================
 *  Exponentiation modulaire (square-and-multiply)
 * ===========================================================
 *
 * BigBinary_expMod(M, exp, mod):
 *   → Calcule (M^exp) mod mod
 *
 *   → Principe :
 *
 *     On utilise l’exponentiation rapide :
 *
 *       result = 1
 *       base   = M mod mod
 *
 *       tant que exp > 0 :
 *          si (exp est impair)
 *              result = (result * base) mod mod
 *
 *          base = (base * base) mod mod
 *          exp >>= 1
 *
 *   → Avantages :
 *       - O(log exp)
 *       - Pas besoin de calculer M^exp en entier
 *       - Très utilisé en cryptographie (RSA, Diffie-Hellman…)
 *
 *   ⚠ exp est converti en*

