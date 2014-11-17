// Author: Sean Davis

#include <iostream>
#include<fstream>
#include<iomanip>
using namespace std;
const int NUMBER_OF_SETS = 10;  // Number of sets
const int LINES_PER_SET = 6;
const int LINE_SIZE = 4;

class Line
{
public:
    int LRU;
    int tag;
    char data[LINE_SIZE];
    bool dirty;
    Line() : LRU(0), tag(-1), dirty(false)
    {for(int i = 0; i < LINE_SIZE; i++) data[i] = 0;}
}; // class Line


int main(int argc, char** argv)
{
    bool hit, dirty;
    Line line;
    char memory[0xFFFF];
    int i, address, read_write, data, lineNum, tag, setNum;
    Line cache[NUMBER_OF_SETS][LINES_PER_SET];
    
    for(i = 0; i < 0xFFFF; i++)
        memory[i] = 0; // not necessary but nice for debugging.
    
    ifstream inf(argv[1]);
    ofstream outf("sa-out.txt");
    outf << hex << setfill('0') << uppercase;
    
    while(inf >> hex >>  address >> read_write >> data)
    {
        setNum = (address / LINE_SIZE) % NUMBER_OF_SETS;
        tag = (address / LINE_SIZE);
        
        for(int i = 0; i < LINES_PER_SET; i++)
            cache[setNum][i].LRU++;
        
        for(lineNum = 0; lineNum < LINES_PER_SET; lineNum++)
        {
            if(cache[setNum][lineNum].tag == tag)
            {
                cache[setNum][lineNum].LRU = 0;
                break;
            }  // if a hit
        } // for each line in the set
        
        hit = lineNum != LINES_PER_SET;
        dirty = cache[setNum][lineNum].dirty;
        
        if(!hit)
        {
            int max = 0;
            lineNum = 0;
            
            for(int i = 0; i < LINES_PER_SET; i++)
                if(cache[setNum][i].LRU > max)
                {
                    max = cache[setNum][i].LRU;
                    lineNum = i;
                } // if larger LRU
            
            dirty = cache[setNum][lineNum].dirty;
            
            if(cache[setNum][lineNum].dirty)
            {
                for(int i = 0; i < LINE_SIZE; i++)
                    (&memory[cache[setNum][lineNum].tag * LINE_SIZE])[i] = cache[setNum][lineNum].data[i];
            }  // if dirty
            
            for(int i = 0; i < LINE_SIZE; i++)
                cache[setNum][lineNum].data[i] = (&memory[tag * LINE_SIZE])[i];
            
            cache[setNum][lineNum].tag = tag;
            cache[setNum][lineNum].dirty = false;
            cache[setNum][lineNum].LRU = 0;
        } // if a miss
        
        if(read_write == 0xFF) // write
        {
            cache[setNum][lineNum].data[address & (LINE_SIZE - 1)] = data;
            cache[setNum][lineNum].dirty = true;
        } // if a write
        else // a read
        {
            outf << setw(2) 
            << (((int) cache[setNum][lineNum].data[address & (LINE_SIZE - 1)]) & 0xFF)
            << ' ' << hit << ' ' << dirty << endl;
        } // else a read
    } // while more in file
    
    inf.close();
    outf.close();
    return 0;
} // main()