#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>



#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------

#define FRAMERATE 40 
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
typedef struct {
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
}Log; 


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
    unsigned char* observations; 
    //float *observations; 
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
    // free(env->observations); 
    // free(env->rewards);
    // free(env->terminals);  
    // free(env->actions); 
}
// Update game (one frame)


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
    env->log = (typeof(env->log)){0};
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

    env->observations = (unsigned char*)(calloc(NUM_OBS, sizeof(unsigned char))); 
    env->rewards = (float*)(calloc(1,sizeof(float))); 

    env->actions = (int*)(calloc(1,sizeof(int))); 
    env->terminals = (unsigned char*)(calloc(1,sizeof(unsigned char))); 
}
// Initialize game variables
void InitGame(GameEnv *env)
{
    _game_init(env,NUM_ENVS); // Default the game 
    printf("[C][InitGame] Fn \r\n");

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

        env->tubes[i/2].active = true;
    }
    env->log.score = 0; 
    env->gamestate.gameOver = false; 
    env->gamestate.superfx = false; 
    env->gamestate.pause = false; 

    compute_observations(env, false); 

    // score = 0;

    // gameOver = false;
    // superfx = false;
    // pause = false;
}

void UpdateGame(GameEnv *env)
{
    printf("[C][update] start update function \r\n"); 
    // if (!env->gamestate.gameOver)
    // {
    //     if (IsKeyPressed('P')) env->gamestate.pause = !env->gamestate.pause;

    //     if (!env->gamestate.pause)
    //     {
    //         for (int i = 0; i < MAX_TUBES; i++) env->tubesPos[i].x -= env->tubesSpeedx;

    //         for (int i = 0; i < MAX_TUBES*2; i += 2)
    //         {
    //             env->tubes[i].rec.x = env->tubesPos[i/2].x;
    //             env->tubes[i+1].rec.x = env->tubesPos[i/2].x;
    //         }

    //         if (IsKeyDown(KEY_SPACE) && !env->gamestate.gameOver) {
    //             env->floppy.position.y -= 3; 
    //             env->actions[0] = 1; 
            
    //         }
    //         else {
    //             env->floppy.position.y += 1; 
    //             env->actions[0] = 0; //no action taken 
            
    //         };

    //         // Check Collisions
    //         for (int i = 0; i < MAX_TUBES*2; i++)
    //         {
    //             if (CheckCollisionCircleRec(env->floppy.position, env->floppy.radius, env->tubes[i].rec))
    //             {
    //                 env->gamestate.gameOver = true;
    //                 env->gamestate.pause = false;
    //                 env->actions[0] = 0; 
    //                 env->log.rewards -= 3; //heavily penalized the model for colliding with objects
    //             }
    //             else if ((env->tubesPos[i/2].x < env->floppy.position.x) && env->tubes[i/2].active && !env->gamestate.gameOver)
    //             {
    //                 env->log.score += 100.0;
    //                 env->log.rewards += 1.0; //reward the agent when it increments the value 
    //                 env->tubes[i/2].active = false;
    //                 printf("[SCORE] updating score value %f \r\n", env->log.score);

    //                 env->gamestate.superfx = true;

    //                 if (env->log.score > env->log.hiScore) env->log.hiScore = env->log.score;
    //                 //c_step(env); 
    //             }
    //         }
    //     }

    // }
    // else
    // {
    //     printf("[C] Game over \r\n"); 
    //     if (IsKeyPressed(KEY_ENTER))
    //     {
    //         printf("[C] Fn key pressed Enter Game over \r\n"); 
    //         InitGame(env);
    //         env->gamestate.gameOver = false;
    //     }
    // }
    // printf("[C][update] update end of function \r\n"); 
}


// Draw game (one frame)
void DrawGame(GameEnv *env)
{
    printf("[C][DrawGame] start of fn \r\n"); 
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
                printf("[C] Game Paused \r\n"); 

            }

        }
        else {
            printf("[C][Game Over]-1 PRESS ENTER button to play again \r\n"); 
            DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
            
        }

    EndDrawing();
    printf("[C][DrawGame] end of fn \r\n"); 
}

// void add_log(GameEnv* env) {
//     env->log.perf += env->log.score;  
//     env->log.score += env->log.score;
//     env->log.episode_length += env->log.tick;
//     env->log.episode_return += env->log.score;
//     env->log.n++;
// }

void add_log(GameEnv* env) {
    // env->log.perf += env->log.score / env->log.goal;  // Normalized performance 0-1
    // env->log.score += env->rewards[0];
    // env->log.episode_length += 1;
    // env->log.episode_return += env->rewards[0];
    // env->log.n++;
   // printf("[C][add_log] Fn \r\n");
    env->log.perf += env->log.score / env->log.goal;
    env->log.score = env->log.hiScore;  // Keep current score
    env->log.episode_length = env->log.tick;
    env->log.episode_return += (float)env->rewards[0];  // Sum of rewards
    env->log.n += 1.0f;  // Episode counter
}

