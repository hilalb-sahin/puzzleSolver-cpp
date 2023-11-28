#include <vector>
#include <string>
#include <iostream>
#include <memory>

using std::to_string;

using std::cout;
using std::endl;
using std::string;
using std::vector;
// do not add additional libraries

class SetSolverSquareSet
{
public:
    std::vector<int> set;
    SetSolverSquareSet()
    {
        // Write your constructor here
        set = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    }

    // You can add any additional functions you'd like in order to help with the task
};

enum CompartmentType
{
    HORIZONTAL,
    VERTICAL
};

struct CompartmentInfo
{
    int row;
    int compartment;
    int index;
    string value;
    string compartmentString;
};

struct CellInfo
{
    int row;
    int column;
    CompartmentInfo compartmentInfo;      // knowing what compartment you belong
    vector<int> safePossibilities = {-1}; // safe possibilities to pick for that cell
                                          // already used possibilities, so you should not pick from those .
};
// this is for tracking individual cells within comaprtment
struct ExploredChoice
{
    size_t step = 0;
    CompartmentInfo changedCompartment;
    vector<CellInfo> changedCells;
    CompartmentType compartmentType;
    vector<int> mergedSafePossibilities;
    vector<vector<int>> possibilities;             // all of the combinations, we don't know which ones are safe or not.
    vector<vector<int>> diagonalSafePossibilities; // actual possibilities that we can use

    vector<vector<int>> usedPossibilities;
};

class SetSolver
{
private:
    int boardSize = 9;
    vector<vector<SetSolverSquareSet>> board;
    // vector<vector<string>> horizontalCompartment;
    // these should be updated with populateCompartment() functions whenever a change is made in the board.
    vector<vector<vector<string>>> horizontalCompartment;
    vector<vector<vector<string>>> verticalCompartment;

    vector<vector<int>> possibilities;

    // these should be updated with getAllInfo(), whenever a change is made in the board.

    vector<CompartmentInfo> horizontalInfo;
    vector<CompartmentInfo> verticalInfo;
    string verticalString = "";
    string horizontalString = "";

    vector<ExploredChoice> history;

    // size_t step = 0;

    ExploredChoice currentExploredChoice;

    vector<vector<int>> currentUsedPossibilities;
    bool backtracking = false;

public:
    SetSolver()
    {
        // implement your code here
        board.resize(boardSize, vector<SetSolverSquareSet>(boardSize));
    }

    void Solve()
    {

        while (!allCompartmentsFilled())
        {

            if (backtracking == false)
            {
                cout << "NOT BACKTRACKING, PICKING NEW"
                     << "\n";
                CompartmentInfo pickedHorizontal = pickSmartCompartment(horizontalInfo);
                vector<vector<int>> possibilities = calculatePossibilities(pickedHorizontal.compartmentString);

                // Put information about cells in the current choice
                initializeExploredChoice(pickedHorizontal, HORIZONTAL);

                // Find every possible possibility with no diagonal check, populates current choice
                findPossibilities();

                // Populates safe possibilities
                populateDiagonalSafePossibilities();
                // Make a decision for this compartment
                bool itWasSuccessful = makeDecision();
                cout << "i just ran makeDecision(), was it successfull? : " << itWasSuccessful << "\n";
                if (itWasSuccessful)
                {
                    backtracking = false;
                    cout << "printing the history"
                         << "\n";
                    printHistory();
                }
                else
                {
                    cout << "setting the backtracking to true";
                    backtracking = true;
                    // it was false, so we are going to need to call again
                    //  cout<<"i am recursively calling solve() with backtracking = true, because we need backtrack "
                    //  solve();
                }
            }
            // we are backtracking
            else
            {
                cout << " we are backtracking "
                     << "\n";
                if (!history.empty())
                {
                    cout << "history is not empty"
                         << "\n";

                    cout << "updating the currentExploredChoice"
                         << "\n";
                    currentExploredChoice = history.back();

                    cout << "popping the last history"
                         << "\n";
                    history.pop_back();

                    cout << " i need to update the board to be empty for the backtracking values";
                    // printCurrentExploredChoice();

                    cout << "before updating the board: "
                         << "\n";
                    PrintFullBoard();

                    for (const auto &cell : currentExploredChoice.changedCells)
                    {

                        cout << "updating the board the for the row: " << cell.row << " and column : " << cell.column << " to be 99 (empty)"
                             << "\n";

                        std::cout << "  Row: " << cell.row << ", Column: " << cell.column << "\n";

                        updateBoard(cell.row, cell.column, 99);
                    }
                    cout << "after update: "
                         << "\n";
                    PrintFullBoard();

                    cout << "i am now going to call makeDecision"
                         << "\n";
                    bool itWasSuccessful = makeDecision();

                    cout << "i just ran makeDecision(), was it successfull? : " << itWasSuccessful << "\n";
                    if (itWasSuccessful)
                    {
                        cout << "it was successful after backtracking, printing history"
                             << "\n";
                        printHistory();

                        backtracking = false;
                    }
                    else
                    {
                        cout << "it was not successful, we will backtrack again";
                        backtracking = true;
                    }
                    cout << "printing the history"
                         << "\n";
                }
                else
                {
                    cout << "the history is empty, what can i do? breaking"
                         << "\n";
                    break;
                }
            }
            cout << " now after this if else, i am clearing the things:"
                 << "\n";
            currentExploredChoice.changedCells.clear();
            currentExploredChoice.mergedSafePossibilities.clear();
            currentExploredChoice.possibilities.clear();
            currentExploredChoice.diagonalSafePossibilities.clear();
            currentExploredChoice.usedPossibilities.clear();
        }
        cout << "all compartments are filled!"
             << "\n";
        PrintFullBoard();
    }

