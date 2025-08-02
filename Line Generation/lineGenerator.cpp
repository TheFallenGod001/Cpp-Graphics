#include <SDL3/SDL.h>
#include <iostream>
#include <vector>

struct lineNodes {
	std::vector<SDL_FPoint> Points;
	std::vector<int> lengths;
	std::vector<bool> dirs; //false => up and down, true => right and left
	 
	lineNodes(SDL_FPoint pts, int length, bool dir) : Points(1, pts), lengths(1, length), dirs(1, dir) {
		Points.push_back(pts);
	}

	void addPoints(SDL_FPoint Point, int length, bool dir) {
		Points[Points.size() - 1] = Point;
		Points.push_back(Point);
		lengths.push_back(length);
		dirs.push_back(dir);
	}

	void removePoint() {
		Points.pop_back();
		lengths.pop_back();
		dirs.pop_back();
	}


	SDL_FPoint* getPoints() {
		Points[Points.size() - 1] = (this->returnCurrPoint());
		SDL_FPoint* temp = Points.data();

		return temp;
	}

	SDL_FPoint returnCurrPoint() {
		SDL_FPoint temp = { dirs[dirs.size() - 1] ? Points[Points.size() - 2].x + lengths[lengths.size() - 1] : Points[Points.size() - 2].x,
						   dirs[dirs.size() - 1] ? Points[Points.size() - 2].y : lengths[Points.size() - 2] + Points[Points.size() - 2].y};
		return temp;
	}
};

void drawLines(SDL_Renderer* renderer, lineNodes& edges) {
	SDL_RenderLines	(renderer, edges.getPoints(), static_cast<int>(edges.Points.size()));
}


int main(int argc, char* argv[]) {
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer;
	SDL_FPoint point = { 20, 20 };
	lineNodes edges(point, 0, true);

	bool dir = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		std::cout << "Initialization failed.\n";

	window = SDL_CreateWindow("Title", 800, 800, SDL_WINDOW_OPENGL);
	renderer = SDL_CreateRenderer(window, NULL);
	SDL_SetRenderVSync(renderer, 1);

	bool windowIsRunning = true;
	const bool* state = SDL_GetKeyboardState(nullptr);
	SDL_Event event;
	while (windowIsRunning) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				windowIsRunning = false;
			}
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		drawLines(renderer, edges);

		if (state[SDL_SCANCODE_RIGHT]) {
			if (dir) {
				if (edges.lengths[edges.lengths.size() - 1] + edges.Points[edges.lengths.size() - 1].x < 800)
					edges.lengths[edges.lengths.size() - 1]++;
			}
			else {
				dir = true;
				edges.addPoints(edges.returnCurrPoint(), 0, true);
			}
		}
		else if (state[SDL_SCANCODE_LEFT]) {
			if (dir) {
				if (edges.lengths[edges.lengths.size() - 1] + edges.Points[edges.lengths.size() - 1].y > 0)
					edges.lengths[edges.lengths.size() - 1]--;
			}
			else {
				dir = true;
				edges.addPoints(edges.returnCurrPoint(), 0, true);
			}
		}
		else if (state[SDL_SCANCODE_UP]) {
			if (!dir) {
				if (edges.lengths[edges.lengths.size() - 1] + edges.Points[edges.lengths.size() - 1].y > 0)
					edges.lengths[edges.lengths.size() - 1]--;
			}
			else {
				dir = false;
				edges.addPoints(edges.returnCurrPoint(), 0, false);
			}
		}
		else if (state[SDL_SCANCODE_DOWN]) {
			if (!dir) {
				if (edges.lengths[edges.lengths.size() - 1] + edges.Points[edges.lengths.size() - 1].y < 800)
					edges.lengths[edges.lengths.size() - 1]++;
			}
			else {
				dir = false;
				edges.addPoints(edges.returnCurrPoint(), 0, false);
			}
		}

		SDL_RenderPresent(renderer);
	}

	std::cout << "Closing window";

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

