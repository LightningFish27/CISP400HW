// Hokeeman.cpp
// CISP 400
// Gavin Williams, Written Oct 7, 2024

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <iomanip>

// Constants for RNG
const int BOREDOM_MIN_REDUCTION = 4;
const int BOREDOM_MAX_REDUCTION = 8;
const int MAX_BOREDOM = 20;
const int HUNGER_MIN_REDUCTION = 3;
const int HUNGER_MAX_REDUCTION = 6;
const int MIN_HUNGER = 0;

/*
    MY STL IMPLEMENTATION COPIED INTO THE HEADER
    The following classes are not assignment-specific.
*/
class Logger
{
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> begin;
    std::string functionName;
    static int depth;
    
public:
    // indent entry as far as depth.
    Logger(std::string functionName)
    {
        this->functionName = std::string(functionName);
        begin = std::chrono::system_clock::now();
        std::clog << std::string(depth, '\t') << "Start of '" << functionName << "' @ " << get_current_time();
        ++depth;
    }
    ~Logger()
    {
        --depth;
        auto end = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
        std::clog << std::string(depth, '\t') << "Elapsed Time: " << duration.count() << " microseconds\n";
        std::clog << std::string(depth, '\t') << "End of '" << functionName << "'\n";
        std::clog << std::string(30, '-') << '\n';
    }
    static std::string get_current_time()
    {
        std::time_t currentTime = std::time(nullptr);
        std::string timeAsString = std::ctime(&currentTime);
        return timeAsString;
    }
};
int Logger::depth = 0; // Init depth

/*
    G_Array is a templated array that automatically resizes to fit data added, 
    and can scale down when elements are removed (only supports removal from
    last indice).
*/
template <typename T>
class G_Array{
private:
    T* array;
    int size;
public:
    // Constructor initializes an array of size 0 and a size tracker.
    G_Array() : array(new T[0]), size(0) {}
    // Destructor to free array on leaving scope.
    ~G_Array() {
        delete[] array;
    }

    // Adding to the array is done through the add_element function, which
    // abstracts away all the resizing done.
    void add_element(T newElement){
        Logger l = Logger("add_element");
        size++;
        T* pTmp = new T[size];
        for (int i = 0; i < size; i++){
            pTmp[i] = array[i];
        }
        pTmp[size - 1] = newElement;
        delete[] array;
        array = pTmp;
    }
    // Remove the last element of the array
    bool remove_last(){
        Logger l = Logger("remove_element");
        if (size == 0) return false;
        size--;
        T* pTmp = new T[size];
        for (int i = 0; i < size; i++){
            pTmp[i] = array[i];
        }
        delete[] array;
        array = pTmp;
        return true;
    }
    // Remove a specified array element
    // Returns false if removal was unsuccessful
    bool remove_element(const T& target){
        int targetIndex = -1;
        for (int i = 0; i < size; i++){
            if (array[i] == target) {
                targetIndex = i;
                break;
            }
        }
        if (targetIndex == -1) return false;
        remove_element_at(targetIndex);
        return true;
    }
    // Removes a specified index from array
    // Returns false if index out of bounds
    bool remove_element_at(int index){
        Logger l = Logger("remove_element_at");
        if (index < 0 || static_cast<size_t>(index) > size){
            std::cout << "Attempted to remove index not within array.\n";
            return false;
        }
        size--;
        T* pTmp = new T[size];
        for (size_t i = 0, j = 0; i < size + 1; i++) {
            if (i != index) {
                pTmp[j++] = array[i];
            }
        }
        delete[] array;
        array = pTmp;
        return true;
    }
    // Overloading the [] operator to allow G_Array[idx] calls, rather
    // than entire function calls written out. 
    T& operator[](size_t index){
        Logger l = Logger("[] operator");
        if (index >= size){
            std::cout << "Attempting to access an out of bounds indice.\n";
            exit(0);
        }
        else
            return array[index];
    }
    size_t length(){
        Logger l = Logger("length");
        return size;
    }
    void ComponentTest(){
        std::cout << "Beginning Component testing of G_Array class template.\n";
        std::cout << "Length: " << length() << '\n';
        std::cout << "Adding element.\n";
        add_element(T());
        std::cout << "Length: " << length() << '\n';
        std::cout << "Removing element.\n";
        remove_element();
        std::cout << "Length: " << length() << '\n';
        std::cout << "Completed component test of G_Array\n\n";
    }
};
/*
    A singleton random number generator that provides multiple methods for creating
    randomness
*/
class RandNo {
public:
    // Deleting the ability to copy instances or set instances equal to another
    RandNo(const RandNo&) = delete;
    RandNo& operator=(const RandNo&) = delete;
    // Overriding stream insertion operator to set seed
    friend std::istream& operator>>(std::istream& is, RandNo& rng) {
        int seed;
        is >> seed;
        std::srand(static_cast<unsigned int>(seed));
        return is;
    }