    /*
    now we need to actually make the decision.

    */
    bool makeDecision()
    {
        cout << "\n";
        cout << "running makeDecision"
             << "\n";
        cout << "\n";

        printCurrentExploredChoice();

        vector<vector<int>> possibilities = currentExploredChoice.diagonalSafePossibilities;
        vector<CellInfo> cells = currentExploredChoice.changedCells;
        CompartmentType compartmentType = currentExploredChoice.compartmentType;

        // Flag to indicate whether to break the outer loop
        bool shouldBreakOuterLoop = false;

        // Flag to indicate whether all diagonal possibilities have been tried and none passed
        bool allDiagonalPossibilitiesFailed = true;

        vector<int> currentPossibility;

        for (vector<int> possibility : possibilities)
        {
            cout << " i am entering to the possibility: "
                 << "\n";
            if (vectorExists(currentExploredChoice.usedPossibilities, possibility))
            {
                cout << "Skipping possibility because it's in usedPossibilities\n";
                continue;
            }

            currentPossibility = possibility;

            cout << "\n";
            cout << "for the possibility vector: ";
            for (int i : possibility)
            {
                cout << i << " "
                     << "\n";
            }

            // Flag to indicate whether all cells are valid for the current possibility
            bool allCellsValid = true;

            for (int i = 0; i < cells.size(); i++)
            {
                cout << "checking for the possibility: " << possibility[i];

                int row = cells[i].row;
                int column = cells[i].column;
                cout << " for the cell: " << row << " " << column << "\n";
                cout << cells[i].compartmentInfo.compartmentString << "\n";
                int numToUpdate = possibility[i];
                int originalNumber = ReturnValue(row, column);
                updateBoard(row, column, numToUpdate);

                if (isCompleteValidCheck(row, column, compartmentType))
                {
                    cout << "VALID! "
                         << "\n";
                    cout << "for the cell row: " << row << "\n"
                         << "column: " << column << "\n"
                         << " this number: " << numToUpdate << " is safe "
                         << "\n";
                    currentUsedPossibilities.push_back(currentPossibility);
                }
                else
                {
                    cout << "INVALID"
                         << "\n";
                    cout << "for the cell row: " << row << "\n"
                         << "column: " << column << "\n"
                         << " this number: " << numToUpdate << " is NOT safe "
                         << "\n";
                    allCellsValid = false;
                    cout << "updating the board with the original number"
                         << "\n";

                    for (const auto &cell : currentExploredChoice.changedCells)
                    {

                        cout << "updating the board the for the row: " << cell.row << " and column : " << cell.column << " to be 99 (empty)"
                             << "\n";

                        std::cout << "  Row: " << cell.row << ", Column: " << cell.column << "\n";

                        updateBoard(cell.row, cell.column, 99);
                    }
                    break;
                }
            }

            // If all cells are valid, break the outer loop
            if (allCellsValid)
            {
                cout << "all cells valid"
                     << "\n";
                allDiagonalPossibilitiesFailed = false;
                currentExploredChoice.usedPossibilities.push_back(currentPossibility);
                shouldBreakOuterLoop = true;
                cout << "pushing into the history... "
                     << "\n";
                history.push_back(currentExploredChoice);
                break;
            }
        }

        // Check if diagonal possibilities are empty or all failed
        return !possibilities.empty() && !allDiagonalPossibilitiesFailed && shouldBreakOuterLoop;
    }

    // Custom function to check if a vector exists in another vector of vectors
    bool vectorExists(const std::vector<std::vector<int>> &vecOfVecs, const std::vector<int> &vecToCheck)
    {
        for (const auto &vec : vecOfVecs)
        {
            if (vec == vecToCheck)
            {
                return true;
            }
        }
        return false;
    }

    bool isSequenced(const std::string &str)
    {
        // Check if the string is empty or has only one character
        if (str.empty() || str.length() == 1)
        {
            return true;
        }

        // Check the difference between consecutive characters
        for (int i = 1; i < str.length(); ++i)
        {
            if (str[i] - str[i - 1] != 1)
            {
                return false;
            }
        }

        return true;
    }

    // Function to swap two characters
    void swap(char &a, char &b)
    {
        char temp = a;
        a = b;
        b = temp;
    }

    int charToInt(char c)
    {
        return static_cast<int>(c);
    }

    std::string sortStringByIntValues(const std::string &input)
    {
        std::string sortedString = input;
        int n = sortedString.length();

        for (int i = 0; i < n - 1; ++i)
        {
            for (int j = 0; j < n - i - 1; ++j)
            {
                if (charToInt(sortedString[j]) > charToInt(sortedString[j + 1]))
                {
                    std::swap(sortedString[j], sortedString[j + 1]);
                }
            }
        }

        return sortedString;
    }

    int ReturnValue(size_t row, size_t col)
    {
        // Access the SetSolverSquareSet object at the specified row and column
        const SetSolverSquareSet &cell = board[row][col];

        // Access the 'set' vector of the SetSolverSquareSet object
        const std::vector<int> &set = cell.set;

        // You can now work with the 'set' vector or return the value you need from it
        return set[0];
    }

    // helper function for generating possibilities of empty compartments
    std::vector<std::vector<int>> generateConsecutiveSequences(int length)
    {
        vector<vector<int>> result;
        if (length < 1 || length > 9)
        {
            std::cerr << "Invalid length. Length should be between 1 and 9." << std::endl;
            return result;
        }

        int min = 1;
        int max = 9 - length + 1;

        for (int i = min; i <= max; ++i)
        {
            std::vector<int> sequence;
            for (int j = 0; j < length; ++j)
            {
                sequence.push_back(i + j);
            }
            result.push_back(sequence);
        }

        return result;
    }

