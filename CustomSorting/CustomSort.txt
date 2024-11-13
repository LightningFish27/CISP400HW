// CustomSort.cpp
// Gavin Williams, CISP 400
// November 13, 2024

#include <iostream>
#include <cmath>

// Although technically a dynamic array, this should work alright for the required
// "homemade vector" of this assignment.
template <typename T>
class G_Array{
private:
    T* array = new T[0];
    size_t size = 0;

    // Helper function to swap elements
    void swap(T& a, T& b) {
        T temp = a;
        a = b;
        b = temp;
    }

public:
    // Adding to the array is done through the add_element function, which
    // abstracts away all the resizing done.
    void add_element(T newElement){
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
        return size;
    }

    // Sort function using a functor for comparison
    // Time complexity O(n^2)? Not great...
    template <typename Compare>
    void sort(Compare comp) {
        if (size > 1) {
            for (size_t i = 0; i < size - 1; i++) {
                for (size_t j = 0; j < size - i - 1; j++) {
                    if (!comp(array[j], array[j + 1])) {
                        swap(array[j], array[j + 1]);
                    }
                }
            }
        }
    }
};

/*
    The following three functors define three sorting methods-- ascending, descending,
    and absolute ascending. Ascending sorts from least to greatest, descending from 
    greatest to least. Absolute Ascending sorts from least to greatest, but considers
    the absolute values of the inputs, not the true values. AKA it sorts by 
    "intensity"
*/
struct Ascending{
    // Overloading () to make this struct a functor
    bool operator()(int a, int b) const {
        return a > b; // For descending order, essentially returning true IF a is greater than b
    }
};

struct Descending{
    // Overloading () to make this struct a functor
    bool operator()(int a, int b) const {
        return b > a; // For ascending, true IF b greater than a
    }
};

struct AbsoluteValueComparison{
    // overloading () to make this a functor
    bool operator()(int a, int b) const {
        return std::abs(a) > std::abs(b); // return true IF abs(a) greater than abs(b)
    }
};





int main(){
    G_Array<int> fauxVec;
    fauxVec.add_element(5);
    fauxVec.add_element(-10);
    fauxVec.add_element(3);
    fauxVec.add_element(2);
    fauxVec.add_element(-7);

    std::cout << "Unsorted: ";

    for (int i = 0; i < fauxVec.length(); i++){
        std::cout << fauxVec[i] << ' ';
    }
    std::cout << "\n";

    fauxVec.sort(Ascending());

    std::cout << "Sorted Ascending: ";

    for (int i = 0; i < fauxVec.length(); i++){
        std::cout << fauxVec[i] << ' ';
    }
    std::cout << "\n";

    fauxVec.sort(Descending());

    std::cout << "Sorted Descending: ";

    for (int i = 0; i < fauxVec.length(); i++){
        std::cout << fauxVec[i] << ' ';
    }
    std::cout << "\n";

    fauxVec.sort(AbsoluteValueComparison());

    std::cout << "Sorted Absolute Ascending: ";

    for (int i = 0; i < fauxVec.length(); i++){
        std::cout << fauxVec[i] << ' ';
    }
    std::cout << "\n";

    return 0;
}