void c_render(GameEnv *env)
{
    printf("[C][c_render] fun \r\n"); 
    //UpdateGame(env);

    DrawGame(env);
    printf("[C][c_render] end of render fun \r\n"); 
}
// Update and Draw (one frame)
void UpdateDrawFrame(GameEnv *env)
{
    //UpdateGame(env);
    //DrawGame(env);
}

// void c_reset (GameEnv *env)
// {
//     //should restart the game 
//     printf("[C][c_reset] Fn to reset the game Env \r\n");
//     InitGame(env);
//     env->gamestate.gameOver = false;
// }
void c_reset(GameEnv* env) {
    printf("[C][c_reset] Fn \r\n");
    env->gamestate.gameOver = true;
    InitGame(env);
    // env->gamestate.gameOver = false;
    // env->log.tick = 0;
    // env->log.score = 0;
    // env->log.episode_return = 0;
    // env->log.episode_length = 0;
    //env->observations[0] = env->floppy.position.y / screenHeight;
}


// void c_step(GameEnv *env)
// {
//     // printf("[C][Floppy] Step function score is -> %d\r\n", env->log.score); 
//     env->log.n += 1; 
//     env->log.tick += 1; 

//     if(env->actions[0] == 1)
//     {
//         //space bar pressed 

//     }
   
// }

void c_step(GameEnv* env) {
    
    env->log.tick += 1;
   // printf("[C][step] action is not zero %d \r\n", env->log.tick ); 
    int action = env->actions[0];

    env->rewards[0] = 0;
    env->terminals[0] = 0;

    if(action > 0)
    {
        printf("[C][step] action is not zero %d \r\n", action ); 
    }
    
    // Reset rewards/terminals
    env->rewards[0] = 0;
    env->terminals[0] = 0;
    if (IsKeyPressed('P')) env->gamestate.pause = !env->gamestate.pause;
    
    // Update game state based on action
    if (!env->gamestate.gameOver && !env->gamestate.pause) {
        //manual control 
        if (IsKeyDown(KEY_SPACE) || action == 1) {
            env->floppy.position.y -= 3; 
            env->log.number_of_ups += 1;
            // env->actions[0] = 1; 
        
        }else {
            env->floppy.position.y += 1;
        }
    
        // Move tubes
        for (int i = 0; i < MAX_TUBES; i++) {
            env->tubesPos[i].x -= env->tubesSpeedx;
        }
        
        // Update tube rectangles
        for (int i = 0; i < MAX_TUBES*2; i += 2) {
            env->tubes[i].rec.x = env->tubesPos[i/2].x;
            env->tubes[i+1].rec.x = env->tubesPos[i/2].x;
        }
        
        // Check collisions
        for (int i = 0; i < MAX_TUBES*2; i++) {
            if (CheckCollisionCircleRec(env->floppy.position, env->floppy.radius, env->tubes[i].rec)) {
                env->gamestate.gameOver = true;
                env->rewards[0] = -3.0;
                env->terminals[0] = 1;
                compute_observations(env, true); 
                add_log(env);
                return;
            }
        }
        
        // Check if passed tubes
        for (int i = 0; i < MAX_TUBES; i++) {
            if (env->tubesPos[i].x < env->floppy.position.x && env->tubes[i/2].active) {
                env->log.hiScore += 100.0;
                env->rewards[0] = 1.0;
                env->tubes[i/2].active = false;
                env->gamestate.superfx = true;
                env->log.score = env->log.hiScore;
                
                // if (env->log.score > env->log.hiScore) {
                //     env->log.hiScore = env->log.score;
                // }
            }
        }
        
        // Check bounds
        if (env->floppy.position.y < 0 || env->floppy.position.y > screenHeight) {
            env->gamestate.gameOver = true;
            env->rewards[0] = -3.0;
            env->terminals[0] = 1;
            add_log(env);
            compute_observations(env, true); 
            return;
        }
        
        // Update observation
        env->observations[0] = env->floppy.position.y / screenHeight;  // Normalize
        add_log(env); 
        compute_observations(env, false); 
    }
    else
    {

        //printf("[C] Game over \r\n");
        env->terminals[0] = 1; 
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
            add_log(env);  // Log the episode
            return;  // Don't reset here - let Python handle it
        }
    }
}

// void add_log(GameEnv *env)
// {
//     env->log.score +=  
//     env.log.ep_return 
//     env->log.ep_length += 
//     en
// }

void c_close(GameEnv* env) {
    env->gamestate.gameOver = true; 
    //freemem(env); 
    if (IsWindowReady()) {
        CloseWindow(); 
        freemem(env); 
    }
}


void compute_observations(GameEnv *env, bool collided)
{
    //ball position 
    env->observations[0] =  env->floppy.position.x / screenWidth; 
    env->observations[1] =  env->floppy.position.y / screenHeight; 
    // Observations at each iteration of the game 
    if(!collided)
    {
         env->observations[2] = 0; 
    }else 
    {
         env->observations[2] = 1; 
    }
}