// pig.cpp
// Gavin Williams, CISP 400
// 08/26/24

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <chrono>
#include <fstream>


// Specification C4 - Three Functions 
void ProgramGreeting();
int DisplayMenu();
std::string GameLoop(int *hiScore);
int D6();
int RandomNumber(int, int);
void DisplayRoundInfo(int, int, int, int);

//Specification B2 - Function Activity to Disk
// A method to time each function and generate a logfile
class Timer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> begin;
    std::string functionName;
public:
    Timer(std::string functionName){
        this->functionName = functionName;
        begin = std::chrono::system_clock::now();
        std::clog << "Start of '" << functionName << "' @ " << get_current_time();
    }
    ~Timer(){
        auto end = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
        std::clog << "\tEnd of '" << functionName << "'\n";
        std::clog << "\tElapsed Time: " << duration.count() << " microseconds\n";
    }
    static std::string get_current_time(){
        std::time_t currentTime = std::time(nullptr);
        std::string timeAsString = std::ctime(&currentTime);
        return timeAsString;
    }
};

// Provides methods for gathering user data
class User {
public:
    int score = 0;

    // Specification C3 - Numeric Menu
    int GetTurnChoice(){
        Timer t("GetTurnChoice");
        int choice = -1;
        bool inputValid;
        do {
            inputValid = true;
            std::cout << "Please choose whether to:\n";
            std::cout << "1: ROLL\n";
            std::cout << "2: HOLD\n";
            std::cin >> choice;
            if (choice < 1 || choice > 2){
                std::cout << "Please choose a value 1 or 2.\n";
                inputValid = false;
            }
        } while (!inputValid);
        return choice;
    }
};

// Provides methods for generating CPU moves
class CPU {
public:
    int score = 0;
    int GetTurnChoice(){
        Timer t("GetTurnChoice");
        int roll = D6();
        if (roll > 3) return 0;
        return 1;
    }
};

// A simple stack representation of limited size
class Stack{
private:
    std::string arr[25];
public:
    int top;
    Stack(){
        top = -1; // default to index of 0
    }
    bool is_empty() {
        Timer t("is_empty");
        if (top == -1) return true;
        return false;
    }
    bool is_full() {
        Timer t("is_full");
        if (top == 25 - 1) return true;
        return false;
    }

    void push (std::string toPush){
        Timer t("push");
        if (!is_full()){
            top++;
            arr[top] = toPush;
        } else {
            // Ideally, this will never happen. But we should know if it does.
            std::cout << "Stack is full, element not pushed.";
        }
    }
    std::string pop(){
        Timer t("pop");
        if (!is_empty()){
            std::string popped = arr[top];
            top--;
            return popped;
        }
        // Ideally, this will never happen. But we should know if it does.
        return "Popped unsuccessfully from stack.";
    }
    std::string peek(int index){
        Timer t("peek");
        if (index <= top){
            return arr[index];
        }
        return "Couldn't peek uninitialized index\n";
    }
};
int display_stack(Stack);

int main(){
    // Specification C3 - Function Activity to Disk
    std::ofstream fileOut("log.txt"); // Opening log file
    std::clog.rdbuf(fileOut.rdbuf()); // Redirecting clog to write to log file
    Timer t("main");

    ProgramGreeting();
    // Specification C2 - Student Name
    std::string hPlayer;
    std::cout << "What is your first and last name?\n";
    getline(std::cin, hPlayer);

    // Specification C1 - Fixed Seed
    unsigned int seed = 3;
    srand(seed);

    // Specification B3 - hiScore on Heap
    // Allows persistent high score between games
    int* hiScore = new int;
    *hiScore = 0;

    bool playingGames = true;
    Stack winlog;
    while (playingGames) {
        // Play a game
        std::string winner = GameLoop(hiScore);
        // store the winner on the record of winners
        winlog.push(winner);

        // Specification A4 - Play Again Option
        char userChoice;
        bool inputValid;
        do {
            inputValid = true;
            // Specification A4 - Play Again Option
            std::cout << "Would you like to play another game? (Y/N)\n";
            std::cout << "Choose 'S' to see score history.\n";
            std::cin >> userChoice;
            userChoice = toupper(userChoice);
            // Specification A3 - Games Played Counter
            if (userChoice == 'S') display_stack(winlog);
            if (!(userChoice == 'N' || userChoice == 'Y')){
                std::cout << "Please enter 'Y' or 'N'\n";
                inputValid = false;
            }
        } while (!inputValid);
        if (userChoice == 'N') playingGames = false;
    };
    // Specification B4 – Display hiScore
    std::cout << "Across all games, high score was " << *hiScore << '\n';
    delete hiScore;

    std::cout << "Thanks for playing, " << hPlayer << ", have a nice day!\n";
}

