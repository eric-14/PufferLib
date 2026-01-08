#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------

#define FRAMERATE 20 
#define MAX_TUBES 100
#define FLOPPY_RADIUS 24
#define TUBES_WIDTH 80

#define NUM_ENVS 10


//Try multiple iterations and test if 
//setting a lower goal scales to a higher goals 
#define GOAL 10000.0
#define NUM_OBS 3 

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Log {
    float perf; 
    float episode_length; 
    float episode_return; 
    float tick; 
    int size; 
    float n; 
    float hiScore; 
    // float rewards; 
    float score;              // score achieved in the game 
    float number_of_ups;     // number of time floppy is moved up 
                            // with this information it is a measure of understanding of game physics                     
    float goal; // the goal the RL should try and achieve 
} Log; 

typedef struct Client {
    int screenWidth; 
    int screenHeight; 
    char* title; 
} Client; 


typedef struct Floppy {
    Vector2 position;
    int radius;
    Color color;
} Floppy;

typedef struct Tubes {
    Rectangle rec;
    Color color;
    bool active;
} Tubes;

// Track the state of the game 
typedef struct GameState {
    bool gameOver; 
    bool pause; 
    bool superfx; 
} GameState; 


typedef struct GameEnv {
    Log log; 
    Client *client; 
    unsigned char* observations;
    int *actions;
    float *rewards; 
    unsigned char *terminals; 
    GameState gamestate; 
    Floppy floppy; 
    Tubes tubes[MAX_TUBES*2]; 
    Vector2 tubesPos[MAX_TUBES]; 
    int tubesSpeedx; 
    int num_envs;  
    int size; 

    
} GameEnv; 

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 450;



void freemem(GameEnv *env)
{
    // free(env->log); 
    free(env->observations); 
    free(env->rewards);
    free(env->terminals);  
    free(env->actions); 
}
// Update game (one frame)


void compute_observations(GameEnv *env, bool collided)
{
    //ball position 
    env->observations[0] =  env->floppy.position.x / screenWidth; 
    env->observations[1] =  env->floppy.position.y / screenHeight; 
    // Observations at each iteration of the game 
    env->observations[2] = collided ? 1 : 0; 

}
// Unload game variables
void UnloadGame(void)
{
    // TODO: Unload all dynamic loaded data (textures, sounds, models...)
}



void _game_init(GameEnv *env, int num_envs)
{
    //initialize game state 
    env->gamestate.gameOver = false; 
    env->gamestate.pause = false; 
    memset(&(env->log), 0,  sizeof(Log)); 
    env->log.hiScore = 0.0;
    env->log.score = 0.0;  
    env->log.episode_return = 0.0; 
    env->log.n = 0.0; 
    //printf("Starting value for score is -> %f and hiScore is -> %f\r\n", env->log.score , env->log.hiScore); 
    env->floppy.radius = 10; 
    env->tubes[0].active= false; 
    env->tubesPos[0] = (Vector2){0, 0}; 
    env->tubesSpeedx= 0; 
    env->gamestate.superfx = false; 
    env->log.goal = GOAL; 
}

void make_client(GameEnv *env)
{  
    //client 
    env->client = (Client*)(calloc(1,sizeof(Client))); 

    env->client->screenWidth = screenWidth; 
    env->client->screenHeight = screenHeight; 
    env->client->title = "Classic: Floppy game"; 

    InitWindow(env->client->screenWidth, env->client->screenHeight, env->client->title);
}
// Initialize game variables
void InitGame(GameEnv *env)
{
   
   // printf("[C][InitGame] Fn \r\n");

    env->floppy.radius = FLOPPY_RADIUS;
    env->floppy.position = (Vector2){80, screenHeight/2 - env->floppy.radius};
    env->tubesSpeedx = 2;

    for (int i = 0; i < MAX_TUBES; i++)
    {
        env->tubesPos[i].x = 400 + 280*i;
        env->tubesPos[i].y = -GetRandomValue(0, 120);
    }

    for (int i = 0; i < MAX_TUBES*2; i += 2)
    {
        env->tubes[i].rec.x =  env->tubesPos[i/2].x;
        env->tubes[i].rec.y = env->tubesPos[i/2].y;
        env->tubes[i].rec.width = TUBES_WIDTH;
        env->tubes[i].rec.height = 255;

        env->tubes[i+1].rec.x =  env->tubesPos[i/2].x;
        env->tubes[i+1].rec.y = 600 +  env->tubesPos[i/2].y - 255;
        env->tubes[i+1].rec.width = TUBES_WIDTH;
        env->tubes[i+1].rec.height = 255;

        env->tubes[i/2].active = true; // /2
    }
    env->log.score = 0; 
    env->log.hiScore = 0; 
    env->gamestate.gameOver = false; 
    env->gamestate.superfx = false; 
    env->gamestate.pause = false; 

    compute_observations(env, false); 
}

void UpdateGame(GameEnv *env)
{
    printf("[C][update] start update function \r\n"); 
}


