/*
    G_STL is my personal version of the STL, for use in CISP 400. It includes a 
    variety of helpful objects, functions, etc. Notable members are:
        - Random Generators
        - Stack
        - Dynamic Array
        - Logger
    Although clearly derived from the philosophies and design of the STL, 
    G_STL is written by me, Gavin Williams, for use in this class
*/


#include <iostream>
#include <chrono>
#include <string>
#include <iomanip>
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
    static const std::string RED = "\e[31m";
    static const std::string GREEN = "\e[32m";
    static const std::string YELLOW = "\e[33m";
    static const std::string NORMAL = "\e[0m";
public:
    /*
        Print the given text in a color provided by FancyText. Use the color's
        name's first character to select a color. If a newline is needed after
        what is being printed, pass a boolean true as the third parameter.
        Newline defaults to false, color defaults to white.
    */
    static void print_colored(std::string text, char col = NORMAL, bool newline = false){
        Logger l = Logger("print_colored");
        std::string color;
        if (col == 'r') color = RED;
        else if (col == 'y') color = YELLOW;
        else if (col == 'g') color = GREEN;
        else color = NORMAL;
        std::cout << color << text << NORMAL << newline ? '\n' : '';
    }
};