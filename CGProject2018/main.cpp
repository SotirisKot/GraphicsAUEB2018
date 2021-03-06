#include <iostream>
#include <chrono>
#include <thread>
#include "GLEW\glew.h"
#include "Renderer.h"
#include "GameState.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

using namespace std;

//Screen attributes
SDL_Window * window;

//OpenGL context 
SDL_GLContext gContext;
const int SCREEN_WIDTH = 1380;	//800;	//640;
const int SCREEN_HEIGHT = 1024;	//600;	//480;

//Event handler
SDL_Event event;

Renderer * renderer = nullptr;
//GameState* game = nullptr;

void func()
{
	system("pause");
}

// initialize SDL and OpenGL
bool init(GameState* game)
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		return false;
	}

	// Initialize SDL TTF
	if (TTF_Init() == -1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		return false;
	}

	// use Double Buffering
	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0)
		std::cout << "Error: No double buffering" << endl;

	// set OpenGL Version (3.3)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Create Window
	window = SDL_CreateWindow("OpenGL Tower Defense Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL)
	{
		printf("Could not create window: %s\n", SDL_GetError());
		return false;
	}

	//Create OpenGL context
	gContext = SDL_GL_CreateContext(window);
	if (gContext == NULL)
	{
		printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	// Disable Vsync
	if (SDL_GL_SetSwapInterval(0) == -1)
		printf("Warning: Unable to disable VSync! SDL Error: %s\n", SDL_GetError());

	// Initialize GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		printf("Error loading GLEW\n");
		return false;
	}
	// some versions of glew may cause an opengl error in initialization
	glGetError();

	renderer = new Renderer();

	// set renderer's game state

	renderer->setGameState(game);

	bool engine_initialized = renderer->Init(SCREEN_WIDTH, SCREEN_HEIGHT);

	//atexit(func);
	
	return engine_initialized;
}


void clean_up()
{
	delete renderer;

	SDL_GL_DeleteContext(gContext);
	SDL_DestroyWindow(window);

	TTF_Quit();

	SDL_Quit();
}

//void changeMusic() {
//	if (game->getPirateWave() % 2 == 0) {
//
//		game->getMusicManager()->PlayMusic("epic_boss.wav", true);
//
//	}
//	else {
//
//		game->getMusicManager()->PlayMusic("dark.wav", false);
//
//	}
//}


