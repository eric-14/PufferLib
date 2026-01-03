#include "floppy.h"
 
typedef GameEnv Env; 
#include "../env_binding.h"

// typedef GameEnv Log; 

static int my_init(Env* env, PyObject* args, PyObject* kwargs) {
    env->log.size = unpack(kwargs, "size");
    // env->log.ep_length = unpack(kwargs, "episode_length");
    // env->log.ep_return = unpack(kwargs, "episode_return");
    InitGame(env);
    // env->log.n = unpack(kwargs, "n");
    // env->log.score = unpack(kwargs, "score");
    // env->log.hiScore = unpack(kwargs, "hiScore");
    

   // env->num_envs = unpack(kwargs, "num_envs");
    return 0;
}

static int my_log(PyObject* dict, Log* log) {
    assign_to_dict(dict, "score", log->score);
    assign_to_dict(dict, "episode_length", log->episode_length);
    assign_to_dict(dict, "episode_return", log->episode_return);
    assign_to_dict(dict, "tick", log->tick);
    assign_to_dict(dict, "n", log->n);
    assign_to_dict(dict, "perf", log->perf);
    // assign_to_dict(dict, "hiScore", log->hiScore);
    // assign_to_dict(dict, "number_of_ups", log->number_of_ups);
    return 0;
}
