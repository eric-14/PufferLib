'''A minimal template for your own envs.'''

import gymnasium
import numpy as np

import pufferlib

from pufferlib.ocean.floppy import binding

class Floppy(pufferlib.PufferEnv):
    def __init__(self, num_envs=1, render_mode=None, log_interval=128, size=5, buf=None, seed=0):
        self.single_observation_space = gymnasium.spaces.Box(low=0, high=1,
            shape=(1,), dtype=np.uint8)
        print("[Py][Floppy]  __init__ fn")
        self.single_action_space = gymnasium.spaces.Discrete(2)
        self.render_mode = render_mode
        self.num_agents = num_envs

        super().__init__(buf)
        self.c_envs = binding.vec_init(self.observations, self.actions, self.rewards,
            self.terminals, self.truncations, num_envs, seed, size=size)
        self.size = size
 
    def reset(self, seed=0):
        binding.vec_reset(self.c_envs, seed)
        print("[Py][Floppy] reset fn")
        return self.observations, []

    def step(self, actions):
        print("[Py][Floppy]  __step__ fn")
        self.actions[:] = actions
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
