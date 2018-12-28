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
//  Cell.cpp
//  SudokuAlgorithm
//

#include "Cell.h"

namespace SudokuAlgorithm {
    UShort Cell::cell_count = 0;
    
    Cell::Cell() {
		// Cell object creation is tracked to assign row and column numbers
        row_ = cell_count/GRID_WIDTH;
        column_ = cell_count%GRID_WIDTH;
        cell_count++;
        if (cell_count == NUM_GRID_CELLS) {
            cell_count = 0;
        }
        empty_ = true;
    }
}