// Handles the logic of running a single game.
// Returns 0 if User wins, 1 if CPU wins.
// Returns -1 if User quit
std::string GameLoop(int *hiScore){
    Timer t("Gameloop");
    User player = User();
    CPU cpu = CPU();
    bool playerWon = false, CPUWon = false;
    do {
        // Initialize turn scores for the player and CPU to 0
        int pTurnScore = 0;
        int cTurnScore = 0;

        DisplayRoundInfo(player.score, pTurnScore, cpu.score, cTurnScore);
        int pChoice;
        std::cout << "Your turn!\n";
        do {
            pChoice = player.GetTurnChoice();
            if (pChoice == 1) {
                int roll = D6();
                std::cout << "Rolling...\n";
                if (roll == 1){
                    std::cout << "You rolled a 1! You lose your points this turn and pass the turn.\n\n";
                    pTurnScore = 0;
                    break;
                }
                else {
                    std::cout << "You rolled a " << roll << '\n';
                    pTurnScore += roll;
                    DisplayRoundInfo(player.score, pTurnScore, cpu.score, cTurnScore);
                }
            }
            if (pChoice == 3) {
                return ("User Quit Game");
            }
        } while (pChoice == 1);
        player.score += pTurnScore;
        std::cout << "CPU turn!\n";
        std::cout << "Thinking...\n";
        sleep(1);
        int cChoice;
        do {
            cChoice = cpu.GetTurnChoice();
            if (cChoice == 1) {
                std::cout << "CPU chose to roll!\n";
                std::cout << "Rolling...\n";
                int roll = D6();
                std::cout << "CPU rolled a " << roll << '\n';
                if (roll == 1){
                    std::cout << "It loses its turn points and passes.\n";
                    cTurnScore = 0;
                    break;
                }
                else {
                    cTurnScore += roll;
                    DisplayRoundInfo(player.score, pTurnScore, cpu.score, cTurnScore);
                }
            }
            else std::cout << "CPU chose to hold.\n";
        } while (cChoice == 1);
        cpu.score += cTurnScore;
        if (player.score > 100) playerWon = true;
        if (cpu.score > 100) CPUWon = true;
    } while (!playerWon && !CPUWon);

    if (player.score > *hiScore) *hiScore = player.score;
    if (cpu.score > *hiScore) *hiScore = cpu.score;

    if (playerWon) return "Player";
    if (CPUWon) return "CPU";
    return "Error";
}

// Specification A1 - D6() function
// Wrapper function for RandRange which only rolls between 1 and 6
int D6(){
    Timer t("D6");
    return RandomNumber(1, 6);
}

// Specification A2 - RandomNumber() function
// Generate a random number between two bounds
int RandomNumber(int lo, int hi){
    Timer t("RandomNumber");
    return (rand() % (hi - lo + 1)) + lo;
}

// Specification B1 - Display Turn Stats
// Display the player and CPU turn and total scores
void DisplayRoundInfo(int pScore, int pTurnScore, int cScore, int cTurnScore){
    Timer t("DisplayRoundInfo");
    std::cout << "Player's total score: " << pScore << '\n';
    std::cout << "Player's turn score: " << pTurnScore << '\n';
    std::cout << "CPU's total score: " << cScore << '\n';
    std::cout << "CPU's turn score: " << cTurnScore << '\n';
    std::cout << std::endl;
}


// Specification C3 - Numeric Menu
// returns the player's choice from the menu.
// 1 = Roll, 2 = Hold, 3 = Quit
int DisplayMenu(){
    Timer t("DisplayMenu");
    int userChoice;
    bool validInput = true;
    do {
        validInput = true;
    std::cout << "Make your move:\n";
    std::cout<< "1. Roll\n2. Hold\n 3. Quit\n\n";
    std::cin >> userChoice;
    if (userChoice < 1 || userChoice > 3){
        std::cout << "Sorry, please enter a response in the range 1-3.\n";
        validInput = false;
    }
    } while (!validInput);
    return userChoice;
}
int display_stack(Stack s){
    Timer t = Timer("display_stack");
    std::cout << "Win Log:\n";
    for (int i = 0; i <= s.top; i++){
        std::cout << s.peek(i) << '\n';
    }
    return 0;
}

