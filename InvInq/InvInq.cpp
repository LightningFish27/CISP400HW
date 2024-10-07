// InvInq.cpp
// CISP 400
// Gavin Williams, written 9/16/24

#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iomanip>


void InVal(int&, int&, float&);
void ProgramGreeting();
void display_menu();
char get_menu_choice();
void UnitTest();


// Specification B3 - Logger Class
/*
    Logger is a class which abstracts away the intricacies of generating log
    files to a single instantiation per function. It keeps track of the start,
    end, and elapsed time, as well as the depth of the "stack trace."
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
    void ComponentTest(){
        std::cout << "Logger logging a log of Logger initialization to std::clog.\n";
        Logger l = Logger("Logger component Test");
        std::cout << "Logging of logger component test logged.\n";
    }
};
int Logger::depth = 0; // Init depth

// Specification C2 - Dynamic Array
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
    // Specification C3 - Resize Array
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

// Specification B1 - Date class
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

// Specification B2 -RandNo Class
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

class Item{
private:
    int IDNumber;
    int qtyOnHand;
    float wholesaleCost;
    float retailCost;
    Date dateAdded;
public:
    Item(){
        Logger l = Logger("Item default constructor");
    }
    Item(int ID, int qty, float incomingWholesaleCost){
        Logger l = Logger("Item Constructor");
        IDNumber = ID;
        qtyOnHand = qty;
        wholesaleCost = incomingWholesaleCost;
        retailCost = wholesaleCost * RandNo::get_instance().random_float(1.1, 1.5);
        dateAdded = Date();
    }

    // Specification A2 - Overload operator«
    friend std::ostream& operator<<(std::ostream& os, Item& item){
        os << item.IDNumber << "\t\t" << item.qtyOnHand << '\t' << item.wholesaleCost << "\t\t" << item.retailCost << '\t' << item.dateAdded.get_date();
        return os;
    }

    int get_ID() { return IDNumber; };

    void ComponentTest(){
        std::cout << "Beginning component testing of Item class.\n";
        Item i = Item(12345, 123, 123.00);
        std::cout << "Item values should be: 12345, 123, 123.00\n";
        std::cout << i << '\n';
        if (i.get_ID() != 12345) std::cout << "ID #s don't match.\n";
        else std::cout << "Item ID#s match.\n";
        std::cout << "Ending component testing of item Class.\n";
    }
};

int main(){
    std::ofstream fileOut("log.txt"); // Opening log file
    std::clog.rdbuf(fileOut.rdbuf()); // Redirecting clog to write to log file
    Logger l = Logger("main");
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Beginning with Unit Testing:\n\n";
    UnitTest();


    ProgramGreeting();

    G_Array<Item> database;

    bool programRunning = true;
    char choice;
    do {
        std::cout << "MAIN MENU:\n";
        choice = get_menu_choice();

        if (choice == 'A'){ // Add Item
            std::cout << "Adding item\n";
            int IDNumber;
            int qtyOnHand;
            float wholesaleCost;
            InVal(IDNumber, qtyOnHand, wholesaleCost);
            database.add_element(Item(IDNumber, qtyOnHand, wholesaleCost));
        }
        else if (choice == 'D'){ // Delete Item
            std::cout << "Deleting item\n";
            if (database.length() == 0){
                std::cout << "No items in database.\n";
            }
            else {
                database.remove_element();
                std::cout << "Removed last element.\n";
            }
        }
        // Specification A1 - Edit Inventory
        else if (choice == 'E'){
            std::cout << "Editing item\n";
            int desiredID = 00000;
            std::cout << "Enter ID of item you wish to edit: ";
            std::cin >> desiredID;
            int index = -1;
            for (int i = 0; i < database.length(); i++){
                if (database[i].get_ID() == desiredID){
                    std::cout << "Requested item found.";
                    index = i;
                }
            }
            if (index == -1){ std::cout << "Requested ID not found in database.\n"; }
            else {
                std::cout << "Enter the new values for the item you are editing:\n";
                int IDNumber;
                int qtyOnHand;
                float wholesaleCost;
                InVal(IDNumber, qtyOnHand, wholesaleCost);
                database[index] = Item(IDNumber, qtyOnHand, wholesaleCost);
            }
        }
        else if (choice == 'P'){
            std::cout << "Printing items\n";
            std::cout << "IDNumber\tQty\tWholesale\tRetail\tDate Added\n";
            for (int i = 0; i < database.length(); i++){
                std::cout << database[i] << '\n';
            }
        }
        else if (choice == 'Q'){
            std::cout << "Quitting Program\n";
            programRunning = false;
        }
        else {
            std::cout << "Input not recognized.\n";
        }
    } while(programRunning);

    // fileOut.close(); // Closing fileOut before program ends causes the end of
    // function main to not be logged. File is closed automatically when program
    // ends, so it's okay to not close it explicitly.
    return 0;
}

void ProgramGreeting(){
    Logger l = Logger("ProgramGreeting");
    std::cout << "Wecome to Universal Inventory Protocol V2,\n";
    std::cout << "The solution to the many competing universal standards for ";
    std::cout << "inventory management.\n";
    std::cout << "Written by Gavin Williams for CISP 400, 9/16/24\n";
}

// Specification B4 - Inventory Entry Input Validation
void InVal( int& ID, int& qty, float& wholesaleCost){
    Logger l = Logger("InVal");
    bool inputValid;
    do{
        std::cout << "Enter item's ID: ";
        std::cin >> ID;
        std::cout << "Enter Quantity on hand: ";
        std::cin >> qty;
        std::cout << "Enter wholesale cost: ";
        std::cin >> wholesaleCost;
        int IDLength = std::to_string(ID).length();
        inputValid = true;
        if (IDLength < 5){
            std::cout << "ID length should be 5 digits. Too short.\n";
            inputValid = false;
        }
        if (IDLength > 5){
            std::cout << "ID length should be 5 digits. Too long.\n";
            inputValid = false;
        }
        if (qty < 0) {
            std::cout << "Quantity cannot be negative. Maybe fix your piece count.\n";
            inputValid = false;
        }
        if (wholesaleCost < 0.0f) {
            std::cout << "Wholesale Cost cannot be negative.\n";
            inputValid = false;
        }
    }while (!inputValid);
}

// Specification C1 - Alpha Menu
char get_menu_choice(){
    Logger l = Logger("get_menu_choice");
    bool inputValid;
    char c;
    do {
        inputValid = true;
        display_menu();
        std::cin >> c;
        c = toupper(c);
        // Specification C4 - Menu Input Validation
        if (!(c == 'A' || c == 'D' || c == 'E' || c == 'P' || c == 'Q')){
            std::cout << "Please choose by entering the first letter of an above option.\n";
            inputValid = false;
        }
    } while (!inputValid);
    return c;
}

void display_menu(){
    Logger l = Logger("display_menu");
    std::cout << "<A>dd Item\n";
    std::cout << "<D>elete Item\n";
    std::cout << "<E>dit Item\n";
    std::cout << "<P>rint Items\n";
    std::cout << "<Q>uit Program\n";
}

void UnitTest(){
    Date date = Date();
    date.CompTest();
    RandNo::get_instance().ComponentTest();
    G_Array<int> testArr;
    testArr.ComponentTest();
    Item i = Item();
    i.ComponentTest();
}