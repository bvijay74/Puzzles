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
//  Block.h
//  SudokuAlgorithm
//

#ifndef Block_h
#define Block_h

#include <array>
#include <memory>

#include "SudokuAlgorithm.h"
#include "Segment.h"

namespace SudokuAlgorithm {
	// Block in a Sudoku puzzle grid
    class Block final : public Segment {
    public:
        Block(UShort index) : Segment(index) {
        }
        
		// Add reference to an intersecting row
        void AddRowRef(const std::shared_ptr<Segment>& row) {
            if (index_/row_refs_.size() != row->GetIndex()/row_refs_.size()) {
                throw std::invalid_argument("row");
            }
            
            row_refs_[row->GetIndex()%row_refs_.size()] = row;
        }
        
		// Add reference to an intersecting column
        void AddColumnRef(const std::shared_ptr<Segment>& column) {
            if (index_%column_refs_.size() != column->GetIndex()/column_refs_.size()) {
                throw std::invalid_argument("column");
            }
            
            column_refs_[column->GetIndex()%column_refs_.size()] = column;
        }
        
		// Prepare the block for the solution
		virtual void Initialize() override;
		// Solve visible and hidden singles
		virtual bool SolveSingles() override;
		// Solve intersections or pointing pairs
		virtual bool SolveIntersections() override;

    private:
		// References to the rows and columns that the block intersects
        std::array<std::weak_ptr<Segment>, (GRID_WIDTH/BLOCK_WIDTH)> row_refs_;
        std::array<std::weak_ptr<Segment>, (GRID_WIDTH/BLOCK_WIDTH)> column_refs_;
    };
}

#endif /* Block_h */
