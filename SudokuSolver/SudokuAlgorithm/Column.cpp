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
//  Column.cpp
//  SudokuAlgorithm
//

#include "Column.h"

using namespace std;

namespace SudokuAlgorithm {
	// Prepares the column for the solution
	void Column::Initialize() {
		Segment::Initialize();

		// Mark the candidates based on the numbers present in the intersecting rows and blocks
		for (UShort n : unsolved_nums_) {
			for (UShort row=0; row<cells_.size(); row++) {
				auto cell = cells_[row].lock();
				if (cell->IsEmpty()) {
					if (!row_refs_[row].lock()->FindNumber(n) &&
						!block_refs_[row/block_refs_.size()].lock()->FindNumber(n)) {
						cell->GetMarking().Mark(n);
					}
				}
			}
		}
	}

	// Solve visible and hidden singles
	bool Column::SolveSingles() {
		bool solved = false;

		for (UShort row=0; row<cells_.size(); row++) {
			auto cell = cells_[row].lock();
			Marking& marking = cell->GetMarking();
			UShort n;

			// Update solved number to the cell if only one candidate is marked for the cell
			if (cell->IsEmpty() && (n = marking.GetResolvedNumber()) != 0) {
				*cell = n;

				// Update the solution in the column and the intersecting rows and blocks
				UpdateSolvedNumber(n);
				row_refs_[row].lock()->UpdateSolvedNumber(n);
				block_refs_[row / block_refs_.size()].lock()->UpdateSolvedNumber(n);

				solved = true;
			}
		}

		vector<UShort> nums(unsolved_nums_);
		// Solve hidden single
		for (UShort n : nums) {
			UShort mark_row = 0;
			UShort mark_count = 0;

			// Count the candidate's occurances in the column
			for (UShort row = 0; row < cells_.size(); row++) {
				auto cell = cells_[row].lock();
				if (cell->IsEmpty()) {
					if (cell->GetMarking().IsMarked(n)) {
						mark_row = row;
						mark_count++;
						if (mark_count > 1) {
							break;
						}
					}
				}
			}

			// Update the solution if a candidate occurs only once in a column
			if (mark_count == 1) {
				auto cell = cells_[mark_row].lock();
				*cell = n;
				UpdateSolvedNumber(n);
				row_refs_[mark_row].lock()->UpdateSolvedNumber(n);
				block_refs_[mark_row / block_refs_.size()].lock()->UpdateSolvedNumber(n);

				solved = true;
			}
		}

		return solved;
	}
    
	// Solve intersections or pointing pairs
    bool Column::SolveIntersections() {
		bool solved = false;

		for (UShort n : unsolved_nums_) {
			if (GetCandidateCount(n) > 2) {
				UShort num_count = 0;
				auto aligned_block = -1;
				for (UShort row = 0; row < cells_.size(); row++) {
					auto cell = cells_[row].lock();
					if (cell->IsEmpty() && cell->GetMarking().IsMarked(n)) {
						num_count++;
					}
					// Find the block intersecting the column in which the candidate is aligned
					// The block does not have any occurance of the candidate apart from those aligned to this column
					if ((row + 1) % BLOCK_WIDTH == 0) {
						if (num_count > 1 && num_count == block_refs_[row / BLOCK_WIDTH].lock()->GetCandidateCount(n)) {
							aligned_block = row / BLOCK_WIDTH;
							break;
						}
						num_count = 0;
					}
				}

				if (aligned_block >= 0) {
					// Erase the candidate occurances in the column which are not aligned in a block
					for (UShort row = 0; row<cells_.size(); row++) {
						if (row / BLOCK_WIDTH != aligned_block) {
							auto cell = cells_[row].lock();
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
