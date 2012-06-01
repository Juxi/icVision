import local_linear_map as llm
from local_linear_map import vec
from numpy import *

def length(x):
    return linalg.norm(x)

class Explorer:
    def __init__(self, n_x, n_q, d, sigma, small_sigma, home_q, home_chance, low_limit_x, high_limit_x, orientation_factor):
        self.map = llm.LocalLinearMap(n_x, n_q, d, sigma, small_sigma)
        self.home_q = vec(home_q)
        self.low_limit_x = low_limit_x
        self.high_limit_x = high_limit_x
        self.home_chance = home_chance
        self.orientation_factor = orientation_factor

    def normalize(self, vec):
        return vec - self.home_x
    
    def denormalize(self, vec):
        return vec + self.home_x

    #grab uniform from operation space, unless home
    def choose_new_point(self):
        home = False
        self.start_pos = self.goal_pos
        self.goal_pos = vec(random.uniform(self.low_limit_x, self.high_limit_x))
        
        if random.random() < self.home_chance:
            self.goal_pos = self.home_x.copy()
            home = True
        print "goal pos", self.goal_pos
        return self.goal_pos, home
    
    def step_func(self, fraction):
        step_pos = add(self.start_pos * (1. - fraction), self.goal_pos * fraction)
        return step_pos

    def set_sigma(self, sigma, small_sigma):
        self.map.set_sigma(sigma, small_sigma)
    
    #def get_deviation(self, x):
    #    dev = self.deviator.activate(self.normalize(x) * 3.)
    #    self.deviator.move()
    #    return dev
    
    def calculate_weight(self, x_prev_real, x_now_real, x_prev_goal, x_now_goal, q_prev, q_now, environment):
        x_real_diff = vec(x_now_real) - vec(x_prev_real)
        x_goal_diff = vec(x_now_goal) - vec(x_prev_goal)
        q_diff = (vec(q_now) - vec(q_prev)) / 10.

        print x_real_diff.transpose(), x_goal_diff.transpose()
        w1_dot = (x_real_diff.transpose() * x_goal_diff) / length(x_real_diff) / length(x_goal_diff)
        w1 = (1.0 + w1_dot) / 2.0
        w1 = w1 #square ALL the things!
        #w1 = w1 ** 2 #square ALL the things!

        #w1 = max(0.0, dot(x_real_diff.transpose(), x_goal_diff) / length(x_real_diff) / length(x_goal_diff))
        w2 =  10. * length(x_real_diff) / length(q_diff)
        w3 = exp(-((15. * length(vec(x_now_goal[0:3]) - vec(x_now_real[0:3]))) ** 2))
        
        #angle_goal = vec([1., 0., 0.])
        #angle_now = environment.getHandOrientation()
        #in_product = float(dot(angle_now, angle_goal) / length(angle_now) / length(angle_goal))

        #w4 = math.exp(- (1.0 - in_product) ** 2 / (.1 ** 2))
        w4 = max(0.0, environment.get_hand_orientation_measure()) ** 2
        
        w = w1 * w2 * w3 * w4
        if isnan(w):
            return 0.
        if isinf(w):
            return 1.0
        print "w1_dot: %s, w1: %s, w2 %s, w3 %s, w4 %s" % (w1_dot, w1, w2, w3, w4)
        return w
    
    def init_map(self, x, q):
        self.home_x = vec(x)
        self.map.activate(vec(x), vec(q))
        self.current_pos = self.home_x.copy()
        self.start_pos = self.home_x.copy()
        self.goal_pos = self.home_x.copy()
