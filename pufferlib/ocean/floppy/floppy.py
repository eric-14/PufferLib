'''A minimal template for your own envs.'''

import gymnasium
import numpy as np

import pufferlib

from pufferlib.ocean.floppy import binding

class Floppy(pufferlib.PufferEnv):
    def __init__(self, num_envs=4, render_mode=None, log_interval=20, size=5, buf=None, seed=0, floppy_pos_x=0, floppy_pos_y=0 ):
        self.single_observation_space = gymnasium.spaces.Box(low=0, high=1,
            shape=(8,), dtype=np.float32)
        print("[Py][Floppy]  __init__ fn")
        self.single_action_space = gymnasium.spaces.Discrete(2)
        self.render_mode = render_mode
        self.num_agents = num_envs
        self.log_interval = log_interval

        super().__init__(buf)
        self.c_envs = binding.vec_init(self.observations, self.actions, self.rewards,
            self.terminals, self.truncations, num_envs, seed, size=size, floppy_pos_x = floppy_pos_x, floppy_pos_y = floppy_pos_y)
        # c_envs = []
        # for i in range(num_envs):
        #     c_env = binding.env_init(
        #         self.observations[i:i+1],      # Observation slice for this env
        #         self.actions[i:i+1],           # Action slice
        #         self.rewards[i:i+1],           # Reward slice
        #         self.terminals[i:i+1],         # Terminal slice
        #         self.truncations[i:i+1],       # Truncation slice
        #         seed + i,                      # Unique seed per env
        #                             # Pass any additional args
        #     )
        #     c_envs.append(c_env)

        # self.c_envs = binding.vectorize(*c_envs)
        self.tick = 0
        self.size = size
 
    def reset(self, seed=0):
       
        binding.vec_reset(self.c_envs, seed)
        self.tick = 0
        print("[Py][Floppy] reset fn")
        return self.observations, []

    def step(self, actions):
        # print("[Py][Floppy]  __step__ fn")
        self.tick += 1 
        self.actions[:] = actions
        # print(f"[Py][Floppy] self.actions {self.actions[:]}")
        binding.vec_step(self.c_envs)
        info = [binding.vec_log(self.c_envs)]
        return (self.observations, self.rewards,
            self.terminals, self.truncations, info)

    def render(self):
        print("[Py][Floppy] render fn")
        binding.vec_render(self.c_envs, 0)

    def close(self):
        print("[Py][Floppy] close fn")
        binding.vec_close(self.c_envs)

if __name__ == '__main__':
    N = 4096
    env = Floppy(num_envs=N)
    env.reset()
    steps = 0
    print("[Py][Floppy] main fn")

    CACHE = 1024
    actions = np.random.randint(0, 5, (CACHE, N))

    import time
    start = time.time()
    while time.time() - start < 40:
        env.step(actions[steps % CACHE])
        steps += 1

    print('Floppy SPS:', int(env.num_agents*steps / (time.time() - start)))
