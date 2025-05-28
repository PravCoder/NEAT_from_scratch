#include <cmath>
#include <random>
#include <iostream>
#include <vector>
#ifndef CartPoleEnvironment_H
#define CartPoleEnvironment_H
#include "Population.h"  /
#include "Genome.h" 


class CartPoleEnvironment {
    private:
        static constexpr double GRAVITY = 9.8;             // m/s^2, pulls pole down
        static constexpr double MASS_CART = 1.0;           // kg, heavier cart -> harder to accelerate
        static constexpr double MASS_POLE = 0.1;           // kg, how heavy the pole is lighter the pole eaiser to controll but less momentum
        static constexpr double TOTAL_MASS = MASS_CART + MASS POLE;   // kg, combined mass of both elements
        static constexpr double LENGTH = 0.5;                         // m, half of poles length
        static constexpr double POLE_MASS_LENGTH = MASS_POLE * LENGTH;  // kg * m, mass*distance = moment of inertia component
        static constexpr double FORCE_MAG = 10.0;          // N, how hard each push of the cart is. 
        static constexpr double TAU = 0.02;                // s, time between each physics update, time step, 0.02s = 50z, 50 times per second.  

        static constexpr double X_BOUNDS = 2.4;      // m, how far cart can travel from center before failure, x-bounds
        static constexpr double THETA_THRESHOLD_RADIANS = 12.0 * 2.0 * M_PI / 360;  // radians, maximum pole tilt allowed before failure,  12° × (2π/360°) = 0.2094 radians, if pole tilts more than 12 degrees its fallen
    
    public:
        // network inputs
        // these variables represent the current values in the simulation environment of the current genome that is being evaluated
        double cart_x;       // m, how far cart is form center, carts x-position
        double x_vel;        // m/s, carts velocity how fast the cart is moving horizontally, pos->right, neg->left
        double pole_angle;       // radians, how much pole has tilted from y-axis
        double pole_angular_vel; // radians, poles angular velocity, how fast pole is rotating. 
        
        int steps_survived;  // number of time steps the current cart-genome current episode has kept the pole balanced
        bool done;           // whather the episode has ended

        CartPoleEnvironment() {
            reset();
        }


        /*
        For each genomes episode we call this func, it randomly sets the starting conditions of the environment
        This is so the simulation doesn't start in the perfect balance case, so it is tested on all cases
        */
        void reset() {
            // init small random values
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution<double> dist(-0.05, 0.05);

            cart_x = dist(gen);            // ex.  Cart slightly off-center (3cm)
            x_vel = dist(gen);             // ex.  Cart moving slowly left  
            pole_angle = dist(gen);        // ex.   Pole tilted 2.3° right
            pole_angular_vel = dist(gen);  // ex.  Pole rotating slightly left, is how the simulation starts adn genome has to control the cart from here given these inputs in the environment

            steps_survived = 0;
            done = false; 
        }

        void step(int action) {
            if (done) {
                return 0.0;
            }

            // action: 0 = push left, 1 = push right
            double force = (action == 1) ? FORCE_MAG : -FORCE_MAG;


            // cart-pole equations
            double costheta = cos(pole_angle);
            double sintheta = sin(pole_angle);
            double temp = (force + POLE_MASS_LENGTH * pole_angular_vel * pole_angular_vel * sintheta) / TOTAL_MASS;
            double thetaacc = (GRAVITY * sintheta - costheta * temp) / 
                            (LENGTH * (4.0/3.0 - MASS_POLE * costheta * costheta / TOTAL_MASS));
            double xacc = temp - POLE_MASS_LENGTH * thetaacc * costheta / TOTAL_MASS;


            // update environment state using euler integrations
            cart_x += TAU * x_vel;
            x_vel += TAU * xacc;
            pole_angle += TAU * pole_angular_vel;
            pole_angular_vel += TAU * thetaacc;

            steps_survied++;  // we just completeed updated a step so increment step survived


            // This checks if the episode is terminated, checks the failure conditions if any is true this episode has ended the genome as failed the simulation
            // Cart falls of left side
            // Cart falls off right side
            // Pole falls too far left
            // Pole falls too far right
            // time limit exceeded
            done = (cart_x < -X_BOUNDS || cart_x > X_BOUNDS ||
                pole_angle < -THETA_THRESHOLD_RADIANS || pole_angle > THETA_THRESHOLD_RADIANS ||
                steps_survived >= 500);  
        }

        vector<double> get_state() {  // get the current state of the environment
            return {cart_x, cart_vel, pole_angle, pole_angular_vel};
        }

        double get_fitness() {
            return static_cast<double>(steps_survived); // number of step of current episode of current genome
        }

        void print_state() const {
            cout << "Cart: x=" << x << " v=" << x_dot 
                    << " | Pole: θ=" << theta << " ω=" << theta_dot 
                    << " | Steps=" << steps_survived << endl;
        }

        bool is_done {
            return done;  // returns if the current genomes current episode has finished
        }

        vector<double> noramlize_state(vector<double>& state) {
            return {
                state[0] / X_BOUNDS,       // cart position / max-position
                state[1] / 3.0,       // cart vel rough normalization
                state[2] / (12.0 * M_PI / 180.0),   // pole angle / max-angle
                state[3] / 2.0       // angular vel  rough normalization
            }
        }

        
        // Given a genome and the state of the environment (input vars) get action of genome-network
        int get_action_from_genome(Genome& genome, const vector<double>& state) {
            vector<double> normalized_state = normalize_state(state);  // normalize state
            vector<double> outputs = genome.forward_propagate_single_example(normalized_state);

            // convert network single output node percentage ot action, >0.5 = right=1, <0.5 = left=0
            return (outputs[0] > 0.5) ? 1 : 0;
        }

        // Given a genome we inject into the simulation environment and see how well it does, num of episodes is how many times we run that genome against the simulation environment
        double evaluate_genome_cart_pole_simulation(Genome& genome, int max_episodes=5) {
            double total_fitness = 0.0;

            // iterate all episodes we want to run on this genome to get its average performance on the environment
            for (int cur_episode; cur_episode<max_episodes; cur_episode++) {
                reset();  // sets the starting conditions randomly (why we need multiple episodes), sets steps-survived=0

                // while is episode simulation is not done
                while (!is_done()) {
                    vector<double> state = get_state();  // get the current state of the environment at the current time step
                    int action = get_action_from_genome(genome, state);  // get prediction-action from genome given state of environment
                    step(action);                                        // given genomes action move the environment into the next step
                }
                total_fitness += get_fitness();  // sum fitness of current episode and average it for robust performance metric
            }

            return total_fitness / max_episodes;
        }


        void run_cartpole_neat_evolution(int population_size, int network_inputs, int network_outputs, double crossover_rate, int num_generations, int tournament_size) {
            Population p1 = Population(population_size, network_inputs, network_outputs, crossover_rate, "fully_connected", num_generations, tournament_size); // rand_connnected, fully_connected
            p1.create_population("fully_connected");


            p1.evolutionary_loop_cartpole();

        }


}
