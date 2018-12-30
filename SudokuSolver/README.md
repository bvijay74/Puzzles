# Sudoku Algorithm

This Sudoku puzzle solving algorithm applies these techniques to solve the puzzle,
1. Visible singles
2. Hidden singles
3. Visible subset (pair, triple, quad)
4. Hidden subset (pair, triple, quad)
5. Intersection removal or pointing pair
6. X Wing

C++ classes that represent the puzzle components are implemented - **Grid, Cell, Row, Column, Block**

The **Cell** objects contain the solved number or a **Marking** object which represent the candidates for the cell.

Each cell is cross referenced by **Row, Column** and **Block** objects that contain the cell. The  **Row** object maintains the reference to the **Column** and **Block** objects that intersect the row, and similarly the **Column** and **Block** objects maintain reference to their intersections.

The Sudoku puzzle solving techniques are applied on each segment (row, column or block) which is not filled to solve the number or reduce the candidates. If a number is solved in a cell or if the candidates are reduced, row, column and blocks that refer it get updated. The process continues until all the cells are filled or if no further solution to reduce the candidates is possible.

The algorithm is validated by testing the 50 puzzles in [Project Euler ](https://projecteuler.net/problem=96)
