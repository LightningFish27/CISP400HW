//Final.cpp
//CISP 400
//12/10/24

#include <iostream>
#include <iomanip>
#include <chrono>
#include <algorithm> // used for sorting population by fitness.

const int GENERATIONS = 100; // How many generations to simulate
const bool GENERATE_INT_WALLS = true; // Whether to generate interior walls
const int INT_WALL_FREQ = 25; // How likely is an interior tile to be a wall?
const int BATT_LIKELIHOOD = 40; // How likely is a tile to be a battery
const int MUT_RATE = 5; // percentage of gene shuffles that incur a mutation

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
    // Return a codon
    SensorState operator[](size_t index) const {
        if (index < 4) return sensorStatePattern[index];
        return sensorStatePattern[0]; // default return should never be used.
    }
    // non-const version
    SensorState& operator[](size_t index) {
        if (index < 4) return sensorStatePattern[index];
        return sensorStatePattern[0];
    }
    // Return the action that corresponds to this gene
    Action get_action() const {
        return actionToTake;
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
            if (RandNo::get_instance().random_int(0, 100) < MUT_RATE){
                Gene mutatedGene = result[i];
                int codon = RandNo::get_instance().random_int(0, 4);
                if (codon < 4) { // decide whether to alter a sensor or action
                    mutatedGene[codon] = static_cast<SensorState>(RandNo::get_instance().random_int(static_cast<int>(SensorState::NoObject), static_cast<int>(SensorState::Ignore)));
                } else {
                    mutatedGene.actionToTake = static_cast<Action>(RandNo::get_instance().random_int(static_cast<int>(Action::North), static_cast<int>(Action::Random)));
                }
                result[i] = mutatedGene;
            }
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
        for (int j = 0; j < 4; j++){ // For each codon
            for (int i = 0; i < 16; i++){ // For each gene
                std::cout << static_cast<int>(genome[i][j]) << ' ';
            }
            std::cout << std::endl;
        }
        for (int i = 0; i < 16; i++){ // for each gene
            std::cout << static_cast<int>(genome[i].get_action()) << ' ';
        }
        return os;
    }

};

class Robot{
private:
    int turnsSurvived = 0; // How many actions the robot took before dying in the most recent sim
    int energyHarvested = 0; // How many batteries the bot collected
    int generationsSurvived; // Keep track of how many generations this bot is in the top 50%
    int ID; // The robot's ID number. IDs are sequential across robots and generations.
    int energy; // The robot's energy. Reaching 0 kills the bot
    const Genome genes; // The robot's genetic material. This will not change after initialization.

public:
    // Create a robot with a specified ID, but random genes.
    // used for first generation of robots
    Robot(int robotNumber) : ID(robotNumber), genes(Genome()), energy(5), generationsSurvived(0){}
    // Create a robot with a specified genome,
    // used for breeding robots. Genome passed by reference to avoid
    // copying data needlessly
    Robot(int robotNumber, const Genome& genesIn) : ID(robotNumber), genes(genesIn), energy(5), generationsSurvived(0) {}
    // Create a robot with a specified genome but no specified ID
    // used in breeding to have the ID applied later in main. Avoids
    // passing down extra arguments and dirtying the syntax of robot breeding.
    Robot(const Genome& genesIn) : genes(genesIn), energy(5), generationsSurvived(0) {}

    // Act on the robot's genome
    Action get_action_from_sensors(SensorState n, SensorState s, SensorState e, SensorState w){
        for (int i = 0; i < 16; i++) { // For each gene
            Gene gene = genes[i];
            bool geneMatches = true;
            if (gene[0] != n && gene[0] != SensorState::Ignore) geneMatches = false;
            if (gene[1] != s && gene[1] != SensorState::Ignore) geneMatches = false;
            if (gene[2] != e && gene[2] != SensorState::Ignore) geneMatches = false;
            if (gene[3] != w && gene[3] != SensorState::Ignore) geneMatches = false;
            if (geneMatches) return gene.get_action();
        }
        return genes[15].get_action(); // Default action if no state matches
    }

    // const function that returns a robot's genome.
    // const because the bot is not modified at all.
    Genome get_genome() const {
        return genes;
    }

    int get_energy() const {
        return energy;
    }

    int get_ID() const {
        return ID;
    }

    int get_turns_survived() const {
        return turnsSurvived;
    }

    int get_energy_harvested() const {
        return energyHarvested;
    }

    int get_generations_survived() const {
        return generationsSurvived;
    }

    void set_ID(int newID){
        ID = newID;
    }

    void set_energy(int newEnergy){
        energy = newEnergy;
    }

    void set_energy_harvested(int newHarv){
        energyHarvested = newHarv;
    }

    void dec_energy(){
        energy--;
    }

    void inc_energy(){
        energy++;
    }

    void inc_turns_survived(){
        turnsSurvived++;
    }

    void inc_generations_survived(){
        generationsSurvived++;
    }

    void set_turns_survived(int newTurns){
        turnsSurvived = newTurns;
    }

    void inc_energy_harvested() {
        energyHarvested++;
    }