    // Static method to access the singleton instance
    // Once RandNo is made, its static nature keeps it present
    // in only one place.
    static RandNo& get_instance() {
        Logger l = Logger("get_instance");
        static RandNo instance;
        return instance;
    }

    // Method to generate a random integer between min and max
    int random_int(int min, int max) {
        Logger l = Logger("random_int");
        return min + rand() % (max - min + 1);
    }
    // Method to generate a random floating point value between min and max
    float random_float(float min, float max) {
        Logger l = Logger("random_float");
        return min + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (max - min)));
    }

    void ComponentTest(){
        std::cout << "Beginning RandNo Component Testing.\n";
        std::cout << "Random int: " << random_int(0, 100) << " Random Float: " << random_float(0.00, 100.00) << '\n';
        std::cout << "Both numbers should be between 0 and 100.\n";
        std::cout << "Ending RandNo component testing.\n\n";
    }

private:
    // Private constructor to prevent instantiation
    RandNo() {
        Logger l = Logger("RandNo Constructor");
        std::srand(static_cast<unsigned int>(std::time(0))); // Seed the random number generator
    }
};

/*
    THE FOLLOWING CLASSES AND DATA ARE DESIGNED FOR USE IN THIS PROGRAM
*/

// Specification C2 - Creature class
// Specification B2 - Virtual Class Creature
class Creature {
protected:
    int boredom, hunger;
    std::string name;
    void copy(Creature& otherCreature){
        boredom = otherCreature.get_boredom();
        hunger = otherCreature.get_hunger();
        name = otherCreature.get_name();
    }
public:
    Creature(){}
    // Specification A2 - Copy Constructor
    /*
        Since I want to be able to copy data between either IHungryCreature OR 
        IBoredCreature, I move the copy constructor into the abstract base class,
        then call it from the child class, passing up the reference to the other
        creature to copy from.
    */
    // Copy constructor
    Creature(Creature& other) {
        copy(other);
    }
    // SETTERS
    void set_name(std::string newName){
        name = newName;
    }
    void set_boredom(int newBoredom){
        boredom = newBoredom;
    }
    void set_hunger(int newHunger){
        hunger = newHunger;
    }
    // GETTERS
    std::string get_name(){
        return name;
    }
    int get_hunger(){
        return hunger;
    }
    int get_boredom(){
        return boredom;
    }
    // Specification C4 - Overload «
    // Prints the Creature's current stats to the console
    friend std::ostream& operator<<(std::ostream& os, Creature& creature){
        os << creature.get_name() << " has hunger " << creature.get_hunger() << " and boredom " << creature.get_boredom() << '\n';
        return os;
    }

    //Specification B4 - Overload + Operator
    // Overridden within child classes. Children return a creature of the same type
    // as the one on the left of the + operator.
    virtual Creature* operator+(Creature& other) = 0;

    //Specification B3 - Overload Assignment Operator
    // No need for this to be virtual, the functionality would be the same for
    // either child.
    Creature* operator=(Creature& other){
        this->boredom = other.get_boredom();
        this->hunger = other.get_hunger();
        this->name = other.get_name();
        return this;
    }
    //METHODS
    // Specification C1 - PassTime()
    /*
        ActionTaken = 0 if user chose to listen, 
                      1 if user chose to play
                      2 if user chose to feed
        Note that this base logic is overriden for hungry or bored creatures.
    */
    virtual void PassTime(int actionTaken) = 0;
    void play(){
        boredom -= RandNo::get_instance().random_int(BOREDOM_MIN_REDUCTION, BOREDOM_MAX_REDUCTION);
        if (boredom < 0) boredom = 0;
    }
    void feed(){
        hunger += RandNo::get_instance().random_int(HUNGER_MIN_REDUCTION, HUNGER_MAX_REDUCTION);
    }
};
// Specification B1 - Child Class
// Bored creatures become bored more quickly than regular creatures.
class IBoredCreature : public Creature {

public:
    IBoredCreature(){
        // Assignment specifies to start hunger and boredom at random numbers 0-5,
        // but if hunger starts at 0, the hokeemon immediately dies. I'm using 1 as a lower bound.
        hunger = RandNo::get_instance().random_int(1,5);
        boredom = RandNo::get_instance().random_int(1,5);
    }
    void PassTime(int actionTaken){
        if ( actionTaken == 1 ) { // Play
            hunger--;
        }
        else if (actionTaken == 2){ //Feed
            boredom += 2;
        }
        else {
            hunger--;
            boredom += 2;
        }
    }
    IBoredCreature(Creature& otherCreature){
        copy(otherCreature);
    }
    Creature* operator+(Creature& other){
        Creature* result = new IBoredCreature();
        result->set_boredom(this->boredom + other.get_boredom());
        result->set_hunger(this->hunger + other.get_hunger());
        result->set_name(this->name + other.get_name());
        return result;
    }
};

