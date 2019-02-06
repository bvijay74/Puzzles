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
//  Row.h
//  SudokuAlgorithm
//

#ifndef Row_h
#define Row_h

#include <array>
#include <memory>

#include "SudokuAlgorithm.h"
#include "Segment.h"

namespace SudokuAlgorithm {
	// Row in a Sudoku puzzle grid
    class Row final : public Segment {
    public:
        Row(UShort index) : Segment(index) {
        }
        
        ~Row() = default;
        
		// Add reference to an intersecting column
        void AddColumnRef(const std::shared_ptr<Segment>& column) {
            column_refs_[column->GetIndex()] = column;
        }
        
		// Add reference to an intersecting block
        void AddBlockRef(const std::shared_ptr<Segment>& block) {
            if (index_/block_refs_.size() != block->GetIndex()/block_refs_.size()) {
                throw std::invalid_argument("block");
            }
            
            block_refs_[block->GetIndex()%block_refs_.size()] = block;
        }
        
		// Prepare the row for the solution
		virtual void Initialize() override;
		// Solve visible and hidden singles
		virtual bool SolveSingles() override;
		// Solve intersections or pointing pairs
		virtual bool SolveIntersections() override;

    private:
		// References to the columns and blocks that the row intersects
        std::array<std::weak_ptr<Segment>, GRID_WIDTH> column_refs_;
        std::array<std::weak_ptr<Segment>, GRID_WIDTH/BLOCK_WIDTH> block_refs_;
    };
}

#endif /* Row_h */
