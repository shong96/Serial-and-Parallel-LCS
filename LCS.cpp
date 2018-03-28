/*
Author: Kevin Hong
*/

#include <iostream>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilktools/cilkview.h>

using namespace std;

int matrix[9999][9999] = {0};
void subProblem(int iStart, int iEnd, int jStart, int jEnd, string &A, string &B);

/**
 * @param A: First string
 * @param B: Second string
 * @return: The length of longest common subsequence of A and B
*/
int serial_LCS(string &A, string &B) {

    // initialize matrix to 0's
    matrix[A.length()+1][B.length()+1] = {0};

    subProblem(1, A.length(), 1, B.length(), A, B);

    return matrix[A.length()][B.length()];
}

/**
 * Divides work into 4 squares similar to Tableau construction,
 * parallelizing square II and III for speedup.
 *
 * @param A: First string
 * @param B: Second string
 * @return: The length of longest common subsequence of A and B
*/
int parallel_LCS(string &A, string &B) {

    // initialize matrix to 0's
    matrix[A.length()+1][B.length()+1] = {0};

    // I
    subProblem(1, A.length()/2 - 1, 1, B.length()/2 - 1, A, B);

    // II
    cilk_spawn subProblem(A.length()/2, A.length(), 1, B.length()/2 - 1, A, B);

    // III
    subProblem(1, A.length()/2 - 1, B.length()/2, B.length(), A, B);

    cilk_sync;

    // IV
    subProblem(A.length()/2 - 1, A.length(), B.length()/2 - 1, B.length(), A, B);

    return matrix[A.length()][B.length()];
}

/**
 * Calculates length of LCS using dynamic programming
 *
 * @param B: Start value of first loop
 * @param B: End value of first loop
 * @param B: Start value of second loop
 * @param B: End value of second loop
 * @param A: First string
 * @param B: Second string
*/
void subProblem(int iStart, int iEnd, int jStart, int jEnd, string &A, string &B) {
    for (int i = iStart; i <= iEnd; i++) {
        for (int j = jStart; j <= jEnd; j++) {
            if (A[i-1] == B[j-1])
                matrix[i][j] = matrix[i-1][j-1]+1;
            else {
                matrix[i][j] = matrix[i-1][j] > matrix[i][j-1] ? matrix[i-1][j] : matrix[i][j-1];
            }
        }
    }
}

int main(int argc , char * argv[]){
    double startTime, endTime, runtime1, runtime2;

    if (argc < 3) {
        cout << "Usage: ./LCS workers string1 string2" <<endl;
        exit(0);
    }

    __cilkrts_set_param("nworkers", argv[1]);
    string first(argv[2]);
    string second(argv[3]);
    string& A = first;
    string& B = second;

    cout<<"Using "<<__cilkrts_get_nworkers()<<" workers."<<endl;

    startTime = __cilkview_getticks();
    int serialLCS = serial_LCS(A, B);
    endTime = __cilkview_getticks();
    runtime1 = (endTime - startTime) / 1.f;

    startTime = __cilkview_getticks();
    int parallelLCS = parallel_LCS(A, B);
    endTime = __cilkview_getticks();
    runtime2 = (endTime - startTime) / 1.f;

    cout << "Length of Serial LCS: " << serialLCS;
    cout << "  |  Length of Parallel LCS: " << parallelLCS << "." << endl;
    cout << "Serial LCS took: " << runtime1 << " mseconds." << endl;
    cout << "Cilk Parallel LCS took: " << runtime2 << " mseconds." << endl;
    cout << "Total Cilk Speedup = " << runtime1/runtime2 << "." << endl;

    return 0;
}