// Greet the user to program initialization
void ProgramGreeting(){
    Timer t("ProgramGreeting");
    std::cout << "Pig.cpp by Gavin Williams\n";
    std::cout << "Written August 26, 2024.\n\n";
}


// Commented Sample Run
/*
Pig.cpp by Gavin Williams
Written August 26, 2024.

What is your first and last name?
gavin willias
Player's total score: 0
Player's turn score: 0
CPU's total score: 0
CPU's turn score: 0

Your turn!
Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 1! You lose your points this turn and pass the turn.

CPU turn!
Thinking...
CPU chose to roll!
Rolling...
CPU rolled a 1
It loses its turn points and passes.
Player's total score: 0
Player's turn score: 0
CPU's total score: 0
CPU's turn score: 0

Your turn!
Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 1! You lose your points this turn and pass the turn.

CPU turn!
Thinking...
CPU chose to hold.
Player's total score: 0
Player's turn score: 0
CPU's total score: 0
CPU's turn score: 0

Your turn!
Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 5
Player's total score: 0
Player's turn score: 5
CPU's total score: 0
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 1! You lose your points this turn and pass the turn.

CPU turn!
Thinking...
CPU chose to hold.
Player's total score: 0
Player's turn score: 0
CPU's total score: 0
CPU's turn score: 0

Your turn!
Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 2
Player's total score: 0
Player's turn score: 2
CPU's total score: 0
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 5
Player's total score: 0
Player's turn score: 7
CPU's total score: 0
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 5
Player's total score: 0
Player's turn score: 12
CPU's total score: 0
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
2
CPU turn!
Thinking...
CPU chose to hold.
Player's total score: 12
Player's turn score: 0
CPU's total score: 0
CPU's turn score: 0

Your turn!
Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 4
Player's total score: 12
Player's turn score: 4
CPU's total score: 0
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 2
Player's total score: 12
Player's turn score: 6
CPU's total score: 0
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
2
CPU turn!
Thinking...
CPU chose to hold.
Player's total score: 18
Player's turn score: 0
CPU's total score: 0
CPU's turn score: 0

Your turn!
Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 4
Player's total score: 18
Player's turn score: 4
CPU's total score: 0
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 1! You lose your points this turn and pass the turn.

CPU turn!
Thinking...
CPU chose to roll!
Rolling...
CPU rolled a 2
Player's total score: 18
Player's turn score: 0
CPU's total score: 0
CPU's turn score: 2

CPU chose to hold.
Player's total score: 18
Player's turn score: 0
CPU's total score: 2
CPU's turn score: 0

Your turn!
Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 2
Player's total score: 18
Player's turn score: 2
CPU's total score: 2
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 1! You lose your points this turn and pass the turn.

CPU turn!
Thinking...
CPU chose to hold.
Player's total score: 18
Player's turn score: 0
CPU's total score: 2
CPU's turn score: 0

Your turn!
Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 1! You lose your points this turn and pass the turn.

CPU turn!
Thinking...
CPU chose to roll!
Rolling...
CPU rolled a 6
Player's total score: 18
Player's turn score: 0
CPU's total score: 2
CPU's turn score: 6

CPU chose to hold.
Player's total score: 18
Player's turn score: 0
CPU's total score: 8
CPU's turn score: 0

Your turn!
Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 6
Player's total score: 18
Player's turn score: 6
CPU's total score: 8
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 5
Player's total score: 18
Player's turn score: 11
CPU's total score: 8
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
2
CPU turn!
Thinking...
CPU chose to hold.
Player's total score: 29
Player's turn score: 0
CPU's total score: 8
CPU's turn score: 0

Your turn!
Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 5
Player's total score: 29
Player's turn score: 5
CPU's total score: 8
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 5
Player's total score: 29
Player's turn score: 10
CPU's total score: 8
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
2
CPU turn!
Thinking...
CPU chose to hold.
Player's total score: 39
Player's turn score: 0
CPU's total score: 8
CPU's turn score: 0

Your turn!
Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 3
Player's total score: 39
Player's turn score: 3
CPU's total score: 8
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 3
Player's total score: 39
Player's turn score: 6
CPU's total score: 8
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 2
Player's total score: 39
Player's turn score: 8
CPU's total score: 8
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
2
CPU turn!
Thinking...
CPU chose to roll!
Rolling...
CPU rolled a 3
Player's total score: 47
Player's turn score: 8
CPU's total score: 8
CPU's turn score: 3

CPU chose to hold.
Player's total score: 47
Player's turn score: 0
CPU's total score: 11
CPU's turn score: 0

Your turn!
Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 1! You lose your points this turn and pass the turn.

CPU turn!
Thinking...
CPU chose to hold.
Player's total score: 47
Player's turn score: 0
CPU's total score: 11
CPU's turn score: 0

Your turn!
Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 3
Player's total score: 47
Player's turn score: 3
CPU's total score: 11
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 5
Player's total score: 47
Player's turn score: 8
CPU's total score: 11
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 2
Player's total score: 47
Player's turn score: 10
CPU's total score: 11
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
2
CPU turn!
Thinking...
CPU chose to hold.
Player's total score: 57
Player's turn score: 0
CPU's total score: 11
CPU's turn score: 0

Your turn!
Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 2
Player's total score: 57
Player's turn score: 2
CPU's total score: 11
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 4
Player's total score: 57
Player's turn score: 6
CPU's total score: 11
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 5
Player's total score: 57
Player's turn score: 11
CPU's total score: 11
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
2
CPU turn!
Thinking...
CPU chose to roll!
Rolling...
CPU rolled a 5
Player's total score: 68
Player's turn score: 11
CPU's total score: 11
CPU's turn score: 5

CPU chose to hold.
Player's total score: 68
Player's turn score: 0
CPU's total score: 16
CPU's turn score: 0

Your turn!
Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 2
Player's total score: 68
Player's turn score: 2
CPU's total score: 16
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 5
Player's total score: 68
Player's turn score: 7
CPU's total score: 16
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 4
Player's total score: 68
Player's turn score: 11
CPU's total score: 16
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
2
CPU turn!
Thinking...
CPU chose to roll!
Rolling...
CPU rolled a 6
Player's total score: 79
Player's turn score: 11
CPU's total score: 16
CPU's turn score: 6

CPU chose to roll!
Rolling...
CPU rolled a 6
Player's total score: 79
Player's turn score: 11
CPU's total score: 16
CPU's turn score: 12

CPU chose to roll!
Rolling...
CPU rolled a 5
Player's total score: 79
Player's turn score: 11
CPU's total score: 16
CPU's turn score: 17

CPU chose to roll!
Rolling...
CPU rolled a 2
Player's total score: 79
Player's turn score: 11
CPU's total score: 16
CPU's turn score: 19

CPU chose to roll!
Rolling...
CPU rolled a 6
Player's total score: 79
Player's turn score: 11
CPU's total score: 16
CPU's turn score: 25

CPU chose to roll!
Rolling...
CPU rolled a 4
Player's total score: 79
Player's turn score: 11
CPU's total score: 16
CPU's turn score: 29

CPU chose to roll!
Rolling...
CPU rolled a 3
Player's total score: 79
Player's turn score: 11
CPU's total score: 16
CPU's turn score: 32

CPU chose to hold.
Player's total score: 79
Player's turn score: 0
CPU's total score: 48
CPU's turn score: 0

Your turn!
Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 6
Player's total score: 79
Player's turn score: 6
CPU's total score: 48
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 3
Player's total score: 79
Player's turn score: 9
CPU's total score: 48
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 3
Player's total score: 79
Player's turn score: 12
CPU's total score: 48
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
2
CPU turn!
Thinking...
CPU chose to hold.
Player's total score: 91
Player's turn score: 0
CPU's total score: 48
CPU's turn score: 0

Your turn!
Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 6
Player's total score: 91
Player's turn score: 6
CPU's total score: 48
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
1
Rolling...
You rolled a 5
Player's total score: 91
Player's turn score: 11
CPU's total score: 48
CPU's turn score: 0

Please choose whether to:
1: ROLL
2: HOLD
2
CPU turn!
Thinking...
CPU chose to hold.
Would you like to play another game? (Y/N)
Choose 'S' to see score history.
s
Win Log:
Player
Please enter 'Y' or 'N'
Would you like to play another game? (Y/N)
Choose 'S' to see score history.
n
Across all games, high score was 102
Thanks for playing, gavin willias, have a nice day!
*/