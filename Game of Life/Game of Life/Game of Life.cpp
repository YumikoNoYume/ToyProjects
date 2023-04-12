#include <iostream> 
#include <process.h> // For exit() function
#include <string> 
#include <sstream> // For extracting numbers from strings
#include <stdlib.h> // For randomizing cell's status     
#include <utility> // For std::pair

enum class Cell : unsigned char {dead, alive}; 

class CellGrid 
{
public: 
	class GridRow // For operator[]
	{
	public:
		GridRow(CellGrid& m_g, std::size_t r) : main_grid(m_g), row(r) { } 

		Cell& operator[](std::size_t column) 
		{
			return main_grid.grid[row][column];
		}
	private:
		CellGrid& main_grid;
		std::size_t row;
	}; 

	class ConstGridRow // For operator[]
	{
	public:
		ConstGridRow(const CellGrid& m_g, std::size_t r) : main_grid(m_g), row(r) { }

		const Cell& operator[](std::size_t column) const
		{
			return main_grid.grid[row][column];
		}
	private:
		const CellGrid& main_grid;
		std::size_t row;
	};

	static const std::size_t max = 50; 

	CellGrid() : max_row(1), max_column(1) // Is used in GameOfLifeTUI and for default display
	{
		Clear();
	}

	CellGrid(std::size_t r, std::size_t c) : max_row(r), max_column(c) 
	{
		CheckMax(max_row); 
		CheckMax(max_column); 
		Clear();
	}

	GridRow operator[] (std::size_t row) 
	{
		return GridRow{ *this, row };
	}

	ConstGridRow operator[] (std::size_t row) const
	{
		return ConstGridRow{ *this, row };
	}

	std::size_t GetMaxRow() const
	{
		return max_row;
	}

	std::size_t GetMaxColumn() const
	{
		return max_column;
	}

	void Clear()
	{
		for (std::size_t row_index = 0; row_index < max_row; row_index++)
		{
			for (std::size_t column_index = 0; column_index < max_column; column_index++)
			{
				grid[row_index][column_index] = Cell::dead;
			}
		}
	}
private:
	static void CheckMax(std::size_t i) //Checks grid's size in resize function
	{
		if (i >= max) { std::cout << "\nError. Size is more than max"; exit(1); }
	}

	Cell grid[max][max]; 
	std::size_t max_row;
	std::size_t max_column;
};

class GameOfLifeRules 
{
public:
	static void TransformCellGrid(CellGrid& cell_grid) // Is used for simulate function
	{
		Stack stack;
		std::pair<std::size_t, std::size_t> pair;
		for (std::size_t row_index = 0; row_index < cell_grid.GetMaxRow(); row_index++)
		{
			for (std::size_t column_index = 0; column_index < cell_grid.GetMaxColumn(); column_index++)
			{
				int alive_cells = CountAliveNeighbourCells(cell_grid, row_index, column_index); 
				pair = std::make_pair(row_index, column_index);
				if (cell_grid[row_index][column_index] == Cell::alive)
				{
					if (alive_cells < 2 || alive_cells > 3)
					{
						stack.Push(pair); //Pushes cell's index in stack 
					}
				}
				else
				{
					if (alive_cells == 3)
					{
						stack.Push(pair);
					}
				}
			}
		}
		while (stack.GetSize() != 0)
		{
			auto[row,col] = stack.Pop();
			TransformCell(cell_grid[row][col]);
		}
	}

	static void TransformCell(Cell& cell) // Is used for set function
	{
		if (cell == Cell::alive) { cell = Cell::dead; }
		else {cell = Cell::alive; }
	}

	static void Clear(CellGrid& cell_grid) // Is used for clear function
	{
		cell_grid.Clear();
	}

private:
	static int CountAliveNeighbourCells(const CellGrid& cell_grid, std::size_t r, std::size_t c) 
	{
		std::size_t left_column = 0, right_column = 0, up_row = 0, down_row = 0;
		if (c == 0) { left_column = cell_grid.GetMaxColumn() - 1; }
		else { left_column = c - 1; }
		if (c == cell_grid.GetMaxColumn() - 1) { right_column = 0; }
		else { right_column = c + 1; }
		if (r == 0) { up_row = cell_grid.GetMaxRow() - 1; }
		else { up_row = r - 1; }
		if (r == cell_grid.GetMaxRow() - 1) { down_row = 0; }
		else { down_row = r + 1; }

		const auto check = [&](std::size_t tr, std::size_t tc)
		{
			if (cell_grid[tr][tc] == Cell::alive) { return 1; }
			else { return 0; }
		};
		return check(up_row, left_column) + check(up_row, c) + check(up_row, right_column)
			+ check(r, left_column) + check(r, right_column)
			+ check(down_row, left_column) + check(down_row, c) + check(down_row, right_column);
	}

