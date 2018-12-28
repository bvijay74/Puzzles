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
//  Marking.h
//  SudokuAlgorithm
//

#ifndef Marking_h
#define Marking_h

#include "SudokuAlgorithm.h"

namespace SudokuAlgorithm {
    class Marking {
    public:
        Marking() {
            candidates_.numbers_ = 0;
        }
        
		Marking(UShort n) {
			candidates_.numbers_ = n;
		}

		Marking(const Marking& m) {
			candidates_.numbers_ = m.candidates_.numbers_;
		}

        void operator= (const Marking& m) {
            candidates_.numbers_ = m.candidates_.numbers_;
        }

        bool operator== (const Marking& m) const {
            return candidates_.numbers_ == m.candidates_.numbers_;
        }
        
		bool operator!= (const Marking& m) const {
			return candidates_.numbers_ != m.candidates_.numbers_;
		}

        void Mark(UShort num) {
            candidates_.numbers_ |= (1 << (num-1));
        }
        
		void Mark(const Marking& m) {
			candidates_.numbers_ |= m.candidates_.numbers_;
		}

        void Erase(UShort num) {
            candidates_.numbers_ &= ~(1 << (num-1));
        }
        
        void EraseAll() {
            candidates_.numbers_ = 0;
        }
        
        bool IsMarked(UShort num) const {
            return (candidates_.numbers_ & (1 << (num-1))) != 0;
        }
        
		bool IsEmpty() const {
			return candidates_.numbers_ == 0;
		}

        Marking MatchSubset(const Marking& subset) const {
            return Marking(subset.candidates_.numbers_ & candidates_.numbers_);
        }
        
		void Intersect(const Marking& m) {
			candidates_.numbers_ &= m.candidates_.numbers_;
		}

		// Count the number of candidates
        UShort GetCount();
		// Erase the candidates based on the given subset
        bool Erase(const Marking& subset);
		// Return the number if only one candidate is present
        UShort GetResolvedNumber() const;
        
    private:
        union Candidates {
            struct {
                UShort one_ : 1;
                UShort two_ : 1;
                UShort three_ : 1;
                UShort four_ : 1;
                UShort five_ : 1;
                UShort six_ : 1;
                UShort seven_ : 1;
                UShort eight_ : 1;
                UShort nine_ : 1;
            };
            UShort numbers_;
        } candidates_;
    };
}

#endif /* Marking_h */