// Specification A3 - Second Child Class
// Hungry creatures become hungry more quickly than regular creatures
class IHungryCreature : public Creature {

public:
    IHungryCreature(){
        // Assignment specifies to start hunger and boredom at random numbers 0-5,
        // but if hunger starts at 0, the hokeemon immediately dies. I'm using 1 as a lower bound.
        hunger = RandNo::get_instance().random_int(1,5);
        boredom = RandNo::get_instance().random_int(1,5);
    }
    void PassTime(int actionTaken){
        if ( actionTaken == 1 ) { // Play
            hunger -= 2;
        }
        else if (actionTaken == 2){ //Feed
            boredom++;
        }
        else {
            hunger -= 2;
            boredom++;
        }
    }
    // Copy constructor
    IHungryCreature(Creature& otherCreature){
        copy(otherCreature);
    }
    Creature* operator+(Creature& other){
        Creature* result = new IHungryCreature();
        result->set_boredom(this->boredom + other.get_boredom());
        result->set_hunger(this->hunger + other.get_hunger());
        result->set_name(this->name + other.get_name());
        return result;
    }
};

// FUNCTION PROTOTYPES
void program_greeting();
bool validate_input(std::string);
Creature* get_referenced_creature(G_Array<Creature*>&, std::string);
std::string to_upper(std::string);


int main(){
    // SETUP
    std::ofstream fileOut("log.txt"); // Opening log file
    std::clog.rdbuf(fileOut.rdbuf()); // Redirecting clog to write to log file
    Logger l = Logger("main");
    std::cout << std::fixed << std::setprecision(2);
    program_greeting();


    G_Array<Creature*> creatureCollection;

    // Specification A4 - Write a Lambda
    auto capital_input = []() -> std::string {std::string in;std::getline(std::cin, in);return to_upper(in);};


    Creature *testCreature = new IBoredCreature();
    testCreature->set_name("HENRY");
    creatureCollection.add_element(testCreature);

    bool programRunning = true;
    while(programRunning){
        // Get user command
        std::string userInput;
        // Get user input
        bool inputValid;
        do {
            inputValid = true;
            std::cout << "Enter a command: ";
            userInput = capital_input();
            inputValid = validate_input(userInput);
            if (!inputValid){
                std::cout << "Please enter a valid command. Follow format '[command] [target]\n";   
            }
        } while (!inputValid);
        // process user command
        std::istringstream inputStream(userInput);
        std::string command;
        inputStream >> command;
        std::string target, word;
        while (inputStream >> word){
            target += word;
            if (!inputStream.eof()) target += ' ';
        }
    
        int playOrFeed = 0;
        Creature *targetCreature = get_referenced_creature(creatureCollection, target);

        bool nonTargetCommand = false;
        if (command == "HELP"){
            std::cout << "Available Commands:\n";
            std::cout << "Play\t:\tplay with target hokeemon, lowering their boredom.\n";
            std::cout << "Feed\t:\tfeed target hokeemon, raising their hunger.\n";
            std::cout << "Breed\t:\tcreate offspring from target hokeemon and another.\n";
            std::cout << "Listen\t:\tlisten to target hokeemon's feelings and status.\n";
            std::cout << "View\t:\tsee a list of your hokeeman.\n";
            std::cout << "Rename\t:\tchange the name of a hokeemon.\n";
            std::cout << "Getnew\t:\tget a new random hokeemon.\n";
            nonTargetCommand = true;
        }
        else if (command == "QUIT"){
            programRunning = false;
            nonTargetCommand = true;
        }
        else if (command == "VIEW"){
            for(int i = 0; i < creatureCollection.length(); i++){
                std::cout << creatureCollection[i]->get_name() << '\n';
            }
            nonTargetCommand = true;
        }
        else if (command == "GETNEW"){
            Creature* newCreature;
            if(RandNo::get_instance().random_int(0, 1)){
                newCreature = new IBoredCreature;
                std::cout << "Your new creature is a Bored Creature! They become bored quickly, so make sure to play lots!\n";
            } 
            else {
                newCreature = new IHungryCreature;
                std::cout << "Your new creature is a Hungry Creature! They become hungry quickly, so make sure to feed them lots.\n";
            }
            std::cout << "What will your new hokeemon's name be?\n";
            newCreature->set_name(capital_input());
            creatureCollection.add_element(newCreature);
            nonTargetCommand = true;
        }

        if (targetCreature == nullptr && !nonTargetCommand) {
            std::cout << "No creature by name '" << target << "' was found.\n";
            continue;
        }
        // act on user command
        if (command == "LISTEN"){ // Listen
            std::cout << *targetCreature;
        }
        else if (command == "PLAY") {
            targetCreature->play();
            playOrFeed = 1;
        }
        else if (command == "FEED") {
            targetCreature->feed();
            playOrFeed = 2;
        }
        // Specification A1 - Critter Name
        else if (command == "RENAME") {
            std::cout << "What should '" << targetCreature->get_name() << "''s new name be?\n";
            targetCreature->set_name(capital_input());
        }
        else if (command == "BREED"){
            std::cout << "Which creature would you like to breed " << targetCreature->get_name() << " with?\n";
            Creature* secondTarget = get_referenced_creature(creatureCollection, capital_input());
            if (secondTarget != nullptr){
                Creature* newCreature = *targetCreature + *secondTarget;
                creatureCollection.add_element(newCreature);
            }
            else {
                std::cout << "No creature by specified name was found.\n";
            }
        }
        

        // Check for any dead creatures and tell the user:
        for (int i = 0; i < creatureCollection.length(); i++){
            Creature* c = creatureCollection[i]; // grab the currently checked creature
            bool starvedThisTurn = false;
            bool comatoseThisTurn = false; // init variables to check status
            // if the current creature was interacted with this turn, take that into account
            if (targetCreature == c){
                c->PassTime(playOrFeed);
            }
            else {
                c->PassTime(0);
            }

            // Check boredom and hunger status
            if (c->get_boredom() > MAX_BOREDOM){
                std::cout << c->get_name() << " has gone catatonic from boredom.\n";
                comatoseThisTurn = true;
            }
            if (c->get_hunger() < MIN_HUNGER){
                std::cout << c->get_name() << " has died of starvation!\n";
                starvedThisTurn = true;
            }
            if (starvedThisTurn || comatoseThisTurn) {
                creatureCollection.remove_element(c);
                i--;
            }
        }
    }
    // perform any cleanup needed
    // not closing fileOut because that prevents the logger from including the end of main.
    exit(0);
}
/*
    Searches through a collection of creatures then returns the one with a matching
    name by reference. If no matching name is found in the array, then it returns
    nullptr
*/
Creature* get_referenced_creature(G_Array<Creature*>& collection, std::string targetName){
    Logger l = Logger("get_reference_creature");
    if (collection.length() == 0) return nullptr;
    for (int i = 0; i < collection.length(); i++){
        if (collection[i]->get_name() == targetName){
            return collection[i];
        }
    }
    return nullptr;
}

