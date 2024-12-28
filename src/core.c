
/*
 *	TetrisGL project - 02.11.2024
 *	Created by Applehat. (ApplehatDoesStuff)
 *
 */


/** 	TO DO
 *	- poprawić CanPlaceBlock by nie zwracał true przy boku lewym
 *	- ...
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <windows.h>
#include "dictionary.h"
#include "Tetris.h"

#define BlockSize 10.0

// leftX rightX
#define lX 32.0f
#define rX 128.0f
// upperY, downerY
#define uY 532.0f
#define dY 268.0f
// szerokość wysokość siatki
#define BOARD_WIDTH 10			// Wynik = (138 - 12) / 12
#define BOARD_HEIGHT 24			// Wynik = (532 - 244) / 12

#define MAX_BLOCKS 100

#define wWidth 280
#define wHeight 600

#define GX 80.0f
#define GY 448.0f
#define TIMER 700

float offset = 12.0f;
float LastX, LastY;
bool UnableMovement = false;

// Domyślne Wartości
float X = GX;
float Y = GY;
int Rotation = 0;
int iteration = 0;

#define BLOCK_LEFT_MOV 001
#define BLOCK_RIGHT_MOV 002
#define BLOCK_ABLE_MOV 120

int DetectBlockOOB(Block *cb){
	
	for (int i = 0; i < 4; i++) {
		float x = cb->bx + cb->shape[cb->rotation][i][0] * offset;
		
		// Sprawdzenie kolizji z lewą granicą
        if (x < lX) {
            return BLOCK_LEFT_MOV;
        }

        // Sprawdzenie kolizji z prawą granicą
        if (x >= rX) {
            return BLOCK_RIGHT_MOV;
        }

	}
	
	return BLOCK_ABLE_MOV;
}

void FreePrevBlocks() {
    for (int i = 0; i < MAX_BLOCKS; i++) {
        free(prevBlock[i]);
    }
}



// dla określenia pozycji bloku
void __DDI(){	// __DisplayDebugInfo

	float Letteroffset = 13.1f;
	
	glColor3f(255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f);
	char XY[24];
	char UnableMovementBool[32];
	char GameMap[64];
	
	// pozycje X i Y
	snprintf(XY, sizeof(XY), "X: %.3f | Y: %.3f", currentBlock->bx, currentBlock->by);
	glRasterPos2f(0.1f, 0.1f);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)XY);
	
	// wartość UnableMovement
	snprintf(UnableMovementBool, sizeof(UnableMovementBool), "DetectBlockOOB returned %d\n", (int)DetectBlockOOB(currentBlock));
	glRasterPos2f(0.1f, 13.2f);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)UnableMovementBool);
	
	snprintf(GameMap, sizeof(GameMap), "DetectBlockOOB returned %s\n", CanPlaceBlock(currentBlock, offset) ? "true" : "false");
	glRasterPos2f(0.1f, 13.2f + Letteroffset);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)GameMap);
}

void __DefBlockPos(Block *cBlock, float bX, float bY){
	cBlock->bx = bX;
	cBlock->by = bY;
}

// zapisz poprzednie współrzędne
void __SaveXYValues(Block *block){
	LastX = block->bx;
	LastY = block->by;
}

// Nowy Blok
void NewBlock() {
    // Zapisywanie poprzednich wartości (tutaj zakładając, że masz odpowiednią funkcję)
    // __SaveXYValues(currentBlock);
	
	printf("[--NEW BLOCK--]\n\n");
	
	prevBlock[iteration] = (Block*)malloc(sizeof(Block));
    if (prevBlock[iteration] != NULL) {
        prevBlock[iteration]->bx = LastX;
        prevBlock[iteration]->by = LastY;
        prevBlock[iteration]->rotation = currentBlock->rotation;
        // Możesz również skopiować inne właściwości bloku, jak kształt
        memcpy(prevBlock[iteration]->shape, currentBlock->shape, sizeof(currentBlock->shape));
        iteration++;
    }
    // Zapisywanie do mapy gry (game_map) współrzędnych
    for (int i = 0; i < 4; i++) {

        // Obliczanie pozycji bloku na ekranie
	float ogX = ReturnBlockX(currentBlock, offset, i);
	float ogY = ReturnBlockY(currentBlock, offset, i);

        // Obliczanie współrzędnych na planszy
        int wX = (int)((ogX - lX) / offset) + 1; 
        int wY = (int)((ogY - dY) / offset); 
		
		int status = GetBlockStatus(wX, wY);
		if (status == 1) {
			printf("Pole (%d, %d) jest zajęte.\n", wX, wY);
		} else if (status == 0) {
			printf("Pole (%d, %d) jest wolne.\n", wX, wY);
		} else {
			printf("Pole (%d, %d) jest poza granicami planszy.\n", wX, wY);
		}
		
        // Debugowanie: wypisz współrzędne
        printf("ogX=%.2f, ogY=%.2f, wX=%d, wY=%d\n", ogX, ogY, wX, wY);

        // Sprawdzanie, czy współrzędne mieszczą się w dozwolonym zakresie
        if(wX >= 0 && wX < BOARD_WIDTH && wY >= 0 && wY < BOARD_HEIGHT) {
            gameBoard[wY][wX] = 1; // Zaznacz obecność bloku na planszy
            printf("Block added at: wX=%d, wY=%d\n", wX, wY); // Debugowanie zapisu
        } else {
            printf("Skipping out of bounds position: wX=%d, wY=%d\n", wX, wY); // Debugowanie: jeśli poza zakresem
        }
    }

    // Wybór nowego bloku
    ChooseRandomBlock();

    // Resetowanie pozycji bloku
    X = GX;
    Y = GY;
    __DefBlockPos(currentBlock, X, Y);
    UnableMovement = false;
}



void DrawBlockArea(){
	glColor3f(255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f);
	
	glBegin(GL_LINES);
		glVertex2f(12.0f, 532.0f);
		glVertex2f(138.0f, 532.0f);
		
		glVertex2f(138.0f, 532.0f);
		glVertex2f(138.0f, 244.0f);
		
		glVertex2f(12.0f, 244.0f);
		glVertex2f(138.0f, 244.0f);
		
		glVertex2f(12.0f, 532.0f);
		glVertex2f(12.0f, 244.0f);
	glEnd();
}

void display(){
	glClearColor(0.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f, 1.0f);	//Background - Black
	glClear(GL_COLOR_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, wWidth, 0, wHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
	
	/* nieistotne funkcje - debugowe funkcje */
	__DDI();
	/* */
	
	DrawBlockArea();
	
	
	// Rysowanie wszystkich poprzednich bloków
    glColor3f(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f); // Szary kolor dla poprzednich bloków
    for (int i = 0; i < iteration; i++) {
        if (prevBlock[i] != NULL) {
            DrawBlock(prevBlock[i], offset, BlockSize);
        }
    }
	

	// w razie co tutaj dodajesz kolor
	glColor3f(201.0f/255.0f, 71.0f/255.0f, 50.0f/255.0f);
	
	__DefBlockPos(currentBlock, X, Y);
	DrawBlock(currentBlock, offset, BlockSize);
	

	
	glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glutSwapBuffers();
} // koniec Display function


