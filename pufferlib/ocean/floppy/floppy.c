#include "floppy.h"

int main(void)
{
    
    InitWindow(screenWidth, screenHeight, "classic game: floppy");
    GameEnv env; 
    env.actions = (int*)calloc(1, sizeof(int)); 
    env.rewards = (float*)calloc(1, sizeof(float));
    env.terminals = (unsigned char*)calloc(1, sizeof(unsigned char));
    env.observations = (unsigned char*)calloc(5, sizeof(unsigned char));

    InitGame(&env);


    SetTargetFPS(FRAMERATE);
   
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        
        //UpdateDrawFrame();
        c_step(&env); 
        c_render(&env); 
    }
    
   
    UnloadGame();         

    CloseWindow();        

    c_close(&env); 


    return 0;
}