// Return given string in uppercase
std::string to_upper(std::string str){
    Logger l = Logger("to_upper");
    for (int i = 0; i < str.length(); i++){
        str[i] = std::toupper(str[i]);
    }
    return str;
}

// Specification C3 - Validate Input
// User input should be in the format "[command] [target]"
// where command is a single letter, and target is the name
// of the hokeemon you want to interact with.
bool validate_input(std::string userInput){
    Logger l = Logger("validate_input");
    std::istringstream inputStream(userInput);
    std::string command;
    inputStream >> command;
    if (command == "PLAY") return true; // Play
    if (command == "FEED") return true; // Feed
    if (command == "LISTEN") return true; // Listen
    if (command == "QUIT") return true; // Quit
    if (command == "BREED") return true; // Breed
    if (command == "HELP") return true; // Help
    if (command == "VIEW") return true; // View collection
    if (command == "RENAME") return true; // Rename
    if (command == "GETNEW") return true; // Get New Creature
    return false;
}

void program_greeting(){
    Logger l = Logger("program_greeting");
    std::cout << "Welcome to Hokeeman.cpp by Gavin Williams, written Oct 7, 2024.\n";
    std::cout << "You will be able to interact with your hokeemen through a variety of methods,\n";
    std::cout << "much like a tomodachi. Enter the command 'help' to see all the things you can do.\n\n";
}

/*
Welcome to Hokeeman.cpp by Gavin Williams, written Oct 7, 2024.
You will be able to interact with your hokeemen through a variety of methods,
much like a tomodachi. Enter the command 'help' to see all the things you can do.

Enter a command: view
HENRY
Enter a command: help
Available Commands:
Play    :       play with target hokeemon, lowering their boredom.
Feed    :       feed target hokeemon, raising their hunger.
Breed   :       create offspring from target hokeemon and another.
Listen  :       listen to target hokeemon's feelings and status.
View    :       see a list of your hokeeman.
Rename  :       change the name of a hokeemon.
Getnew  :       get a new random hokeemon.
Enter a command: getnew
Your new creature is a Hungry Creature! They become hungry quickly, so make sure to feed them lots.
What will your new hokeemon's name be?
patrick
Enter a command: feed patrick
Enter a command: feed patrick
Enter a command: view
HENRY
PATRICK
HENRY has died of starvation!
Enter a command: play patrick
Enter a command: feed patrick
Enter a command: ^C
*/