#include <iostream>
#include "persistent_set.h"

using namespace std;

int main() {

    persistent_set a;
    int arr[9] = { 8, 3, 1, 6, 4, 7, 10, 14, 13};
    for (int i = 0; i < 9; i++) {
        a.insert(arr[i]);
    }
    //cout << (a.find(3) == a.end());
    for (persistent_set::iterator it = a.begin(); it != a.end(); ++it){
        cout<< *it << " ";
    }
    persistent_set::iterator ex = a.end();
    std::cout << *(ex) << std::endl;
    while (ex != a.begin()) {
        std::cout << *(ex--) << std::endl;
    }
    for (int i = 0; i < 9; i++) {
        a.erase(a.find(arr[i]));
        for (persistent_set::iterator it = a.begin(); it != a.end(); ++it){
            cout<< *it << " ";
        }
        cout << endl;
    }
    return 0;
}