    vector<vector<int>> calculatePossibilities(string compartmentString)
    {
        cout << "\n";
        vector<vector<int>> possibilities;
        int spaceCount = 0;
        vector<int> completeNumbers;
        vector<int> numbers;

        for (char c : compartmentString)
        {
            if (c != '_')
            {
                numbers.push_back(c - '0');
            }
        }

        if (numbers.size() > 1)
        {
            int min = findMinVector(numbers);
            int max = findMaxVector(numbers);

            for (int i = min; i <= max; i++)
            {
                completeNumbers.push_back(i);
            }
        }
        if (numbers.size() == 1)
        {
            completeNumbers.push_back(numbers[0]);
        }

        spaceCount = compartmentString.size() - completeNumbers.size();
        // cout << "space count: " << spaceCount << "\n";

        if (numbers.size() == 0)
        {
            vector<vector<int>> result = generateConsecutiveSequences(spaceCount);

            // for (const auto &innerVector : result)
            // {
            //     for (int value : innerVector)
            //     {
            //         std::cout << value << " ";
            //     }
            //     std::cout << std::endl;
            // }
            return result;
        }

        vector<string> combinations = generateCombinations(spaceCount);

        for (string combination : combinations)
        {
            vector<int> possibility = completeNumbers;

            for (char operation : combination)
            {
                int max;
                int min;

                if (operation == '+')
                {
                    max = findMaxVector(possibility);
                    if (max + 1 > 9)
                    {
                        break;
                    }
                    else
                    {
                        possibility.push_back(max + 1);
                    }
                }
                else
                {
                    min = findMinVector(possibility);
                    if (min - 1 < 1)
                    {
                        break;
                    }
                    else
                    {
                        possibility.push_back(min - 1);
                    }
                }
            }

            if (possibility.size() == compartmentString.size())
            {
                auto iter = possibility.begin();
                while (iter != possibility.end())
                {
                    bool found = false;
                    for (int number : numbers)
                    {
                        if (*iter == number)
                        {
                            found = true;
                            break;
                        }
                    }

                    if (found)
                    {
                        iter = possibility.erase(iter);
                    }
                    else
                    {
                        ++iter;
                    }
                }
                possibilities.push_back(possibility);
            }
        }
        // cout<<"calculatePossibilities: ";
        // for (const auto &innerVector : possibilities)
        // {
        //     for (int value : innerVector)
        //     {
        //         std::cout << value<<" " ;
        //     }

        // }

        return possibilities;
    }

    void removeValueFromVector(std::vector<int> &myVector, int valueToRemove)
    {
        for (auto it = myVector.begin(); it != myVector.end(); /* no increment here */)
        {
            if (*it == valueToRemove)
            {
                it = myVector.erase(it); // erase returns the iterator to the next valid position
            }
            else
            {
                ++it; // move to the next element
            }
        }
    }

    int findMinVector(const std::vector<int> &myVector)
    {
        if (myVector.empty())
        {
            std::cerr << "Vector is empty." << std::endl;
            return -1; // Return a default value or handle the empty case as needed
        }

        int minValue = myVector[0];
        for (int value : myVector)
        {
            if (value < minValue)
            {
                minValue = value;
            }
        }

        return minValue;
    }

    int findMaxVector(const std::vector<int> &myVector)
    {
        if (myVector.empty())
        {
            std::cerr << "Vector is empty." << std::endl;
            return -1; // Return a default value or handle the empty case as needed
        }

        int maxValue = myVector[0];
        for (int value : myVector)
        {
            if (value > maxValue)
            {
                maxValue = value;
            }
        }

        return maxValue;
    }

    void generateCombinations(int emptySpace, int index, std::string current, std::vector<std::string> &result)
    {

        std::vector<char> set = {'+', '-'};
        if (emptySpace == 0)
        {
            result.push_back(current);
            return;
        }

        for (int i = index; i < set.size(); ++i)
        {
            generateCombinations(emptySpace - 1, i, current + set[i], result);
        }
    }

    vector<string> generateCombinations(int emptySpace)
    {
        std::vector<std::string> combinations;
        generateCombinations(emptySpace, 0, "", combinations);

        return combinations;
    }

    void PrintFullBoard()
    {
        for (int row = 0; row < boardSize; row++)
        {
            for (int col = 0; col < boardSize; col++)
            {
                for (int value : board[row][col].set)
                {
                    std::cout << value << " ";
                }
                std::cout << " | "; // Separate values within a square
            }
            std::cout << std::endl;
            if ((row + 1) % 3 == 0 && row + 1 < boardSize)
            {
                // Add horizontal lines to separate 3x3 subgrids
                std::cout << "------------------------" << std::endl;
            }
        }
    }

    void populateHorizontalCompartment()
    {
        // Clear the existing horizontalCompartment
        horizontalCompartment.clear();

        for (int row = 0; row < boardSize; row++)
        {
            int column = 0;

            // Create a compartment for the current row
            vector<string> currentCompartment;

            while (column < boardSize)
            {
                int cellValue = ReturnValue(row, column);

                if (cellValue < 0)
                {
                    // Add the information to the current compartment
                    currentCompartment.push_back("|");
                }
                else if (cellValue == 0)
                {
                    // Add a placeholder for an empty cell
                    currentCompartment.push_back("|");
                }
                else if (cellValue == 99)
                {
                    // Add a placeholder for a cell with value 99
                    currentCompartment.push_back("_");
                }
                else
                {
                    // Add the value to the current compartment
                    currentCompartment.push_back(to_string(cellValue));
                }

                column++;
            }

            // Split the currentCompartment into groups based on "|"
            vector<vector<string>> groups;
            vector<string> currentGroup;
            for (const string &value : currentCompartment)
            {
                if (value == "|")
                {
                    // Start a new group
                    if (!currentGroup.empty())
                    {
                        groups.push_back(currentGroup);
                        currentGroup.clear();
                    }
                    // Add "|" as a separate vector
                    groups.push_back({"|"});
                }
                else
                {
                    // Add to the current group
                    currentGroup.push_back(value);
                }
            }
            // Add the last group
            if (!currentGroup.empty())
            {
                groups.push_back(currentGroup);
            }

            // Store the groups in horizontalCompartment
            horizontalCompartment.push_back(groups);
        }
    }

