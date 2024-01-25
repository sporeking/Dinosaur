#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 400
#define DINO_WIDTH 80
#define DINO_HEIGHT 86
#define ACCELERATING_WIDTH 22
#define ACCELERATING_HEIGHT 86
#define LOW_DINO_WIDTH 110
#define LOW_DINO_HEIGHT 52
#define BIRD_WIDTH 86
#define BIRD_HEIGHT 72
#define CACTUS12_WIDTH 30
#define CACTUS12_HEIGHT 66
#define CACTUS34_WIDTH 45
#define CACTUS34_HEIGHT 99
#define CACTUS5_WIDTH 44
#define CACTUS5_HEIGHT 94
#define CACTUS6_WIDTH 98
#define CACTUS6_HEIGHT 94
#define CACTUS7_WIDTH 44
#define CACTUS7_HEIGHT 92
#define GROUND_HEIGHT 380

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *window_surface;
SDL_Surface *start_surface;
SDL_Surface *pause_surface;
SDL_Surface *game_over_surface;
SDL_Surface *ground_surface;
SDL_Surface *cloud_surface;
SDL_Surface *dino_surface;
SDL_Surface *low_dino_surface;
SDL_Surface *accelerating_surface;
SDL_Surface *barrier_surface[11];

SDL_Texture *ground;
SDL_Texture *cloud;
SDL_Texture *start;
SDL_Texture *pause;
SDL_Texture *game_over;
SDL_Texture *dino;
SDL_Texture *low_dino;
SDL_Texture *accelerating;
SDL_Texture *barrier[11];
SDL_Texture *next_barrier;

SDL_Rect barrier_rect[11];
SDL_Rect ground_rect1;
SDL_Rect ground_rect2;
SDL_Rect dino_rect;
SDL_Rect low_dino_rect;
SDL_Rect dino_clip[3];
SDL_Rect low_dino_clip[2];
SDL_Rect bird_clip[2];
SDL_Rect cloud_rect[100];
SDL_Rect accelerating_rect;
SDL_Rect pause_rect;
SDL_Rect game_over_rect;

SDL_Event event;

//font
TTF_Font *score_font = NULL;
SDL_Surface *score_surface = NULL;
SDL_Surface *highest_score_surface = NULL;
SDL_Surface *hi_surface = NULL;
SDL_Texture *score_texture = NULL;
SDL_Texture *highest_score_texture = NULL;
SDL_Texture *hi_texture = NULL;
SDL_Rect score_rect;
SDL_Rect highest_score_rect;
SDL_Rect hi_rect;
SDL_Color score_color = {83, 83, 83, 255};

time_t start_time;
time_t stop_time;

Mix_Music *music = NULL;
Mix_Chunk *jump_chunk = NULL;
Mix_Chunk *over_chunk = NULL;

int speed = 8;
int vertical_speed = 18;
int score = 0;
int real_score = 0;
int highest_score = 0;
int expected_distance = 0, real_distance = 0;
int expected_cloud_distance = 0, real_cloud_distance = 0;
int dino_clip_num = 0;
int barrier_num = 0;
int bird_clip_num = 0;
int distance_time = 0;
int pause_time = 0;

bool is_on_floor = true;
bool is_dino_down = false;
bool is_stop = false;
bool isOpen = true;
bool isStart = false;
bool isAccelerate = false;

void Init();
void Get_highest_score(int *highest_score);
void Store_highest_score(int score);
void Pause();
void Moveground();
void time_and_score();
void Appear_barriers();
void Appear_dinosaur();
void Appear_clouds();
void Crush_or_not();
void Restart();
void Reinit();
bool is_overlap(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2);
void Quit();
int minimum(int a, int b);

#undef main

