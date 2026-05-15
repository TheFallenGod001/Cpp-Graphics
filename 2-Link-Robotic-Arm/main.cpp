#include "SDL3/SDL.h"
#include <iostream>
#include <cmath>
#include <numbers>
#include <functional>
#include "SDL3_ttf/SDL_ttf.h"
constexpr float PI = 3.14159265358979323846;

class FPointBuffer {
	SDL_FPoint* buffer;
	int maxSize = 100, head, tail;
public:
	FPointBuffer(int size) : maxSize(size), head(-1), tail(-1) {
		buffer = new SDL_FPoint[maxSize];
	}
	FPointBuffer() : head(-1), tail(-1) {
		buffer = new SDL_FPoint[maxSize];
	}
	void enqueue(SDL_FPoint p) {
		if ((head + 1) % maxSize == tail) {
			std::cout << "Queue is full, sorry :<\n";
			return;
		}
		if (head == -1) {
			head = 0;
			tail = 0;
			buffer[head] = p;
			return;
		}
		head = (head + 1) % maxSize;
		buffer[head] = p;
	}
	SDL_FPoint dequeue() {
		if (head == -1) {
			std::cout << "Queue is empty, sorry :<\n";
			return { 0, 0 };
		}
		SDL_FPoint elem = buffer[tail];
		if (tail == head) { // last element removed
			head = tail = -1;
		}
		else {
			tail = (tail + 1) % maxSize;
		}
		return elem;
	}

	bool isEmpty() {
		if (head == -1) {
			return true;
		}
		else return false;
	}

	~FPointBuffer() {
		delete[] buffer;
	}
};

class FPointStack {
	SDL_FPoint* buffer;
	int maxSize = 100, top;
public:

	FPointStack(int size) : maxSize(size), top(-1) {
		buffer = new SDL_FPoint[maxSize];
	}
	FPointStack() : top(-1) {
		buffer = new SDL_FPoint[maxSize];
	}
	FPointStack(const FPointStack& other) : maxSize(other.maxSize), top(other.top) {
		buffer = new SDL_FPoint[maxSize];
		for (int i = 0; i <= top; i++) {
			buffer[i] = other.buffer[i];
		}
	}

	FPointStack& operator=(const FPointStack& Other) {
		if (this == &Other)
			return *this;
		delete[] buffer;
		this->buffer = new SDL_FPoint[Other.maxSize];
		for (int i = 0; i <= Other.top; i++) {
			this->buffer[i] = Other.buffer[i];
		}
		this->top = Other.top;
		return *this;
	}
	void operator +=(const FPointStack& other) {
		SDL_FPoint* bufferTemp = new SDL_FPoint[this->maxSize + other.maxSize];
		if (!this->buffer || !other.buffer) {
			std::cout << "Buffer not allocated!\n";
			return;
		}
		if (this->top >= this->maxSize || other.top >= other.maxSize) {
			std::cout << "Top is out of bounds!\n";
			return;
		}

		for (int i = 0; i <= this->top; i++) {
			bufferTemp[i] = this->buffer[i];
		}
		delete[] this->buffer;
		for (int i = 0; i <= other.top; i++) {
			bufferTemp[this->top + 1 + i] = other.buffer[i];
		}
		this->buffer = bufferTemp;
		this->top += other.top + 1;
		this->maxSize += other.maxSize;
	}


	void push(SDL_FPoint p) {
		if (top >= maxSize - 1) {
			std::cout << "Stack is full, sorry :<\n";
			return;
		}
		buffer[++top] = p;
	}

	SDL_FPoint pop() {
		if (top == -1) {
			std::cout << "Stack is empty, sorry :<\n";
			return { 0, 0 };
		}
		SDL_FPoint elem = buffer[top--];
		return elem;
	}

	bool isEmpty() {
		return top == -1;
	}

	~FPointStack() {
		delete[] buffer;
	}
};

void drawArc(SDL_Renderer* renderer ,SDL_FPoint center, float radius, float angleStart, float angleEnd, float numOfPoints = 100) { //draws anticlockwise clockwise start->end
	
	auto circle = [center, radius](float angle) -> SDL_FPoint {
		return {center.x + radius * std::cos(angle) ,center.y - radius * std::sin(angle)};
	};
	SDL_FPoint *pixels = new SDL_FPoint[numOfPoints+1];
	float delta = (angleEnd - angleStart)/numOfPoints;
	for (int i = 0; i < numOfPoints+1; i++) {
		pixels[i] = circle(angleStart + i * delta);
	}
	SDL_RenderPoints(renderer, pixels, numOfPoints+1);	

	delete[] pixels;
}

