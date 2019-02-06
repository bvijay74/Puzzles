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
//  Grid.cpp
//  SudokuAlgorithm
//

#include <iostream>

#include "Grid.h"

using namespace std;

namespace SudokuAlgorithm {
    // Allocate the arrays and initialize the cross references
    void Grid::Initialize() {
        for (auto i=0; i<NUM_GRID_CELLS ;i++) {
            cells_[i] = make_shared<Cell>();
        }
        
        for (UShort n=0; n<GRID_WIDTH ;n++) {
            rows_[n] = make_shared<Row>(n);
            
            columns_[n] = make_shared<Column>(n);
            
            blocks_[n] = make_shared<Block>(n);
        }
        
        // Initialize each row, column and block with their respective cells
        for (auto cell : cells_) {
            Row& row = *rows_.at(cell->GetRow());
            row[cell->GetColumn()] = cell;
            
            Column& col = *columns_.at(cell->GetColumn());
            col[cell->GetRow()] = cell;
            
            Block& block = *blocks_.at(((cell->GetRow()/BLOCK_WIDTH)*BLOCK_WIDTH) + (cell->GetColumn()/BLOCK_WIDTH));
            auto block_row = cell->GetRow() - (block.GetIndex()/BLOCK_WIDTH)*BLOCK_WIDTH;
            auto block_column = cell->GetColumn() % BLOCK_WIDTH;
            block[block_row*BLOCK_WIDTH+block_column] = cell;
        }
        
        // Initialize the rows<->columns cross references
        for (auto row : rows_) {
            for (auto column : columns_) {
                row->AddColumnRef(column);
                column->AddRowRef(row);
            }
        }
        
        // Initialize the blocks<->rows and blocks<->columns cross references
        for (auto block : blocks_) {
            for (auto row : rows_) {
                if (block->GetIndex()/BLOCK_WIDTH == row->GetIndex()/BLOCK_WIDTH) {
                    block->AddRowRef(row);
                    row->AddBlockRef(block);
                }
            }
            for (auto column : columns_) {
                if (block->GetIndex()%BLOCK_WIDTH == column->GetIndex()/BLOCK_WIDTH) {
                    block->AddColumnRef(column);
                    column->AddBlockRef(block);
                }
            }
        }
    }
    
    // Set the number to the cell at the given row, column
    void Grid::UpdateCell(UShort row, UShort column, UShort num) {
        if (row >= GRID_WIDTH || column >= GRID_WIDTH) {
            throw std::out_of_range("Grid::UpdateCell");
        }
        
        Cell& cell = *cells_[row*GRID_WIDTH+column];
        cell = num;
    }
    
    // Display the numbers in the grid
    void Grid::DisplayGrid() const {
        auto row_separator = " ----- ----- ----- \n";
        
        for (auto cell : cells_) {
            if (cell->GetColumn() == 0 &&
                cell->GetRow() % 3 == 0) {
                    cout << row_separator;
            }
            
            if (cell->GetColumn() % 3 == 0) {
                cout << "|";
            } else {
                cout << " ";
            }
            
            if (cell->IsEmpty()) {
                cout << "X";
            } else {
                cout << cell->GetNumber();
            }
            
            if (cell->GetColumn() == 8) {
                cout << "|" << endl;
                if (cell->GetRow() == 8) {
                    cout << row_separator;
                }
            }
        }
    }
    