    void populateVerticalCompartment()
    {
        // Clear the existing verticalCompartment
        verticalCompartment.clear();

        for (int column = 0; column < boardSize; column++)
        {
            int row = 0;

            // Create a compartment for the current column
            vector<string> currentCompartment;

            while (row < boardSize)
            {
                int cellValue = ReturnValue(row, column);

                if (cellValue < 0)
                {
                    // Add the information to the current compartment
                    currentCompartment.push_back("|");
                }
                else if (cellValue == 0)
                {
                    // Add a placeholder for an empty cell
                    currentCompartment.push_back("|");
                }
                else if (cellValue == 99)
                {
                    // Add a placeholder for a cell with value 99
                    currentCompartment.push_back("_");
                }
                else
                {
                    // Add the value to the current compartment
                    currentCompartment.push_back(to_string(cellValue));
                }

                row++;
            }

            // Split the currentCompartment into groups based on "|"
            vector<vector<string>> groups;
            vector<string> currentGroup;
            for (const string &value : currentCompartment)
            {
                if (value == "|")
                {
                    // Start a new group
                    if (!currentGroup.empty())
                    {
                        groups.push_back(currentGroup);
                        currentGroup.clear();
                    }
                    // Add "|" as a separate vector
                    groups.push_back({"|"});
                }
                else
                {
                    // Add to the current group
                    currentGroup.push_back(value);
                }
            }
            // Add the last group
            if (!currentGroup.empty())
            {
                groups.push_back(currentGroup);
            }

            // Store the groups in horizontalCompartment
            verticalCompartment.push_back(groups);
        }
    }

    /*
    process the string prior and remember the index of - and remove it to make it
    easier putting into board
    */
    void PreprocessString(std::string &str, std::vector<int> &negPositions)
    {
        size_t pos = 0;
        while ((pos = str.find("-", pos)) != std::string::npos)
        {
            if (pos + 1 < str.length() && isdigit(str[pos + 1]))
            {
                // Remember the position of '-'
                negPositions.push_back(pos);
                // Remove '-' from the string
                str.replace(pos, 1, "");
            }
            pos += 1; // Skip to the next character
        }
    }

    void PopulateBoard(const std::vector<std::string> &skeletonBoard)
    {
        for (int row = 0; row < boardSize; row++)
        {
            std::string modifiedString = skeletonBoard[row];
            std::vector<int> negPositions;
            PreprocessString(modifiedString, negPositions);

            for (int col = 0; col < boardSize; col++)
            {
                char currentChar = modifiedString[col];

                if (currentChar == '0')
                {
                    board[row][col].set = {0};
                }
                else if (currentChar == '*')
                {
                    board[row][col].set = {99};
                }
                else
                {
                    if (!negPositions.empty() && negPositions.front() == col)
                    {
                        // Handle as a negative number
                        int value = -(currentChar - '0');
                        negPositions.erase(negPositions.begin()); // Remove the recorded position
                        board[row][col].set = {value};
                    }
                    else if (isdigit(currentChar))
                    {
                        int value = currentChar - '0';
                        board[row][col].set = {value};
                    }
                }
            }
        }
        populateVerticalCompartment();

        populateHorizontalCompartment();
        horizontalInfo = getAllHorizontalInfo();
        verticalInfo = getAllVerticalInfo();
    }

    // function to populate the STRUCT
    vector<CompartmentInfo> getAllHorizontalInfo()
    {
        vector<CompartmentInfo> compartmentInfo;

        for (int row = 0; row < horizontalCompartment.size(); ++row)
        {
            int index = 0;
            for (int col = 0; col < horizontalCompartment[row].size(); ++col)
            {
                const vector<string> &group = horizontalCompartment[row][col];

                // Concatenate the strings in the group to form the complete compartment string
                string compartmentString = "";
                for (const string &value : group)
                {
                    compartmentString += value;
                }

                for (const string &value : group)
                {
                    CompartmentInfo info;
                    info.row = row;
                    info.compartment = col;
                    info.index = index;
                    info.value = value;
                    info.compartmentString = compartmentString;

                    compartmentInfo.push_back(info);
                    ++index;
                }
            }
        }

        return compartmentInfo;
    }

    // function to populate the STRUCT
    vector<CompartmentInfo> getAllVerticalInfo()
    {
        vector<CompartmentInfo> compartmentInfo;

        for (int row = 0; row < verticalCompartment.size(); ++row)
        {
            int index = 0;
            for (int col = 0; col < verticalCompartment[row].size(); ++col)
            {
                const vector<string> &group = verticalCompartment[row][col];

                // Concatenate the strings in the group to form the complete compartment string
                string compartmentString = "";
                for (const string &value : group)
                {
                    compartmentString += value;
                }

                for (const string &value : group)
                {
                    CompartmentInfo info;
                    info.row = row;
                    info.compartment = col;
                    info.index = index;
                    info.value = value;
                    info.compartmentString = compartmentString;

                    compartmentInfo.push_back(info);
                    ++index;
                }
            }
        }

        return compartmentInfo;
    }

    // Function to print compartment information

