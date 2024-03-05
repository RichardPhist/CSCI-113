/*              

************************* DOCUMENTATION: **************************

                            Utilized: 
VScode with GCC C++ complier (g++) and GDB debugger from mingw-w64

                        Program Description: 
This program is a simulation of the Booth's Multiplication Algorithm. It takes in two 2's complement 16-bit binary numbers, output the contents of registers step by step, and displaying the product in 32-bit binary.

Enter multiplicand and multiplier when prompted, display product along with register content at each cycle count.

                            Functions:
# ALU_1bit() - performs addition/subtraction on 2 1-bit numbers, subcomponent of ALU_16bit(). Reference c_out, return int.
# ALU_16bit() - performs addition/subtraction on 2 16-bit numbers, subcomponent of BoothsMultiplication(). Store result of addition/subtraction in passed in parameter called output[], void return.
# BoothsMultiplication() - calculate product of 2 16-bit numbers using ALU_16bit as subcomponent to calculate AC, MD, MQ.
# cycleCounterToBinary() - display cycle counter in binary, need for displaying register content.
# shiftRight() - combine AC, MQ, Q-1 into a array[33] to perform shift and repopulate AC, MQ, Q-1 with shifted binary numbers.
# arrayToString() - used for turning AC, MQ array into string and return from BoothsMultiplication() as product.
# stringToArray() - use to turn string user input MD and MQ into array.
# display() - used to display register content for each cycle count.

*/

#include <iostream>
#include <string>
using namespace std;

// convert int to binary
void cycleCounterToBinary(int n)
{
    int bitNum[4];
    int k = n;
    
    // 0 or 1 at nth bit
    int i = 0;
    while (n > 0)
    {
        bitNum[i] = n % 2;
        i++;
        n = n / 2;
    }

    // hardcode 0s if less than 4 binary number
    if(k < 8 && k >= 4)
    {
        bitNum[3] =  0;
    }else if(k < 4 && k >=2){
        bitNum[3] = 0;
        bitNum[2] = 0;
    }else if(k < 2 && k > 0){
        bitNum[3] = 0;
        bitNum[2] = 0;
        bitNum[1] = 0;
    }else if (k==0){
        for(int z = 0; z < 4; z++){
            bitNum[z] = 0;
        }
    }

    // bit store backward in array, print in reverse
    for (int b = 3; b >= 0; b--)
    {
        cout << bitNum[b];
    }
    cout << "\t\t";
    return;
}

// combine AC + MQ + MQ_neg1 and then shift then repopulate each reg and MQ_neg1
void shiftRight(int array_AC[], int array_MQ[], int MQ_1[], int size)
{
    // store AC MQ MQ_1
    int shiftArray[33];
    int j = 0;
    // populate with AC
    for (int i = 0; i < size; i++)
    {
        shiftArray[j] = array_AC[i];
        j++;
    }

    //populate with MQ
    for (int i = 0; i < size; i++)
    {
        shiftArray[j] = array_MQ[i];
        j++;
    }

    //populate with MQ1
    shiftArray[32] = MQ_1[0];

    //shifting
    for (int i = 32; i > 0; i--)
    {
        shiftArray[i] = shiftArray[i - 1];
    }
    shiftArray[0] = shiftArray[1];

    //repopulate AC
    j = 0;
    for (int i = 0; i < size; i++)
    {
        array_AC[i] = shiftArray[j];
        j++;
    }

    //repopulate MQ
    for (int i = 0; i < size; i++)
    {
        array_MQ[i] = shiftArray[j];
        j++;
    }

    //repopulate MQ_1
    MQ_1[0] = shiftArray[j];
}

// array to string, for returning product
string arrayToString(int array[], int size)
{
    string str = "";
    for (int i = 0; i < size; i++)
    {
        str += to_string(array[i]);
    }
    return str;
}

// convert string of int to int array
//# for MD MQ
void stringToArray(int array[], string userStr)
{
    for (int i = 0; i < userStr.length(); i++)
    {
        array[i] = userStr[i] - '0';
    }
}

// display content in registers/results
void display(int array[], int size)
{
    for (int i = 0; i < size; i++)
    {
        cout << array[i];
    }
    cout << "\t";
}

