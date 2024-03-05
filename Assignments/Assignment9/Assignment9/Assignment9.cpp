// Assignment9.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/*
Documentation
The BinToDec and DecToBin functions convert their inputs from either binary to decimal or decimal to binary respectively

The LoadWord function does the logic for the lw instruction. It takes two arguments, the string of the address and the register index, 
and then gets the index by converting the address into a decimal value and sets the tag to the first 4 binary digits of the address.
Then it searches the 2 blocks to see if the tag matches, if it matches then it's a hit then it stores the cache value in the register. 
If it's a miss then it loads the data into the cache.

The StoreWord function does the logic of the sw instrunction. It takes two arguments, the string of the address and the register index,
and then it converts the index to decimal and sets the tag to the first 4 binary digits of the address. Then it checks each block and
if the tags match and if the block is valid, if both are true then it stores the value in the register in the cache. If it can't find
a matching tag then the value is stored in the memory at the decimal value of the address.

The Simulate function reads the instructions given by the input file and then splits up the string into substrings to find the op codes
base register, offset, and store or load register. Then calls the appropriate LoadWord or StoreWord instruction based on the op code.
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include <math.h>

using namespace std;

struct Block {
    int LRU;
    bool valid;
    string tag;
    string data;
};

struct Set {
public:
    Block block0;
    Block block1;
};

vector<string> reg;
vector<Set> cache;
vector<string> mem;

int BinToDec(string num) { //binary to decimal converter
    int length = num.length() - 1;
    int power = 0;
    int sum = 0;
    int temp;
    while (length >= 0) {
        temp = num[length--] == '1' ? pow(2, power) : 0;
        power++;
        sum += temp;
    }

    return sum;
}

string DecToBin(int num) { //decimal to binary converter
    string res;
    while (num != 0) {
        int temp = num % 2;
        res.push_back(temp + '0');
        num /= 2;
    }
    while (res.length() < 7)
        res.push_back('0');
    for (int i = 0; i < res.length() / 2; i++) {
        swap(res[i], res[res.length() - 1 - i]);
    }
    return res;
}

bool LoadWord(string addr, int ind) { //load word logic
    int index = BinToDec(addr) % 8;
    string tag = addr.substr(0, 4);
    bool hit = false;
    if (cache[index].block0.tag == tag) {
        if (cache[index].block0.valid) {
            hit = true;
            reg[ind] = cache[index].block0.data;
            cache[index].block0.LRU = 1;
            cache[index].block1.LRU = 0;
        }
    }
    else if (cache[index].block1.tag == tag) {

        if (cache[index].block1.valid) {
            hit = true;
            reg[ind] = cache[index].block1.data;
            cache[index].block1.LRU = 1;
            cache[index].block0.LRU = 0;
        }
    }
    if (!hit) {
        string data = mem[BinToDec(addr)];
        reg[ind] = data;
        if (!cache[index].block0.valid) {
            cache[index].block0.valid = true;
            cache[index].block0.tag = tag;
            cache[index].block0.data = data;
            cache[index].block0.LRU = 1;
            cache[index].block1.LRU = 0;
        }
        else if (!cache[index].block1.valid) {
            cache[index].block1.valid = true;
            cache[index].block1.tag = tag;
            cache[index].block1.data = data;
            cache[index].block1.LRU = 1;
            cache[index].block0.LRU = 0;
        }
        else {
            if (!cache[index].block0.LRU) {
                cache[index].block0.valid = true;
                cache[index].block0.tag = tag;
                cache[index].block0.data = data;
                cache[index].block0.LRU = 1;
                cache[index].block1.LRU = 0;
            }
            else {
                cache[index].block1.valid = true;
                cache[index].block1.tag = tag;
                cache[index].block1.data = data;
                cache[index].block1.LRU = 1;
                cache[index].block0.LRU = 0;
            }
        }
    }

    return hit;
}

