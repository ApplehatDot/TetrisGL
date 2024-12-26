/*		TetrisGL
 *  Tetris.h - Plik nagłówkowy logiki [Tetris.c]
 *
 */
 
#ifndef _TETRIS_H
#define _TETRIS_H

typedef struct {
    int shape[4][4][2];  // Współrzędne (x, y) dla każdej rotacji i punktu
    int rotation;        // Aktualna rotacja (0, 1, 2, 3)
    float bx, by;        // Pozycja bazowa bloku
} Block;

extern Block RL_BLOCK;
extern Block LL_BLOCK;
extern Block T_BLOCK;
extern Block O_BLOCK;
extern Block I_BLOCK;
extern Block RZ_BLOCK;
extern Block LZ_BLOCK;
extern Block* currentBlock;
extern Block* prevBlock[100];
extern int gameBoard[10][20];

/** Funkcja Rysująca Block *, wedle jej struktury
 * @param *block Struktura budowy bloku, według jej opcji.
 * @param offset odstęp między blokami.
 * @param BlockDrawSize Rozmiar rysowanych bloków 
 */
void DrawBlock(Block *block, float offset, float BlockDrawSize);

/** Funkcja Obracająca podany Blok (Block *block)
 * @param Struktura Bloku
 */
void RotateBlock(Block *block);

/** Funkcja losowania bloków
 * @return wybiera blok i go ustawia w 'currentBlock'
 */
void ChooseRandomBlock();

/** Przekazuje wartości (From) do podanej wartości (To)
 * @param *From [block] wartość z której będzie przekazywane
 * @param *To [block] wartość do której przekazane wartości przybędą
 */
void PassdownValues();

/** Sprawdza czy blok może być w danym miejscu (game_map)
 * @param *cb Blok, z którego będą odczytywane parametry
 * @param offset liczba przecinkowa z odstępem 
 * @return kiedy jest jako True, blok nie może przejść
 */
bool CanPlaceBlock(Block *cb, float offset);

int GetBlockStatus(int wX, int wY);


#endif // _TETRIS_H