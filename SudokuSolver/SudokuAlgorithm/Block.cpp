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
//  Block.cpp
//  SudokuAlgorithm
//

#include "Block.h"

using namespace std;

namespace SudokuAlgorithm {
	// Prepare the block for the solution
	void Block::Initialize() {
		Segment::Initialize();

		// Mark the candidates based on the numbers present in the intersecting rows and columns
		for (UShort n : unsolved_nums_) {
			for (UShort i=0; i<cells_.size(); i++) {
				auto cell = cells_[i].lock();
				if (cell->IsEmpty()) {
					if (!row_refs_[i/row_refs_.size()].lock()->FindNumber(n) &&
						!column_refs_[i%column_refs_.size()].lock()->FindNumber(n)) {
						cell->GetMarking().Mark(n);
					}
				}
			}
		}
	}

	// Solve visible and hidden singles
	bool Block::SolveSingles() {
		bool solved = false;

		for (UShort i=0; i<cells_.size(); i++) {
			auto cell = cells_[i].lock();
			Marking& marking = cell->GetMarking();
			UShort n;

			// Update solved number to the cell if only one candidate is marked for the cell
			if (cell->IsEmpty() && (n = marking.GetResolvedNumber()) != 0) {
				*cell = n;

				UpdateSolvedNumber(n);
				row_refs_[i/row_refs_.size()].lock()->UpdateSolvedNumber(n);
				column_refs_[i%row_refs_.size()].lock()->UpdateSolvedNumber(n);

				solved = true;
			}
		}

		vector<UShort> nums(unsolved_nums_);
		// Solve hidden single
		for (UShort n : nums) {
			UShort mark_index = 0;
			UShort mark_count = 0;

			// Count the candidate's occurances in the block
			for (UShort i=0; i<cells_.size(); i++) {
				auto cell = cells_[i].lock();
				if (cell->IsEmpty()) {
					if (cell->GetMarking().IsMarked(n)) {
						mark_index = i;
						mark_count++;
						if (mark_count > 1) {
							break;
						}
					}
				}
			}

			// Update the solution if a candidate occurs only once in a block
			if (mark_count == 1) {
				auto cell = cells_[mark_index].lock();
				*cell = n;
				UpdateSolvedNumber(n);
				row_refs_[mark_index/row_refs_.size()].lock()->UpdateSolvedNumber(n);
				column_refs_[mark_index%row_refs_.size()].lock()->UpdateSolvedNumber(n);

				solved = true;
			}
		}

		return solved;
	}
    
	// Solve intersections or pointing pairs
	bool Block::SolveIntersections() {
		bool solved = false;

		for (UShort n : unsolved_nums_) {
			if (GetCandidateCount(n) > 2) {
				UShort num_count = 0;
				auto aligned_row = -1;
				for (UShort i=0; i<cells_.size(); i++) {
					auto cell = cells_[i].lock();
					if (cell->IsEmpty() && cell->GetMarking().IsMarked(n)) {
						num_count++;
					}
					// Find the row intersecting the block in which the candidate is aligned
					// The row does not have any occurance of the candidate apart from those aligned to this block
					if ((i + 1) % BLOCK_WIDTH == 0) {
						if (num_count > 1 && num_count == row_refs_[i / BLOCK_WIDTH].lock()->GetCandidateCount(n)) {
							aligned_row = i / BLOCK_WIDTH;
							break;
						}
						num_count = 0;
					}
				}
				
				if (aligned_row >= 0) {
					// Erase the candidate occurances in the block,
					// where the rows have the candidate occurances outside the block
					for (UShort i=0; i<cells_.size(); i++) {
						if (i / BLOCK_WIDTH != aligned_row) {
							auto cell = cells_[i].lock();
							Marking& marking = cell->GetMarking();
							if (cell->IsEmpty() && marking.IsMarked(n)) {
								marking.Erase(n);
								solved = true;
							}
						}
					}
				}
			}

			if (!solved) {
				// solve column intersections in the block if row intersections are not solved
				array<UShort, BLOCK_WIDTH> num_counts = { 0 };

				for (UShort i=0; i<cells_.size(); i++) {
					auto cell = cells_[i].lock();
					if (cell->IsEmpty() && cell->GetMarking().IsMarked(n)) {
						num_counts[i%BLOCK_WIDTH]++;
					}
				}

				auto aligned_column = -1;
				for (UShort column=0; column<BLOCK_WIDTH; column++) {
					// Find the column intersecting the block in which the candidate is aligned
					// The column does not have any occurance of the candidate apart from those aligned to this block
					if (num_counts[column] > 1 &&
						num_counts[column] == column_refs_[column].lock()->GetCandidateCount(n)) {
						aligned_column = column;
						break;
					}
				}

				if (aligned_column >= 0) {
					// Erase the candidate occurances in the block,
					// where the columns have the candidate occurances outside the block
					for (UShort i=0; i<cells_.size(); i++) {
						if (i % BLOCK_WIDTH != aligned_column) {
							auto cell = cells_[i].lock();
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
