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

class Date{
private:
    int day, month, year;
public:
    Date(){
        Logger l = Logger("Date Constructor");
        // Initialize with System Date
        // Get the system time as a time_point
        auto now = std::chrono::system_clock::now();
        // Convert the time_point to a time_t
        std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
        // Convert to local time
        std::tm now_tm = *std::localtime(&now_time_t);
        // Extract the day, month, and year
        day = now_tm.tm_mday;
        month = now_tm.tm_mon + 1; // tm_mon is 0-based
        year = now_tm.tm_year + 1900; // tm_year is years since 1900
    }
    // Return the date as a string
    std::string get_date(){
        Logger l = Logger("get_date");
        std::stringstream s;
        s << day << '/' << month << '/' << year;
        return s.str();
    }

    void ComponentTest(){
        std::cout << "Comparing system-acquired year to developer-set year.\n";
        std::cout << "Actual year: 2024 -- System year: " << year << ".\n";
        if (year != 2024) std::cout << "Years do not match! Error.\n";
        else std::cout << "Date Component testing successful.\n";
    }
};

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