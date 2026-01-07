#include "floppy.h"
 
typedef GameEnv Env; 
#include "../env_binding.h"

#define INIT my_init
#define RESET c_reset 
#define STEP c_step 
#define CLOSE c_close 
#define RENDER c_render 
#define LOG my_log 
#define LOG_TYPE LOG 

static int my_init(Env* env, PyObject* args, PyObject* kwargs) {
    env->log.size = unpack(kwargs, "size");
    env->floppy.position.x = unpack(kwargs, "floppy_pos_x");
    env->floppy.position.y = unpack(kwargs, "floppy_pos_y");
   // env->log.score = unpack(kwargs, "score");

    // env->log.ep_length = unpack(kwargs, "episode_length");
    // env->log.ep_return = unpack(kwargs, "episode_return");
   _game_init(env, 1);
    // env->log.n = unpack(kwargs, "n");
   
    // env->log.hiScore = unpack(kwargs, "hiScore");
    

   // env->num_envs = unpack(kwargs, "num_envs");
    return 0;
}

static int my_log(PyObject* dict, Log* log) {
    assign_to_dict(dict, "perf", log->perf);
    assign_to_dict(dict, "score", log->score);
    assign_to_dict(dict, "episode_return", log->episode_return);
    assign_to_dict(dict, "episode_length", log->episode_length);
    assign_to_dict(dict, "n", log->n);
    assign_to_dict(dict, "tick", log->tick);
   // printf("Binding %f \r\n", log->score); 
    // assign_to_dict(dict, "hiScore", log->hiScore);
    // assign_to_dict(dict, "number_of_ups", log->number_of_ups);
    return 0;
}
