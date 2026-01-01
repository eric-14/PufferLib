#include "floppy.h"
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "classic game: floppy");
    GameEnv env; 
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
        c_render(&env); 
        //----------------------------------------------------------------------------------
    }
    
#endif
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadGame();         // Unload loaded data (textures, sounds, models...)

    CloseWindow();        // Close window and OpenGL context

    c_close(&env); 

    //--------------------------------------------------------------------------------------

    return 0;
}
//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------



