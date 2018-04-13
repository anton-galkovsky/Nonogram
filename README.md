Autumn 2016

# Nonogram Solution

The program solves nonograms with a size of not more than 64x64 cells. The solution is based on the logic used by the person to solve the crossword, but allows the ability to make the necessary number of assumptions (only if necessary)

In the input file, information about the nonogram should be represented as follows: first, information about the lines in the following format: the first line contains one number equal to the number of lines, followed by the lines in the specified number, each of which describes one line of the nonogram: first the number of groups filled cells in the described line, then the lengths of groups (the numbers in each line are separated by spaces). After that, information about the columns follows in a similar format. In the output file, either the string "impossible", if the problem does not have a solution, or the answer as a picture consisting of characters (space - an empty cell, # - shaded cell)
