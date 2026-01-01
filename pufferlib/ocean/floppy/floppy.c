#include "floppy.h"
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "classic game: floppy");
    GameEnv env =  { .goal = 0 }; 
    InitGame(&env);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(FRAMERATE);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update and Draw
        //----------------------------------------------------------------------------------
        //UpdateDrawFrame();
        c_step(&env); 
        //----------------------------------------------------------------------------------
    }
#endif
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadGame();         // Unload loaded data (textures, sounds, models...)

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

// Initialize game variables
void InitGame(GameEnv *env)
{
    _game_init(env,NUM_ENVS); // Default the game 

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

    // score = 0;

    // gameOver = false;
    // superfx = false;
    // pause = false;
}

// Update game (one frame)
void UpdateGame(GameEnv *env)
{
    if (!env->gamestate.gameOver)
    {
        if (IsKeyPressed('P')) env->gamestate.pause = !env->gamestate.pause;

        if (!env->gamestate.pause)
        {
            for (int i = 0; i < MAX_TUBES; i++) env->tubesPos[i].x -= env->tubesSpeedx;

            for (int i = 0; i < MAX_TUBES*2; i += 2)
            {
                env->tubes[i].rec.x = env->tubesPos[i/2].x;
                env->tubes[i+1].rec.x = env->tubesPos[i/2].x;
            }

            if (IsKeyDown(KEY_SPACE) && !env->gamestate.gameOver) env->floppy.position.y -= 3;
            else env->floppy.position.y += 1;

            // Check Collisions
            for (int i = 0; i < MAX_TUBES*2; i++)
            {
                if (CheckCollisionCircleRec(env->floppy.position, env->floppy.radius, env->tubes[i].rec))
                {
                    env->gamestate.gameOver = true;
                    env->gamestate.pause = false;
                }
                else if ((env->tubesPos[i/2].x < env->floppy.position.x) && env->tubes[i/2].active && !env->gamestate.gameOver)
                {
                    env->log.score += 100;
                    env->tubes[i/2].active = false;
                    printf("[SCORE] updating score value %d \r\n", env->log.score);

                    env->gamestate.superfx = true;

                    if (env->log.score > env->log.hiScore) env->log.hiScore = env->log.score;
                    //c_step(env); 
                }
            }
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame(env);
            env->gamestate.gameOver = false;
        }
    }
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

            DrawText(TextFormat("%04i", env->log.score), 20, 20, 40, GRAY);
            DrawText(TextFormat("HI-SCORE: %04i", env->log.hiScore), 20, 70, 20, LIGHTGRAY);

            if (env->gamestate.pause) DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, GRAY);
        }
        else DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);

    EndDrawing();
}

// Unload game variables
void UnloadGame(void)
{
    // TODO: Unload all dynamic loaded data (textures, sounds, models...)
}

// Update and Draw (one frame)
void UpdateDrawFrame(GameEnv *env)
{
    UpdateGame(env);
    DrawGame(env);
}

void c_step(GameEnv *env)
{
    env->log.n += 1; 
    env->log.tick += 1; 
    UpdateGame(env);
    DrawGame(env);
}


void _game_init(GameEnv *env, int num_envs)
{
    //initialize game state 
    env->gamestate.gameOver = false; 
    env->gamestate.pause = false; 
    env->log = (typeof(env->log)){0};
    env->log.hiScore = 0; 
    printf("Starting value for score is -> %d and hiScore is -> %d\r\n", env->log.score , env->log.hiScore); 



    env->floppy.radius = 10; 
    env->tubes[0].active= false; 
    env->tubesPos[0] = (Vector2){0, 0}; 
    env->tubesSpeedx= 0; 
    env->gamestate.superfx = false; 
    env->goal = 0;     

    env->observations = (float*)(calloc(1,sizeof(float))); 
}