// dla Kontroli np. strzałek
void ArrowControl(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
			if(!UnableMovement) {
				RotateBlock(currentBlock);
				DetectBlockOOB(currentBlock);
			} else if(DetectBlockOOB(currentBlock) == BLOCK_RIGHT_MOV || BLOCK_LEFT_MOV ){
				//nothing.
			}
            break;
			
		case GLUT_KEY_LEFT:
			if (!UnableMovement){
				X -= offset;	
				if(DetectBlockOOB(currentBlock) != BLOCK_LEFT_MOV){
					currentBlock->bx = X;
				} else {	// jeżeli funkcja poda BLOCK_LEFT_MOV, przysuń w przeciwną stronę
					X += offset;
				}
				DetectBlockOOB(currentBlock);
			}
			break;
			
		case GLUT_KEY_RIGHT:
            if (!UnableMovement) {
                X += offset;
                if (DetectBlockOOB(currentBlock) != BLOCK_RIGHT_MOV) {
                    currentBlock->bx = X;
                } else {
                    X -= offset; // Cofnij ruch
                }
				DetectBlockOOB(currentBlock);
            }
            break;
        
    }
    glutPostRedisplay();
}


// dla klawiszy ASCII
void KeyControl(unsigned char key, int x, int y) {
	switch(key){
		
		case 27:  // Kod ASCII dla klawisza `Esc`
			if(MessageBoxW(NULL, CAPTION_CONFIRM, TITLE_CONFIRM, MB_YESNO | MB_ICONQUESTION) == IDYES){
				exit(0);
			}
			break;

		case 'r':
			ChooseRandomBlock();
			DrawBlock(currentBlock, offset, BlockSize);
			glutPostRedisplay();
			break;
	
	}	// koniec switch
} // koniec KeyControl

void AutoDrop(int value) {
	if (!(Y < dY) && !(CanPlaceBlock(currentBlock, offset)) ){	// jeżeli wartość Y nie jest na granicy, zniżaj
		Y -= offset/2;
	} else if (CanPlaceBlock(currentBlock, offset) == true){
		__SaveXYValues(currentBlock);
		UnableMovement = true;
		NewBlock();	
	} else if (Y < dY) {
		__SaveXYValues(currentBlock);
		UnableMovement = true;
		NewBlock();	
	}
	

    glutPostRedisplay(); // Odśwież ekran
    glutTimerFunc(TIMER, AutoDrop, 0); 
} // koniec AutoDrop


void reshape(int w, int h){
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluOrtho2D(0, wWidth, 0, wHeight);
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glutReshapeWindow(wWidth, wHeight);
} // koniec Reshape

int main(int argc, char** argv){
	printf("[### DEBUG INFO ###]\n");
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(wWidth, wHeight);

    glutCreateWindow(_wTITLE);
	ChooseRandomBlock();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
    glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	AutoDrop(0);
	glutTimerFunc(TIMER, AutoDrop, 0);
	glutSpecialFunc(ArrowControl);
	glutKeyboardFunc(KeyControl);

    glutMainLoop();

	FreePrevBlocks();
    return 0;
}


