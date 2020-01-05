#include "SDL.h"
#include "SDL_ttf.h"
#include <windows.h>
#include <time.h>
#include <vector>
#include <random>

enum class Directions {
	UP = 1,
	DOWN = 2,
	LEFT = 3,
	RIGHT = 4
};

struct Food {
	bool eaten = false;
	SDL_Rect foodRect;
};

int Window_WIDTH = 500;
int Window_HEIGHT = 500;

bool Quit = false;

std::vector<SDL_Rect> tail;

SDL_Rect headPosition;
Food foodPosition;

// **************************

SDL_Texture* CreateTextTexture(SDL_Renderer* renderer, const char* text) {

	TTF_Font* Sans = TTF_OpenFont("Fonts/Sans/OpenSans-Bold.ttf", 24); //this opens a font style and sets a size
	SDL_Color White = { 255, 255, 255 };  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, text, White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
	return SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture
}

// This method checks for the head collisions with the food and itself
void checkHeadCollisions() {

	// I check if the head is at the same position of the food
	if (headPosition.x == foodPosition.foodRect.x && headPosition.y == foodPosition.foodRect.y) {
		SDL_Rect temp;
		temp.w = 10;
		temp.h = 10;

		tail.push_back(temp);

		foodPosition.eaten = true;
	}
	else {
		foodPosition.eaten = false;
	}

	// I check if the head has collided with itself
	for (size_t i = 0; i < tail.size(); i++) {
		if (tail[i].x == headPosition.x && tail[i].y == headPosition.y) {
			Quit = true;
			break;
		}
	}
}

// This method checks if the head has gone out of the map bounds
bool checkIfOutBound() {

	if (headPosition.x < 0 || headPosition.x >(Window_WIDTH - headPosition.w)) {
		return true;
	}

	if (headPosition.y < 0 || headPosition.y >(Window_HEIGHT - headPosition.h)) {
		return true;
	}

	return false;
}

// This method draws the current window content after clearing the previous one
void draw(SDL_Renderer* renderer) {
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(renderer, &foodPosition.foodRect);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	for (size_t i = 0; i < tail.size(); i++) {
		SDL_RenderDrawRect(renderer, &tail[i]);
	}

	SDL_RenderFillRect(renderer, &headPosition);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderPresent(renderer);
}

// This method listens for user input events and change the direction accordingly
void checkInput(SDL_Event& e, Directions& currentDirection) {
	while (SDL_PollEvent(&e) != 0) {

		if (e.type == SDL_QUIT) {
			Quit = true;
		}
		else {
			switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:
				Quit = true;
				break;

			case SDLK_UP:
			case SDLK_w:
				currentDirection = Directions::UP;
				break;

			case SDLK_DOWN:
			case SDLK_s:
				currentDirection = Directions::DOWN;
				break;

			case SDLK_LEFT:
			case SDLK_a:
				currentDirection = Directions::LEFT;
				break;

			case SDLK_RIGHT:
			case SDLK_d:
				currentDirection = Directions::RIGHT;
				break;

			}
		}
	}
}

// This method randomize the position of the food object
void randomFoodPosition() {
	std::random_device                 randomDevice;
	std::mt19937                       engine(randomDevice());
	std::uniform_int_distribution<int> distribution(0, (Window_WIDTH / 10) - 1);

	foodPosition.foodRect.x = distribution(engine) * 10;
	foodPosition.foodRect.y = distribution(engine) * 10;
}

// This method moves all the tail blocks in the next position
void moveTail(SDL_Rect& headPosition) {

	if (tail.size() > 0) {
		for (int i = tail.size() - 1; 0 < i; i--) {
			tail[i].x = tail[i - 1].x;
			tail[i].y = tail[i - 1].y;
		}

		tail[0].x = headPosition.x;
		tail[0].y = headPosition.y;
	}
}

// *****************************************************************************

int main(int argc, char* argv[]) {
	TTF_Init();

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* gWindow = SDL_CreateWindow("Game #1", 200, 200, Window_WIDTH, Window_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_Renderer* renderer = SDL_CreateRenderer(gWindow, -1, 0);
	SDL_Event e;

	headPosition.x = 0;
	headPosition.y = 0;
	headPosition.w = 10;
	headPosition.h = 10;

	foodPosition.foodRect.w = 10;
	foodPosition.foodRect.h = 10;
	randomFoodPosition();

	int Movement = 10;
	Directions currentDirection = Directions::DOWN;

	SDL_Rect messageRect;
	messageRect.x = Window_WIDTH / 2 - 150;
	messageRect.y = Window_HEIGHT / 2 - 50;
	messageRect.w = 300;
	messageRect.h = 100;

	SDL_Texture* Message = CreateTextTexture(renderer, "Starting...");
	SDL_RenderCopy(renderer, Message, NULL, &messageRect);

	SDL_RenderPresent(renderer);

	Sleep(3000);

	while (!Quit) {

		checkHeadCollisions();

		if (!Quit) {
			if (foodPosition.eaten) {
				randomFoodPosition();
				foodPosition.eaten = false;
			}

			checkInput(e, currentDirection);

			if (!checkIfOutBound()) {

				moveTail(headPosition);

				switch (currentDirection) {
				case Directions::UP:
					headPosition.y -= Movement;
					break;
				case Directions::DOWN:
					headPosition.y += Movement;
					break;
				case Directions::LEFT:
					headPosition.x -= Movement;
					break;
				case Directions::RIGHT:
					headPosition.x += Movement;
					break;
				}
			}
			else {
				Quit = true;
			}

			draw(renderer);

			Sleep(1000 / 10);
		}
	}

	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Rip", "You lost :(", NULL);

	SDL_DestroyWindow(gWindow);
	SDL_Quit();

	return 0;
}
