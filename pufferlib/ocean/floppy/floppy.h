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



//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------


typedef struct {
    float perf; 
    float ep_length; 
    float ep_return; 
    int tick; 
    int size; 
    int num_envs; 
    float n; 
    int hiScore; 
    int score;           // score achieved in the game 
    int number_of_ups;     // number of time floppy is moved up 
                            // with this information it is a measure of understanding of game physics 
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
    float *observations; 
    int *actions; 
    float *rewards; 
    unsigned char *terminals; 
    int tick; 
    float ep_return; 
    GameState gamestate; 
    Floppy floppy; 
    Tubes tubes[MAX_TUBES*2]; 
    Vector2 tubesPos[MAX_TUBES]; 
    int tubesSpeedx; 
   
    
    int goal; 

} GameEnv; 

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 450;



void freemem(GameEnv *env)
{
    free(env->log); 
    free(env->observations); 
}

void c_reset (GameEnv *env)
{
    
    env->floppy = (struct Floppy){.position = 0}; 
    for(int i =0; i < MAX_TUBES * 2; i++)
    {
        env->tubes[i] = (struct Tubes){
                                .rec ={ 0, 0},
                                .active = false
                                }; 
    }
    for(int i =0; i < MAX_TUBES; i++)
    {
        env->tubesPos[i] = (Vector2){0, 0}; 
    }
    
    env->tubesSpeedx = 0; 
    env->gamestate.superfx = false; 
    env->log = {0};

}

// static bool gameOver = false;
// static bool pause = false;
// static int score = 0;
// static int hiScore = 0;

// static Floppy floppy = { 0 };
// static Tubes tubes[MAX_TUBES*2] = { 0 };
// static Vector2 tubesPos[MAX_TUBES] = { 0 };
// static int tubesSpeedX = 0;
// static bool superfx = false;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(GameEnv *env);         // Initialize game
static void UpdateGame(GameEnv *env);       // Update game (one frame)
static void DrawGame(GameEnv *env);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(GameEnv *env);  // Update and Draw (one frame)
void c_step(GameEnv *env); 
void _game_init(GameEnv *env, int num_envs);



