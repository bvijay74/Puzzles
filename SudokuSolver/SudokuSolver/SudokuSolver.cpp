// Copyright 2018 Vijayakumar Balakrishnan
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//
//  SudokuSolver.cpp
//  SudokuSolver
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "Grid.h"

using namespace std;

using namespace SudokuAlgorithm;

void ParseRow(const string& row_str, vector<UShort>& row_nums);

int main(int argc, char* argv[]) {
    string row_str;
    vector<UShort> row_nums[GRID_WIDTH];
	
    cout << "Enter the numbers row-wise." << endl;
    cout << "Enter X for blank cells." << endl;
    cout << "For example:" << endl;
    cout << "X 3 1 6 X 7 X X X" << endl;
    cout << "6 X X 8 X X 2 5 7" << endl;
    cout << "8 X X X 9 X 6 X 3" << endl;
    cout << "4 X X X X X 8 3 2" << endl;
    cout << "X 1 X X 6 9 X X X" << endl;
    cout << "7 X 3 2 4 X X X 6" << endl;
    cout << "9 X 2 4 X 1 X 7 8" << endl;
    cout << "X 8 5 X X X X X 9" << endl;
    cout << "3 X 4 X X X X 6 1" << endl << endl;
    
    for (UShort i=0; i<GRID_WIDTH; i++) {
		getline(cin, row_str);
        ParseRow(row_str, row_nums[i]);
        if (row_nums[i].size() != GRID_WIDTH) {
            cout << "Invalid input!";
            return EXIT_FAILURE;
        }
    }
    
    try {
        Grid grid;
        
        for (UShort i=0; i<GRID_WIDTH; i++) {
            for (UShort j=0; j<GRID_WIDTH; j++) {
                grid.UpdateCell(i, j, row_nums[i][j]);
            }
        }
        
        if (grid.Solve()) {
            cout << endl << "Solution Successful!" << endl;
            grid.DisplayGrid();
        } else {
            cout << endl << "Solution incomplete!" << endl;
            grid.DisplayGrid();
            return EXIT_FAILURE;
        }
    }
    catch (const runtime_error& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

// Parse the row numbers of the grid from the input string
void ParseRow(const string& rowStr, vector<UShort>& rowNums) {
	for (auto c : rowStr) {
		if (isdigit(c)) {
			rowNums.push_back(c - '0');
		}
		else if (isalpha(c)) {
			if (c == 'x' || c == 'X') {
				rowNums.push_back(0);
			}
		}
	}
}
