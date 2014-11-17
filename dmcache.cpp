#include <iostream>
#include<fstream>
#include<iomanip>

using namespace std;
#define CACHE_SIZE 32  // lines in cache
#define LINE_SIZE 8  // bytes per line

class Line
{
public:
    int tag;
    char data[LINE_SIZE];
    bool dirty;
    Line() : tag(-1), dirty(false)
    {for(int i = 0; i < LINE_SIZE; i++) data[i] = 0;}
};


int main(int argc, char** argv)
{
    bool hit, dirty;
    char memory[0xFFFF];
    int address, read_write, data, lineNum, tag;
    Line cache[CACHE_SIZE];
    
    for(int i = 0; i < 0xFFFF; i++)
        memory[i] = 0; // not necessary but nice for debugging.
    
    ifstream inf(argv[1]);
    ofstream outf("dm-out.txt");
    outf << hex << setfill('0') << uppercase;
    
    while(inf >> hex >>  address >> read_write >> data)
    {
        lineNum = (address / LINE_SIZE) % CACHE_SIZE;
        tag = address / LINE_SIZE;
        hit = cache[lineNum].tag == tag;
        dirty = cache[lineNum].dirty;
        if(!hit)//if tags dont match up
        {
            if(cache[lineNum].dirty)
                for(int i = 0; i < LINE_SIZE; i++)
                    (&memory[cache[lineNum].tag * LINE_SIZE])[i] = cache[lineNum].data[i];
            
            for(int i = 0; i < LINE_SIZE; i++)
                cache[lineNum].data[i] = (&memory[tag * LINE_SIZE])[i];
            
            cache[lineNum].tag = tag;
            cache[lineNum].dirty = false;
        } // if a miss and dirty
        
        if(read_write == 0xFF) // write
        {
            cache[lineNum].data[address & (LINE_SIZE - 1)] = data;
            cache[lineNum].dirty = true;
        } // if a write
        else // a read
        {
            outf << setw(2)
            << (((int) cache[lineNum].data[address & (LINE_SIZE - 1)]) & 0xFF)
            << ' ' << hit << ' ' << dirty << endl;
        }
    } // while more in file
    
    inf.close();
    outf.close();
    return 0;
} // main()