    // Solve the puzzle
    bool Grid::Solve() {
        vector<shared_ptr<Segment>> unfilled_segs;
        
        auto erase_filled_segments = [] (vector<shared_ptr<Segment>>& unfilled_segs) {
			auto itr = unfilled_segs.begin();
            while (itr != unfilled_segs.end()) {
                auto seg = *itr;
                if (seg->IsFilled()) {
                    itr = unfilled_segs.erase(itr);
                } else {
                    itr++;
                }
            }
        };
        
		// Intialize the row, column, block cross references
        for (UShort n=0; n<GRID_WIDTH; n++) {
            rows_[n]->Initialize();
            
            columns_[n]->Initialize();
            
            blocks_[n]->Initialize();
        }

		// Create the list of segments not filled
        for (auto seg : rows_) {
            if (!seg->IsFilled()) {
                unfilled_segs.push_back(seg);
            }
        }
        for (auto seg : columns_) {
            if (!seg->IsFilled()) {
                unfilled_segs.push_back(seg);
            }
        }
        for (auto seg : blocks_) {
            if (!seg->IsFilled()) {
                unfilled_segs.push_back(seg);
            }
        }
        
		while (unfilled_segs.size() > 0) {
			sort(begin(unfilled_segs), end(unfilled_segs),
				[](shared_ptr<Segment> sega, shared_ptr<Segment> segb)
			{
				return sega->GetEmptyCellCount() < segb->GetEmptyCellCount();
			});

			auto solved = false;
			// Solve visible and hidden singles first
			for (auto seg : unfilled_segs) {
				if (seg->SolveSingles()) {
					solved = true;
				}
			}

			if (solved) {
				erase_filled_segments(unfilled_segs);
			}

			// Solve intersections, visible subsets and hidden subsets
			for (auto seg : unfilled_segs) {
				if (seg->SolveIntersections()) {
					solved = true;
				}
			}

			for (auto seg : unfilled_segs) {
				if (seg->SolveVisibleSubsets()) {
					solved = true;
				}
			}
		
			for (auto seg : unfilled_segs) {
				if (seg->SolveHiddenSubsets()) {
					solved = true;
				}
			}

			// If simple methods does not find any solution, solve X Wing
			if (!solved) {
				solved = SolveXWing();
			}

			if (solved) {
				continue;
			} else {
				break;
			}
        }
        
        return unfilled_segs.size() == 0;
    }

	// Solve the candidates by identifying X Wing pattern in the grid
	bool Grid::SolveXWing() {
		auto solved = false;

		for (UShort n=1; n<=NUM_BASE; n++) {
			int x1, x2, y1, y2;

			// Find if the candidate is present in a X Wing pattern,
			// and the rows forming the pattern does not have the candidate
			// apart from the cells forming the pattern
			x1 = x2 = y1 = y2 = -1;
			for (UShort row = 0; row<GRID_WIDTH; row++) {
				if (!rows_[row]->IsFilled() && rows_[row]->GetCandidateCount(n) == 2) {
					y1 = row;
					break;
				}
			}
			if (y1 < 0) {
				continue;
			}

			for (UShort column=0; column<GRID_WIDTH; column++) {
				auto cell = (*rows_[y1])[column].lock();
				if (cell->IsEmpty() && cell->GetMarking().IsMarked(n)) {
					if (x1 < 0) {
						x1 = column;
					} else {
						x2 = column;
						break;
					}	
				}
			}

			if (x1 >= 0 && x2 >= 0) {
				for (UShort row=0; row<GRID_WIDTH; row++) {
					if (row == y1) {
						continue;
					}

					if (!rows_[row]->IsFilled() && rows_[row]->GetCandidateCount(n) == 2) {
						auto cell_y2x1 = (*rows_[row])[x1].lock();
						auto cell_y2x2 = (*rows_[row])[x2].lock();

						if ((cell_y2x1->IsEmpty() && cell_y2x2->IsEmpty()) &&
							(cell_y2x1->GetMarking().IsMarked(n) && cell_y2x2->GetMarking().IsMarked(n))) {
							y2 = row;
							break;
						}
					}
				}
			}

			if (y2 > 0) {
				// Erase the outlying candidates in the rows which intersect the X Wing
				for (UShort row=0; row<GRID_WIDTH; row++) {
					if (row == y1 || row == y2) {
						continue;
					}
					auto cell_x1 = (*rows_[row])[x1].lock();
					auto cell_x2 = (*rows_[row])[x2].lock();
					if ((cell_x1->IsEmpty() && cell_x2->IsEmpty()) &&
						(cell_x1->GetMarking().IsMarked(n) && cell_x2->GetMarking().IsMarked(n))) {
						cell_x1->GetMarking().Erase(n);
						cell_x2->GetMarking().Erase(n);
						solved = true;
					}
				}

				if (!solved) {
					// Erase the outlying candidates in the columns which intersect the X Wing
					for (UShort column=0; column<GRID_WIDTH; column++) {
						if (column == x1 || column == x2) {
							continue;
						}
						auto cell_y1 = (*columns_[column])[y1].lock();
						auto cell_y2 = (*columns_[column])[y2].lock();
						if ((cell_y1->IsEmpty() && cell_y2->IsEmpty()) &&
							(cell_y1->GetMarking().IsMarked(n) && cell_y2->GetMarking().IsMarked(n))) {
							cell_y1->GetMarking().Erase(n);
							cell_y2->GetMarking().Erase(n);
							solved = true;
						}
					}
				}
			}
		}

		return solved;
	}
}