    void printCompartmentInfo(const vector<CompartmentInfo> &allCompartmentInfo)
    {
        for (const CompartmentInfo &info : allCompartmentInfo)
        {
            cout << "Row " << info.row << ", Compartment " << info.compartment << ", Index " << info.index << ":\n";
            cout << "Value: " << info.value << "\n";
            cout << "Compartment String: " << info.compartmentString;
            cout << "\n\n";
        }
    }

    void printHorizontalCompartment()
    {
        std::cout << "Horizontal Compartments:\n";
        for (const auto &compartmentRow : horizontalCompartment)
        {
            for (const auto &group : compartmentRow)
            {
                for (const std::string &value : group)
                {
                    std::cout << value;
                }
            }
            std::cout << "\n";
        }
    }

    void printVerticalCompartment()
    {
        std::cout << "\nVertical Compartments:\n";
        for (const auto &compartmentColumn : verticalCompartment)
        {
            for (const auto &group : compartmentColumn)
            {
                for (const std::string &value : group)
                {
                    std::cout << value;
                }
            }
            std::cout << "\n";
        }
    }

    vector<CompartmentInfo> getHorizontalInfo()
    {
        return horizontalInfo;
    }

    vector<CompartmentInfo> getVerticalInfo()
    {
        return verticalInfo;
    }

    void printHistory() const
    {
        std::cout << "History:\n";
        cout << "---------------------------------"
             << "\n";

        for (const auto &exploredChoice : history)
        {
            std::cout << "Step: " << exploredChoice.step << "\n";
            std::cout << "Changed Compartment:\n"; // Assuming CompartmentInfo has its own print function
            // printCompartmentInfo(exploredChoice.changedCompartment);
            std::cout << "Changed Cells:\n";
            for (const auto &cell : exploredChoice.changedCells)
            {
                std::cout << "  Row: " << cell.row << ", Column: " << cell.column << "\n";
                // Assuming CellInfo and CompartmentInfo have their own print functions
                // printCellInfo(cell);
                // printCompartmentInfo(cell.compartmentInfo);
            }
            std::cout << "Compartment Type: " << exploredChoice.compartmentType << "\n";
            std::cout << "Merged Safe Possibilities: ";
            for (int possibility : exploredChoice.mergedSafePossibilities)
            {
                std::cout << possibility << " ";
            }
            std::cout << "\n";
            std::cout << "Possibilities:\n";
            for (const auto &possibility : exploredChoice.possibilities)
            {
                for (int value : possibility)
                {
                    std::cout << value << " ";
                }
                std::cout << "\n";
            }
            std::cout << "Diagonal Safe Possibilities:\n";
            for (const auto &possibility : exploredChoice.diagonalSafePossibilities)
            {
                for (int value : possibility)
                {
                    std::cout << value << " ";
                }
                std::cout << "\n";
            }
            std::cout << "Used Possibilities:\n";
            for (const auto &possibility : exploredChoice.usedPossibilities)
            {
                for (int value : possibility)
                {
                    std::cout << value << " ";
                }
                std::cout << "\n";
            }
            std::cout << "\n";
        }
        cout << "---------------------------------"
             << "\n";
    }

    void printCurrentExploredChoice() const
    {
        cout << "PRINTING CURRENT EXPLORED CHOICE"
             << "\n";
        const ExploredChoice &exploredChoice = currentExploredChoice;
        std::cout << "Step: " << exploredChoice.step << "\n";
        std::cout << "Changed Compartment:\n";
        // Assuming CompartmentInfo has its own print function
        std::cout << "  Row: " << exploredChoice.changedCompartment.row << ", Compartment: " << exploredChoice.changedCompartment.compartment << "\n";

        std::cout << "Changed Cells:\n";
        for (const auto &cell : exploredChoice.changedCells)
        {
            std::cout << "  Row: " << cell.row << ", Column: " << cell.column << "\n";
            // Print CompartmentInfo directly
            std::cout << "    Compartment Info - Row: " << cell.compartmentInfo.row << ", Compartment: " << cell.compartmentInfo.compartment << "\n";
        }

        std::cout << "Compartment Type: " << exploredChoice.compartmentType << "\n";
        std::cout << "Merged Safe Possibilities: ";
        for (int possibility : exploredChoice.mergedSafePossibilities)
        {
            std::cout << possibility << " ";
        }
        std::cout << "\n";

        std::cout << "Possibilities:\n";
        for (const auto &possibility : exploredChoice.possibilities)
        {
            for (int value : possibility)
            {
                std::cout << value << " ";
            }
            std::cout << "\n";
        }

        std::cout << "Diagonal Safe Possibilities:\n";
        for (const auto &possibility : exploredChoice.diagonalSafePossibilities)
        {
            for (int value : possibility)
            {
                std::cout << value << " ";
            }
            std::cout << "\n";
        }

        std::cout << "Used Possibilities:\n";
        for (const auto &possibility : exploredChoice.usedPossibilities)
        {
            for (int value : possibility)
            {
                std::cout << value << " ";
            }
            std::cout << "\n";
        }

        std::cout << "\n";
    }

    // Function to find the compartment for a specific row and index
    CompartmentInfo findCompartment(const vector<CompartmentInfo> &compartmentInfo, int targetRow, int targetIndex)
    {
        // Iterate through compartmentInfo to find the matching compartment

        for (const CompartmentInfo &info : compartmentInfo)
        {
            // cout<<"info row while looping: "<<info.row;

            if (info.row == targetRow && info.index == targetIndex)
            {
                cout << "Compartment found - Row: " << info.row
                     << ", Compartment: " << info.compartment
                     << ", Index: " << info.index << endl;

                cout << "Compartment Information:\n"
                     << "Value: " << info.value << "\n"
                     << "Compartment String: " << info.compartmentString << endl;
                return info; // Return the found compartment
            }
        }

        // If no match is found, return a default CompartmentInfo with row = -1
        return CompartmentInfo{-1, -1, -1, "", ""};
    }