int main(int argc, char *argv[])
{
    start_time = time(NULL);

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_StopTextInput();
    TTF_Init();
    window = SDL_CreateWindow("Dino!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    score_font = TTF_OpenFont("PublicPixel.ttf", 20);

    int rect_w[11] = {CACTUS12_WIDTH, CACTUS12_WIDTH, CACTUS34_WIDTH, CACTUS34_WIDTH, CACTUS5_WIDTH, CACTUS6_WIDTH, CACTUS7_WIDTH, BIRD_WIDTH, BIRD_WIDTH, BIRD_WIDTH, BIRD_WIDTH};
    int rect_h[11] = {CACTUS12_HEIGHT, CACTUS12_HEIGHT, CACTUS34_HEIGHT, CACTUS34_HEIGHT, CACTUS5_HEIGHT, CACTUS6_HEIGHT, CACTUS7_HEIGHT, BIRD_HEIGHT, BIRD_HEIGHT, BIRD_HEIGHT, BIRD_HEIGHT};
    for(int i = 0; i <= 6; i++)
    {
        barrier_rect[i].w = rect_w[i];
        barrier_rect[i].h = rect_h[i];
        barrier_rect[i].x = WINDOW_WIDTH;
        barrier_rect[i].y = GROUND_HEIGHT - rect_h[i];
    }
    for(int i = 7; i <= 10; i++)
    {
        barrier_rect[i].w = rect_w[i];
        barrier_rect[i].h = rect_h[i];
        barrier_rect[i].x = WINDOW_WIDTH;
        barrier_rect[i].y = GROUND_HEIGHT - (i - 6) * rect_h[i];
    }
    Get_highest_score(&highest_score);

    bool is_first_start = true;

    window_surface = SDL_GetWindowSurface(window);
    ground_surface = IMG_Load("pictures/ground.png");
    cloud_surface = IMG_Load("pictures/cloud.png");
    dino_surface = IMG_Load("pictures/dino.png");
    low_dino_surface = IMG_Load("pictures/lowdino.png");
    start_surface = IMG_Load("pictures/start.png");
    pause_surface = IMG_Load("pictures/pause.png");
    game_over_surface = IMG_Load("pictures/game_over.png");
    barrier_surface[0] = IMG_Load("pictures/cactus1.png");
    barrier_surface[1] = IMG_Load("pictures/cactus2.png");
    barrier_surface[2] = IMG_Load("pictures/cactus3.png");
    barrier_surface[3] = IMG_Load("pictures/cactus4.png");
    barrier_surface[4] = IMG_Load("pictures/cactus5.png");
    barrier_surface[5] = IMG_Load("pictures/cactus6.png");
    barrier_surface[6] = IMG_Load("pictures/cactus7.png");
    barrier_surface[7] = IMG_Load("pictures/bird.png");
    barrier_surface[8] = IMG_Load("pictures/bird.png");
    barrier_surface[9] = IMG_Load("pictures/bird.png");
    barrier_surface[10] = IMG_Load("pictures/bird.png");
    accelerating_surface = IMG_Load("pictures/accelerating.png");

    ground = SDL_CreateTextureFromSurface(renderer, ground_surface);
    cloud = SDL_CreateTextureFromSurface(renderer, cloud_surface);
    dino = SDL_CreateTextureFromSurface(renderer, dino_surface);
    low_dino = SDL_CreateTextureFromSurface(renderer, low_dino_surface);
    accelerating = SDL_CreateTextureFromSurface(renderer, accelerating_surface);
    start = SDL_CreateTextureFromSurface(renderer, start_surface);
    pause = SDL_CreateTextureFromSurface(renderer, pause_surface);
    game_over = SDL_CreateTextureFromSurface(renderer, game_over_surface);
    for(int i = 0; i <= 10; i++)
        barrier[i] = SDL_CreateTextureFromSurface(renderer, barrier_surface[i]);

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    music = Mix_LoadMUS("music/BGM.wav");
    over_chunk = Mix_LoadWAV("music/game_over.wav");
    jump_chunk = Mix_LoadWAV("music/jump.wav");

    ground_rect1.x = 0;
    ground_rect1.y = 0; 
    ground_rect1.w = ground_surface -> w;
    ground_rect1.h = ground_surface -> h;
    ground_rect2.x = WINDOW_WIDTH;
    ground_rect2.y = 0;
    ground_rect2.w = ground_surface -> w;
    ground_rect2.h = ground_surface -> h;
    dino_rect.x = 50;
    dino_rect.y = GROUND_HEIGHT - DINO_HEIGHT;
    dino_rect.w = dino_surface -> w / 3;
    dino_rect.h = dino_surface -> h;
    low_dino_rect.x = 50;
    low_dino_rect.y = GROUND_HEIGHT - LOW_DINO_HEIGHT;
    low_dino_rect.w = low_dino_surface -> w;
    low_dino_rect.h = low_dino_surface -> h / 2;
    accelerating_rect.x = dino_rect.x - ACCELERATING_WIDTH;
    accelerating_rect.y = dino_rect.y;
    accelerating_rect.w = ACCELERATING_WIDTH;
    accelerating_rect.h = ACCELERATING_HEIGHT;
    pause_rect.x = 0;
    pause_rect.y = 0;
    pause_rect.w = pause_surface -> w;
    pause_rect.h = pause_surface -> h;
    game_over_rect.x = 0;
    game_over_rect.y = 0;
    game_over_rect.w = game_over_surface -> w;
    game_over_rect.h = game_over_surface -> h;
    for(int i = 0; i <= 2; i++)
    {
        dino_clip[i].x = i * DINO_WIDTH;
        dino_clip[i].y = 0;
        dino_clip[i].w = DINO_WIDTH;
        dino_clip[i].h = DINO_HEIGHT;
    }
    for(int i = 0; i <= 1; i++)
    {
        low_dino_clip[i].x = 0;
        low_dino_clip[i].y = i * LOW_DINO_HEIGHT;
        low_dino_clip[i].w = LOW_DINO_WIDTH;
        low_dino_clip[i].h = LOW_DINO_HEIGHT;
        bird_clip[i].x = i * BIRD_WIDTH + 2 * i;//不加一总是出现一个像素点影响观感
        bird_clip[i].y = 0;
        bird_clip[i].w = BIRD_WIDTH;
        bird_clip[i].h = BIRD_HEIGHT;
    }
    for(int i = 0; i <= 99; i++)
    {
        cloud_rect[i].x = WINDOW_WIDTH;
        cloud_rect[i].y = rand() % 250 + 50;
        cloud_rect[i].w = cloud_surface -> w;
        cloud_rect[i].h = cloud_surface -> h;
    }

    Init();

    //main game loop
    while(isOpen)
    {
        if(!isStart)
        {
            if(distance_time > 0)
                Restart();
            while(SDL_PollEvent(&event))
            {
                if(event.type == SDL_QUIT)
                    isOpen = false;
                if(event.type == SDL_KEYDOWN)
                    if(event.key.keysym.sym == SDLK_RETURN)
                    {
                        srand((unsigned)time(NULL));
                        Mix_PlayMusic(music, -1);
                        isStart = true;
                        is_first_start = false;
                        for(int i = 0; i <= 10; i++)
                        {
                            barrier_rect[i].x = WINDOW_WIDTH;
                        }
                        dino_rect.y = GROUND_HEIGHT - DINO_HEIGHT;
                        start_time = time(NULL);
                        score = 0;
                        is_on_floor = true;
                        is_dino_down = false;
                        vertical_speed = 18;
                    }
            }
        }
        if(isStart)
        {
            while(SDL_PollEvent(&event))
            {
                if(event.type == SDL_QUIT)
                    isOpen = false;
                if((event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_RIGHT))
                {
                    if(event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        stop_time = time(NULL);
                        is_stop = true;
                        Mix_PauseMusic();
                    }
                    else if(event.key.keysym.sym == SDLK_RETURN && is_stop)
                    {
                        pause_time += time(NULL) - stop_time;
                        is_stop = false;
                        Mix_ResumeMusic();
                    }
                    if(event.key.keysym.sym == SDLK_RIGHT)
                        isAccelerate = true;
                    if(event.key.keysym.sym == SDLK_DOWN)
                        is_dino_down = true;
                    else if(!is_dino_down && (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_SPACE))
                    {
                        if(is_on_floor)
                            Mix_PlayChannel(-1, jump_chunk, 0);
                        is_on_floor = false;
                    }    
                }
                if(event.type == SDL_KEYUP)
                {
                    if(event.key.keysym.sym == SDLK_DOWN)
                    {
                        is_dino_down = false;
                        is_on_floor = true;
                        vertical_speed = 18;
                        dino_rect.y = GROUND_HEIGHT - DINO_HEIGHT;
                    } 
                    if(event.key.keysym.sym == SDLK_RIGHT)
                        isAccelerate = false;
                }       
                
            }
            if(is_stop)
                Pause();
            else
            {
                SDL_RenderClear(renderer);
                Moveground();
                Appear_clouds();
                Appear_barriers();
                Appear_dinosaur();
                time_and_score();
                Crush_or_not();
            }
            SDL_RenderPresent(renderer);
            SDL_Delay(1000 / 60);
        }
    }
    Quit();
    return 0;
}

void Get_highest_score(int *highest_score)
{
    FILE *highest_score_file = fopen("highest_score.txt", "r");
    if(highest_score_file == NULL)
    {
        highest_score_file = fopen("highest_score.txt", "w");
        fprintf(highest_score_file, "%05d", *highest_score);
    }
    else
        fscanf(highest_score_file, "%d", highest_score);
    fclose(highest_score_file);
}

void Store_highest_score(int score)
{
    if(real_score >= highest_score)
    {
        FILE *highest_score_file = NULL;
        highest_score_file = fopen("highest_score.txt", "w");
        fprintf(highest_score_file, "%05d", real_score);
        fclose(highest_score_file);
        char highest_score_array[6];
        Get_highest_score(&highest_score);
        sprintf(highest_score_array, "%05d", highest_score);
        highest_score_surface = TTF_RenderUTF8_Blended(score_font, highest_score_array, score_color);
        highest_score_texture = SDL_CreateTextureFromSurface(renderer, highest_score_surface);
    }
    else
        return;
}

void Init()
{
    //init the "HI"
    hi_surface = TTF_RenderUTF8_Blended(score_font, "HI", score_color);
    hi_texture = SDL_CreateTextureFromSurface(renderer, hi_surface);
    hi_rect.x = 1270;
    hi_rect.y = 30;
    hi_rect.w = hi_surface -> w;
    hi_rect.h = hi_surface -> h;
    //init the score
    char score_array[6];
    sprintf(score_array, "%05d", score);
    score_surface = TTF_RenderUTF8_Blended(score_font, score_array, score_color);
    score_texture = SDL_CreateTextureFromSurface(renderer, score_surface);
    score_rect.x = 1450;
    score_rect.y = 30;
    score_rect.w = score_surface -> w;
    score_rect.h = score_surface -> h;
    //init the highest score
    char highest_score_array[6];
    sprintf(highest_score_array, "%05d", highest_score);
    highest_score_surface = TTF_RenderUTF8_Blended(score_font, highest_score_array, score_color);
    highest_score_texture = SDL_CreateTextureFromSurface(renderer, highest_score_surface);
    highest_score_rect.x = 1330;
    highest_score_rect.y = 30;
    highest_score_rect.w = highest_score_surface -> w;
    highest_score_rect.h = highest_score_surface -> h;
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, start, NULL, &ground_rect1);
    SDL_RenderCopy(renderer, dino, &dino_clip[2], &dino_rect);
    SDL_RenderPresent(renderer);
}

