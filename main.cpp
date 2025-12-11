#include <iostream>
using namespace std;

class Suffix
{
public:
    int ind; //store the Starting index of the suffix in the text
    int r1; //current rank of the suffix
    int r2; //next rank used for tie-breaking in the (k<1) iterations
    Suffix()//constructor to just initialize all attributes to zeros
    {
        r1=0;//initialize first rank to 0
        r2=0;//initialize second rank to 0
        ind=0;//initialize index to 0
    }
};
//the base structure for suffix array construction
class SuffixArray
{
public:
    char* char_arr;//character array for the input text (as strings are forbidden)
    int n;//length of the text -> to be able to initialize any array
    Suffix* arr;//array of Suffix elements
    SuffixArray(char* x)//constructor used to initializes text and suffix data
    {
        for(n=0; x[n]!='\0'; n++);//compute the text length manually to prevent any usage of any STLs libraries
        char_arr=new char[n];
        for(int i=0; i<n; i++)//allocate new character array and copy the input
            char_arr[i]=x[i];

        arr=new Suffix[n];//allocate memory for suffix array
//first we should identify which ASCII characters exist in the input
        bool exists[256]={false};//boolean array for character presence in the input
        for(int i=0; i<n; i++)
            exists[char_arr[i]]=true;
//second we should assign ASCII-based ranks to each character(each has rank according to 1-presence in the input  2-the ASCII sequence)
        int ranks[256]={0};//array to store rank of each character
        ranks['$']=0;//'$' is always the smallest rank
        int rank=1;//start assigning ranks from 1
        for(int i=0; i<256; i++)//assign ranks sequentially based on ASCII order
        {
            if(i=='$') continue;//skip '$' since it already has its own rank (0)
            if(exists[i])//If character exists in the text
                ranks[i]=rank++;//assign the next available rank
        }
//third we should initialize suffix array elements
        for(int i=0; i<n; i++)
        {
            arr[i].ind=i;//index for the starting of the suffix
            arr[i].r1=ranks[char_arr[i]];//initial rank from first character
            arr[i].r2=0;//tie-breaker rank not used for k=0 as its only initialization
        }
    }
    ~SuffixArray()
    {
        delete[] char_arr;
        delete[] arr;
    }
    //compare between 2 suffixes to arrange the array
    bool smaller(Suffix a, Suffix b)
    {
        if (a.r1 != b.r1)// arrange depending on r1
            return a.r1 < b.r1;
        //if a.r1 = b.r1 , arrange depending on r2
        return a.r2 < b.r2;
    }

    // Function to sort an array of suffixes based on the pair (r1, r2)
    void sort_suffixes(Suffix suffixes[])
    {
        // Outer loop: determines the number of passes for Bubble Sort
        for (int i = 0; i < n - 1; i++)
        {
            // Inner loop: compares adjacent elements and swaps them if needed
            for (int j = 0; j < n - i - 1; j++)
            {
                // If the current element is not smaller than the next element
                // according to the 'smaller' function
                if (!smaller(suffixes[j], suffixes[j + 1]))
                {
                    // Swap the two elements to maintain ascending order
                    Suffix temp = suffixes[j];//make temp value to set the second suffix in it
                    suffixes[j] = suffixes[j + 1]; // shift the first suffix to the second one
                    suffixes[j + 1] = temp; //make the first suffix = the smaller one
                }
            }
        }

    }

};


int main(){
    /*SuffixArray t("ACGACTACGATAAC$");

    t.ConstructUsingPrefixDoubling();

    t.Print(); // Prints:  14 11 12  0  6  3  9 13  1  7  4  2  8 10  5

    return 0;*/

}