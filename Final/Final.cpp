//Final.cpp
//CISP 400
//12/10/24

#include <iostream>
#include <chrono>

/*
    Overview of design decisions:
    1. I elected not to use my Logger class in this project, as it was not required
        by the assignment specifically and provides no value to me for debugging or
        optimizing without a complementary profiler. I *could* read each nanosecond 
        readout of function call times, but that feels like a highly inefficient 
        use of my time.
    2. Additionally, since I've already thoroughly unit tested my STL implementation
        and the classes used for this project are relatively simple, I don't feel a
        need to include any form of unit testing. I do respect the need for unit 
        testing in general, but here it feels unnecessary.
    3. A robot has a genome made of 16 genes each containing 5 codons. This is
        represented through a nesting of several classes. I also provide enumerations
        for the possible states a sensor could read out and the actions a robot could take
        to avoid future developers (of which there will be none) having to read 
        an unnecessary number of single-digit integers and make sense of them. It is
        "self documenting" this way.
*/


/*
    A singleton random number generator
*/
class RandNo {
public:
    // Deleting the ability to copy instances or set instances equal to another
    RandNo(const RandNo&) = delete;
    RandNo& operator=(const RandNo&) = delete;
    // Static method to access the singleton instance
    // Once RandNo is made, its static nature keeps it present
    // in only one place.
    static RandNo& get_instance() {
        static RandNo instance;
        return instance;
    }
    // Method to generate a random integer between min and max
    int random_int(int min, int max) const {
        return min + rand() % (max - min + 1);
    }
private:
    // Private constructor to prevent instantiation
    RandNo() {
        std::srand(static_cast<unsigned int>(std::time(0))); // Seed the random number generator
    }
};

/*
    Enums used for readability within genomes, genes, and codons. 
    Without these, you would see a lot of int types that are seemingly "magic numbers,"
    and you would have to refer to the documentation a lot.
*/
enum class SensorState{
    NoObject,
    Wall,
    Battery,
    Ignore
};
enum class Action {
    North,
    South,
    East,
    West,
    Random
};
class Gene {
/*
    Making internal data members public because the gene class is essentially
    an abstraction of 5 ordered integers. There isn't any particularly important
    data to hide or obfuscate, and it will all be accessible from any other class
    through the Genome class regardless.
*/
public:
    SensorState sensorStatePattern[4];
    Action actionToTake;

    Gene(){}
    // Define a specific gene
    Gene(SensorState sense1, SensorState sense2, SensorState sense3, SensorState sense4, Action action){
        sensorStatePattern[0] = sense1;
        sensorStatePattern[1] = sense2;
        sensorStatePattern[2] = sense3;
        sensorStatePattern[3] = sense4;
        actionToTake= action;
    }

    int operator[](size_t index) const {
        if (index < 4) return static_cast<int>(sensorStatePattern[index]);
        return static_cast<int>(actionToTake);
    }
};

/*
    The Genome class represents a single robot's set of genetic material--
    16 genes, each with 5 codons (represented by integers).
    The first 4 codons correspond to a possible sensor state the robot can have
    The final codon is the action the robot should take if the sensor state
        matches the first four codons of the gene.
*/
class Genome {
    Gene genes[16];
public:
    // Random initializer used for all genomes in the first generation
    Genome() {
        // Lambda to handle the gross static_cast chain to generate random sensor states
        auto get_random_sensor_state = [](){
            return static_cast<SensorState>(RandNo::get_instance().random_int(static_cast<int>(SensorState::NoObject), static_cast<int>(SensorState::Ignore)));
        };

        for (int i = 0; i < 16; i++){ // For each gene
            // generate random codons
            SensorState s1 = get_random_sensor_state();
            SensorState s2 = get_random_sensor_state();
            SensorState s3 = get_random_sensor_state();
            SensorState s4 = get_random_sensor_state();
            Action a1 = static_cast<Action>(RandNo::get_instance().random_int(static_cast<int>(Action::North), static_cast<int>(Action::Random)));
            Gene newGene = Gene(s1, s2, s3, s4, a1);
            genes[i] = newGene;
        }
    }
    // Adding a + operator override for the Genome class. This will select
    // each gene randomly from one of the two operands, and return the new genome.
    Genome operator+(const Genome& other) const {
        Genome result;
        // For each gene in genome from this or other
        for (int i = 0; i < 16; i++){
            // Assign indice to one or the other at random.
            (RandNo::get_instance().random_int(0, 1)) ? result[i] = other[i] : result[i] = genes[i];
        }
        return result;
    }

    // Overloading the [] operator to allow accessing the Genome as though it were 
    // an array.
    // Assume that all requests will be within the bounds of the genome. I'll have
    // bigger problems if they aren't.
    // I'm not certain why I need both a const and non-const version of this operator,
    // but oh well.
    const Gene& operator[](size_t index) const{
        return genes[index];
    }
    Gene& operator[](size_t index){
        return genes[index];
    }
    // stream extraction operator overload that will print out the formatted genome
    // with each gene as a column and each codon as a row.
    friend std::ostream& operator<<(std::ostream& os, const Genome& genome) {
        for (int j = 0; j < 5; j++){ // For each codon
            for (int i = 0; i < 16; i++){ // For each gene
                std::cout << genome[i][j] << ' ';
            }
            std::cout << std::endl;
        }
        return os;
    }

};