void Moveground()
{
    SDL_RenderCopy(renderer, ground, NULL, &ground_rect1);
    SDL_RenderCopy(renderer, ground, NULL, &ground_rect2);
    ground_rect1.x -= speed;
    ground_rect2.x -= speed;
    if(ground_rect1.x <= -WINDOW_WIDTH)
        ground_rect1.x = ground_rect2.x + WINDOW_WIDTH;
    if(ground_rect2.x <= -WINDOW_WIDTH)
        ground_rect2.x = ground_rect1.x + WINDOW_WIDTH;    
}

void Appear_clouds()
{
    real_cloud_distance = WINDOW_WIDTH;
    for(int i = 0; i <= 99; i++)
    {
        if(cloud_rect[i].x < WINDOW_WIDTH)
        {
            cloud_rect[i].x -= speed / 2;
            SDL_RenderCopy(renderer, cloud, NULL, &cloud_rect[i]);
            if(cloud_rect[i].x <= - (cloud_surface -> w))
            cloud_rect[i].x = WINDOW_WIDTH;
            if(real_cloud_distance >= WINDOW_WIDTH - cloud_rect[i].x)
                real_cloud_distance = WINDOW_WIDTH - cloud_rect[i].x;
        }
    }
    if(real_cloud_distance >= expected_cloud_distance)
    {
        int the_cloud_num = 0;//某个瞬间产生的云朵的编号
        while(cloud_rect[the_cloud_num].x != WINDOW_WIDTH)
            the_cloud_num = rand() % 100;
        cloud_rect[the_cloud_num].x -= speed / 2;
        expected_cloud_distance = rand() % 280 + 100;
    }
    return;
}