    // update the board, also update the vertical and horizontal compatments (row, col, as in horizontal way)

    void updateBoard(int row, int col, int value)
    {
        if (ReturnValue(row, col) <= 0)
        {
            cout << "Cannot update this cell "
                 << "\n";
        }
        else
        {
            // Update the board first
            board[row][col].set = {value};

            // Populate the compartments based on the updated board
            populateHorizontalCompartment();
            populateVerticalCompartment();

            // Clear and re-populate the compartment information vectors
            horizontalInfo.clear();
            verticalInfo.clear();
            horizontalInfo = getAllHorizontalInfo();
            verticalInfo = getAllVerticalInfo();
        }
    }

    int findMaxInString(const std::string &input)
    {
        int maxNum = -1; // Assuming all numbers are positive

        for (char c : input)
        {
            if (std::isdigit(c))
            {
                int num = c - '0';
                maxNum = std::max(maxNum, num);
            }
        }

        return maxNum;
    }

    int findMinInString(const std::string &input)
    {
        int minNum = 2147483647; // Initialize with a large value, equivalent to INT_MAX

        for (char c : input)
        {
            if (std::isdigit(c))
            {
                int num = c - '0';
                minNum = std::min(minNum, num);
            }
        }

        return minNum;
    }

    // this code is to check specific cell perpendicular compartment full and sequenced or not
    // this code is to check specific cell perpendicular compartment full and sequenced or not
    bool isCompleteValidCheck(int row, int column, CompartmentType type)
    {

        cout << "running is Complete Valid Check"
             << "\n";

        // this is the perpendicular compartment
        CompartmentInfo foundCompartment;

        if (type == HORIZONTAL)
        {
            cout << "I am in horizontal"
                 << "\n";
            foundCompartment = findCompartment(verticalInfo, column, row);
        }
        else if (type == VERTICAL)
        {
            foundCompartment = findCompartment(horizontalInfo, row, column);
        }

        // Check if foundCompartment is valid
        if (foundCompartment.compartmentString.empty())
        {
            cout << "Invalid compartment found.\n";
            return false;
        }

        cout << "\n";
        cout << "doing complete check for this string: ";
        cout << foundCompartment.compartmentString;
        cout << "\n";

        // this is the perpendicular string
        string compString = foundCompartment.compartmentString;

        int underScoreCount = 0;
        for (char c : compString)
        {
            if (c == '_')
            {
                underScoreCount++;
            }
        }

        // we need to fill in between the smallest and the biggest one, iterate while the
        int min = findMinInString(compString);
        int max = findMaxInString(compString);
        int holderMin = min;
        string newString;

        // there is a single number ,so its ok
        if (min == max)
        {
            return true;
        }

        if (underScoreCount == 0)
        {
            // if there are no underscores, then sort it and check
            cout << "there are no empty, is it sequenced correctly? ";
            cout << "COMP STRING: " << compString << "\n";

            // Check if compString is not empty before sorting
            if (compString.empty())
            {
                cout << "Empty compString found.\n";
                return false;
            }

            string sortedStr = sortStringByIntValues(compString);
            cout << "Sorted string: " << sortedStr << "\n";

            // Check if isSequenced function is implemented correctly
            bool isItSequenced = isSequenced(sortedStr);

            cout << "RETURNING IS SEQUENCED: ";
            cout << isItSequenced << "\n";
            return isItSequenced;
        }

        while (holderMin < max)
        {
            newString += std::to_string(holderMin);
            holderMin++;
        }
        newString += std::to_string(max);
        if (newString.size() > compString.size())
        {
            cout << "newstring string:" << newString << "\n";
            cout << "size of the newString is bigger, returning false";
            return false;
        }
        cout << "newstring:" << newString << "\n";
        cout << "size of the compstring is bigger or equal";
        return true;
    }

    // bool isCompleteValidCheck(string compartmentString, string strToCheck ) {
    //     //first find underscores
    //     int underScoreCount = 0;
    //     for (char c : strToCheck) {
    //         if (c == '_') {
    //             underScoreCount++;
    //         }
    //     }
    //     //then do this :
    //     //then the perpendicular is not complete, can it be completed?
    //     if(underScoreCount >0)
    //     {

    //          int min =  findMinInString(strToCheck);
    //          int max= findMaxInString(strToCheck);
    //         cout<<"min: "<<min;
    //         cout<<" max: "<<max;
    //         if(max-min-1 != underScoreCount)
    //         {
    //             cout<<"returning false";
    //             return false;
    //         }
    //         //then it can be completed,
    //         cout<<"returning true";
    //         return true;
    //     }

    //     //if there are no underscores, then sort it and check:
    //     cout<<"there are no empty, is it sequenced correctly? ";
    //     cout << "COMP STRING: " << strToCheck << "\n";
    //     string sortedStr = sortStringByIntValues(strToCheck);
    //     cout << "Sorted string: " << sortedStr << "\n";
    //     bool isItSequenced = isSequenced(sortedStr);
    //     cout<<"RETURNING IS SEQUENCED: ";
    //     cout<<isItSequenced<<"\n";
    //     return isItSequenced;

    // }

