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
//  Segment.h
//  SudokuAlgorithm
//

#ifndef Segment_h
#define Segment_h

#include <array>
#include <vector>
#include <memory>

#include "SudokuAlgorithm.h"
#include "Cell.h"

namespace SudokuAlgorithm {
	// Base class for the segment types in the grid - row, column and block
    class Segment {
    public:
        Segment(UShort index)  : index_(index), filled_(false) {
        }
        
        UShort GetIndex() const {
            return index_;
        }
        
        bool IsFilled() const {
            return filled_;
        }
        
        UShort GetEmptyCellCount() const {
            return static_cast<UShort>(unsolved_nums_.size());
        }
        
		// Refer the cell in a segment
		std::weak_ptr<Cell>& operator[] (UShort index);
	
		// Count the candidate's marking in a segment
        UShort GetCandidateCount(UShort num) const;
		// Update the solved number in the segment and its cell markings
        void UpdateSolvedNumber(UShort number);
		// Find if the number is solved in the given segment
        bool FindNumber(UShort number);

		// Solve visible pairs, triples, quads etc.,
        bool SolveVisibleSubsets();
		// Solve hidden pairs,triples, quads etc.,
		bool SolveHiddenSubsets();

		// Intialize is called to prepare the segment for the solution
		virtual void Initialize();
		// Solve visible and hidden singles
		virtual bool SolveSingles() = 0;
		// Solve intersections or pointing pairs
		virtual bool SolveIntersections() = 0;

    protected:
        UShort index_;
        bool filled_;
        
        std::array<std::weak_ptr<Cell>, GRID_WIDTH> cells_;
        std::vector<UShort> unsolved_nums_;
    };
}

#endif /* Segment_h */