float centralDiff(std::function<float(float)> func, float x, float deltaX = 0.0069f) {
	return (-func(x + 2 * deltaX)
		+ 8 * func(x + deltaX)
		- 8 * func(x - deltaX)
		+ func(x - 2 * deltaX)) / (12 * deltaX);
}

float numericalIntTrap(std::function<float(float)> func, float a, float b, int n = 300) {
	float deltaX = (b - a) / n;
	float output = func(a);
	for (int i = 1; i < n; i++) {
		output += 2*func(a + i * deltaX);
	}
	output += func(b);
	output *= deltaX / 2;

	return output;
}

float pointToAngle(FPointStack points, float& theta, float& alpha, float& beta) {

}

float calcFuncPath(std::function<float(float)> func, float startPoint, float endPoint) {
	auto intermediateFunc = [func, startPoint, endPoint](float x) {
		return (float)std::sqrt(1 + std::pow(centralDiff(func, x), 2));
		};
	return std::abs(numericalIntTrap(intermediateFunc, startPoint, endPoint));
}

float circFunc(float x) { return std::sqrt((40 - (3*x * x))); }
float straightLine(float x) {
	return circFunc(100);
}

FPointStack followPath(std::function<float(float)> path, float x1, float x2, float speed, float FPS, float yStart, float xStart) { //m/s, f/s, m
	int frames = std::ceil((FPS * calcFuncPath(path, x1, x2) / speed));
	if (frames <= 0) frames = 1; // prevent zero or negative allocation
	FPointStack points(frames);

	float delta = (x2 - x1)/frames;
	for (int i = 0; i < frames; i++) {
		points.push({ xStart + x1 + i * delta, yStart + path(x1 + i*delta) });
	}

	return points;
}

FPointStack genSemi(float sPoint, float ePoint, float begY, int n = 500) {
	float radius = (ePoint - sPoint) / 2.0f;
	float centerX = (ePoint + sPoint) / 2.0f;

	auto semiCirc = [radius, centerX, begY](float x) -> float {
		float val = radius * radius - (x - centerX) * (x - centerX);
		if (val < 0) val = 0;
		return begY - std::sqrt(val);
		};

	FPointStack points(n + 1);
	float delta = (ePoint - sPoint) / n;

	for (int i = 0; i <= n; i++) {
		float x = sPoint + i * delta;
		points.push({ x, semiCirc(x) });
	}

	return points;
}