    // give horizontal or vertical compartment.
    //  this returns the index as the first index of the compartment
    CompartmentInfo pickSmartCompartment(vector<CompartmentInfo> &compartmentInfo)
    {
        // Assuming compartmentInfo is not empty
        cout << "\n";
        cout << "running pick smart compartment"
             << "\n";
        PrintFullBoard();

        if (compartmentInfo.empty())
        {
            // Handle the case when compartmentInfo is empty
            // You might want to throw an exception, return a default compartment, or handle it based on your requirements.
        }

        CompartmentInfo selectedCompartment;
        size_t minUnderscoreCount = std::string::npos; // Initialize to a large value

        for (const auto &compartment : compartmentInfo)
        {
            // Check the number of "_" in the compartmentString
            size_t underscoreCount = 0;
            for (char c : compartment.compartmentString)
            {
                if (c == '_')
                {
                    underscoreCount++;
                }
            }

            // Update selectedCompartment if the current one has fewer remaining possibilities and at least one "_"
            if (underscoreCount > 0 && (minUnderscoreCount == std::string::npos || underscoreCount < minUnderscoreCount))
            {
                minUnderscoreCount = underscoreCount;
                selectedCompartment = compartment;
            }
        }
        cout << "string of selected: " << selectedCompartment.compartmentString << "\n";
        cout << "index of selected: " << selectedCompartment.index << "\n";
        cout << "row of selected: " << selectedCompartment.row << "\n";
        cout << "compartment of selected: " << selectedCompartment.compartment << "\n";

        return selectedCompartment;
    }

    /*check whethter there is any _ in the whole compartments,

    horizontal or vertical
    */

    bool allCompartmentsFilled()
    {
        cout << "Checking compartments";

        // Variable to track the last checked compartment
        int lastCheckedCompartment = -1;

        for (const CompartmentInfo &info : horizontalInfo)
        {
            // Check if the compartment is the same as the last checked compartment
            if (info.compartment == lastCheckedCompartment)
            {
                continue; // Skip this compartment as it has already been checked
            }

            // cout << "Compartment: " << info.compartment << "\n";
            // cout << "Compartment String: " << info.compartmentString << "\n";

            bool hasUnderscore = false;

            for (char c : info.compartmentString)
            {
                if (c == '_')
                {
                    // cout << "Found '_'\n";
                    hasUnderscore = true;
                    break; // Break the inner loop as soon as an underscore is found in the compartment
                }
            }

            if (hasUnderscore)
            {
                cout << "Returning false\n";
                return false;
            }

            // Update the last checked compartment
            lastCheckedCompartment = info.compartment;
        }

        cout << "Returning true\n";
        return true;
    }

    /*
    diagonal check, returns the safe numbers
    */
    vector<int> diagonalSafeNumbers(int row, int col)
    {
        // Check the current row and column for the same number
        vector<int> safeNumbers;
        vector<int> unsafeNumbers;

        // check the rows and column numbers
        // checking everything in the same column and row and adding them to unsafenumbers

        for (int i = 0; i < boardSize; i++)
        {
            int rowValue = ReturnValue(row, i);
            int colValue = ReturnValue(i, col);

            // Taking the absolute value because there may be some -x within the row and column.
            if (rowValue != 99 && rowValue != 0)
            {
                unsafeNumbers.push_back(abs(rowValue));
            }
            if (colValue != 99 && colValue != 0)
            {
                unsafeNumbers.push_back(abs(colValue));
            }
        }
        cout << "UNSAFE numbers: ";
        for (int num : unsafeNumbers)
        {
            std::cout << num << " ";
        }

        // populating safe numbers
        if (safeNumbers.empty())
        {
            // Populate safeNumbers with values from 1 to 9 that are not in unsafeNumbers
            for (int num = 1; num <= 9; num++)
            {
                bool isSafe = true;

                // Check if num is in unsafeNumbers
                for (int unsafeNum : unsafeNumbers)
                {
                    if (unsafeNum == num)
                    {
                        isSafe = false;
                        break;
                    }
                }

                if (isSafe)
                {
                    safeNumbers.push_back(num);
                }
            }
        }
        cout << "SAFE NUMBERS: ";
        for (int num : safeNumbers)
        {
            std::cout << num << " ";
        }

        return safeNumbers;
    }

    /*
    put information about cells in exploredChoice, for you to later find possibilities for them.
    */
    void initializeExploredChoice(CompartmentInfo compartmentInfo, CompartmentType compartmentType)
    {
        cout << "\n";
        cout << "\n";

        string compartmentString = compartmentInfo.compartmentString;

        int indexOfUnderscore = compartmentInfo.index;

        // creating the explored choice, there should be 1 choice for the each compartment that you update that links it to which cells within that compartment you updated.

        currentExploredChoice.changedCompartment = compartmentInfo;

        currentExploredChoice.compartmentType = compartmentType;

        for (int i = 0; i < compartmentString.size(); i++)
        {
            // cout<< "iterating the string, populating changedCells : " <<i<<"\n" ;

            if (compartmentString[i] == '_')
            {
                // we want to create a cell for those ones and store them.
                // cout<< "the correct index : "<< indexOfUnderscore+i<<"\n" ;
                // Create a CellInfo object for each "_" you found, and record details
                CellInfo cellInfo;
                cellInfo.compartmentInfo = compartmentInfo;
                cellInfo.row = compartmentInfo.row;
                cellInfo.column = indexOfUnderscore + i; // Update the index for the cell
                cellInfo.safePossibilities = diagonalSafeNumbers(cellInfo.row, cellInfo.column);
                currentExploredChoice.changedCells.push_back(cellInfo);
            }
        }
        // Iterate through each cell in the compartment
        vector<int> mergedSafePossibilities;
        for (const CellInfo &cell : currentExploredChoice.changedCells)
        {
            // Merge safe possibilities for the cell
            mergedSafePossibilities = mergeAndRemoveDuplicates(mergedSafePossibilities, cell.safePossibilities);
        }

        // Assign merged safe possibilities to the explored choice
        currentExploredChoice.mergedSafePossibilities = mergedSafePossibilities;
    }

