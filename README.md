# Puzzle Solver

## Introduction

This puzzle solver is a C++ program designed to solve a 9x9 puzzle with horizontal and vertical compartments. The input puzzle is represented as a string, and the solver follows a set of rules to fill in the compartments with uninterrupted numbers.

## Puzzle Rules

- Minus integers represent black cells with that integer.
- Asterisk (*) denotes an empty cell, recorded as 99 during solving.
- Zero (0) represents a black cell with no effect.
- Compartments are formed by numbers between black cells and 0’s.

## Solver Approach

1. **Iterate through horizontal and vertical compartments:**
   - Start with those already 30% completed.

2. **For each compartment:**
   - Check possible values.
   - Sort the compartment.
   - Perform diagonal check on possibilities.
   - Record possible values for each index.
   - Choose a random value from the possibilities.

3. **Record changes:**
   - Compartment, index, value, and step.

4. **Perform a complete check:**
   - If completion affects another compartment, check its interruption.

5. **Backtrack if needed:**
   - Identify the cause of being stuck.
   - Check the same row or column for an alternative possibility.

## Usage

1. Copy and paste the puzzle boards into the `InputBoard()` function in `main.cpp`.
2. Compile the program with C++11.
3. Run the compiled version.

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

### Board 2 (Medium)
"0***700*1",
"-5**7*0***",
"*3-1*****0",
"42*0-8***0",
"****0*5**",
"04**00**6",
"07****0**",
"***-9***30",
"*9-701***0"

### Board 3 (Hard)
"0-1**90**0",
"******4*0",
"***7-54*00",
"0*502*0*6",
"******6**",
"5*0**-97*0",
"-30**-4****",
"0****5***",
"0**0**50-2"


Print statements have been added to showcase the solving process. Use this program to work on solvable boards and understand the solving steps.