	class Stack // To remember the cells to transform //std::pair
	{
	public:
		Stack() : top(0)
		{
			for (std::size_t index = 0; index < max; index++)
			{
				std::pair<std::size_t, std::size_t> p(0, 0);
				stack[index] = p; 
			}
		}
		void Push(std::pair<std::size_t, std::size_t> p)
		{
			stack[top++] = p;
		}
		std::pair<std::size_t,std::size_t> Pop() 
		{
			return stack[--top];
		}
		std::size_t GetSize() const
		{
			return top;
		}
	private:
		static const std::size_t max = CellGrid::max * CellGrid::max;
		std::pair<std::size_t, std::size_t> stack[max];
		std::size_t top;
	};
};

class GameOfLifeTUI
{
public:
	GameOfLifeTUI() : cell_grid() {}
	
	void Run()
	{
		ShowCommnds();
		ShowGrid();
		bool exit_or_no = true;
		while(exit_or_no)
		{
			std::string command;
			std::cout << "\n\nEnter the command: ";
			std::getline(std::cin, command);
			system("cls");
			if (command.find("commands") != std::string::npos)
			{
				ShowCommnds();
				ShowGrid();
			}
			if (command.find("resize") != std::string::npos)
			{
				command.erase(0, 6);
				std::size_t row = ExtractNumber(command);
				std::size_t column = ExtractNumber(command);
				CellGrid temp_greed{ row, column };
				cell_grid = temp_greed;
				ShowGrid();
			}
			if (command.find("clear") != std::string::npos)
			{
				GameOfLifeRules::Clear(cell_grid);
				ShowGrid();
			}
			if (command.find("set") != std::string::npos)
			{
				command.erase(0, 3);
				std::size_t row = ExtractNumber(command);
				std::size_t column = ExtractNumber(command);
				GameOfLifeRules::TransformCell(cell_grid[row - 1][column - 1]);
				ShowGrid();
			}
			if (command.find("randomize") != std::string::npos)
			{
				command.erase(0, 9);
				std::size_t possibility = ExtractNumber(command);
				for (std::size_t row_index = 0; row_index < cell_grid.GetMaxRow(); row_index++)
				{
					for (std::size_t column_index = 0; column_index < cell_grid.GetMaxColumn(); column_index++)
					{
						int result = rand() % 100 + 1;
						if (result <= possibility) { GameOfLifeRules::TransformCell(cell_grid[row_index][column_index]); }
					}
				}
				ShowGrid();
			}
			if (command.find("simulate") != std::string::npos)
			{
				GameOfLifeRules::TransformCellGrid(cell_grid);
				ShowGrid();
			}
			if (command.find("exit") != std::string::npos)
			{
				exit_or_no = false;
			}
		}
	}
private:
	static void ShowCommnds()
	{
		std::cout << "Here are the commands you can use:\n" <<
			"1) commands - Shows a list of commands;\n" <<
			"2) resize r c - Sets the grid with r rows and c columns;\n" <<
			"3) clear - Returns all cells to a dead state;\n" <<
			"4) set x y - Changes the state of the cell at x row and c column;\n" <<
			"5) randomize x - Sets up a grid in which with a x% chance each cell can change its state;\n" <<
			"6) simulate - Acts one step of Game of Life;\n" <<
			"7) exit - Cancels the project;\n\n";
	}
	void ShowGrid() const
	{
		for (std::size_t row_index = 0; row_index < cell_grid.GetMaxRow(); row_index++)
		{
			for (std::size_t column_index = 0; column_index < cell_grid.GetMaxColumn(); column_index++)
			{
				if (cell_grid[row_index][column_index] == Cell::dead) 
				{
					std::cout << char(176);
				}
				else {
					std::cout << char(178);
				}
			}
			std::cout << '\n';
		}
	}
	
	static int ExtractNumber(std::string& str)
	{
		str.erase(0, 1);
		std::stringstream temp_num;
		str.push_back(char(32));
		while (str.at(0) != char(32))
		{
				temp_num << str.at(0);
				str.erase(0, 1);
		}
		int num = 0;
		temp_num >> num;
		return num;
	}

	CellGrid cell_grid;
};

int main()
{
	GameOfLifeTUI process;
	process.Run();
	return 0;
}