int main(int argc, char *argv[])
{
	// Initialize the Game State
	GameState* game = new GameState();

	//Initialize
	if (init(game) == false)
	{
		system("pause");
		return EXIT_FAILURE;
	}

	//Quit flag
	bool quit = false;
	bool mouse_button_pressed = false;
	glm::vec2 prev_mouse_position(0);

	auto simulation_start = chrono::steady_clock::now();

	//// Initialize position of tile to center of the map
	//int tileX, tileY;
	//tileX = 4;
	//tileY = 4;
	//renderer->TileSetPos(tileX, tileY);

	// Timers for timed events
	unsigned int lastTimeT1 = 0, lastTimeT2 = 0, lastTimeT3 = 0, lastTimeT4 = 0, lastTimeT5 = 0, lastTimeT6 = 0, lastTimeT7 = 0 ,currentTime, timePaused, timeRender=0, prevTimeRender=0;

	unsigned int img_toggle = 0;
	unsigned int animL = 0;
	unsigned int animLR = 0;
	unsigned int fpsShowLim = 0;
	int prevFps = 0;

	//unsigned int piratesInWave = 0;

	struct wave {
		int num_of_pirates;
		std::vector<int> types;
		std::vector<int> levels;
	};

	wave piratesInWave;

	// start the background music
	//game->getMusicManager()->PlayMusic("imperial_march.wav");
	//game->getMusicManager()->setMusicPause(false);

	// Wait for user exit
	while (quit == false)
	{

		/*
		// NOT SURE IF HELPS

		lastTimeT1 += dt*1000 > 500 ? dt*1000 : 0;
		lastTimeT2 += dt*1000 > 500 ? dt*1000 : 0;
		lastTimeT3 += dt*1000 > 500 ? dt*1000 : 0;
		lastTimeT4 += dt*1000 > 500 ? dt*1000 : 0;
		lastTimeT5 += dt*1000 > 500 ? dt*1000 : 0;
		lastTimeT6 += dt*1000 > 500 ? dt*1000 : 0;*/

		timeRender = SDL_GetTicks();

		if (game->getGameOver()) {
			// If game over then pause the game
			game->getMusicManager()->QuitMusic();
			if (!game->isPaused()) {
				game->getMusicManager()->PlaySFX("gameover.wav", 3, 0, 2);
			}
			game->setPaused(true);
		}

		// While there are events to handle
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				// Key down events
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = true;
				else if (event.key.keysym.sym == SDLK_k) renderer->ReloadShaders();
				else if (event.key.keysym.sym == SDLK_i) renderer->SetRenderingMode(Renderer::RENDERING_MODE::TRIANGLES);
				else if (event.key.keysym.sym == SDLK_o) renderer->SetRenderingMode(Renderer::RENDERING_MODE::LINES);
				else if (event.key.keysym.sym == SDLK_p) renderer->SetRenderingMode(Renderer::RENDERING_MODE::POINTS);
				else if (event.key.keysym.sym == SDLK_t)
				{
					bool inroad = game->getInRoad();
					if (!inroad) {
						if (game->getActions() >= 3) {
							if (game->getAvailableTowers().empty()) game->createTower();
							bool added = game->addTower();
							if (added) {
								game->setActions(game->getActions() - 3);
								printf("Spent 3 action points\nActions: %d\n", game->getActions());
							}
							else {
								printf("There is already a tower in this tile!\n");
							}
						}
						else {
							printf("You don't have 3 action points!\nYou need %d more action points to perform this action.\n", (3 - game->getActions()));
						}
					}

				}
				else if (event.key.keysym.sym == SDLK_r)
				{
					bool inroad = game->getInRoad();
					if (!inroad) {
						bool rearranged = game->rearrangeTower();
						if (rearranged) {
							game->setActions(game->getActions() + 2);
							printf("Gained 2 action points\nActions: %d\n", game->getActions());
						}
						else {
							printf("There is no tower in this tile!\n");
						}
					}
				}
				else if (event.key.keysym.sym == SDLK_w)
				{
					renderer->CameraMoveForward(true);
				}
				else if (event.key.keysym.sym == SDLK_s)
				{
					renderer->CameraMoveBackWard(true);
				}
				else if (event.key.keysym.sym == SDLK_a)
				{
					renderer->CameraMoveLeft(true);
				}
				else if (event.key.keysym.sym == SDLK_d)
				{
					renderer->CameraMoveRight(true);
				}
				else if (event.key.keysym.sym == SDLK_UP && game->getTileY() + 1 <= 9)
				{
					game->setTileY(game->getTileY() + 1);
					game->updateInRoad();
				}
				else if (event.key.keysym.sym == SDLK_DOWN && game->getTileY() - 1 >= 0)
				{
					game->setTileY(game->getTileY() - 1);
					game->updateInRoad();
				}
				else if (event.key.keysym.sym == SDLK_LEFT && game->getTileX() + 1 <= 9)
				{
					game->setTileX(game->getTileX() + 1);
					game->updateInRoad();
				}
				else if (event.key.keysym.sym == SDLK_RIGHT && game->getTileX() - 1 >= 0)
				{
					game->setTileX(game->getTileX() - 1);
					game->updateInRoad();
				}
				else if (event.key.keysym.sym == SDLK_q)
				{
					// TEST TO CREATE PIRATE ON THE FLY
					//game->createPirate();
					/*if(game->getCreatedTowers().size() != 0)
						game->shootCannonBall(game->getCreatedTowers()[0], game->getPirates()[0]);*/

					// Create wave
					/*piratesInWave.num_of_pirates = 5;
					for (int i = 0; i < 5; i++) {
						piratesInWave.types.push_back(1);
						piratesInWave.levels.push_back(1);
					}*/
					game->setActions(200);

				}
				else if (event.key.keysym.sym == SDLK_0)
				{
					// Pause the game
					game->setPaused(!game->isPaused());
					// Pause the music as well
					game->getMusicManager()->PauseMusic();
					game->getMusicManager()->setMusicPause(true);
				}
				else if (event.key.keysym.sym == SDLK_u)
				{
					// Upgrade Tower
					if (game->getActions() >= 3) {
						int upgraded = game->upgradeTower(game->getActions());
						if (upgraded == 1) {
							game->setActions(game->getActions() - 3);
							printf("Spent 3 action points\nActions: %d\n", game->getActions());
						}
						else if (upgraded == 2) {
							game->setActions(game->getActions() - 6);
							printf("Spent 6 action points\nActions: %d\n", game->getActions());
						}
						else if(upgraded == 0){
							printf("Can't perform upgrade!\nYou need at least 6 points to upgrade tower to level 3!\n");
						}
					}
					else {
						printf("You don't have enough action points!\nYou need at least 3 action points to perform this action.\n");
					}
				}
				else if (event.key.keysym.sym == SDLK_n){
					// Nuke
					if (game->getActions() >= 30 && !game->getHeilMary()) {
						game->sendNukes();
						game->setActions(game->getActions() - 30);
					}
					else {
						printf("You don't have enough action points!\nYou need at least 30 action points to send nukes!\n");
					}
					
				}
			}
			else if (event.type == SDL_KEYUP)
			{
				if (event.key.keysym.sym == SDLK_w)
				{
					renderer->CameraMoveForward(false);
				}
				else if (event.key.keysym.sym == SDLK_s)
				{
					renderer->CameraMoveBackWard(false);
				}
				else if (event.key.keysym.sym == SDLK_a)
				{
					renderer->CameraMoveLeft(false);
				}
				else if (event.key.keysym.sym == SDLK_d)
				{
					renderer->CameraMoveRight(false);
				}
			}
			else if (event.type == SDL_MOUSEMOTION)
			{
				int x = event.motion.x;
				int y = event.motion.y;

				if (mouse_button_pressed)
				{
					renderer->CameraLook(glm::vec2(x, y) - prev_mouse_position);
					prev_mouse_position = glm::vec2(x, y);
				}
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					int x = event.button.x;
					int y = event.button.y;
					mouse_button_pressed = (event.type == SDL_MOUSEBUTTONDOWN);
					prev_mouse_position = glm::vec2(x, y);
				}
			}
			else if (event.type == SDL_MOUSEWHEEL)
			{
				int x = event.wheel.x;
				int y = event.wheel.y;
			}
			else if (event.type == SDL_WINDOWEVENT)
			{
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					renderer->ResizeBuffers(event.window.data1, event.window.data2);
				}
			}
		}

		float dt = 0.0f;

		if(!game->isPaused()){

			if (game->getWasPaused()) {
				timePaused = SDL_GetTicks() - timePaused;
				lastTimeT1 += timePaused;
				lastTimeT2 += timePaused;
				lastTimeT3 += timePaused;
				lastTimeT4 += timePaused;
				lastTimeT5 += timePaused;
				lastTimeT6 += timePaused;
				lastTimeT7 += timePaused;
				// the game was paused so was the music..so resume it
				game->getMusicManager()->ResumeMusic();
				game->getMusicManager()->setMusicPause(false);
			}
			else {
				timePaused = 0;
			}

			// Compute the ellapsed time
			auto simulation_end = chrono::steady_clock::now();
			dt = chrono::duration <float>(simulation_end - simulation_start).count(); // in seconds
			simulation_start = chrono::steady_clock::now();

			// Create a timed event
			currentTime = SDL_GetTicks();
			if (currentTime > lastTimeT1 + 5 * 1000) {
				size_t sz = game->getPirates().size();
				if (sz != 0 && !game->getDarth()) {
					if (sz < 5) game->getMusicManager()->PlaySFX("skeleton_breath.wav", 1 ,0, 0);
					else if (sz < 10) game->getMusicManager()->PlaySFX("skeleton_breath2.wav", 1 ,0, 0);
					else game->getMusicManager()->PlaySFX("skeleton_breath3.wav", 1 ,0, 0);
				}
				printf("Timed Event: 5 seconds have passed, one more action is available.\n");
				game->setActions(game->getActions() + 1);
				printf("Actions: %d\n", game->getActions());
				//game->createTower();
				lastTimeT1 = currentTime;
			}

			// every 100ms
			currentTime = SDL_GetTicks();
			//printf("CurrentTime: %d\nLastTimeT2: %d\n", currentTime, lastTimeT2);
			if (currentTime > lastTimeT2 + 100) {
				game->towersFire();
				lastTimeT2 = currentTime;
			}

			// every 10ms
			currentTime = SDL_GetTicks();
			if (currentTime > lastTimeT3 + 10) {
				//printf("Timed Event: 10ms - Updated Pirate Targets\n");
				game->updatePirateTargets();
				game->checkCollidingPirates();
				lastTimeT3 = currentTime;
			}

			// every 500ms
			currentTime = SDL_GetTicks();
			if (currentTime > lastTimeT4 + 500) {
				if (game->getShowGoldParticles()) game->setShowGoldParticles(false);
				game->checkPiratesAtChest();
				game->checkDeadPirates();
				lastTimeT4 = currentTime;
			}

			// Create Waves
			currentTime = SDL_GetTicks();
			// if it is the first wave wait for 5 secs not 20
			if (game->getPirateWave() == 0) {
				if (currentTime > lastTimeT6 + 5000) {
					game->getMusicManager()->PlayMusic("dark.wav",1);
					game->getMusicManager()->setMusicPause(false);
					printf("THE WAVE IS: %d\n", game->getPirateWave());
					piratesInWave.num_of_pirates = 4;
					piratesInWave.types.clear();
					piratesInWave.levels.clear();
					for (int i = 0; i < piratesInWave.num_of_pirates; i++) {
						piratesInWave.types.push_back(1);
						piratesInWave.levels.push_back(1);
					}
					// testing only TODO:REMOVE
					//piratesInWave.types[2] = 0;
					//piratesInWave.types[1] = 0;
					//
					lastTimeT6 = currentTime;
					game->setPirateWave(game->getPirateWave() + 1);
				}
			}else if (game->getPirateWave() % 2 == 0){
				if (currentTime > lastTimeT6 + 20000) {
					printf("THE WAVE IS: %d\n", game->getPirateWave());
					// Every 6 waves spawn the boss!!
					// and play the boss music

					piratesInWave.num_of_pirates = 1;
					piratesInWave.types.clear();
					piratesInWave.levels.clear();

					float r = float(rand()) / RAND_MAX;
					// first halt the background music
					game->getMusicManager()->QuitMusic();

					// Reset animation int to 0
					animL = 0;

					if (r <= 0.01f) {
						game->setBoss(true);
						
						//Mix_HookMusicFinished(changeMusic);
						float r_music = float(rand()) / RAND_MAX;
						if (r_music <= 0.5f) {
							game->getMusicManager()->PlayMusic("epic_boss.wav", 2);
						}
						else {
							game->getMusicManager()->PlayMusic("bensound-epic.wav", 2);
						}
						
						game->getMusicManager()->setMusicPause(false);
						piratesInWave.types.push_back(3);
						
					}
					else {
						game->setDarth(true);
						game->getMusicManager()->PlaySFX("saber_on.wav", 4 ,0, 7);
						game->getMusicManager()->PlayMusic("imperial_march.wav",0);
						game->getMusicManager()->PlaySFX("vader_breath.wav",2,-1,0);
						game->getMusicManager()->setMusicPause(false);

						piratesInWave.types.push_back(4);
					}
					
					piratesInWave.levels.push_back(1 + rand() % game->getPirateWave());
					lastTimeT6 = currentTime;
					game->setPirateWave(game->getPirateWave() + 1);
					game->setStopWaves(true);
				}
			}
			else if (!game->getStopWaves() && game->getBoss()) {
				// halt the boss music if it is playing and start the background music again
				//Mix_HookMusicFinished(changeMusic); //NOTE: an thelete na to tsekarete kante global thn game
				game->getMusicManager()->QuitMusic();
				// Mix_HookMusicFinished(changeMusic); lene einai poly kako na to kaneis etsi 
				game->getMusicManager()->PlayMusic("dark.wav", 1);
				game->getMusicManager()->setMusicPause(false);
				game->setBoss(false);
			}
			else if (!game->getStopWaves() && game->getDarth()) {
				
				game->getMusicManager()->QuitMusic();
				game->getMusicManager()->QuitSFX(0);
				// Mix_HookMusicFinished(changeMusic); lene einai poly kako na to kaneis etsi 
				game->getMusicManager()->PlayMusic("dark.wav", 1);
				game->getMusicManager()->setMusicPause(false);
				game->setDarth(false);
			}
			else{	
				if (currentTime > lastTimeT6 + 20000 && !game->getStopWaves()) {	
					printf("THE WAVE IS: %d\n", game->getPirateWave());
					//if ((game->getPirateWave() - 1) % 2 == 0) {
					//	game->getMusicManager()->QuitMusic();
					//	//Mix_HookMusicFinished(changeMusic); lene einai poly kako na to kaneis etsi 
					//	game->getMusicManager()->PlayMusic("dark.wav", false);
					//	game->getMusicManager()->setMusicPause(false);
					//}
					piratesInWave.num_of_pirates = (game->getPirateWave() * 2 > 10) ? 10 : (game->getPirateWave() * 2);
					piratesInWave.types.clear();
					piratesInWave.levels.clear();
					for (int i = 0; i < piratesInWave.num_of_pirates; i++) {
						piratesInWave.types.push_back(rand() % 3);
						piratesInWave.levels.push_back(1 + rand() % game->getPirateWave());
					}
					std::sort(piratesInWave.types.begin(), piratesInWave.types.end(), std::greater<int>());
					lastTimeT6 = currentTime;
					game->setPirateWave(game->getPirateWave() + 1);
				}
			}
			
			// Create Pirate
			currentTime = SDL_GetTicks();
			if (currentTime > lastTimeT5 + game->getPirateRate()) {
				if (piratesInWave.num_of_pirates > 0) {
					piratesInWave.num_of_pirates -= 1;
					if (piratesInWave.num_of_pirates <= piratesInWave.types.size()) {
						int type = piratesInWave.types[piratesInWave.num_of_pirates];
						int level = piratesInWave.levels[piratesInWave.num_of_pirates];
						game->createPirate(type, level);
						// Change rate that pirates are placed according to their type
						// in order to avoid collisions
						switch (type) {
						case 0:
							game->setPirateRate(1000);
							break;
						case 1:
							game->setPirateRate(2000);
							break;
						case 2:
							game->setPirateRate(3000);
							break;
						case 3:case 4:
							game->setPirateRate(2000);
							break;
						}
					}
					lastTimeT5 = currentTime;
				}
			}

			//every 250ms
			currentTime = SDL_GetTicks();
			if (currentTime > lastTimeT7 + 250) {
				if (game->getHeilMary()) {
					game->sendNextNuke(game->getDeathIdx());
				}
				lastTimeT7 = currentTime;
			}

			// Update
			renderer->Update(dt);

			game->setWasPaused(false);
		}
		else {
			// pause
			if (!game->getWasPaused()) {
				timePaused = SDL_GetTicks();
				//game->getMusicManager()->QuitSFX(0); // stop breath
				game->setWasPaused(true);
			}
		}

		// Draw
		renderer->Render();
		SDL_Color color = { 170,40,20 };
		renderer->RenderText("Score: " + std::to_string(game->getScore()), color, 10, 10, 74);
		renderer->RenderText("Actions: " + std::to_string(game->getActions()), color, 10, 60, 74);
		renderer->RenderText("Gold: " + std::to_string(game->getGold()), color, 10, 110, 74);
		renderer->RenderText("Wave: " + std::to_string(game->getPirateWave()==0?0:game->getPirateWave()-1), color, 10, 160, 74);
		fpsShowLim++;
		if (fpsShowLim <= 5) {
			renderer->RenderText("FPS: " + std::to_string(prevFps), color, SCREEN_WIDTH - 200, 10, 74);
		}
		else {
			renderer->RenderText("FPS: " + std::to_string(int(1.0f / dt) >= 60 ? 60 : int(1.0f / dt)), color, SCREEN_WIDTH - 200, 10, 74);
			fpsShowLim = 0;
			prevFps = int(1.0f / dt) >= 60 ? 60 : int(1.0f / dt);
		}
		renderer->setFontSize(74);

		if (game->getGameOver()) {
			renderer->RenderText("GAME OVER", color, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 3, 240);
			game->getMusicManager()->QuitSFX(0); // stop playing breath
			renderer->setFontSize(240);
		}

		// Test image rendering
		if (game->getBoss() || game->getDarth()) {
			if (game->getDarth()) {
				// Lightsaber animation
				animL++;
				unsigned int animIdx = 0;
				if (animL <= 2) animIdx = 1;
				else if (animL <= 4) animIdx = 2;
				else if(animL <= 6) animIdx = 3;
				else if(animL <= 8) animIdx = 4;
				else if(animL <= 10) animIdx = 5;
				else if(animL <= 12) animIdx = 6;
				else if(animL <= 14) animIdx = 7;
				else if(animL <= 16) animIdx = 8;
				else if(animL <= 18) animIdx = 9;
				else if (animL <= 20) animIdx = 10;
				else if (animL <= 22) animIdx = 11;
				else if (animL <= 24) animIdx = 12;
				else if (animL <= 26) animIdx = 13;
				else if (animL <= 28) animIdx = 14;
				else if (animL <= 30) animIdx = 15;
				else if (animL <= 32) animIdx = 16;
				else if (animL <= 34) animIdx = 17;
				else if (animL <= 36) animIdx = 18;
				else if (animL <= 38) animIdx = 19;
				else {
					animIdx = 20;
					animL = 40;
				}
				renderer->RenderImage("../Data/Images/LightsaberAnim/sith_lightsaber"+std::to_string(animIdx)+".png", -50 + SCREEN_WIDTH / 4 + 145, 45, 1.5, 1.5, false);
				renderer->RenderImage("../Data/Images/pirate_small.png", -50 + SCREEN_WIDTH / 4, 10, 1.5, 1.5, false);
				renderer->RenderImage("../Data/Images/pirate_small.png", -50 + 3 * SCREEN_WIDTH / 4, 10, 1.5, 1.5, true);
			}
			else {
				if (img_toggle <= 30) {
					renderer->RenderImage("../Data/Images/pirate_small_red.png", -50 + SCREEN_WIDTH / 4, 10, 1.5, 1.5, false);
					renderer->RenderImage("../Data/Images/pirate_small.png", -50 + 3 * SCREEN_WIDTH / 4, 10, 1.5, 1.5, true);
				}
				else if (img_toggle <= 60) {
					renderer->RenderImage("../Data/Images/pirate_small.png", -50 + SCREEN_WIDTH / 4, 10, 1.5, 1.5, false);
					renderer->RenderImage("../Data/Images/pirate_small_red.png", -50 + 3 * SCREEN_WIDTH / 4, 10, 1.5, 1.5, true);
				}
				else {
					img_toggle = -1;
				}
				img_toggle += 1;
			}
		}
		else {
			if (game->getWasDarth()) {
				// Lightsaber animation
				animLR++;
				unsigned int animIdx = 0;
				if (animLR <= 2) animIdx = 20;
				else if (animLR <= 4) animIdx = 19;
				else if (animLR <= 6) animIdx = 18;
				else if (animLR <= 8) animIdx = 17;
				else if (animLR <= 10) animIdx = 16;
				else if (animLR <= 12) animIdx = 15;
				else if (animLR <= 14) animIdx = 14;
				else if (animLR <= 16) animIdx = 13;
				else if (animLR <= 18) animIdx = 12;
				else if (animLR <= 20) animIdx = 11;
				else if (animLR <= 22) animIdx = 10;
				else if (animLR <= 24) animIdx = 9;
				else if (animLR <= 26) animIdx = 8;
				else if (animLR <= 28) animIdx = 7;
				else if (animLR <= 30) animIdx = 6;
				else if (animLR <= 32) animIdx = 5;
				else if (animLR <= 34) animIdx = 4;
				else if (animLR <= 36) animIdx = 3;
				else if (animLR <= 38) animIdx = 2;
				else {
					animIdx = 1;
					animLR = 0;
					game->setWasDarth(false);
				}
				renderer->RenderImage("../Data/Images/LightsaberAnim/sith_lightsaber" + std::to_string(animIdx) + ".png", -50 + SCREEN_WIDTH / 4 + 145, 45, 1.5, 1.5, false);
			}
			renderer->RenderImage("../Data/Images/pirate_small.png", -50 + SCREEN_WIDTH / 4, 10, 1.5, 1.5, false);
			renderer->RenderImage("../Data/Images/pirate_small.png", -50 + 3 * SCREEN_WIDTH / 4, 10, 1.5, 1.5, true);
		}

		// test image no alpha
		//renderer->RenderImage("../Data/Images/testj.jpeg", 0, 0, 1.5, 1.5, false);

		//Update screen (swap buffer for double buffering)
		SDL_GL_SwapWindow(window);

		// LIMIT FPS TO 60
		int timeToSleep = (1000 / 60) - dt*1000;

		if (timeToSleep > 0) {
			//printf("Sleeping for: %d\n", timeToSleep);
			std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleep));
		}

	}

	// Delete the game state
	delete game;

	//Clean up
	clean_up();

	return 0;
}