bool StoreWord(string addr, int ind) { //store word logic
    int index = BinToDec(addr) % 8;
    string tag = addr.substr(0, 4);
    bool hit = true;
    if (cache[index].block0.tag == tag && cache[index].block0.valid) {
        cache[index].block0.data = reg[ind];
        cache[index].block0.valid = true;
        cache[index].block0.LRU = 1;
        cache[index].block1.LRU = 0;
    }
    else if (cache[index].block1.tag == tag && cache[index].block1.valid) {

        cache[index].block1.data = reg[ind];
        cache[index].block1.valid = true;
        cache[index].block0.LRU = 0;
        cache[index].block1.LRU = 1;
    }
    else {
        mem[BinToDec(addr)] = reg[ind];
        hit = false;
    }
    return hit;
}

void initialData() { //sets the initial values of the memory, cache, and registers to 0's
    string zeros = "00000000000000000000000000000000";
    int num = 5;
    Block bl;
    Set st;
    bl.LRU = 0;
    bl.data = zeros;
    bl.tag = "0000";
    bl.valid = 0;
    st.block0 = st.block1 = bl;
    for (int i = 0; i < 128; i++) {
        string temp = DecToBin(num);
        mem.push_back(zeros.substr(0, zeros.length() - temp.length()) + temp);
        num++;
    }

    for (int i = 0; i < 8; i++) {
        reg.push_back(zeros);
        cache.push_back(st);
    }
}

bool Simulate(string ins) { //does the simulation of memory hierarchy
    string addr = DecToBin(BinToDec(ins.substr(16, ins.length() - 16)) / 4);
    int regIndx = BinToDec(ins.substr(13, 3));
    if (ins.substr(0, 6) == "100011")
        return LoadWord(addr, regIndx);
    else
        return StoreWord(addr, regIndx);
    return 1;
}

void display() { //creates the output
    string letterS = "$S";
    int i;
    cout << "Register File" << endl;
    for (int i = 0; i < reg.size(); i++) {
        cout << (letterS + to_string(i)) << "     " << reg[i] << endl;
    }
    cout << endl;
    cout << setw(30) << "Cache" << endl;
    cout << "Block0" << setw(56) << "Block1" << endl;
    cout << "Set" << setw(8) << "Valid" << setw(6) << "Tag" << setw(8) << "Data" << setw(36) << "Valid" << setw(8) << "Tag" << setw(8) << "Data" << endl;
    for (int i = 0; i < cache.size(); i++) {
        cout << i << setw(6) << cache[i].block0.valid << setw(11) << cache[i].block0.tag << "   " << cache[i].block0.data << setw(4) << cache[i].block1.valid << setw(13) << cache[i].block1.tag << "   " << cache[i].block1.data << endl;
    }
    cout << endl;
    cout << setw(32) << "Main Memory" << endl;
    cout << "Address" << setw(14) << "Data" << setw(32) << "Address" << setw(14) << "Data" << endl;
    for (i = 0; i <= 63; i++) {
        cout << i << (i >= 10 ? "      " : "       ") << mem[i] << (i >= 100 ? "       " : "      ") << i + 64 << (i >= 36 ? "      " : "       ") << mem[i + 64] << endl;
    }
}

int main() { //initiates the data, opens the input file, and runs the Simulationa and display
    string Instruc;
    ifstream File;
    File.open("instructionInput.txt", ios::in); //read from input file
    if (File.is_open()) {
        initialData();
        cout << "Istruction" << setw(60) << "Cache-Access" << endl;
        while (!File.eof()) {

            File >> Instruc;
            File.get();
            cout << Instruc << setw(30) << (Simulate(Instruc) == true ? "hit" : "miss") << endl;
            if (File.peek() == EOF)
                break;
        }
        cout << endl;
        display();
        File.close();
    }
    else {
        cout << "Failed to open file." << endl;
    }
    return 0;
}

