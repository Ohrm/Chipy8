#include "chip8.hpp" // CPU core
#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>

chip8 myChip8;

const int SCREEN_WIDTH = 64;
const int SCREEN_HEIGHT = 32;

void drawGraphics();
void handleInput(SDL_Event *e);

unsigned char screenData[32][64][3] ; 

int main(int argc, char **argv){

    // Setup Graphics
	if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
	{
		return 2; 
	} 

	SDL_Window* window = SDL_CreateWindow("Chipy8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GL_CreateContext(window);
	SDL_GL_SwapWindow(window);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1.0, 1.0);
	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_FLAT);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DITHER);
	glDisable(GL_BLEND);

    // SetupInput();

    // Init the CPU and load the game
    myChip8.Init();
    if(!myChip8.LoadGame("Invaders.ch8"))
		return 1;

	bool quit = false;;

	SDL_Event e;

    //Emulation loop
    for(;;){

        //Emulate one cycle
        myChip8.EmualteCycle();

        //If draw flag is set update screen
        //if(myChip8.drawFlag){

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glRasterPos2i(-1, 1);
		glPixelZoom(1, -1);

		for(int y = 0; y < 32; y++){

			for(int x = 0; x < 64; x++){

				if(myChip8.gfx[(y * 64) + x] == 0)
					screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 0;	// Disabled
				else 
					screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 255;  // Enabled

			}

		}

		glDrawPixels(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, screenData);
		SDL_GL_SwapWindow(window);
		glFlush();

		//}
        // Store key press state (pressed or released)
        myChip8.SetKeys();

		while(SDL_PollEvent(&e) != 0){

			//User requests quit
            if(e.type == SDL_QUIT)
			{

				quit = true;
            
			}
			else if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP){

				handleInput(&e);

			}

		}

		if(quit){

			SDL_DestroyWindow(window);
			SDL_Quit();
			break;
		
		}
    }

    return 0;

}

void drawGraphics(){


}

void handleInput(SDL_Event *e){

	if(e->type == SDL_KEYDOWN){
		switch(e->key.keysym.sym){

		case SDLK_x:
			myChip8.key[0] = 1;
			break;

		case SDLK_1:
			myChip8.key[1] = 1;
			break;

		case SDLK_2:
			myChip8.key[2] = 1;
			break;

		case SDLK_3:
			myChip8.key[3] = 1;
			break;

		case SDLK_q:
			myChip8.key[4] = 1;
			break;

		case SDLK_w:
			myChip8.key[5] = 1;
			break;

		case SDLK_e:
			myChip8.key[6] = 1;
			break;

		case SDLK_a:
			myChip8.key[7] = 1;
			break;

		case SDLK_s:
			myChip8.key[8] = 1;
			break;

		case SDLK_d:
			myChip8.key[9] = 1;
			break;

		case SDLK_z:
			myChip8.key[10] = 1;
			break;

		case SDLK_c:
			myChip8.key[11] = 1;
			break;
		
		case SDLK_4:
			myChip8.key[12] = 1;
			break;

		case SDLK_r:
			myChip8.key[13] = 1;
			break;

		case SDLK_f:
			myChip8.key[14] = 1;
			break;

		case SDLK_v:
			myChip8.key[15] = 1;
			break;

		}
	}
	else if(e->type == SDL_KEYUP){
		switch(e->key.keysym.sym){

		case SDLK_x:
			myChip8.key[0] = 0;
			break;

		case SDLK_1:
			myChip8.key[1] = 0;
			break;

		case SDLK_2:
			myChip8.key[2] = 0;
			break;

		case SDLK_3:
			myChip8.key[3] = 0;
			break;

		case SDLK_q:
			myChip8.key[4] = 0;
			break;

		case SDLK_w:
			myChip8.key[5] = 0;
			break;

		case SDLK_e:
			myChip8.key[6] = 0;
			break;

		case SDLK_a:
			myChip8.key[7] = 0;
			break;

		case SDLK_s:
			myChip8.key[8] = 0;
			break;

		case SDLK_d:
			myChip8.key[9] = 0;
			break;

		case SDLK_z:
			myChip8.key[10] = 0;
			break;

		case SDLK_c:
			myChip8.key[11] = 0;
			break;
		
		case SDLK_4:
			myChip8.key[12] = 0;
			break;

		case SDLK_r:
			myChip8.key[13] = 0;
			break;

		case SDLK_f:
			myChip8.key[14] = 0;
			break;

		case SDLK_v:
			myChip8.key[15] = 0;
			break;

		}
	}
}