#include <iostream>
using namespace std;

class Suffix{
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
    Suffix* temp_arr; // Temporary array used for Merge Sort operations (O(n) space)
    SuffixArray(char* x)//constructor used to initializes text and suffix data
    {
        for(n=0; x[n]!='\0'; n++);//compute the text length manually to prevent any usage of any STLs libraries
        char_arr=new char[n];
        for(int i=0; i<n; i++)//allocate new character array and copy the input
            char_arr[i]=x[i];

        arr=new Suffix[n];//allocate memory for suffix array
        temp_arr=new Suffix[n]; // Allocate memory for the temporary merge array
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

    //Comparison function to sort suffixes based ONLY on their starting index (ind)
    bool smaller_by_ind(Suffix a, Suffix b)
    {
        // The sorting must be based on the original position in the text (ind)
        return a.ind < b.ind;
    }

    // Merge Helper function for sorting by <r1, r2>
    void merge_r1r2(Suffix arr[], Suffix temp[], int left, int mid, int right)
    {
        int i = left;  // Pointer for the left half
        int j = mid;   // Pointer for the right half
        int k = left;  // Pointer for the temporary array

        // Merge the two halves into the temporary array, comparing using 'smaller'
        while (i < mid && j <= right)
        {
            // Use the comparison (smaller) for the <r1, r2> pair
            if (smaller(arr[i], arr[j]))
                temp[k++] = arr[i++];
            else
                temp[k++] = arr[j++];
        }

        // Copy remaining elements from the left half
        while (i < mid)
            temp[k++] = arr[i++];

        // Copy remaining elements from the right half
        while (j <= right)
            temp[k++] = arr[j++];

        // Copy the sorted elements back to the original array
        for (int idx = left; idx <= right; idx++)
            arr[idx] = temp[idx];
    }

    // Recursive Merge Sort function for sorting by <r1, r2>
    void merge_sort_r1r2(Suffix arr[], Suffix temp[], int left, int right)
    {
        if (left < right)
        {
            int mid = left + (right - left) / 2; // Calculate the midpoint

            // Recursively sort the left and right halves
            merge_sort_r1r2(arr, temp, left, mid);
            merge_sort_r1r2(arr, temp, mid + 1, right);

            // Merge the two sorted halves
            merge_r1r2(arr, temp, left, mid + 1, right);
        }
    }

    // Function to sort an array of suffixes based on the pair (r1, r2)
    void sort_suffixes(Suffix suffixes[])
    {
        // Calls Merge Sort function
        merge_sort_r1r2(suffixes, temp_arr, 0, n - 1);
    }

    // Merge Helper function for sorting by 'ind'
    void merge_ind(Suffix arr[], Suffix temp[], int left, int mid, int right)
    {
        int i = left;
        int j = mid;
        int k = left;

        // Merge the two halves using 'smaller_by_ind'
        while (i < mid && j <= right)
        {
            // Use the comparison (smaller_by_ind) for the index
            if (smaller_by_ind(arr[i], arr[j]))
                temp[k++] = arr[i++];
            else
                temp[k++] = arr[j++];
        }

        // Copy remaining elements
        while (i < mid)
            temp[k++] = arr[i++];

        while (j <= right)
            temp[k++] = arr[j++];

        // Copy the sorted elements back
        for (int idx = left; idx <= right; idx++)
            arr[idx] = temp[idx];
    }

    // Recursive Merge Sort function for sorting by 'ind'
    void merge_sort_ind(Suffix arr[], Suffix temp[], int left, int right)
    {
        if (left < right)
        {
            int mid = left + (right - left) / 2;

            merge_sort_ind(arr, temp, left, mid);
            merge_sort_ind(arr, temp, mid + 1, right);

            // Merge the two sorted halves
            merge_ind(arr, temp, left, mid + 1, right);
        }
    }

    //Function to sort the array of suffixes based on 'ind'
    // This is necessary to prepare the array for the r2 calculation in the next step.
    void sort_by_ind(Suffix suffixes[])
    {
        // Calls Merge Sort function
        merge_sort_ind(suffixes, temp_arr, 0, n - 1);
    }

    void UpdateR1(Suffix* suffixes, int n , int prefixLength){
        int currentRank = 1; // Initialize the rank counter for normal suffixes (start from 1 because '$' has rank 0)

        int *newRanks = new int[n]; // Temporary array to store updated ranks for each suffix according to its original index

        newRanks[suffixes[0].ind] = suffixes[0].r1; // Assign the first suffix its current rank (preserve '$' as 0 if present)

        for (int i = 1; i < n; i++) { // Loop through suffixes starting from the second
            // If the current suffix's pair (r1, r2) is different from the previous suffix, assign a new rank
            if (suffixes[i].r1 != suffixes[i - 1].r1 || suffixes[i].r2 != suffixes[i - 1].r2) {
                if (suffixes[i].r1 != 0) // Ignore '$' suffix which should remain rank 0
                    currentRank++; // Increment the rank for a new unique suffix
            }
            // Store the new rank in the temporary array
            // If this suffix is '$' → keep rank 0, otherwise assign currentRank
            newRanks[suffixes[i].ind] = (suffixes[i].r1 == 0) ? 0 : currentRank;
        }

        for (int i = 0; i < n; i++) { // Update the suffixes array with new ranks
            suffixes[i].r1 = newRanks[i]; // Update r1 to the new rank
        }

        delete[] newRanks; // Free the temporary memory
    }

    //Calculates and updates the next rank (r2) based on the shift
    void UpdateR2(Suffix* suffixes, int n , int prefixLength)
    {
        for (int i = 0; i < n; i++)// Loop through the array, which is now sorted by 'ind' (suffixes[i].ind == i)
        {
            // Calculate the index of the suffix starting 'prefixLength' positions ahead (the required shift)
            int next_suffix_index = i + prefixLength;

            // Check if the shifted suffix index is within the text boundaries
            if (next_suffix_index < n) {
                // The r2 for the current suffix is the r1 rank of the suffix starting k positions ahead
                suffixes[i].r2 = suffixes[next_suffix_index].r1;// Get the r1 of the shifted suffix
            }
            // If the shifted suffix is outside the text (i.e., we hit the '$' sentinel or beyond)
            else {
                // Assign -1 (or any value smaller than all valid ranks) to represent the string end.
                suffixes[i].r2 = -1;
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