// 1 Bit ALU Simulation
//# for ALU 16bit
int ALU_1bit(int a, int b, int b_inv, int c_in, int op, int &c_out)
{
    if (op == 10)
    {
        // addition
        if (b_inv == 0)
        {
            // carry = 1
            if (c_in == 1)
            {
                // a&b = 1 or 0
                if (a == b)
                {
                    // 1 + 1 + 1 = (cout 1) 1
                    if (a == 1)
                    {
                        c_out = 1;
                        return 1;
                    }
                    // 0 + 0 + 1 = 1
                    else
                    {
                        c_out = 0;
                        return 1;
                    }
                }
                // 1 + 0 + 1 = (cout 1) 0
                // 0 + 1 + 1 = (cout 1) 0
                else
                {
                    c_out = 1;
                    return 0;
                }
            }
            // carry = 0
            else
            {
                // a&b = 1 or 0
                if (a == b)
                {
                    // 1 + 1 + 0 = (cout 1) 0
                    if (a == 1)
                    {
                        c_out = 1;
                        return 0;
                    }
                    // 0 + 0 + 0 = 0
                    else
                    {
                        c_out = 0;
                        return 0;
                    }
                }
                // 1 + 0 + 0 = (cout 0) 1
                // 0 + 1 + 0 = (cout 0) 1
                else
                {
                    c_out = 0;
                    return 1;
                }
            }
        }
        // subtraction
        else
        {
            // carry = 1
            if (c_in == 1)
            {
                //a != b
                if (a == b)
                {
                    //1 + 0 + 1 = cout(1) 0
                    //0 + 1 + 1 = cout(1) 0
                    c_out = 1;
                    return 0;
                }
                //1 + 1 + 1 = cout(1) 1
                else if (a == 1)
                {
                    c_out = 1;
                    return 1;
                }
                //0 + 0 + 1 = cout(0) 1
                else
                {
                    c_out = 0;
                    return 1;
                }
            }
            // carry = 0
            else
            {
                // a != b
                if (a == b)
                {
                    //1 + 0 + 0 = cout(0) 1
                    //0 + 1 + 0 = cout(0) 1
                    c_out = 0;
                    return 1;
                }
                // 1 + 1 + 0 = cout(1) 0
                else if (a == 1)
                {
                    c_out = 1;
                    return 0;
                }
                // 0 + 0 + 0 = cout(0) 0
                else
                {
                    c_out = 0;
                    return 0;
                }
            }
        }
    }
    else{return -1;}
}

// 16 Bits ALU Simulation
//# for Booths Multiplication
void ALU_16bit(int inputA[], int inputB[], int b_inv, int op, int result[])
{

    // first call w/ c_in = b_inv
    int c_out = 0;
    result[15] = ALU_1bit(inputA[15], inputB[15], b_inv, b_inv, op, c_out);

    // loop for other 15 bits w/ c_out as reference into c_in
    for (int i = 14; i >= 0; i--)
    {
        result[i] = ALU_1bit(inputA[i], inputB[i], b_inv, c_out, op, c_out);
    }
}

// Booths Multiplication Algorithm
string BoothsMultiplication(string MD_user, string MQ_user)
{
    int cycleCounter = 15;
    int zero[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int AC[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int MD[16], MQ[16];
    int MQ_neg1[1] = {0};
    string product;
    // for shift right
    int shiftArray[33];

    // convert string user_input into int array
    stringToArray(MD, MD_user);
    stringToArray(MQ, MQ_user);

    while (cycleCounter >= 0)
    {
        if (MQ[15] == 0 && MQ_neg1[0] == 0)
        {
            //AC <- AC + 0
            ALU_16bit(AC, zero, 0, 10, AC);
        }
        else if (MQ[15] == 0 && MQ_neg1[0] == 1)
        {
            //AC <- AC + MD
            ALU_16bit(AC, MD, 0, 10, AC);
        }
        else if (MQ[15] == 1 && MQ_neg1[0] == 0)
        {
            //AC <- AC - MD
            ALU_16bit(AC, MD, 1, 10, AC);
        }
        else if (MQ[15] == 1 && MQ_neg1[0] == 1)
        {
            //AC <- AC + 0
            ALU_16bit(AC, zero, 0, 10, AC);
        }
        //display
        cycleCounterToBinary(cycleCounter);
        display(MD, 16);
        display(AC, 16);
        display(MQ, 16);
        display(MQ_neg1, 1);
        cout << "\n";

        //shift right 1 bit
        shiftRight(AC, MQ, MQ_neg1, 16);

        //display after shift
        cycleCounterToBinary(cycleCounter);
        display(MD, 16);
        display(AC, 16);
        display(MQ, 16);
        display(MQ_neg1, 1);
        cout << "\n";

        //update cycle counter
        cycleCounter--;
    }
    product = arrayToString(AC, 16) + arrayToString(MQ, 16);
    return product;
}

// driver code
int main()
{ 
    string userInputMD, userInputMQ, product;

    cout << "Input a 16-bit 2's complement binary number multiplicand! (no spaces)\n";
    cin >> userInputMD;
    cout << "Input a 16-bit 2's complement binary number multiplier! (no spaces)\n";
    cin >> userInputMQ;
    cout << "\n\n";

    cout << "cycle-counter\t\t" << "MD\t\t\t" << "AC \t\t\t" << "MQ\t\t" << "MQ-1\n";
    cout << "-----------------------------------------------------------------------------------------\n";
    product = BoothsMultiplication(userInputMD, userInputMQ);
    cout << "\n\t\t product: " << product;
    return 0;
}