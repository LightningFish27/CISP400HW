// Hokeeman.cpp
// CISP 400
// Gavin Williams, Written Oct 7, 2024

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <iomanip>
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
    T* array = new T[0];
    size_t size = 0;
public:
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
    void remove_element(){
        Logger l = Logger("remove_element");
        size--;
        T* pTmp = new T[size];
        for (int i = 0; i < size; i++){
            pTmp[i] = array[i];
        }
        delete[] array;
        array = pTmp;
    }
    // Remove a specified array element
    // Returns false if removal was unsuccessful
    bool remove_element(T target){
        int targetIndex = -1;
        for (int i = 0; i < size; i++){
            if (array[i] == target) targetIndex = i;
        }
        if (targetIndex == -1) return false;
        // Following code executes if target is found:
        G_Array tempArr;
        for(int i = 0; i < size; i++){
            if (i != targetIndex) tempArr.add_element(array[i]);
        }
        for (int i = 0; i < tempArr.length(); i++){
            array[i] = tempArr[i];
        }
        size--;
        return true;
    }
    // Removes a specified index from array
    // Returns false if index out of bounds
    bool remove_element(int index){
        if (index < 0 || index > length()){
            std::cout << "Attempted to remove index not within array.\n";
            return false;
        }
        G_Array tempArr;
        for(int i = 0; i < size; i++){
            if (i != index) tempArr.add_element(array[i]);
        }
        for (int i = 0; i < tempArr.length(); i++){
            array[i] = tempArr[i];
        }
        // it seems dumb to call remove_element within remove element,
        // but it's a good way to avoid rewriting code already within that function.
        // I may still change this later.
        array.remove_element(); 
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
    // Specification A3 - Overload operator»
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
    void setDate(std::string inputDate){
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
/*
    A static class that prints colored text.
*/
class FancyText {
private:
    static inline std::string RED = "\e[31m";
    static inline std::string GREEN = "\e[32m";
    static inline std::string YELLOW = "\e[33m";
    static inline std::string NORMAL = "\e[0m";
public:
    /*
        Print the given text in a color provided by FancyText. Use the color's
        name's first character to select a color. If a newline is needed after
        what is being printed, pass a boolean true as the third parameter.
        Newline defaults to false, color defaults to white.
    */
    static void print_colored(std::string text, char col = '\0', bool newline = false){
        Logger l = Logger("print_colored");
        std::string color;
        if (col == 'r') color = RED;
        else if (col == 'y') color = YELLOW;
        else if (col == 'g') color = GREEN;
        else color = NORMAL;
        std::cout << color << text << NORMAL << newline ? '\n' : '\0';
    }
};

/*
    A basic implementation of a dictionary that supports key-value pairs between
    a string (always) and another type. The second type is templated.
    I may eventually support non-string keys, but I don't want to implement that yet.
*/
template <typename T>
class G_Dictionary {
private:
    // These are parallel arrays, and special attention is paid to them in order
    // to avoid any indexing errors.
    G_Array<std::string> keys;
    G_Array<T> values;
public:
    // Overloading the [] operator to allow accessing a value in the style
    // Dictionary[key] 
    T& operator[](std::string key){
        Logger l = Logger("[] operator within Dictionary");
        int keyIndex = -1;
        for (int i = 0; i < keys.length(); i++){
            if (keys[i] == key) keyIndex = i;
        }
        if (keyIndex == -1){
            std::cout << "Requested key not found within dictionary.\n";
        }
        else return values[keyIndex];
    }

    size_t length(){
        Logger l = Logger("length");
        return keys.length();
    }

    // Add pair
    void add_pair(std::string key, T value){
        keys.add_element(key);
        values.add_element(value);
    }

    // Remove pair
    void remove_pair(std::string key){
        int keyIndex = -1;
        for (int i = 0; i < keys.length(); i++){
            if (keys[i] == key) keyIndex = i;
        }
        if (keyIndex == -1) {
            std::cout << "Couldn't remove " << key << ": key not found.\n";
        }
        else {
            keys.remove_element(keyIndex);
            values.remove_element(keyIndex);
            std::cout << "Removed " << key << '\n';
        }
    }

    std::ostream& operator<<(std::ostream& os){
        for (int i = 0; i < keys.length(); i++){
            std::cout << keys[i] << ',' << values[i] << '\t';
        }
        return os;
    }
};

template<typename T>
struct Node {
    T data;
    Node* next;
};
template<typename T>
class G_List{
    Node<T>* head;
    int length = 0;
public:
    G_List() : head(NULL){}

    // Insert a node at beginning of list
    void push_front(T value){
        Node<T>* newNode = new Node<T>();
        newNode->data = value;
        newNode->next = head;
        head = newNode;
        length++;
    }

    // Insert a node at end of list
    void push_back(T value){
        Node<T>* newNode = new Node<T>();
        newNode->data = value;
        newNode->next = NULL;
        if (!head) { // Set new node as head if list is empty
            head = newNode;
            return;
        }
        // Traverse to find current last node
        Node<T>* temp = head;
        while (temp->next){
            temp = temp->next;
        }
        temp->next = newNode; // Set new node following current last node
        length++;
    }
    
    // Insert at a specified position
    void insert(T value, int position){
        if (position < 1){
            std::cout << "Position should be >= 1.\n";
            return;
        }
        if (position == 1){
            this.push_front(value);
            return;
        }

        Node<T>* newNode = new Node<T>();
        newNode->data = value;
        // Traverse to specified position
        Node<T>* temp = head;
        for (int i = 1; i < position - 1 && temp; ++i) {
            temp = temp->next;
        }
        // Return an error if specified position is beyond the end of the list.
        if (!temp) {
            std::cout << "Position out of range.\n";
            delete newNode;
            return;
        }
        // Insert the new node at the desired position
        newNode->next = temp->next;
        temp->next = newNode;
        length++;
    }
    // Return a pointer to the head of the list and remove that node from the list
    T pop_front(){
        if (length < 1) {
            std::cout << "List is empty.\n";
            return;
        }

        Node<T>* temp = head; 
        head = head->next; 
        return temp; 
        length--;
    }
    // Returns a pointer to last element in list and removes it from the list
    T pop(){
        if (length < 1) {
            std::cout << "List is empty.\n";
            return;
        }
        if (!head->next) {
            delete head;   
            head = NULL;   
            return;
        }
        // Traverse to the second-to-last node
        Node<T>* temp = head;
        while (temp->next->next) {
            temp = temp->next;
        }
        return temp->next; 
        temp->next = NULL; 
        length--;
    }
    // Removes the last element of the list from the list.
    void delete_back(){
        if (length < 1) {
            std::cout << "List is empty.\n";
            return;
        }
        if (!head->next) {
            delete head;   
            head = NULL;   
            return;
        }
        // Traverse to the second-to-last node
        Node<T>* temp = head;
        while (temp->next->next) {
            temp = temp->next;
        }
        delete temp->next; 
        temp->next = NULL; 
        length--;
    }
    // Delete the head of the list
    void delete_front() {
        if (!head) {
            std::cout << "List is empty.\n";
            return;
        }

        Node<T>* temp = head; 
        head = head->next; 
        delete temp;
        length--;  
    }
};


/*
    THE FOLLOWING CLASSES AND DATA ARE DESIGNED FOR USE IN THIS PROGRAM
*/

// Mood data for hokeeman
enum MOODS {
    ANGRY = -2,
    UNHAPPY = -1,
    NEUTRAL = 0,
    HAPPY = 1,
    ECSTATIC = 2
};
std::string MOODSARR = std::string[5];
MOODSARR[0] = "angry";
MOODSARR[1] = "unhappy";
MOODSARR[2] = "neutral";
MOODSARR[3] = "happy";
MOODSARR[4] = "ecstatic";

class Creature {
private:
    int mood, boredom, hunger;
    std::string name;
public:
    void set_name(std::string newName){
        name = newName;
    }
    std::string get_name(){
        return name;
    }
    // Specification C4 - Overload «
    std::ostream& operator<<(std::ostream& os){
        std::cout << name << " is feeling " << MOODSARR[mood] << " and has hunger " << hunger << " and boredom " << boredom << '\n';
        return os;
    }
}

// FUNCTION PROTOTYPES
void program_greeting();


int main(){
    // SETUP
    std::ofstream fileOut("log.txt"); // Opening log file
    std::clog.rdbuf(fileOut.rdbuf()); // Redirecting clog to write to log file
    Logger l = Logger("main");
    std::cout << std::fixed << std::setprecision(2);
    program_greeting();

}

void program_greeting(){
    std::cout << "Welcome to Hokeeman.cpp by Gavin Williams, written Oct 7, 2024.\n";
    std::cout << "You will be able to interact with your hokeemen through a variety of methods,\n";
    std::cout << "much like a tomodachi. Enter the command 'help' to see all the things you can do.\n\n";
    std::cout << "Remember-- your Hokeeman loves you, and doesn't like it when you close the game.\n";
}