void time_and_score()
{
    distance_time = (int)time(NULL) - (int)start_time;
    score += speed / 5;
    real_score = score / 5;//a way to get score;
    char score_array[6];
    sprintf(score_array, "%05d", real_score);
    score_surface = TTF_RenderUTF8_Blended(score_font, score_array, score_color);
    score_texture = SDL_CreateTextureFromSurface(renderer, score_surface);
    score_rect.w = score_surface -> w;
    SDL_RenderCopy(renderer, hi_texture, NULL, &hi_rect);
    SDL_RenderCopy(renderer, score_texture, NULL, &score_rect);
    if(real_score <= highest_score)
        SDL_RenderCopy(renderer, highest_score_texture, NULL, &highest_score_rect);
    else
        SDL_RenderCopy(renderer, score_texture, NULL, &highest_score_rect);
    SDL_FreeSurface(score_surface);
    SDL_DestroyTexture(score_texture);
    if(distance_time >= 60)
        speed = 20;
    else if(distance_time >= 50)
        speed = 18;
    else if(distance_time >= 40)
        speed = 16;
    else if(distance_time >= 30)
        speed = 14;
    else if(distance_time >= 20)
        speed = 12;
    else if(distance_time >= 10)
        speed = 10;
    else 
        speed = 8;
    if(isAccelerate)
        speed += 5;
}