class Robot{
private:
    int turnsSurvived = 0; // How many actions the robot took before dying in the most recent sim
    int generationsSurvived = 0; // Keep track of how many generations this bot is in the top 50%
    int ID; // The robot's ID number. IDs are sequential across robots and generations.
    const Genome genes; // The robot's genetic material. This will not change after initialization.

public:
    // Create a robot with a specified ID, but random genes.
    // used for first generation of robots
    Robot(int robotNumber) : ID(robotNumber), genes(Genome()) {}
    // Create a robot with a specified genome,
    // used for breeding robots. Genome passed by reference to avoid
    // copying data needlessly
    Robot(int robotNumber, const Genome& genesIn) : ID(robotNumber), genes(genesIn) {}
    // Create a robot with a specified genome but no specified ID
    // used in breeding to have the ID applied later in main. Avoids
    // passing down extra arguments and dirtying the syntax of robot breeding.
    Robot(const Genome& genesIn) : genes(genesIn){}

    // const function that returns a robot's genome.
    // const because the bot is not modified at all.
    Genome get_genome() const {
        return genes;
    }

    void set_ID(int newID){
        ID = newID;
    }

    // "Breed" two bots. That is, combine their genomes and return a robot with 
    // the new genome.
    // const because it does not modify either parameter bot.
    Robot operator+(const Robot& other) const{
        Genome combinedGenes = other.get_genome() + genes;
        return Robot(combinedGenes);
    }

    // Output all the information contained in a robot to the stream in case I want
    // to inspect an individual bot.
    friend std::ostream& operator<<(std::ostream& os, Robot& robot){
        std::cout << "Robot ID: " << robot.ID << '\n';
        std::cout << "Most Recent Simulation Fitness: " << robot.turnsSurvived << " turns\n";
        std::cout << "Generations Survived: " << robot.generationsSurvived << '\n';
        std::cout << "Genome: \n";
        std::cout << robot.get_genome();

        return os;
    }
};


enum class MapCell{
    Open,
    Wall,
    Battery,
};
class Map{
private:
    // for now, map will always be 10x10. I may change that later.
    MapCell map[10][10];
    int robotx, roboty;
public:
    /*
        Default map constructor. Allows passing in a boolean for whether or not
        to generate interior walls. Additionally, allows specifying the frequency of
        battery generation. I may want to use these variables to change the difficulty
        of the sim for later robots and see how well the algorithm adapts to 
        more difficult mazes. Interior walls will also test whether robots "slack"
        on evolving wall avoidance.
    */
    Map(bool generateInteriorWalls = false, int interiorWallLikelihood = 0, int batteryLikelihood = 40){
        for (int i = 0; i < 10; i++){ // For each row
            for (int j = 0; j < 10; j++){ // For each column

                // Construct walls around the edges
                if (i == 0 || i == 9){
                    map[i][j] = MapCell::Wall;
                    continue; // don't generate anything else
                }
                if (j == 0 || j == 9){
                    map[i][j] = MapCell::Wall;
                    continue; // don't generate anything else
                }

                // Battery generation
                int cellContainsBattery = RandNo::get_instance().random_int(0, 100);
                if (cellContainsBattery < batteryLikelihood){
                    map[i][j] = MapCell::Battery;
                    continue; // don't generate further
                } else {
                    map[i][j] = MapCell::Open;
                    continue; // don't generate further
                }

                // Interior wall generation
                if (!generateInteriorWalls) break; // stop if interior walls shouldn't be generated
                int cellIsWall = RandNo::get_instance().random_int(0, 100);
                if (cellIsWall < interiorWallLikelihood){
                    map[i][j] = MapCell::Wall;
                    continue;
                }
            }
        }
        robotx = -1;
        roboty = -1;
    }

    // Allows a map to keep track of the robots location. Used primarily
    // for outputting the map to the console. However, simulations handle the
    // robot's actual movement. This is just a redundant storage of the information.
    void update_robot_position(int rx, int ry){
        robotx = rx;
        roboty = ry;
    }

    // stream insertion operator to print out a map. Used to double check generation.
    friend std::ostream& operator<<(std::ostream& os, Map& map){
        for (int i = 0; i < 10; i++){ // For each row
            for (int j = 0; j < 10; j++){ // For each column
                // Show the robot's location, skip the rest of the cell info if robot is there.
                if (i == map.robotx && j == map.roboty){
                    std::cout << 'X';
                    continue;
                }
                if (map.map[i][j] == MapCell::Wall) std::cout << '#';
                if (map.map[i][j] == MapCell::Battery) std::cout << 'o';
                if (map.map[i][j] == MapCell::Open) std::cout << ' ';
            }
            std::cout << std::endl;
        }
    }
};


/*
    The main loop of the code occurs within main. Main is responsible for tracking
    values across individual simulations, and holds the population of robots.
    However, each individual bot has its sim run in a separate function that 
    returns the bot updated with relevant info like turns survived. If the bot goes on
    to breed, main will increment its generations survived value.
*/
int main(){
    Robot r1 = Robot(1);
    Robot r2 = Robot(2);
    Robot r3 = r1 + r2;
    r3.set_ID(3);
    std::cout << "Robot 3 created with genes from one and two. Genes are:\n";
    std::cout << r3;

    Map map = Map();
    std::cout << map;
}