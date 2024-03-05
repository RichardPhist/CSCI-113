/*                                   DOCUMENTATION


*/
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include <math.h>
using namespace std;

struct Block
{
    int LRU;     
    bool valid;  
    string tag;  
    string data; 
};

struct Set
{
public:
    Block block0;
    Block block1;
};

vector<string> reg;
vector<Set> cache;
vector<string> mem;

int BinaryToDecimal(string num)
{
    int length = num.length() - 1;
    int power = 0;
    int sum = 0;
    int temp;
    while (length >= 0)
    {
        temp = num[length--] == '1' ? pow(2, power) : 0;
        power++;
        sum += temp;
    }

    return sum;
}

string DtoB(int num)
{
    string res;
    while (num != 0)
    {
        int temp = num % 2;
        res.push_back(temp + '0');
        num /= 2;
    }
    while (res.length() < 7)
        res.push_back('0');
    for (int i = 0; i < res.length() / 2; i++)
    {
        swap(res[i], res[res.length() - 1 - i]);
    }
    return res;
}

bool lw(string addr, int rt)
{
    int index = BinaryToDecimal(addr) % 8;
    string tag = addr.substr(0, 4);
    bool hit = false;
    if (cache[index].block0.tag == tag)
    {
        if (cache[index].block0.valid)
        {
            hit = true;
            reg[rt] = cache[index].block0.data;
            cache[index].block0.LRU = 1;
            cache[index].block1.LRU = 0;
        }
    }
    else if (cache[index].block1.tag == tag)
    {

        if (cache[index].block1.valid)
        {
            hit = true;
            reg[rt] = cache[index].block1.data;
            cache[index].block1.LRU = 1;
            cache[index].block0.LRU = 0;
        }
    }
    if (!hit)
    {
        string data = mem[BinaryToDecimal(addr)];
        reg[rt] = data;
        if (!cache[index].block0.valid)
        {
            cache[index].block0.valid = true;
            cache[index].block0.tag = tag;
            cache[index].block0.data = data;
            cache[index].block0.LRU = 1;
            cache[index].block1.LRU = 0;
        }
        else if (!cache[index].block1.valid)
        {
            cache[index].block1.valid = true;
            cache[index].block1.tag = tag;
            cache[index].block1.data = data;
            cache[index].block1.LRU = 1;
            cache[index].block0.LRU = 0;
        }
        else
        {
            if (!cache[index].block0.LRU)
            {
                cache[index].block0.valid = true;
                cache[index].block0.tag = tag;
                cache[index].block0.data = data;
                cache[index].block0.LRU = 1;
                cache[index].block1.LRU = 0;
            }
            else
            {
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

bool sw(string addr, int rt)
{
    int index = BinaryToDecimal(addr) % 8;
    string tag = addr.substr(0, 4);
    bool hit = true;
    if (cache[index].block0.tag == tag && cache[index].block0.valid)
    {
        cache[index].block0.data = reg[rt];
        cache[index].block0.valid = true;
        cache[index].block0.LRU = 1;
        cache[index].block1.LRU = 0;
    }
    else if (cache[index].block1.tag == tag && cache[index].block1.valid)
    {

        cache[index].block1.data = reg[rt];
        cache[index].block1.valid = true;
        cache[index].block0.LRU = 0;
        cache[index].block1.LRU = 1;
    }
    else
    {
        mem[BinaryToDecimal(addr)] = reg[rt];
        hit = false;
    }
    return hit;
}

void init_data()
{
    string zeros = "00000000000000000000000000000000";
    int num = 5; 
    Block bl;
    Set st;
    bl.LRU = 0;
    bl.data = zeros;
    bl.tag = "0000";
    bl.valid = 0;
    st.block0 = st.block1 = bl;
    for (int i = 0; i < 128; i++)
    {
        string temp = DtoB(num);
        mem.push_back(zeros.substr(0, zeros.length() - temp.length()) + temp);
        num++;
    }

    for (int i = 0; i < 8; i++)
    {
        reg.push_back(zeros);
        cache.push_back(st);
    }
}
bool mhs(string ins)
{
    string addr = DtoB(BinaryToDecimal(ins.substr(16, ins.length() - 16)) / 4);
    int rt = BinaryToDecimal(ins.substr(13, 3));
    if (ins.substr(0, 6) == "100011")
        return lw(addr, rt);
    else
        return sw(addr, rt);
    return 1;
}

void display()
{
    string letterS = "s";
    int i;
    cout << "Register File" << endl;
    for (int i = 0; i < reg.size(); i++)
    {
        cout << (letterS + to_string(i)) << "     " << reg[i] << endl;
    }
    cout << endl;
    cout << setw(30) << "Cache" << endl;
    cout << "block0" << setw(56) << "block1" << endl;
    cout << "set" << setw(8) << "valid" << setw(6) << "tag" << setw(8) << "data" << setw(36) << "valid" << setw(8) << "tag" << setw(8) << "data" << endl;
    for (int i = 0; i < cache.size(); i++)
    {
        cout << i << setw(6) << cache[i].block0.valid << setw(11) << cache[i].block0.tag << "   " << cache[i].block0.data << setw(4) << cache[i].block1.valid << setw(13) << cache[i].block1.tag << "   " << cache[i].block1.data << endl;
    }
    cout << endl;
    cout << setw(32) << "Main Memory" << endl;
    cout << "Address" << setw(14) << "Data" << setw(32) << "Address" << setw(14) << "Data" << endl;
    for (i = 0; i <= 63; i++)
    {
        cout << i << (i >= 10 ? "      " : "       ") << mem[i] << (i >= 100 ? "       " : "      ") << i + 64 << (i >= 36 ? "      " : "       ") << mem[i + 64] << endl;
    }
}

int main()
{
    string instruction;
    ifstream input;
    input.open("instructionInput.txt", ios::in);
    if (input.is_open())
    {
        init_data();
        cout << "Istruction" << setw(60) << "Cache-Access" << endl;
        while (!input.eof())
        {

            input >> instruction;
            input.get();
            cout << instruction << setw(30) << (mhs(instruction) == true ? "hit" : "miss") << endl;
            if (input.peek() == EOF)
                break;
        }
        cout << endl;
        display();
        input.close();
    }
    else
    {
        cout << "fail to open file." << endl;
    }
    return 0;
}