void Appear_barriers()
{
    real_distance = WINDOW_WIDTH;
    for(int i = 0; i <= 10; i++)//遍历所有障碍物，若处于画面中，则使其左移， 并且寻找最靠前的障碍物用于碰撞检测
    {
        if(barrier_rect[i].x < WINDOW_WIDTH)
        {
            barrier_rect[i].x -= speed;
            if(i <= 6)
                SDL_RenderCopy(renderer, barrier[i], NULL, &barrier_rect[i]);   
            else 
                SDL_RenderCopy(renderer, barrier[i], &bird_clip[(bird_clip_num++ % 20) / 10], &barrier_rect[i]);       
            if(real_distance >= WINDOW_WIDTH - barrier_rect[i].x)
                real_distance = WINDOW_WIDTH - barrier_rect[i].x;
        }    
        if(barrier_rect[i].x < -barrier_rect[i].w)
            barrier_rect[i].x = WINDOW_WIDTH;
        if(barrier_rect[i].x < barrier_rect[barrier_num].x)
            barrier_num = i;
    }
    if(real_distance >= expected_distance)//当最后一个障碍物的距离大于预期两个障碍物之间的距离 生成一个新的障碍物
    {
        int the_barrier_num;//待随机初始化的障碍物序号
        while(barrier_rect[the_barrier_num].x != WINDOW_WIDTH)
            the_barrier_num = rand() % 11;
        barrier_rect[the_barrier_num].x -= speed;
        expected_distance = rand() % 500 + 500; 
    }   
}

