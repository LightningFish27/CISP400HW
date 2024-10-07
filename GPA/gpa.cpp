// gpa.cpp
// Gavin Williams
// 09/09/24

#include <iostream>
#include <fstream>
#include <sstream>
#include <string> 
#include <chrono>
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

// Specification B1 - Dynamic Array
// Because GPA Analyzer specifications specify that the array should resize
// every time an element is added, that is the implementation used here.
template <typename T>
class G_Array{
private:
    T* array = new T[0];
    size_t size = 0;
public:
    //Specification B2 - Add Elements
    // Adding to the array is done through the add_element function, which
    // abstracts away all the resizing done.
    void add_element(T newElement){
        Logger l = Logger("add_element");
        size++;
        T* pTmp = new T[size];
        for (T i = 0; i < size; i++){
            pTmp[i] = array[i];
        }
        pTmp[size - 1] = newElement;
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
};

// Specification B4 - Highlight Failing Grades
/*
    FancyText objects allow easily printing colored lines to the console.
    You can either print an entire colored line, or only a colored section.
    Colors revert to normal after the function finishes.

    I feel a bit of shame that my two functions differ by only 5 characters,
    but it makes function calls simpler than having to include a boolean for 
    whether to newline at the end. I /could/ have used a default value argument,
    but still.
*/
class FancyText {
private:
    const std::string RED = "\e[31m";
    const std::string GREEN = "\e[32m";
    const std::string YELLOW = "\e[33m";
    const std::string NORMAL = "\e[0m";
public:
    void print_colored(std::string text, char col){
        Logger l = Logger("print_colored");
        std::string color;
        if (col == 'r') color = RED;
        else if (col == 'y') color = YELLOW;
        else if (col == 'g') color = GREEN;
        else color = NORMAL;
        std::cout << color << text << NORMAL;
    }
    void print_ln_colored(std::string text, char col = 'x'){
        Logger l = Logger("print_ln_colored");
        std::string color;
        if (col == 'r') color = RED;
        else if (col == 'y') color = YELLOW;
        else if (col == 'g') color = GREEN;
        else color = NORMAL;
        std::cout << color << text << NORMAL << '\n';
    }
};

// Specification A1 - Date class
class Date{
private:
    int day, month, year;
public:
    Date(){
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
    // Specification A2 - External date initialization
    void SetDate(int day, int month, int year){
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
    // Specification A3 - Component Test Method in Date
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
        testDate.SetDate(day, month, year);
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
    }
};

// Specification C1 - Three Functions 
int get_menu_choice();
void ProgramGreeting();
char Grade2Lttr(int);
void UnitTest();

int main(){
    // Specification C4 - Function Activity to Disk
    std::ofstream fileOut("log.txt"); // Opening log file
    std::clog.rdbuf(fileOut.rdbuf()); // Redirecting clog to write to log file
    Logger l = Logger("main");

    std::cout << "Beginning unit tests and initialization process.\n\n";
    Date date;
    std::cout << "COMPTEST:\n";
    date.CompTest();
    std::cout << "UNITTEST\n";
    UnitTest();


    ProgramGreeting();

    // FancyText object used for color printing
    FancyText printer = FancyText();

    G_Array<int> gradeArray;

    bool progRunning = true;
    do {
        // Display Menu and get user input
        int userChoice = get_menu_choice();
        // Act on input
        // Add an entry
        if (userChoice == 1){
            int grade;
            std::cout << "What grade would you like to add?\n";
            bool inputValid;
            do{
                inputValid = true;
                std::cin >> grade;
                if (grade < 0) {
                    std::cout << "Sorry, grade out of valid grade range.\n";
                    inputValid = false;
                }
            } while (!inputValid);
            gradeArray.add_element(grade);
        }
        // Specification C2 - Print Scores
        // Displays all entries
        else if (userChoice == 2){
            std::cout << "All current entries:\n";
            for (int i = 0; i < gradeArray.length(); i++){
                int grade = gradeArray[i];
                char lGrade = Grade2Lttr(grade);
                printer.print_ln_colored(std::string(1, lGrade),
                grade < 70 ? 'r' : grade < 80 ? 'y' : 'g');
            }
        }
        // Specification C3 - Compute GPA
        // Process GPA
        else if (userChoice == 3){
            std::cout << "Average GPA from entries:\n";
            int sum = 0;
            for (int i = 0; i < gradeArray.length(); i++){
                std::cout << gradeArray[i] << '\n';
                sum += gradeArray[i];
            }
            float avg = float(sum) / gradeArray.length();
            printer.print_ln_colored(std::to_string(avg),
                avg < 60 ? 'r' : avg < 80 ? 'y' : 'g');
        }
        // Quit Program
        else if (userChoice == 4){
            progRunning = false;
        }
        else {
            std::cout << "Sorry, something went wrong processing your input.\n";
            std::cout << "Execution aborted.\n";
            exit(0);
        }
        // Cleanup
    } while (progRunning);
    std::cout << "Thank you for utilizing the GPA calculator.\n";
    std::cout << "Have a nice day.\n";

    fileOut.close();
    return 1;
}

// Specification B3 - Menu Input Validation
int get_menu_choice(){
    int choice;
    std::cout << "1. Add Grade.\n";
    std::cout << "2. Display All Grades.\n";
    std::cout << "3. Process All Grades.\n";
    std::cout << "4. Quit Program.\n";
    bool inputValid;
    do {
        inputValid = true;
        std::cin >> choice;
        if (choice < 1 || choice > 4){
            std::cout <<  "Please choose an option 1-4 from above menu.\n";
            inputValid = false;
        }
    } while (!inputValid);
    return choice;
}

char Grade2Lttr(int grade){
    // Simple Error checking
    if (grade > 200) return 'X';
    if (grade < 0) return 'X';

    if (grade >= 90) return 'A';
    else if (grade >= 80) return 'B';
    else if (grade >= 70) return 'C';
    else if (grade >= 60) return 'D';
    else return 'F';
}

void ProgramGreeting(){
    Logger l = Logger("ProgramGreeting");
    std::cout << "Welcome to the GPA analyzer.\n";
    std::cout << "Written by Gavin williams for CISP 400.\n";
    std::cout << "You may use this program to create a list of test scores,";
    std::cout << "then find your average.\n";
    std::cout << "Enjoy!\n";
}

// Specification A4 - Unit Test
void UnitTest(){
    Logger l = Logger("UnitTest");
    std::cout << "Beginning unit testing of Grade2Lttr function.\n";
    std::cout << "Testing '98'\n";
    std::cout << "Expected result: 'A'. Actual result: " << Grade2Lttr(98) << '\n';
    if ('A' != Grade2Lttr(98))
        std::cout << "Results do not match.\n";
    std::cout << "Testing '66'\n";
    std::cout << "Expected result: 'D'. Actual result: " << Grade2Lttr(66) << '\n';
    if ('D' != Grade2Lttr(66))
        std::cout << "Results do not match.\n";
    std::cout << "Testing '-20'\n";
    std::cout << "Expected result: 'X'. Actual result: " << Grade2Lttr(-20) << '\n';
    if ('X' != Grade2Lttr(-20))
        std::cout << "Results do not match.\n";
    std::cout << "Testing '201'\n";
    std::cout << "Expected result: 'X'. Actual result: " << Grade2Lttr(201) << '\n';
    if ('X' != Grade2Lttr(201))
        std::cout << "Results do not match.\n";
    std::cout << std::endl;
}