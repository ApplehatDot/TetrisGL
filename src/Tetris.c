#include <windows.h>
#include <GL/glut.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 24
#define BlockSize 10.0

// leftX rightX
#define lX 12.0f
#define rX 140.0f

// upperY, downerY
#define uY 532.0f
#define dY 256.0f

// Struktura dla bloku w stylu Tetrisa
typedef struct {
    int shape[4][4][2];  // Współrzędne (x, y) dla każdej rotacji i każdego punktu
    int rotation;        // Aktualna rotacja (0, 1, 2, 3)
    float bx, by;        // Pozycja podstawowa bloku
} Block;

Block* prevBlock[100] = {NULL};
Block* currentBlock = NULL;


/*
 * 		SPOSÓB RYSOWANIA BLOKÓW [Block.shape[4][4][2]]
 * Patrz ewentualnie w pliku dokumentacji
 */

// Prawostronny L-block
Block RL_BLOCK = { 
    .shape = {
        { {0, 0}, {1, 0}, {0, 1}, {0, 2} },    // 0° rotacja (początkowa forma)
        { {0, 0}, {0, -1}, {1, 0}, {2, 0} },   // 90° rotacja
        { {0, 0}, {0, -1}, {0, -2}, {-1, 0} }, // 180° rotacja
        { {0, 0}, {-1, 0}, {-2, 0}, {0, 1} }   // 270° rotacja
    }, 
    .rotation = 0,  // Początkowa rotacja
    .bx = 5.0f, .by = 10.0f  // Pozycja bloku
};

// Lewostronny L-block
Block LL_BLOCK = { 
    .shape = {
        { {0, 0}, {-1, 0}, {0, 1}, {0, 2} },   // 0° rotacja (początkowa forma)
        { {0, 0}, {0, -1}, {-1, 0}, {-2, 0} },  // 90° rotacja
        { {0, 0}, {0, -1}, {0, -2}, {1, 0} },   // 180° rotacja
        { {0, 0}, {1, 0}, {2, 0}, {0, 1} }      // 270° rotacja
    }, 
    .rotation = 0,  // Początkowa rotacja
    .bx = 5.0f, .by = 10.0f  // Pozycja bloku
};

// T-block
Block T_BLOCK = {
    .shape = {
        {{0, 0}, {-1, 0}, {1, 0}, {0, 1}},     // 0°
        {{0, 0}, {0, 1}, {0, -1}, {1, 0}},     // 90°
        {{0, 0}, {-1, 0}, {1, 0}, {0, -1}},    // 180°
        {{0, 0}, {0, 1}, {0, -1}, {-1, 0}}     // 270°
    },
    .rotation = 0,
    .bx = 5.0f,
    .by = 10.0f
};

// Lewostronny Z-block
Block LZ_BLOCK = {
    .shape = {
        {{0, 0}, {1, 0}, {0, 1}, {-1, 1}},     // 0°
        {{0, 0}, {0, -1}, {1, 0}, {1, 1}},     // 90°
        {{0, 0}, {1, 0}, {0, 1}, {-1, 1}},     // 180° (taka sama jak 0°)
        {{0, 0}, {0, -1}, {1, 0}, {1, 1}}      // 270° (taka sama jak 90°)
    },
    .rotation = 0,
    .bx = 5.0f,
    .by = 10.0f
};

// Prawostronny Z-Block
Block RZ_BLOCK = {
    .shape = {
        {{0, 0}, {-1, 0}, {0, 1}, {1, 1}},     // 0°
        {{0, 0}, {0, 1}, {1, 0}, {1, -1}},     // 90°
        {{0, 0}, {-1, 0}, {0, 1}, {1, 1}},     // 180° (taka sama jak 0°)
        {{0, 0}, {0, 1}, {1, 0}, {1, -1}}      // 270° (taka sama jak 90°)
    },
    .rotation = 0,
    .bx = 5.0f,
    .by = 10.0f
};

// 4-Blokowy I
Block I_BLOCK = {
    .shape = {
        {{0, 0}, {0, 1}, {0, 2}, {0, 3}},      // 0° (pionowy)
        {{0, 0}, {1, 0}, {2, 0}, {3, 0}},      // 90° (poziomy)
        {{0, 0}, {0, 1}, {0, 2}, {0, 3}},      // 180° (taka sama jak 0°)
        {{0, 0}, {1, 0}, {2, 0}, {3, 0}}       // 270° (taka sama jak 90°)
    },
    .rotation = 0,
    .bx = 5.0f,
    .by = 10.0f
};

// Square-Block
Block O_BLOCK = {
    .shape = {
        {{0, 0}, {1, 0}, {0, 1}, {1, 1}},     // 0°
		
	/*	reszta sekcji są takie same jak w 0°	*/
        {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
        {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
        {{0, 0}, {1, 0}, {0, 1}, {1, 1}}
    },
    .rotation = 0,
    .bx = 5.0f,
    .by = 10.0f
};



// Lista Bloków
Block* List[] = { 
		&RL_BLOCK, &LL_BLOCK, &T_BLOCK,
		&LZ_BLOCK, &RZ_BLOCK, &I_BLOCK,
				&O_BLOCK
		}; 

void DrawBlock(Block *block, float offset, float BlockDrawSize) {
    glPointSize(BlockDrawSize);
    glBegin(GL_POINTS);
    for (int i = 0; i < 4; i++) {
        float x = block->bx + block->shape[block->rotation][i][0] * offset;
        float y = block->by + block->shape[block->rotation][i][1] * offset;
        glVertex2f(x, y);
    }
    glEnd();
    glFlush();
}

// funkcja obrotu:
//		Każde 'wezwanie' tej funkcji zmienia obrót bloku
void RotateBlock(Block *block) {
    block->rotation = (block->rotation + 1) % 4; 
}

void ChooseRandomBlock() {
    int totalBlocks = sizeof(List) / sizeof(List[0]);
    int randomIndex = rand() % totalBlocks;
    currentBlock = List[randomIndex];
}

// LOGIKA MIEJSCA BLOKU
			// 20			10
int gameBoard[BOARD_HEIGHT][BOARD_WIDTH] = {0}; // 0 - wolne, 1 - zajęte



int GetBlockStatus(int wX, int wY) {
    // Sprawdzenie, czy współrzędne są w granicach planszy
    if (wX >= 0 && wX < 10 && wY >= 0 && wY < 20) {
        return gameBoard[wY][wX];
    } else {
        // Jeśli poza granicami, zwróć np. -1 (oznacza "nieistniejące pole")
        return -1;
    }
}

float ReturnBlockX(Block *cb, float offset, int i){
	return cb->bx + cb->shape[cb->rotation][i][0] * offset;
}

float ReturnBlockY(Block *cb, float offset, int i){
	return cb->by + cb->shape[cb->rotation][i][1] * offset;
}

bool CanPlaceBlock( Block *cb, float offset) {
    for (int i = 0; i < 4; i++) {
        float ogX = cb->bx + cb->shape[cb->rotation][i][0] * offset;
        float ogY = cb->by + cb->shape[cb->rotation][i][1] * offset;

        int wX = (int)((ogX - lX) / offset) + 1; 
        int wY = (int)((ogY - dY) / offset) - 1; 

        if (GetBlockStatus(wX, wY) != 0) {
            return true; // Blok nie może być umieszczony
		}
    }
    return false; // Wszystkie pola są wolne
}





