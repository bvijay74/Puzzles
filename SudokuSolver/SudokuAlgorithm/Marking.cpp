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
//  Marking.cpp
//  SudokuAlgorithm
//

#include "Marking.h"

namespace SudokuAlgorithm {
	// Count the number of candidates
    UShort Marking::GetCount() {
        UShort count = 0;
        
        for (UShort n=0; n<NUM_BASE; n++) {
            if (candidates_.numbers_ & (1 << n)) {
                count++;
            }
        }
        
        return count;
    }
    
	// Erase the candidates based on the given subset
    bool Marking::Erase(const Marking& subset) {
        UShort n = candidates_.numbers_ & subset.candidates_.numbers_;
        n ^= candidates_.numbers_;
        if (n != 0 && n != candidates_.numbers_) {
            candidates_.numbers_ = n;
			return true;
        }

		return false;
    }
    
	// Return the number if only one candidate is present
    UShort Marking::GetResolvedNumber() const {
		UShort num = 0;
		UShort count = 0;

        for (UShort n=0; n<NUM_BASE; n++) {
            if (candidates_.numbers_ & (1 << n)) {
                num = n+1;
				count++;
            }
			if (count > 1) {
				num = 0;
				break;
			}
        }
        
		return num;
    }
}
