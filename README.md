# Paraller Game Of Life Game

**The Game of Life** (or simply Life) is not a game in the conventional sense. There are no players, and no
winning or losing. Once the "pieces" are placed in the starting position, the rules determine everything
that happens later.

Life is played on a grid of square cells-like a chess board but extending infinitely in every direction. A cell
can be alive or dead. A live cell is shown by putting a marker on its square. A dead cell is shown by
leaving the square empty. Each cell in the grid has a neighborhood consisting of the eight cells in every
direction including diagonals, each cell belongs to a certain species, and each species is color-coded (see
source code).
The cycle of life in this game is divided into 2 phases, the first phase is where new cells are created and
some cells die :( , and in the second phase the cells get to know their lovely neighborhood and change its
properties according to the neighborhood it is in.

You will implement a parallel Game of Life utilizing the thread-pool pattern.
In this exercise the field size is not infinite but known and provided as an input file. All cells outside the
field are assumed to be always dead.
The field is split between different threads so that in every generation (iteration), each thread works on
its own rows of the field (which are split as equally as possible between all the working threads)
calculating the next generation’s field. We will call the rows the thread works on: the thread’s “tile”.

I implemented a parallel Game of Life utilizing the thread-pool pattern.
In this game the field size is not infinite but known and provided as an input file. All cells outside the
field are assumed to be always dead.
The field is split between different threads so that in every generation (iteration), each thread works on
its own rows of the field (which are split as equally as possible between all the working threads)
calculating the next generation’s field.

**FINAL GRADE 100**
