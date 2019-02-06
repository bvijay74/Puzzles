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
//  Row.cpp
//  SudokuAlgorithm
//

#include "Row.h"

using namespace std;

namespace SudokuAlgorithm {
	// Prepares the row for the solution
	void Row::Initialize() {
		Segment::Initialize();

		// Mark the candidates based on the numbers present in the intersecting columns and blocks
		for (auto n : unsolved_nums_) {
			for (auto column = 0; column<cells_.size(); column++) {
				auto cell = cells_[column].lock();
				if (cell->IsEmpty()) {
					if (!column_refs_[column].lock()->FindNumber(n) &&
						!block_refs_[column/block_refs_.size()].lock()->FindNumber(n)) {
						cell->GetMarking().Mark(n);
					}
				}
			}
		}
	}

	// Solve visible and hidden singles
	bool Row::SolveSingles() {
		bool solved = false;
		
		for (auto column=0; column<cells_.size(); column++) {
			auto cell = cells_[column].lock();
			Marking& marking = cell->GetMarking();
			UShort n;

			// Update solved number to the cell if only one candidate is marked for the cell
			if (cell->IsEmpty() && (n = marking.GetResolvedNumber()) != 0) {
				*cell = n;

				// Update the solution in the row and the intersecting columns and blocks
				UpdateSolvedNumber(n);
				column_refs_[column].lock()->UpdateSolvedNumber(n);
				block_refs_[column / block_refs_.size()].lock()->UpdateSolvedNumber(n);

				solved = true;
			}
		}

		vector<UShort> nums(unsolved_nums_);
		// Solve hidden single
		for (UShort n : nums) {
			UShort mark_col = 0;
			UShort mark_count = 0;

			// Count the candidate's occurances in the row
			for (auto column=0; column<cells_.size(); column++) {
				auto cell = cells_[column].lock();
				if (cell->IsEmpty()) {
					if (cell->GetMarking().IsMarked(n)) {
						mark_col = column;
						mark_count++;
						if (mark_count > 1) {
							break;
						}
					}
				}
			}

			// Update the solution if a candidate occurs only once in a row
			if (mark_count == 1) {
				auto cell = cells_[mark_col].lock();
				*cell = n;
				UpdateSolvedNumber(n);
				column_refs_[mark_col].lock()->UpdateSolvedNumber(n);
				block_refs_[mark_col / block_refs_.size()].lock()->UpdateSolvedNumber(n);

				solved = true;
			}
		}

		return solved;
	}

	// Solve intersections or pointing pairs
    bool Row::SolveIntersections() {
        bool solved = false;
        
        for (auto n : unsolved_nums_) {
			if (GetCandidateCount(n) > 2) {
				UShort num_count = 0;
				auto aligned_block = -1;
				for (auto column=0; column<cells_.size(); column++) {
					auto cell = cells_[column].lock();
					if (cell->IsEmpty() && cell->GetMarking().IsMarked(n)) {
						num_count++;
					}
					// Find the block intersecting the row in which the candidate is aligned
					// The block does not have any occurance of the candidate apart from those aligned to this row
					if ((column + 1) % BLOCK_WIDTH == 0) {
						if (num_count > 1 && num_count == block_refs_[column / BLOCK_WIDTH].lock()->GetCandidateCount(n)) {
							aligned_block = column / BLOCK_WIDTH;
							break;
						}
						num_count = 0;
					}
				}

				if (aligned_block >= 0) {
					// Erase the candidate occurances in the row which are not aligned in a block
					for (auto column = 0; column<cells_.size(); column++) {
						if (column / BLOCK_WIDTH != aligned_block) {
							auto cell = cells_[column].lock();
							Marking& marking = cell->GetMarking();
							if (cell->IsEmpty() && marking.IsMarked(n)) {
								marking.Erase(n);
								solved = true;
							}
						}
					}
				}
			}
        }
        
        return solved;
    }
}
