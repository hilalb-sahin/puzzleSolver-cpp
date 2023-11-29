# Puzzle Solver

## Introduction

This puzzle solver is a C++ program designed to solve a 9x9 puzzle with horizontal and vertical compartments. The input puzzle is represented as a string, and the solver follows a set of rules to fill in the compartments with uninterrupted numbers.

## Puzzle Syntax Explanation

- Negative integers represent black cells with that integer.
- Asterisk (*) denotes an empty cell, recorded as 99 during solving.
- Zero (0) represents a black cell with no effect.
- Compartments are formed by numbers between black cells and 0’s.

## Puzzle Rules

1. **Uninterrupted Compartments:**
   - Compartments must contain an uninterrupted sequence of numbers.
   - Sorting compartments should result in a continuous sequence.

2. **Unordered Compartments:**
   - Compartments can be arranged in any order.

3. **Diagonal Uniqueness:**
   - No two compartments within the same diagonal can have the same number.


## Solver Approach

1. **Iterate through horizontal and vertical compartments:**
   - Start with compartments that has the least empty cell in it.

2. **For each compartment:**
   - Check possible values.
   - Sort the compartment.
   - Perform diagonal check on possibilities.
   - Record possible values for each index.
   - Choose a random set from the possibilities.

3. **Record changes:**
   - Compartment, index, value, and step.

4. **Perform a complete check:**
   - If completion affects another compartment, check its interruption.

5. **Backtrack if needed:**
   - Backtrack to the last step in history and pick a different possibility for compartment.
   - Move on and repeat if needed.

## Usage

1. Clone the repository.
2. Copy and paste the puzzle boards into the `InputBoard()` function in `int main()`.
3. Compile the program with C++11 `g++ -std=c++11 puzzleSolver.cpp -o puzzleSolver` in terminal.
4. Run the compiled version. `./puzzleSolver`

## Example Boards

### Board 1 (Easy)

```plaintext
"-6*300**00",
"41*30*8*6",
"**0**0***",
"***-2***70",
"0-9**6**00",
 "0****0***",
"**804*06*",
"**9*-12***",
"00560-42*0"
```

### Board 2 (Medium)
```plaintext
"0***700*1",
"-5**7*0***",
"*3-1*****0",
"42*0-8***0",
"****0*5**",
"04**00**6",
"07****0**",
"***-9***30",
"*9-701***0"
```

### Board 3 (Hard)
```plaintext
"0-1**90**0",
"******4*0",
"***7-54*00",
"0*502*0*6",
"******6**",
"5*0**-97*0",
"-30**-4****",
"0****5***",
"0**0**50-2"
```

Print statements have been added to showcase the solving process. 