    /*
    so here is what we are trying to do:

    after finding the possibility , that does NOT NOT contain the value merged,

    then we get that vector, then we do calculatePermutation,
    then we are just gonna randomly pick one out of it

    then we are gonna update the cell respectively putting the stuff in that vector

    then we will record all the other permutations in possibilities in explored choices, or perhaps not, im not sure , not necessary.
    then we will add to the used values

    then we will update the board

    then we will move on to the next compartment ez.
    */

    // Function to find possibilities for each cell
    // ...

    void findPossibilities()
    {

        // Find the possible choices
        std::vector<std::vector<int>> possibilities = calculatePossibilities(currentExploredChoice.changedCompartment.compartmentString);

        for (const std::vector<int> &possibility : possibilities)
        {
            // std::cout << "For the possibility: ";
            // for (int p : possibility)
            // {
            //     std::cout << p;
            // }
            // std::cout << "\n";

            bool shouldSkipPossibility = false;

            // For each value in the possibility, check if it's in merged safe possibilities
            for (int singlePossibility : possibility)
            {
                if (!contains(currentExploredChoice.mergedSafePossibilities, singlePossibility))
                {
                    std::cout << "The value " << singlePossibility << " is not in merged safe possibilities.\n";
                    shouldSkipPossibility = true;
                    break;
                }
            }

            if (!shouldSkipPossibility)
            {
                // Process the possibility since it passed the check

                // Calculate permutations for the possibility
                std::vector<std::vector<int>> permutations = getAllPermutations(possibility);

                // Append permutations to currentExploredChoice.possibilities
                for (const auto &perm : permutations)
                {
                    currentExploredChoice.possibilities.push_back(perm);
                }
                // // Print or use the permutations as needed
                std::cout << "Permutations of merge safe:\n";
                for (const auto &perm : permutations)
                {
                    for (int val : perm)
                    {
                        std::cout << val << " ";
                    }
                    std::cout << "\n";
                }
                // printCurrentExploredChoice();
            }
        }
    }

    /*
    pick a vector from the combinations.
    [1,2,3] = picked combination.

    cellInfo(1) row and column in board is going to be updated with the pickedCombination[1]

    this populates the diagonal safe possibilities in currentExploredChoice !!!

    so all those things in diagonallySafePossibilities in currentExploredChoice, is safe to pick .

    the only thing to check left is to do the completeValidCheck


    */
    void populateDiagonalSafePossibilities()
    {
        cout << "\n";

        vector<vector<int>> possibilities = currentExploredChoice.possibilities;

        vector<vector<int>> diagonalSafePossibilities;
        for (vector<int> chosenPossibility : possibilities)
        {
            for (int i = 0; i < currentExploredChoice.changedCells.size(); i++)
            {
                int row = currentExploredChoice.changedCells[i].row;
                int column = currentExploredChoice.changedCells[i].column;
                CellInfo currentCell = currentExploredChoice.changedCells[i];

                if (contains(currentCell.safePossibilities, chosenPossibility[i]))
                {
                }
                else
                {
                    // the possibility is not safe
                    cout << "THIS POSSIBILITY NOT SAFE for cell row " << row << "column " << column << "for number: " << chosenPossibility[i];
                    goto nextPossibility; // Jump to the label if the cell is not safe
                }
            }

            // if so now , we can add that vector to the actual safe possibility, we need to update the possibilities
            diagonalSafePossibilities.push_back(chosenPossibility);

        // Label to jump to if a cell is not safe
        nextPossibility:;
        }
        currentExploredChoice.diagonalSafePossibilities = diagonalSafePossibilities;

        // printCurrentExploredChoice();
    }

    // Function to swap two elements in a vector
    void swap(std::vector<int> &vec, int i, int j)
    {
        int temp = vec[i];
        vec[i] = vec[j];
        vec[j] = temp;
    }

    // Recursive function to generate permutations
    void generatePermutations(std::vector<int> &vec, int start, std::vector<std::vector<int>> &result)
    {
        if (start == vec.size() - 1)
        {
            result.push_back(vec);
            return;
        }

        for (int i = start; i < vec.size(); ++i)
        {
            swap(vec, start, i);
            generatePermutations(vec, start + 1, result);
            swap(vec, start, i); // Backtrack
        }
    }

    // Function to get all permutations
    std::vector<std::vector<int>> getAllPermutations(const std::vector<int> &nums)
    {
        std::vector<std::vector<int>> permutations;
        std::vector<int> temp = nums; // Create a copy to avoid modifying the original vector
        generatePermutations(temp, 0, permutations);
        return permutations;
    }

    bool contains(const std::vector<int> &vec, int value)
    {
        for (int element : vec)
        {
            if (element == value)
            {
                return true; // Value found
            }
        }
        return false; // Value not found
    }

    // Function to merge two vectors and remove duplicates
    vector<int> mergeAndRemoveDuplicates(const vector<int> &vec1, const vector<int> &vec2)
    {
        vector<int> result;

        // Add elements from vec1
        for (int num : vec1)
        {
            result.push_back(num);
        }

        // Add elements from vec2 that are not already in result
        for (int num : vec2)
        {
            bool found = false;

            // Check if num is already in result
            for (int existingNum : result)
            {
                if (existingNum == num)
                {
                    found = true;
                    break;
                }
            }

            // If not found, add to result
            if (!found)
            {
                result.push_back(num);
            }
        }

        return result;
    }
};

int main()
{
    SetSolver solver;
    // Define your input Sudoku board as a vector of strings
    std::vector<std::string> inputBoard = {
        "-6*300**00",
        "41*30*8*6",
        "**0**0***",
        "***-2***70",
        "0-9**6**00",
        "0****0***",
        "**804*06*",
        "**9*-12***",
        "00560-42*0"

    };

    // Populate the Sudoku board
    solver.PopulateBoard(inputBoard);

    solver.PrintFullBoard();
    solver.Solve();

    return 0;
}
