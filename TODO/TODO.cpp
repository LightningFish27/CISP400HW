// TODO.cpp
// 09/18/24
// Gavin Williams

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

void ProgramGreeting();
bool file_exists(std::string);
bool validate_input(std::string);
void unit_test();

/*
    Logger provides abstracted logging functionality, handling both the
    beginning and ending of a function call with a single line per function,
    and indenting the log file in relation to the depth of the stack.
*/
class Logger{
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
    T* array = new T[0];
    size_t size = 0;
public:

    ~G_Array() {
        delete[] array;
    }
    // Adding to the array is done through the add_element function, which
    // abstracts away all the resizing done.
    void add_element(T newElement){
        Logger l = Logger("add_element");
        T* pTmp = new T[size + 1];
        for (int i = 0; i < size; ++i){
            pTmp[i] = array[i];
        }
        pTmp[size] = newElement;
        delete[] array;
        array = pTmp;
        size++;
    }
    // Remove the last element of the array
    void remove_element(){
        Logger l = Logger("remove_element");
        size--;
        T* pTmp = new T[size];
        for (int i = 0; i < size; i++){
            pTmp[i] = array[i];
        }
        delete[] array;
        array = new T[size];
        for (int i = 0; i < size; i++){
            array[i] = pTmp[i];
        }
        delete[] pTmp;
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
        Logger l = Logger("G_Array ComponentTest");
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
    A simple class which represents a date and provides methods for interacting with it.
*/
class Date{
private:
    int day, month, year;
public:
    // Initialize with System Date if none specified
    Date(){
        Logger l = Logger ("Date Constructor");
        auto now = std::chrono::system_clock::now(); // Get the system time as a time_point
        std::time_t now_time_t = std::chrono::system_clock::to_time_t(now); // Convert the time_point to a time_t
        std::tm now_tm = *std::localtime(&now_time_t);// Convert to local time
        // Extract the day, month, and year
        day = now_tm.tm_mday;
        month = now_tm.tm_mon + 1; // tm_mon is 0-based
        year = now_tm.tm_year + 1900; // tm_year is years since 1900
    }
    // Initialize with specified date
    Date(int day, int month, int year){
        Logger l = Logger("SetDate");
        day = day;
        month = month;
        year = year;
    }
    // Allow manual date overriding by passing the day, month, and year to this function
    void set_date(int day, int month, int year){
        Logger l = Logger("SetDate");
        day = day;
        month = month;
        year = year;
    }
    // Allow setting the date with a formatted string split at '/' characters.
    void set_date(std::string inputDate){
        Logger l = Logger ("date setDate");
        std::istringstream dateStream(inputDate);
        std::string dayIn, monthIn, yearIn;
        std::getline(dateStream, dayIn, '/');
        std::getline(dateStream, monthIn, '/');
        std::getline(dateStream, yearIn, '/');
        day = std::stoi(dayIn);
        month = std::stoi(monthIn);
        year = std::stoi(yearIn);
    }
    // Return the date as a string
    std::string get_date(){
        Logger l = Logger("get_date");
        std::stringstream s;
        s << std::setfill('0');
        s << std::setw(2) << day << '/' << std::setw(2) << month << '/' << year;
        return s.str();
    }
    // Self-diagnostics of the time class's functionality
    void CompTest(){
        Logger l = Logger("CompTest");
        std::cout << "Beginning CompTest initialization diagnostics.\n";
        Date testDate;
        std:: cout << "CompTest result: " << testDate.get_date() << "vs Self result: " << get_date() << '\n';
        if (testDate.day != day)
            std::cout << "Days are not equal.\n";
        if (testDate.month != month)
            std::cout << "Months are not equal.\n";
        if (testDate.year != year)
            std::cout << "Years are not equal.\n";
        if (testDate.get_date() != get_date())
            std::cout << "Formatting or components do not match.\n";

        std::cout << "\nNow testing set_date functionality:\n";
        testDate.set_date(day, month, year);
        std:: cout << "CompTest result: " << testDate.get_date() << "vs Self result: " << get_date() << '\n';
        if (testDate.day != day)
            std::cout << "Days are not equal.\n";
        if (testDate.month != month)
            std::cout << "Months are not equal.\n";
        if (testDate.year != year)
            std::cout << "Years are not equal.\n";
        if (testDate.get_date() != get_date())
            std::cout << "Formatting or components do not match.\n";
        std::cout << "CompTest finished diagnosis. Program beginning.\n";

        std::cout << "Finally, testing year accuracy.\n";
        if (year != 2024) std::cout << "Year is inaccurate!\n";
        else std::cout << "Year is accurate.\n";
    }
};

// A TODO list entry. Contains an ID, the date added, and the todo description.
// immutable struct type, as todos are only added or deleted, never edited.
struct TODO {
    int ID; // IDs are not stored in the savefile, as they are run-specific.
    Date dateAdded;
    std::string description;
    TODO(int id, Date date, const std::string& desc) : ID(id), dateAdded(date), description(desc) {}
    // Specification A4 - Overload Constructor
    /*
        Note that most of the instructions included within Spec A4 are within the main
        function while getting input. If the user enters only the '+' command, and 
        no arguments, a dummy is created. Otherwise, a real TODO is made
        I put the Specification here because it made more sense given the *name* of the
        Spec, even if not most of the contents. It also allows my TODO struct to hold 
        5 Specs, which is fun.
    */
    TODO(){
        ID = -2;
        dateAdded = Date(1,1,1111);
        description = "a dummy todo";
    }
    // Specification A1 - Overload Copy Constructor
    TODO(TODO& otherTODO) : ID(otherTODO.ID), description(otherTODO.description), dateAdded(otherTODO.dateAdded) {}
    // Specification A2 - Overload Assignment Operator
    TODO& operator=(const TODO& otherTODO) {
        if (this != &otherTODO) {
            ID = otherTODO.ID;
            description = otherTODO.description;
            dateAdded = otherTODO.dateAdded;
        }
        return *this;
    }
    // Specification C1 - Overload <<
    friend std::ostream& operator<<(std::ostream& os, TODO& todo){
        os << todo.ID << '\t' << todo.description << '\t' << todo.dateAdded.get_date() ;
        return os;
    }
    // Specification C2 - Overload >>
    friend std::istream& operator>>(std::istream& is, TODO& todo){
        return is;
    }

    void ComponentTest(){
        std::cout << "Component Testing the TODO struct.\n";
        TODO dummy = TODO();
        std::cout << "A dummy TODO: " << dummy << '\n';
        TODO test = TODO(2, Date(), "A test TODO");
        std::cout << "A test TODO: " << test << '\n';
        std::cout << "Testing assignment operator with dummy and test.\n";
        test = dummy;
        std::cout << "New values of test: " << test << '\n';
        // Specification C3 - Test TODO’s
        std::cout << "Now creating 5 unique TODOs to see the general format.\n";
        TODO t0 = TODO(1, Date(), "First test TODO.");
        TODO t1 = TODO(2, Date(), "Second test TODO.");
        TODO t2 = TODO(3, Date(), "Third test TODO.");
        TODO t3 = TODO(4, Date(), "Fourth test TODO.");
        TODO t4 = TODO(5, Date(), "Fifth test TODO.");
        std::cout << t0 << '\n' << t1 << '\n' << t2 << '\n' << t3 << '\n' << t4 << '\n' << '\n';
        std::cout << "Finished component testing of TODO object.\n";
    }
};

int main(){
    // LOGFILE SETUP //
    std::ofstream fileOut("log.txt"); // Opening log file
    std::clog.rdbuf(fileOut.rdbuf()); // Redirecting clog to write to log file
    Logger l = Logger("main");

    // UNIT TESTING //
    unit_test();

    // PROGRAM BEGINS //
    ProgramGreeting();
    //Specification C4 - TODO array
    // initialize todoList array
    G_Array<TODO> todoListArr = G_Array<TODO>();
    int TODOsAdded = 0;
    // check for existing todoList.txt file. If one exists, load it into memory
    std::string fileName = "todoList.txt";
    if (file_exists(fileName)){ 
        std::cout << "Found todoList.txt. Loading data into list memory structure.\n";
        std::ifstream file(fileName);
        std::string line;
        while (std::getline(file, line)){ // For each line in input file
            std::istringstream stream(line); // Create a stream from the line
            TODO newTODO;

            std::string description, dateAdded; // define each component
            //std::getline(stream, idstr, '~'); // Split at tildes into ID
            std::getline(stream, description, '~'); // Split at tildes into description
            std::getline(stream, dateAdded, '~'); // Split at tildes into dateAdded

            Date newDate = Date();
            newDate.set_date(dateAdded);

            //newTODO.ID = std::stoi(idstr); // Convert ID into int and store in newTODO
            newTODO.ID = TODOsAdded;
            TODOsAdded++;
            newTODO.description = description; // store description in newTODO
            newTODO.dateAdded = newDate; // Convert ID into int and store in newTODO
            todoListArr.add_element(newTODO); // Add newTODO into the array of TODOs
        }
        std::cout << "Added all TODOs from savefile to list.\n";
        file.close();
    } else {
        std::cout << "No existing todoList.txt file found. A new one will be created.\n";
        std::cout << "At program close.\n";
    }
    // begin programRunning loop execution.
    bool programRunning = true;
    do{
        std::string userInput;
        // Get user input
        bool inputValid;
        do {
            inputValid = true;
            std::cout << "Enter a command: ";
            std::getline(std::cin, userInput);
            inputValid = validate_input(userInput);
        } while (!inputValid);

        // Act on user input
        // Although splitting the description from the command would be cleaner,
        // this substring method is guaranteed to work based on the input format.
        // work smarter, not harder!
        // Specification B1 - + Symbol
        if (userInput[0] == '+'){
            if (userInput.length() < 3){
                std::cout << "Adding a dummy TODO.\n";
                TODO newTODO = TODO();
                todoListArr.add_element(newTODO);
            }
            else {
                std::cout << "Adding " << userInput.substr(2) << ".\n";
                // Specification A3 - System Date.
                TODO newTODO = TODO(TODOsAdded, Date(), userInput.substr(2));
                TODOsAdded++;
                todoListArr.add_element(newTODO);
            }
            
        }
        // Specification B3 - - symbol
        else if (userInput[0] == '-'){
            std::cout << "Removing " << userInput.substr(2) << ".\n";
            bool foundTODO = false;
            int foundAt = -1;
            for (int i = 0; i < todoListArr.length(); i++){
                if (todoListArr[i].description == userInput.substr(2)){
                    foundAt = i;
                }
            }
            if (foundAt != -1){
                std::cout << "Found requested TODO at " << foundAt <<'\n';
                // Instead of deleting TODOs from the array, replace them with
                // null TODOs that are ignored by the program.
                // This greatly simplifies the entire implementation process.
                // Although it *could* become a memory problem, no one on Earth will
                // be adding one million todos to this list.
                struct TODO nullTODO = TODO(-1, Date(0, 0, 0), "Null TODO");
                todoListArr[foundAt] = nullTODO;
            }
            else{
                std::cout << "Requested todo, '" << userInput.substr(2) << "' not found.\n";
            }
        }
        // Specification B2 - ? Symbol
        else if (userInput[0] == '?'){
            std::cout << "Displaying all TODOs...\n";
            std::cout << "ID\tDESCRIPTION\tDATE ADDED\n";
            std::cout << "-------------------------------------\n";
            for (int i = 0; i < todoListArr.length(); i++){
                if (todoListArr[i].ID != -1)
                    std::cout << todoListArr[i].ID << '\t' << todoListArr[i].description << '\t' << todoListArr[i].dateAdded.get_date() << '\n';
            }
        }
        else if (userInput[0] == 'q'){
            std::cout << "Quitting program.\n";
            programRunning = false;
        }
        // Cleanup loop if necessary
    } while (programRunning);

    // Specification B4 - Persistence
    // Save TODOlist to file.
    std::ofstream outFile(fileName);
    if(!outFile.is_open()){
        std::cout << "Problem opening output file.\n";
        return 1;
    }
    // Write all non-null TODOs line by line using ~ as a delimiter between fields.
    for (int i = 0; i < todoListArr.length(); i++){
        if (todoListArr[i].ID != -1)
            outFile << todoListArr[i].description << '~' << todoListArr[i].dateAdded.get_date() << '\n';
    }
    std::cout << "Finished adding all unresolved TODOs to output file todoList.txt.\n";
    std::cout << "Thank you for using TODO.cpp. May your days be productive.\n";
    outFile.close();
    return 0;
}

// Confirm existence of fileName.
bool file_exists(std::string fileName){
    Logger l = Logger("file_exists");
    std::ifstream file(fileName);
    return file.good();
}
// Confirm that user entered their command in the correct format.
bool validate_input(std::string userInput){
    Logger l = Logger("validate_input");
    bool isValid = true;
    if (!(userInput[0] == '+' || userInput[0] == '-' || userInput[0] == '?' || userInput[0] == 'q')){
        isValid = false;
        std::cout << "Start of command should be '+', '-', '?', or 'q'\n";
    }
    if (userInput.length() > 20){
        isValid = false;
        std::cout << "Entered command is too long.\n";
    }
    if (userInput.length() > 1 && userInput[1] != ' '){
        isValid = false;
        std::cout << "Commands other than ? and q should have a space after the command.\n";
    }
    // Further 'gauntlet' conditions can be added as necessary here
    return isValid;
}

void ProgramGreeting(){
    Logger l = Logger("ProgramGreeting");
    Date currentDate = Date();
    std::cout << "Welcome to TODO.cpp, a TODO list that can fill all your needs!\n";
    std::cout << "You can add tasks to your list and remove tasks from your list.\n";
    std::cout << "It even keeps track of your TODOs between runs!\n";
    std::cout << "Format commands as follows: ";
    std::cout << "'[command] [thing to do]', without the square braces.\n";
    std::cout << "For example, '+ do laundry', to add a 'do laundry' TODO.\n";
    std::cout << "To complete a TOTO, enter '- [thing to complete].\n";
    std::cout << "For example, '- do laundry'.\n";
    std::cout << "To view the TODO list, enter '?'\n";
    std::cout << "To quit, enter 'q'\n";
    std::cout << "Current date: " << currentDate.get_date() << "\n\n";
}       

void unit_test(){
    std::cout << "Beginning unit tests.\n";
    TODO testTODO = TODO();
    testTODO.ComponentTest();

    Date testDate = Date();
    testDate.CompTest();

    G_Array<int> testArr = G_Array<int>();
    testArr.ComponentTest();
}