void Appear_dinosaur()
{
    if(is_dino_down)//when the DOWN key is being pressed
    {
        if(!is_on_floor)
        {
            dino_rect.y += minimum(18, GROUND_HEIGHT - DINO_HEIGHT);
            accelerating_rect.y = dino_rect.y;
            SDL_RenderCopy(renderer, dino, &dino_clip[2], &dino_rect);
            vertical_speed = -minimum(18, GROUND_HEIGHT - DINO_HEIGHT);
            if(dino_rect.y >= GROUND_HEIGHT - DINO_HEIGHT)
            {
                is_on_floor = true;
                vertical_speed = 18;
            }
        }
        else
        {
            SDL_RenderCopy(renderer, low_dino, &low_dino_clip[(dino_clip_num % ((30 - speed) * 2)) / (30 - speed)], &low_dino_rect);
            dino_clip_num += 2;
        }
        
    }
    else if(is_on_floor)
    {
        SDL_RenderCopy(renderer, dino, &dino_clip[(dino_clip_num % ((30 - speed) * 2)) / (30 - speed)], &dino_rect);
        dino_clip_num += 2;
    }
    else//when the dino in the sky and you do not press DOWN
    {
        dino_rect.y -= (vertical_speed--);
        SDL_RenderCopy(renderer, dino, &dino_clip[2], &dino_rect);
        if(dino_rect.y >= GROUND_HEIGHT - DINO_HEIGHT)
        {
            is_on_floor = true;
            vertical_speed = 18;
        }
    }   
    if(isAccelerate)
    {
        accelerating_rect.y = dino_rect.y;
        SDL_RenderCopy(renderer, accelerating, NULL, &accelerating_rect);
    }
        
}

