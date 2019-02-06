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
//  Cell.h
//  SudokuAlgorithm
//

#ifndef Cell_h
#define Cell_h

#include "SudokuAlgorithm.h"
#include "Marking.h"

namespace SudokuAlgorithm {
	// Defines the cell in the Sudoku puzzle grid
    class Cell final {
    public:
        Cell();
        Cell(const Cell&) = delete; // cell cannot be copied
        
		// Assign number to the cell
        void operator= (UShort num) {
            number_ = num;
			if (num == 0) {
				empty_ = true;
			} else {
				empty_ = false;
				marking_.EraseAll();
			}
        }
        
        UShort GetRow() const {
            return row_;
        }
        
        UShort GetColumn() const {
            return column_;
        }
        
        UShort GetNumber () const {
            return number_;
        }
        
        bool IsEmpty() const {
            return empty_;
        }
        
        Marking& GetMarking() {
            return marking_;
        }
        
    private:
        static UShort cell_count;
        
        UShort row_;
        UShort column_;
        UShort number_;
        bool empty_;
        Marking marking_;
    };
}

#endif /* Cell_h */