int main() {
	float upperLimit, lowerLimit, mouseX = 0, mouseY = 0;
	float RH, beta, alpha, theta, l1, l2, a; //Known parameters
	
	upperLimit = 100;
	lowerLimit = 403;
	float ground = 600;
	RH = lowerLimit - upperLimit;
	l1 = 300;
	l2 = 300;
	a = 400;
	SDL_FPoint midPoint1 = { 0, 0 };

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::cout << "Initilization error.\n";
	}

	if (!TTF_Init()) {
		std::cout << "TTF_Init failed: " << "\n";
		SDL_Quit();
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Window", 800, 800, SDL_WINDOW_RESIZABLE); 

	if (!window) {
		std::cout << "Failed to create SDL window.\n";
		SDL_Quit();
		return 1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
	bool success = SDL_SetRenderVSync(renderer, 1);
	if (!success) {
		std::cout << "Could not enable VSync! SDL error: " << SDL_GetError() << std::endl;
	}


	if (!renderer) {
		std::cout << "Failed to create renderer.\n";
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	float epsilon = 5;
	bool running = true;
	bool upper = false;
	bool lower = false;
	bool slider = false;
	bool paused = false;
	SDL_Event e;
	int frameCount = 0;
	double FPS = 0;
	FPointStack buffer = genSemi(300, 600, ground, 400);
	const float targetFPS = 120.0f;
	const float frameTime = 1000.0f / targetFPS;

	while (running) {
		Uint64 frameStart = SDL_GetTicks();
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT) {
				running = false;
			}
			else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
				lower = (e.button.y <= (lowerLimit + epsilon)) && (e.button.y >= (lowerLimit - epsilon));
				slider = (e.button.x <= (a + epsilon)) && (e.button.x >= (a - epsilon));
				upper = (e.button.y <= (upperLimit + epsilon)) && (e.button.y >= (upperLimit - epsilon));
				paused = lower || slider || upper;
			}
			else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
				lower = false;
				upper = false;
				slider = false;
				paused = false;
			}
		}
		if (paused) {
			if (lower) {
				SDL_GetMouseState(&mouseX, &mouseY);
				if (mouseY < upperLimit)
					lowerLimit = upperLimit - 1;
				else
					lowerLimit = mouseY;

				if (lowerLimit > ground) lowerLimit = ground;
				else if (lowerLimit - upperLimit > l1 + l2)lowerLimit = l2 + l1 + upperLimit;
			}
			if (upper) {
				SDL_GetMouseState(&mouseX, &mouseY);
				if (mouseY > lowerLimit)
					upperLimit = lowerLimit + 1;
				else
					upperLimit = mouseY;

				if (lowerLimit - upperLimit > l1 + l2) upperLimit = lowerLimit - l1 - l2;
			}
			if (slider) {
				SDL_GetMouseState(&mouseX, &mouseY);
				a = mouseX;
			}
		}
		else if (buffer.isEmpty()){
			buffer = genSemi(300, 600, ground, 400);
		}
		else {
			SDL_FPoint p = buffer.pop();
			lowerLimit = p.y;
			a = p.x;
			std::cout << a << ", " << lowerLimit << std::endl;
		}


		RH = lowerLimit - upperLimit;
		float dx = a - 400;
		float dy = RH;
		float effectiveDist = std::sqrt(dx * dx + dy * dy);
		float A = (l1 * l1 + l2 * l2 - effectiveDist * effectiveDist) / (2 * l1 * l2);
		if (A < -1) A = -1;
		if (A > 1) A = 1;
		float alpha = std::acos(A);

		float gamma = std::atan2(dy, dx);
		float cosB = (effectiveDist * effectiveDist + l1 * l1 - l2 * l2) / (2 * l1 * effectiveDist);
		if (cosB < -1) cosB = -1;
		if (cosB > 1) cosB = 1;
		float theta = gamma - std::acos(cosB);

		beta = alpha - theta;

		auto startPoint = SDL_FPoint({ 400, upperLimit });
		auto lowerPoint = SDL_FPoint({ a, lowerLimit });
		midPoint1 = SDL_FPoint({ 400 + l1 * std::cos(theta), upperLimit + l1 * std::sin(theta) });

		/*std::cout << std::sqrt(std::pow((startPoint.x - midPoint1.x), 2));*/

		std::cout << "Angles: beta - " << beta << ",Alpha - " << alpha << ",theta - " << theta << ", " << std::endl;
		//std::cout << "Lengths: l1 - " << l1 << ",l2 - " << l2 << ",RH - " << RH << std::endl;
		//std::cout << midPoint1.x << ",  " << midPoint1.y << ", " << A << std::endl;

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderLine(renderer, 0, lowerLimit, 800, lowerLimit);
		SDL_RenderLine(renderer, 0, upperLimit, 800, upperLimit);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderLine(renderer, startPoint.x, startPoint.y, midPoint1.x, midPoint1.y); //drawing section 1
		SDL_RenderLine(renderer, midPoint1.x, midPoint1.y, lowerPoint.x, lowerPoint.y); //drawing section 2

		SDL_SetRenderDrawColor(renderer, 255, 206, 0, 255);
		drawArc(renderer, { 400, upperLimit }, 15, -theta, 0);
		drawArc(renderer, { midPoint1.x, midPoint1.y }, 15, PI - theta, PI - theta + alpha);
		drawArc(renderer, { a, lowerLimit }, 15, 0, beta);

		SDL_SetRenderDrawColor(renderer, 150, 75, 0, 255);
		SDL_RenderLine(renderer, 0, ground, 800, ground);

		SDL_RenderPresent(renderer);
		Uint64 elapsed = SDL_GetTicks() - frameStart;
		if (elapsed < frameTime) {
			SDL_Delay(frameTime - elapsed);
		}
	}



	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
//DSCO - 4, DS - 4, DMS - 3, DCCN - 5, DA - 5
