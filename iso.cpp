#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#define WIDTH 920
#define HEIGHT 620
#define FONT_SIZE 32
#define TILE_SIZE 64

struct object {
 SDL_Rect dest, src;
 SDL_Texture* img;
};

object grid[12][12];
object selected;

bool running;

SDL_Renderer* renderer;
SDL_Window* window;
TTF_Font *font;
SDL_Color fcolor;

SDL_Point mouse;
int frameCount, timerFPS, lastFrame, fps;

SDL_Texture* setImage(std::string filename) {
    return IMG_LoadTexture(renderer, filename.c_str());
}

void draw(object o) {
  SDL_RenderCopyEx(renderer, o.img, &o.src, &o.dest, 0, NULL, SDL_FLIP_NONE);
}

SDL_Surface *surface;
SDL_Texture *texture;
SDL_Rect wrect;
void write(std::string text, int x, int y) {
 if (font == NULL) {
  fprintf(stderr, "error: font not found\n");
  exit(EXIT_FAILURE);
 }
 fcolor.r = 0;
 fcolor.g = 0;
 fcolor.b = 0;
 const char* t = text.c_str();
 surface = TTF_RenderText_Solid(font, t, fcolor);
 texture = SDL_CreateTextureFromSurface(renderer, surface);
 wrect.w = surface->w;
 wrect.h = surface->h;
 wrect.x = x-wrect.w;
 wrect.y = y-wrect.h;
 SDL_FreeSurface(surface);
 SDL_RenderCopy(renderer, texture, NULL, &wrect);
 SDL_DestroyTexture(texture);
}

void update() {
}

const Uint8 *keystates;
void input() {
    SDL_Event e;
    keystates = SDL_GetKeyboardState(NULL);
    while(SDL_PollEvent(&e)) {
        if(e.type == SDL_QUIT) running=false;
    }
    if(keystates[SDL_SCANCODE_ESCAPE]) running=false;

    SDL_GetMouseState(&mouse.x, &mouse.y);
}

bool touching;
std::string m;
void render() {
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_RenderClear(renderer);

    frameCount++;
    int timerFPS = SDL_GetTicks()-lastFrame;
    if(timerFPS<(1000/60)) {
        SDL_Delay((1000/60)-timerFPS);
    }

    touching=0;
    for(int i=0; i<12; i++) {
     for(int j=0; j<12; j++) {
      draw(grid[i][j]);
      if(SDL_PointInRect(&mouse, &grid[i][j].dest)) {
       m = std::to_string(i) + " " + std::to_string(j);
       selected.dest = grid[i][j].dest;
       touching=1;
       //SDL_RenderDrawRect(renderer, &grid[i][j].dest);
      }
     }
    }
    if(touching) {
        draw(selected);
        write(m, mouse.x, mouse.y);
    }

    SDL_RenderPresent(renderer);
}

void init() {
 object tile;
 tile.img=setImage("res/block.png");
 tile.src.w=32;tile.src.h=32;
 tile.src.x=tile.src.y=0;
 tile.dest.w=TILE_SIZE; tile.dest.h=TILE_SIZE;
 for(int i=0; i<12; i++) {
  for(int j=0; j<12; j++) {
   tile.dest.x=(i*TILE_SIZE - j*TILE_SIZE)/2 + 426;
   tile.dest.y=(j*TILE_SIZE + i*TILE_SIZE)/4 + 140;
   grid[i][j]=tile;
  }
 }
 selected=tile; selected.img=setImage("res/select.png");
}

int main() {
    running=1;
    static int lastTime=0;
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) std::cout << "Failed at SDL_Init()" << std::endl;
    if(SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) std::cout << "Failed at SDL_CreateWindowAndRenderer()" << std::endl;
    SDL_SetWindowTitle(window, "Isometric Grid");
    TTF_Init();
    font = TTF_OpenFont("res/BebasNeue.ttf", FONT_SIZE);
    if(font == NULL) std::cout << "failed to load font" << std::endl;

    init();

    while(running) {
        lastFrame=SDL_GetTicks();
        if(lastFrame>=(lastTime+1000)) {
            lastTime=lastFrame;
            fps=frameCount;
            frameCount=0;
        }

        update();
        input();
        render();
    }
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
