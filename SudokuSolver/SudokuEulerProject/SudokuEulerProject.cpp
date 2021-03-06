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
// SudokuEulerProject.cpp
// SudokuEulerProject
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
    try {
        if (argc > 1) {
            string row_str;
            vector<UShort> row_nums[GRID_WIDTH];
            ifstream puzzle_stream;
            
            puzzle_stream.open(argv[1]);
            UShort puzzle_count = 0;
            UShort success_count = 0;
            UShort line_count = 0;
            while (getline(puzzle_stream, row_str)) {
                if (row_str.compare(0, 4, "Grid") == 0) {
                    puzzle_count++;
                    cout << "PUZZLE: " << puzzle_count << endl;
                    continue;
                }
                
                ParseRow(row_str, row_nums[line_count]);
                if (row_nums[line_count].size() != GRID_WIDTH) {
                    cout << "Invalid input!";
                    return EXIT_FAILURE;
                }
                if (++line_count == GRID_WIDTH) {
                    line_count = 0;
                    
                    Grid grid;
                    
                    for (UShort i = 0; i < GRID_WIDTH; i++) {
                        for (UShort j = 0; j < GRID_WIDTH; j++) {
                            grid.UpdateCell(i, j, row_nums[i][j]);
                        }
                        
                        row_nums[i].clear();
                    }
                    
                    grid.DisplayGrid();
                    
                    if (grid.Solve()) {
                        cout << endl << "Solution Successful!" << endl;
                        grid.DisplayGrid();
                        cout << endl;
                        success_count++;
                    }
                    else {
                        cout << endl << "Solution incomplete!" << endl;
                        grid.DisplayGrid();
                        cout << endl;
                    }
                }
            }
            
            cout << success_count << "/" << puzzle_count << " completed successfully!" << endl;
            
            return EXIT_SUCCESS;
        }
    }
    catch (const runtime_error& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

// Parse the row numbers of the grid from the input string
void ParseRow(const string& row_str, vector<UShort>& row_nums) {
    for (auto c : row_str) {
        if (isdigit(c)) {
            row_nums.push_back(c - '0');
        }
        else if (isalpha(c)) {
            if (c == 'x' || c == 'X') {
                row_nums.push_back(0);
            }
        }
    }
}