    // "Breed" two bots. That is, combine their genomes and return a robot with 
    // the new genome.
    // const because it does not modify either parameter bot.
    // Return a pointer to the new robot
    Robot* operator+(const Robot& other) const{
        Genome combinedGenes = other.get_genome() + genes;
        return new Robot(combinedGenes);
    }

    // Output all the information contained in a robot to the stream in case I want
    // to inspect an individual bot.
    friend std::ostream& operator<<(std::ostream& os, Robot& robot){
        os << "Robot ID: " << robot.ID << '\n';
        os << "Most Recent Turns Survived: " << robot.turnsSurvived << " turns\n";
        os << "Most Recent Energy Harvested: " << robot.energyHarvested << " batteries\n";
        os << "Generations Survived: " << robot.generationsSurvived << '\n';
        os << "Genome: \n";
        os << robot.get_genome();
        os << std::endl;

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
                if (RandNo::get_instance().random_int(0, 100) < batteryLikelihood){
                    map[i][j] = MapCell::Battery;
                    continue; // don't generate further
                }

                // Interior wall generation
                if (RandNo::get_instance().random_int(0, 100) < interiorWallLikelihood && generateInteriorWalls){
                    map[i][j] = MapCell::Wall;
                    continue;
                }

                map[i][j] = MapCell::Open;
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
    
    // Return the type of cell at x and y
    MapCell get_cell(int x, int y) const {
        return map[y][x];
    }

    // Set a cell to a new type
    void set_cell(int x, int y, MapCell newType) {
        map[y][x] = newType;
    }

    // stream insertion operator to print out a map. Used to double check generation.
    friend std::ostream& operator<<(std::ostream& os, Map& map){
        for (int i = 0; i < 10; i++){ // For each row
            for (int j = 0; j < 10; j++){ // For each column
                // Show the robot's location, skip the rest of the cell info if robot is there.
                if (j == map.robotx && i == map.roboty){
                    os << "X ";
                    continue;
                }
                if (map.map[i][j] == MapCell::Wall) os << "# ";
                if (map.map[i][j] == MapCell::Battery) os << "o ";
                if (map.map[i][j] == MapCell::Open) os << "_ ";
            }
            os << std::endl;
        }
        return os;
    }
};


void run_single_sim(Robot& robot);

/*
    The main loop of the code occurs within main. Main is responsible for tracking
    values across individual simulations, and holds the population of robots.
    However, each individual bot has its sim run in a separate function that 
    returns the bot updated with relevant info like turns survived. If the bot goes on
    to breed, main will increment its generations survived value.
*/
int main(){

    /*
        keep track of the robot's improvement over time.
    */
   float averageTurnsPerGeneration[GENERATIONS];
   float averageEnergyHarvestedPerGeneration[GENERATIONS];
   float averageGenerationsPerGeneration[GENERATIONS];

    /*
        population initialization
    */
    // Used for creating robot IDs
    int robotsGenerated = 0;
    // The population of robots in the simulation. Since there will always be 200,
    // no need for the array to be dynamic.
    Robot* population[200];
    // Generate a random robot for each index to start. Successive pops will be bred
    for(int i = 0; i < 200; i++){
        population[i] = new Robot(robotsGenerated++);
    }

    // The loop of each generation. Individual simulations will be run from within
    // here.
    int generations = 0;
    while (generations < GENERATIONS){
        // Run each bot's simulation
        for (int i = 0; i < 200; i++){
            run_single_sim(*population[i]); // Dereferenced pointer to allow reference passing
            continue;
        }

        // Process data collected and add to arrays
        float averageTurnsSurvived = 0;
        float averageGenerationsSurvived = 0;
        float averageEnergyHarvested = 0;
        for (int i = 0; i < 200; i++){
            averageTurnsSurvived += population[i]->get_turns_survived();
            averageGenerationsSurvived += population[i]->get_generations_survived();
            averageEnergyHarvested += population[i]->get_energy_harvested();
        }
        averageTurnsPerGeneration[generations] = averageTurnsSurvived / 200;
        averageGenerationsPerGeneration[generations] = averageGenerationsSurvived / 200;
        averageEnergyHarvestedPerGeneration[generations] = averageEnergyHarvested / 200;

        // Sort bots by fitness using std::sort and a lamba defined descending sort
        // I would have written a simple sort myself, but this array of 200 elements
        // will be sorted 100 times, and performance felt important.
        std::sort(population, population + 200, [](const Robot* a, const Robot* b) {
            return a->get_energy_harvested() > b->get_energy_harvested();
        });

        // Breed highest performing bots into empty pop slots
        for (int i = 0; i < 100; i += 2){
            // Clear the old robots (KILL THEM)
            delete population[100 + i];
            delete population[100 + i + 1];
            // Breeding new bots into the opened slots
            population[100 + i] = *population[i] + *population[i + 1]; // First child
            population[100 + i + 1] = *population[i] + *population[i + 1]; // Second child
            population[100 + i]->set_ID(robotsGenerated++);
            population[100 + i + 1]->set_ID(robotsGenerated++);
            // increment the survival time of the surviving bots here to avoid having another loop
            population[i]->inc_generations_survived();
            population[i + 1]->inc_generations_survived();
        }
        generations++;
    }

    std::cout << "Average turns, energy, and generations survived over time.\n";
    std::cout << "TURNS\tENERGY\tGENERATIONS\n";
    std::cout << std::setprecision(4);
    for (int i = 0; i < GENERATIONS; i++){
        std::cout << averageTurnsPerGeneration[i] << '\t' << averageEnergyHarvestedPerGeneration[i] << '\t' << averageGenerationsPerGeneration[i] << '\n';
    }

    std::cout << '\n' << "Now, the best robot was: \n";
    std::cout << *population[0];

    std::cout << "\nWhat have I noticed from playing with the parameters of this experiment?\n";
    std::cout << "For one, the generationsSurvived parameter remains low throughout the entirety\n";
    std::cout << "of the simulation. I believe this is because robots are quickly out-evolved,\n";
    std::cout << "and therefore never grow to a great age without breeding a stronger succesor.\n";
    std::cout << "Additionally, I've noticed that the average fitness per generation only increases\n";
    std::cout << "by a few turns throughout the sim, but that the best performing robot is usually MUCH\n";
    std::cout << "more fit than the average. improvement seems to level off after some time.\n";
    std::cout << "Notably, I programmed the ability to view the map of a run into this code,\n";
    std::cout << "but do not have it actively print out for each sim. You are welcome to << map to see it.\n";
    std::cout << "You may alter most parameters via the constants at the top of the file.\n";

    // Free memory used by population at the end of the program.
    for (int i = 0; i < 200; i++){
        delete population[i];
    }
}

/*
    Runs a single simulation. That is, take a robot from the population and run it
    through a random map until it dies. Robot is passed by reference so that the 
    robot in main's population pool gets its turnsSurvived incremented.
*/
void run_single_sim(Robot& robot){
    // Robot initialization
    robot.set_energy(5); // Set the robot back to full energy
    robot.set_energy_harvested(0); // reset the energy harvested this sim
    robot.set_turns_survived(0); // reset turns survived
    int x;
    int y;

    Map map = Map(GENERATE_INT_WALLS, INT_WALL_FREQ, BATT_LIKELIHOOD);

    // Ensure that the bot does not spawn on a wall
    bool validStartingPos;
    while (!validStartingPos){
        validStartingPos = true;
        x = RandNo::get_instance().random_int(1, 8);
        y = RandNo::get_instance().random_int(1, 8);
        if (map.get_cell(x, y) == MapCell::Wall) validStartingPos = false;
    }

    while(robot.get_energy() > 0){ // until the robot runs out of energy
        // query map for robot's surroundings
        // interestingly, static cast works in this case as the map will never
        // have a cell that is greater than battery.
        SensorState northSensor = static_cast<SensorState>(map.get_cell(x, y - 1));
        SensorState southSensor = static_cast<SensorState>(map.get_cell(x, y + 1));
        SensorState eastSensor = static_cast<SensorState>(map.get_cell(x + 1, y));
        SensorState westSensor = static_cast<SensorState>(map.get_cell(x - 1, y));

        // check sensor states against robot genome
        Action robotAction = robot.get_action_from_sensors(northSensor, southSensor, eastSensor, westSensor);
        // act on robot genome
        if (robotAction == Action::North){
            if (map.get_cell(x, y - 1) != MapCell::Wall) y -= 1;
        } else if (robotAction == Action::South){
            if (map.get_cell(x, y + 1) != MapCell::Wall) y += 1;
        } else if (robotAction == Action::East){
            if (map.get_cell(x + 1, y) != MapCell::Wall) x += 1;
        } else if (robotAction == Action::West){
            if (map.get_cell(x - 1, y) != MapCell::Wall) x -= 1;
        } else if (robotAction == Action::Random){
            // The random logic chain is a little ugly, but it is basically just two coinflips
            if (RandNo::get_instance().random_int(0, 1)){ // North/South
                if (RandNo::get_instance().random_int(0, 1)){ // North
                    if (map.get_cell(x, y - 1) != MapCell::Wall) y -= 1;
                } else { // South
                    if (map.get_cell(x, y + 1) != MapCell::Wall) y += 1;
                }
            } else { // East/West
                if (RandNo::get_instance().random_int(0, 1)){ // East
                    if (map.get_cell(x + 1, y) != MapCell::Wall) x += 1;
                } else { // West
                    if (map.get_cell(x - 1, y) != MapCell::Wall) x -= 1;
                }
            }
        }
        robot.dec_energy(); // expend energy used to move
        if (map.get_cell(x, y) == MapCell::Battery){ // If there is a battery, consume it
            robot.inc_energy();
            robot.inc_energy_harvested();
            map.set_cell(x, y, MapCell::Open);
        }
        // update robot energy and map
        if (robot.get_energy() > 0) robot.inc_turns_survived();
        map.update_robot_position(x, y);
    }
}