void Crush_or_not()
{
    if(is_on_floor && is_dino_down)//the dino is down on the floor
    {
        if(is_overlap(low_dino_rect.x, low_dino_rect.y, low_dino_rect.w, low_dino_rect.h, barrier_rect[barrier_num].x, barrier_rect[barrier_num].y, barrier_rect[barrier_num].w, barrier_rect[barrier_num].h))
        {
            Reinit();
            Store_highest_score(real_score);
        }    
    }    
    else //先检测两个宽矩形是否碰撞，若宽矩形碰撞，则将宽矩形细分为数个小矩形进行碰撞检测
        if(is_overlap(dino_rect.x, dino_rect.y, dino_rect.w, dino_rect.h, barrier_rect[barrier_num].x, barrier_rect[barrier_num].y, barrier_rect[barrier_num].w, barrier_rect[barrier_num].h))
        {
            if(barrier_num <= 1)
            {
                if(is_overlap(dino_rect.x + 40, dino_rect.y, 40, 30, barrier_rect[barrier_num].x, barrier_rect[barrier_num].y + 20, 30, 33) || is_overlap(dino_rect.x + 40, dino_rect.y, 40, 30, barrier_rect[barrier_num].x + 10, barrier_rect[barrier_num].y, 10, 66) || is_overlap(dino_rect.x + 15, dino_rect.y + 30, 36, 56, barrier_rect[barrier_num].x, barrier_rect[barrier_num].y + 10, 30, 33) || is_overlap(dino_rect.x + 15, dino_rect.y + 30, 40, 56, barrier_rect[barrier_num].x + 10, barrier_rect[barrier_num].y, 10, 66))
                {
                    Reinit();
                    Store_highest_score(real_score);
                    return;
                }
            }
            else if(barrier_num <= 3)
            {
                if(is_overlap(dino_rect.x + 40, dino_rect.y, 40, 30, barrier_rect[barrier_num].x, barrier_rect[barrier_num].y + 13, 45, 62) || is_overlap(dino_rect.x + 40, dino_rect.y, 40, 30, barrier_rect[barrier_num].x + 15, barrier_rect[barrier_num].y, 15, 99) || is_overlap(dino_rect.x + 15, dino_rect.y + 30, 40, 56, barrier_rect[barrier_num].x, barrier_rect[barrier_num].y + 13, 45, 62) || is_overlap(dino_rect.x + 15, dino_rect.y + 30, 40, 56, barrier_rect[barrier_num].x + 15, barrier_rect[barrier_num].y, 15, 99))
                {
                    Reinit();
                    Store_highest_score(real_score);
                    return;
                }
            }
            else if(barrier_num == 4)
            {
                if(is_overlap(dino_rect.x + 40, dino_rect.y, 40, 30, barrier_rect[barrier_num].x, barrier_rect[barrier_num].y + 24, 44, 40) || is_overlap(dino_rect.x + 40, dino_rect.y, 40, 30, barrier_rect[barrier_num].x + 16, barrier_rect[barrier_num].y, 14, 94) || is_overlap(dino_rect.x + 15, dino_rect.y + 30, 40, 56, barrier_rect[barrier_num].x, barrier_rect[barrier_num].y + 24, 44, 40) || is_overlap(dino_rect.x + 15, dino_rect.y + 30, 40, 56, barrier_rect[barrier_num].x + 16, barrier_rect[barrier_num].y, 14, 94))
                {
                    Reinit();
                    Store_highest_score(real_score);
                    return;
                }
            }
            else if(barrier_num == 5)
            {
                if(is_overlap(dino_rect.x + 40, dino_rect.y, 40, 30, barrier_rect[barrier_num].x, barrier_rect[barrier_num].y + 32, 98, 34) || is_overlap(dino_rect.x + 40, dino_rect.y, 40, 30, barrier_rect[barrier_num].x + 12, barrier_rect[barrier_num].y, 70, 94) || is_overlap(dino_rect.x + 15, dino_rect.y + 30, 40, 56, barrier_rect[barrier_num].x, barrier_rect[barrier_num].y + 32, 98, 34) || is_overlap(dino_rect.x + 15, dino_rect.y + 30, 40, 56, barrier_rect[barrier_num].x + 12, barrier_rect[barrier_num].y, 70, 94))
                {
                    Reinit();
                    Store_highest_score(real_score);
                    return;
                }
            }
            else if(barrier_num == 6)
            {
                if(is_overlap(dino_rect.x + 40, dino_rect.y, 40, 30, barrier_rect[barrier_num].x, barrier_rect[barrier_num].y + 24, 44, 38) || is_overlap(dino_rect.x + 40, dino_rect.y, 40, 30, barrier_rect[barrier_num].x + 16, barrier_rect[barrier_num].y, 14, 92) || is_overlap(dino_rect.x + 15, dino_rect.y + 30, 40, 56, barrier_rect[barrier_num].x, barrier_rect[barrier_num].y + 24, 44, 38) || is_overlap(dino_rect.x + 15, dino_rect.y + 30, 40, 56, barrier_rect[barrier_num].x + 16, barrier_rect[barrier_num].y, 14, 92))
                {
                    Reinit();
                    Store_highest_score(real_score);
                    return;
                }
            }
            else
            {
                if(is_overlap(dino_rect.x + 40, dino_rect.y, 40, 30, barrier_rect[barrier_num].x + 8, barrier_rect[barrier_num].y + 12, 40, 20) || is_overlap(dino_rect.x + 40, dino_rect.y, 40, 30, barrier_rect[barrier_num].x + 36, barrier_rect[barrier_num].y + 4, 12, 64) || is_overlap(dino_rect.x + 40, dino_rect.y, 40, 30, barrier_rect[barrier_num].x + 47, barrier_rect[barrier_num].y + 25, 33, 27)|| is_overlap(dino_rect.x + 15, dino_rect.y + 30, 40, 56, barrier_rect[barrier_num].x + 8, barrier_rect[barrier_num].y + 12, 40, 20) || is_overlap(dino_rect.x + 15, dino_rect.y + 30, 40, 56, barrier_rect[barrier_num].x + 36, barrier_rect[barrier_num].y + 4, 12, 64) || is_overlap(dino_rect.x + 15, dino_rect.y + 30, 40, 56, barrier_rect[barrier_num].x + 47, barrier_rect[barrier_num].y + 25, 33, 27))
                {
                    Reinit();
                    Store_highest_score(real_score);
                    return;
                }
            }
        }
}