// Draw game (one frame)
void DrawGame(GameEnv *env)
{
   
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!env->gamestate.gameOver)
        {
            DrawCircle(env->floppy.position.x, env->floppy.position.y, env->floppy.radius, DARKGRAY);

            // Draw tubes
            for (int i = 0; i < MAX_TUBES; i++)
            {
                DrawRectangle(env->tubes[i*2].rec.x, env->tubes[i*2].rec.y, env->tubes[i*2].rec.width, env->tubes[i*2].rec.height, GRAY);
                DrawRectangle(env->tubes[i*2 + 1].rec.x, env->tubes[i*2 + 1].rec.y, env->tubes[i*2 + 1].rec.width, env->tubes[i*2 + 1].rec.height, GRAY);
            }

            // Draw flashing fx (one frame only)
            if (env->gamestate.superfx)
            {
                DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
                env->gamestate.superfx = false;
            }

            DrawText(TextFormat("%.1f", env->log.score), 20, 20, 40, GRAY);
            DrawText(TextFormat("HI-SCORE: %.1f", env->log.hiScore), 20, 70, 20, LIGHTGRAY);

            if (env->gamestate.pause) {
                DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, GRAY);

            }

        }
        else {
            printf("[C][Game Over]-1 PRESS ENTER button to play again \r\n"); 
            DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
            // env->rewards[0] += -3.0;
            // _game_init(env,NUM_ENVS);
            // InitGame(env); 
           
            
        }

    EndDrawing();
   
}

void add_log(GameEnv* env) {
    // 1. Accumulate per-step metrics
    env->log.episode_return += env->rewards[0]; // Sum rewards for the episode
    env->log.episode_length += 1.0f;            // Count steps in the episode

  //  2. Update overall score/high-score (for your game's logic)
    env->log.score = env->log.hiScore;
    env->log.n += 1.0f;   
    if (env->terminals[0] == 1) {
        env->log.perf = env->log.hiScore / env->log.goal;
    }  
}
void c_render(GameEnv *env)
{
    if(env->client == NULL)
    {
        make_client(env); 
    }
    DrawGame(env);
 
}
// Update and Draw (one frame)
void UpdateDrawFrame(GameEnv *env)
{
}

void c_reset(GameEnv* env) {
    // printf("[C][c_reset] Fn ep_length > %f ep_return > %f terminals > %d reward > %f action > %d \r\n",
    //     env->log.episode_length, 
    //     env->log.episode_return, 
    //     env->terminals[0], 
    //     env->rewards[0], 
    //     env->actions[0]
    //     );

    env->gamestate.gameOver = true;
    InitGame(env);
    env->log.episode_return = 0.0f;
    env->log.episode_length = 0.0f;
    env->log.tick = 0;

    env->gamestate.gameOver = false;
    env->terminals[0] = 0; 
    env->rewards[0] = 0.0; 
    env->log.tick = 0;
    env->log.score = 0.0; 
    env->log.hiScore = 0.0; 
  
    compute_observations(env, false);
}


void c_step(GameEnv* env) {
    
    env->log.tick += 1;
   // printf("[C][step] action is not zero %d \r\n", env->log.tick ); 
    int action = env->actions[0];
    env->terminals[0] = 0;
    //env->rewards[0] = 0.0; 
    //printf("[C][step] action is not zero %d \r\n", action ); 
    if (IsKeyPressed('P')) env->gamestate.pause = !env->gamestate.pause;
    
    // Update game state based on action
    if (!env->gamestate.gameOver && !env->gamestate.pause) {
        
         // Move tubes
        for (int i = 0; i < MAX_TUBES; i++) {
            env->tubesPos[i].x -= env->tubesSpeedx;
        }
        // Update tube rectangles
        for (int i = 0; i < MAX_TUBES*2; i += 2) {
            env->tubes[i].rec.x = env->tubesPos[i/2].x;
            env->tubes[i+1].rec.x = env->tubesPos[i/2].x;
        }
        //manual control 
        if (IsKeyDown(KEY_SPACE) || action == 1) {
            env->floppy.position.y -= 3; 
            env->log.number_of_ups += 1;
            // env->actions[0] = 1; 
        }else {
            env->floppy.position.y += 1;
        }
        // Check collisions
        for (int i = 0; i < MAX_TUBES*2; i++) {
            if (CheckCollisionCircleRec(env->floppy.position, env->floppy.radius, env->tubes[i].rec)) {
                env->gamestate.gameOver = true;
                env->rewards[0] -= 0.5;
                env->terminals[0] = 1;
            }else if (env->tubesPos[i/2].x < env->floppy.position.x && env->tubes[i/2].active && !env->gamestate.gameOver) { // /2
                env->log.hiScore += 100.0;
                env->rewards[0] += 1.0; 
                env->tubes[i/2].active = false; // / 2
                env->gamestate.superfx = true;
            }
        }
        // Check bounds
        if (env->floppy.position.y < 0 || env->floppy.position.y > screenHeight) {
            env->gamestate.gameOver = true;
            env->rewards[0] -= 0.5;
            env->terminals[0] = 1;
            //return;
        }

        env->log.episode_return += env->rewards[0]; 
        env->log.episode_length += 1.0f; 

        // Update observation
        //env->observations[0] = env->floppy.position.y / screenHeight;  // Normalize
        add_log(env); 
        compute_observations(env, false); 
    }
    else
    {

        //printf("[C] Game over \r\n");
        env->terminals[0] = 1; 

        add_log(env);
        compute_observations(env, true);
        //action 2 allows the game to reset
        if (IsKeyPressed(KEY_ENTER) || action == 2)
        {
            printf("[C] Fn key pressed Enter Game over \r\n"); 
            InitGame(env);
            env->gamestate.gameOver = false;
        }

        if (env->gamestate.gameOver) {
            env->terminals[0] = 1;
            env->rewards[0] = -1.0;

            c_reset(env); 

        }
    }
}


void c_close(GameEnv* env) {
    env->gamestate.gameOver = true; 
    //freemem(env); 
    if (IsWindowReady()) {
        CloseWindow(); 
        freemem(env); 
    }
}

