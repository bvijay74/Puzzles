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
//  Grid.h
//  SudokuAlgorithm
//

#ifndef Grid_h
#define Grid_h

#include <array>
#include <memory>

#include "SudokuAlgorithm.h"
#include "Cell.h"
#include "Row.h"
#include "Column.h"
#include "Block.h"

namespace SudokuAlgorithm {
    class Grid final {
    public:
        Grid() : cells_{} {
            Initialize();
        }
        
        ~Grid() = default;
        
		// Get the number in the given cell addressed by its row and column
		UShort GetCellNumber(UShort row, UShort column) const {
			if (row >= GRID_WIDTH || column >= GRID_WIDTH) {
				throw std::out_of_range("Grid:: GetCellNumber");
			}

			return cells_[row*GRID_WIDTH + column]->GetNumber();
		}

        // Set the number to the cell at the given row, column
        void UpdateCell(UShort row, UShort column, UShort num);
        
        // Display the numbers in the grid
        void DisplayGrid() const;
        
        // Solve the puzzle
        bool Solve();
		// Solve the candidates by identifying X Wing pattern in the grid
		bool SolveXWing();

    private:
        // Allocate the arrays and initialize the cross references
        void Initialize();
        
        std::array<std::shared_ptr<Cell>, NUM_GRID_CELLS> cells_;
        std::array<std::shared_ptr<Row>, GRID_WIDTH> rows_;
        std::array<std::shared_ptr<Column>, GRID_WIDTH> columns_;
        std::array<std::shared_ptr<Block>, GRID_WIDTH> blocks_;
    };
}

#endif /* Grid_h */
