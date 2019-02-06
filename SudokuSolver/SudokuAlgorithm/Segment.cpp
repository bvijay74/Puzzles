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
//  Segment.cpp
//  SudokuAlgorithm
//

#include "Segment.h"

using namespace std;

namespace SudokuAlgorithm {
	// Intialize is called to prepare the segment for the solution
    void Segment::Initialize() {
        unsolved_nums_.clear();
        
		// Add the numbers not present in the cells of the segment 
		// to the unsolved numbers list
        for (UShort n=1; n<=NUM_BASE; n++) {
            if (find_if(begin(cells_), end(cells_),
                    [n](const weak_ptr<Cell>& cell)
                        { return n == cell.lock()->GetNumber();}) == end(cells_)) {
                unsolved_nums_.push_back(n);
            }
        }
        
        if (unsolved_nums_.empty()) {
            filled_ = true;
        }
    }
    
	// Count the candidate's marking in a segment
    UShort Segment::GetCandidateCount(UShort num) const {
        UShort count = 0;
        
        for (auto cell : cells_) {
            if (cell.lock()->GetMarking().IsMarked(num)) {
                count++;
            }
        }
        
        return count;
    }
    
	// Refer the cell in a segment
    weak_ptr<Cell>& Segment::operator[] (UShort index) {
        if (index >= cells_.size()) {
            throw out_of_range("Segment[]");
		}

		return cells_[index];
	}

	// Update the solved number in the segment and its cell markings
	void Segment::UpdateSolvedNumber(UShort number) {
		auto itr = find(begin(unsolved_nums_), end(unsolved_nums_), number);
		if (itr != end(unsolved_nums_)) {
			unsolved_nums_.erase(itr);
		}
		if (unsolved_nums_.empty()) {
			filled_ = true;
		}

		for (auto cell : cells_) {
			if (cell.lock()->IsEmpty()) {
				cell.lock()->GetMarking().Erase(number);
			}
		}
	}

	// Find if the number is solved in the given segment
	bool Segment::FindNumber(UShort number) const {
		return find_if(begin(cells_), end(cells_),
					[number](const weak_ptr<Cell>& cell)
						{ return number == cell.lock()->GetNumber(); }) != end(cells_);
	}

	// Solve visible pairs, triples, quads etc.,
	bool Segment::SolveVisibleSubsets() {
		auto solved = false;

		for (UShort i=0; i<GRID_WIDTH; i++) {
			auto cell = cells_[i].lock();
			if (!cell->IsEmpty()) {
				continue;
			}

			// Count the number of occurances of the same set of candidates
			vector<UShort> target_indices;
			UShort count = 1;
			Marking subset = cell->GetMarking();
			for (UShort j=i+1; j<GRID_WIDTH; j++) {
				if (cells_[j].lock()->IsEmpty()) {
					if (subset == cells_[j].lock()->GetMarking()) {
						count++;
					} else {
						target_indices.push_back(j);
					}
				}
			}

			if (count > 1 && count == subset.GetCount()) {
				// Set of candidate occurances match the number of candidates in the set
				// So, the candiates in the set can be eliminated from other cells in the segment
				for (auto ti : target_indices) {
					Marking& marking = cells_[ti].lock()->GetMarking();
					if (marking.Erase(subset)) {
						solved = true;
					}
				}
			}

			if (solved) {
				break;
			}
		}

		return solved;
	}

	// Solve hidden pairs,triples, quads etc.,
	bool Segment::SolveHiddenSubsets() {
		bool solved = false;

		vector<weak_ptr<Cell>> empty_cells;
		for (UShort i=0; i<GRID_WIDTH; i++) {
			auto cell = cells_[i].lock();
			if (cell->IsEmpty()) {
				empty_cells.push_back(cell);
			}
		}

		Marking subset;
		UShort least_marking = GRID_WIDTH + 1;
		for (UShort n=1; n<=NUM_BASE; n++) {
			UShort marking_count = 0;
			for (auto cell : empty_cells) {
				if (cell.lock()->GetMarking().IsMarked(n)) {
					if (++marking_count > least_marking) {
						break;
					}
				}
			}
			if (marking_count != 0) {
				// The least marked candidates in the segment could probably form a hidden subset
				if (marking_count == least_marking) {
					subset.Mark(n);
				}
				if (marking_count < least_marking) {
					least_marking = marking_count;
					subset.EraseAll();
					subset.Mark(n);
				}
			}
		}

		// Elimnate the candidates which are already locked pairs, triples etc.,
		for (auto i=0; i<empty_cells.size()-1; i++) {
			Marking m1 = empty_cells[i].lock()->GetMarking();
			UShort count = 1;
			for (auto j=i+1; j<empty_cells.size(); j++) {
				Marking m2 = empty_cells[j].lock()->GetMarking();
				if (m1 == m2) {
					count++;
				}
			}
			if (count > 1 && count == m1.GetCount()) {
				subset.Erase(m1);
			}
		}

		if (subset.GetCount() != 0) {
			if (subset.GetCount() > least_marking) {
				// The least occuring candidates could be distributed across cells and may not form hidden subset
				// Find out if a set of candidates appear in multiple cells
				for (auto i=0; i<empty_cells.size()-1; i++) {
					Marking s1 = subset.MatchSubset(empty_cells[i].lock()->GetMarking());
					if (s1.GetCount() < 2) {
						continue;
					}
					UShort count = 1;
					for (auto j=i+1; j<empty_cells.size(); j++) {
						Marking s2 = subset.MatchSubset(empty_cells[j].lock()->GetMarking());
						if (s1 == s2) {
							count++;
					 	}
					}
					if (count > 1 && count == s1.GetCount()) {
						subset = s1;
						break;
					}
				}
			}

			if (subset.GetCount() < empty_cells.size()) {
				vector<UShort> subset_indices;
				for (UShort i=0; i<GRID_WIDTH; i++) {
					auto cell = cells_[i].lock();
					if (cell->IsEmpty()) {
						if (!subset.MatchSubset(cell->GetMarking()).IsEmpty()) {
							subset_indices.push_back(i);
							if (subset_indices.size() > subset.GetCount()) {
								break;
							}
						}
					}
				}

				// If the subset occurs in as many cells as the size of the subset, 
				// eliminate other candidates in those cells
				if (subset_indices.size() == subset.GetCount()) {
					for (auto index : subset_indices) {
						Marking& marking = cells_[index].lock()->GetMarking();
						if (marking != subset) {
							marking.Intersect(subset);
							solved = true;
						}
					}
				}
			}
		}

		return solved;
	}
}
