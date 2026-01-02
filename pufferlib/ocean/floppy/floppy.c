#include "floppy.h"

int main(void)
{
    
    InitWindow(screenWidth, screenHeight, "classic game: floppy");
    GameEnv env; 
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