//四角检测两个矩形是否重叠
bool is_overlap(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2)
{
    if((x1 <= x2 && x1 + width1 >= x2) || (x1 <= x2 + width2 && x1 + width1 >= x2 + width2))
        if((y1 <= y2 && y1 + height1 >= y2) || (y1 <= y2 + height2 && y1 + height1 >= y2 + height2))
            return true;
    return false;
}

void Quit()
{
    SDL_FreeSurface(window_surface);
    SDL_FreeSurface(start_surface);
    SDL_FreeSurface(pause_surface);
    SDL_FreeSurface(game_over_surface);
    SDL_FreeSurface(ground_surface);
    SDL_FreeSurface(cloud_surface);
    SDL_FreeSurface(dino_surface);
    SDL_FreeSurface(low_dino_surface);
    for(int i = 0; i <= 10; i++)
        SDL_FreeSurface(barrier_surface[i]);
    SDL_FreeSurface(score_surface);
    SDL_FreeSurface(highest_score_surface);
    SDL_FreeSurface(hi_surface);

    Mix_FreeMusic(music);
    Mix_FreeChunk(jump_chunk);
    Mix_FreeChunk(over_chunk);

    SDL_DestroyTexture(ground);
    SDL_DestroyTexture(cloud);
    SDL_DestroyTexture(dino);
    SDL_DestroyTexture(start);
    SDL_DestroyTexture(pause);
    SDL_DestroyTexture(game_over);
    SDL_DestroyTexture(low_dino);
    SDL_DestroyTexture(accelerating);
    SDL_DestroyTexture(next_barrier);
    for(int i = 0; i <= 10; i++)
        SDL_DestroyTexture(barrier[i]);
    SDL_DestroyTexture(score_texture);
    SDL_DestroyTexture(highest_score_texture);
    SDL_DestroyTexture(hi_texture);

    SDL_DestroyRenderer(renderer);

    TTF_CloseFont(score_font);
    SDL_DestroyWindow(window);
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Pause()
{
    SDL_PauseAudio(SDL_TRUE);
    SDL_RenderCopy(renderer, pause, NULL, &pause_rect);
}

void Restart()
{
    SDL_RenderCopy(renderer, game_over, NULL, &game_over_rect);
    SDL_RenderPresent(renderer);
}

void Reinit()
{
    Mix_HaltMusic();
    Mix_PlayChannel(-1, over_chunk, 0);
    isStart = false;
    isAccelerate = false;
    is_dino_down = false;
    pause_time = 0;
    accelerating_rect.y = GROUND_HEIGHT - DINO_HEIGHT;
}

int minimum(int a, int b)
{
    if(a >= b)
        return b;
    else 
        return a;
}