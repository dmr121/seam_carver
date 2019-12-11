// David Rozmajzl

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <algorithm>

enum Line {
	Horizontal,
	Vertical,
};

int energy(int**, int, int, int, int);
int leastEnergy(int**, Line, int, int, int, int);
void adjustMatrix(int**, Line line, int**, int &numRows, int &numColumns);
void traceLeastPath(int**, Line line, int numRows, int numColumns);

int
main(int argc, char* argv[])
{
	// Simple conditional to make sure the user provides correct number or args
	// Aborts if incorrect # of args
	if (argc == 3) {
		std::cerr << "Need 1 more argument\n";
		abort();
	} else if (argc == 2) {
		std::cerr << "Need 2 more arguments\n";
		abort();
	} else if (argc == 1) {
		std::cerr << "Need 3 more arguments\n";
		abort();
	} else if (argc > 4) {
		std::cerr << "Too many arguments\n";
		abort();
	}

	// Open the file and quit if it cannot be opened
	std::ifstream image;
	image.open(argv[1]);
	if (!image.is_open()) {
		std::cerr << "Image cannot be opened\n";
		abort();
	}

	std::string input;
	// Image type should be P2
	// If it's not, abort
	std::getline(image, input);
	if (input.substr(0, 2) != "P2") {
		abort();
	}

	// Any comments are located after P2. If there are comments, this loop
	// sifts past them until it reaches the number of rows and columns
	do {
		std::getline(image, input);
	} while(input[0] == '#');

	// Input should now be a string with the number of rows and columns
	// We convert the sting to a streamable object
	int rows, columns;
	{
		std::stringstream input_(input);
		input_ >> columns >> rows;
	}

	// Create the 2D array that is "rows" rows and "columns" columns 
	int **imageMap;
	imageMap = new int *[rows];
	for (int i = 0; i < rows; ++i) {
		imageMap[i] = new int[columns];
	}

	// Retrieve the max pixel value and we will just ignore it
	std::getline(image, input);

	// Filling in the 2D array with values from the file
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < columns; ++j) {
			image >> input;
			imageMap[i][j] = std::stoi(input);
		}
	}

	// Get the number of columns and rows to remove
	input = argv[2];
	int columnsToRemove = std::stoi(input);
	input = argv[3];
	int rowsToRemove = std::stoi(input);

	if (columnsToRemove > columns) {
		std::cerr << "Too many vertical seams to remove\n";
		abort();
	}

	if (rowsToRemove > rows) {
		std::cerr << "Too many horizontal seams to remove\n";
		abort();
	}

	// Creating the energy matrix
	int **energyMatrix;
	energyMatrix = new int *[rows];
	for (int i = 0; i < rows; ++i) {
		energyMatrix[i] = new int[columns];
	}

	// Creating the cumulative energy matrix
	int **cumuEnergyMatrix;
	cumuEnergyMatrix = new int *[rows];
	for (int i = 0; i < rows; ++i) {
		cumuEnergyMatrix[i] = new int[columns];
	}

	// Removing the columns
	for (int k = 0; k < columnsToRemove; ++k) {
		// Calculate the energy for each individual pixel in the matrix
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < columns; ++j) {
				energyMatrix[i][j] = energy(imageMap, i, j, rows, columns);
			}
		}

		// Copy energy matrix to cumulative energy matrix
		cumuEnergyMatrix = energyMatrix;

		// Creating the cumulative energy matrix
		for (int i = 1; i < rows; ++i) {
			for (int j = 0; j < columns; ++j) {
				cumuEnergyMatrix[i][j] += leastEnergy(cumuEnergyMatrix, Vertical, i, j, rows, columns);
			}
		}

		// Find the least energy path
		traceLeastPath(cumuEnergyMatrix, Vertical, rows, columns);
		// Readjust the image map matrix
		adjustMatrix(cumuEnergyMatrix, Vertical, imageMap, rows, columns);
	}

	// Removing the rows
	for (int k = 0; k < rowsToRemove; ++k) {
		// Calculate the energy for each individual pixel in the matrix
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < columns; ++j) {
				energyMatrix[i][j] = energy(imageMap, i, j, rows, columns);
			}
		}

		// Copy energy matrix to cumulative energy matrix
		cumuEnergyMatrix = energyMatrix;

		// Creating the cumulative energy matrix
		for (int i = 1; i < columns; ++i) {
			for (int j = 0; j < rows; ++j) {
				cumuEnergyMatrix[j][i] += leastEnergy(cumuEnergyMatrix, Horizontal, j, i, rows, columns);
			}
		}

		// Find the least energy path
		traceLeastPath(cumuEnergyMatrix, Horizontal, rows, columns);
		// Readjust the image map matrix
		adjustMatrix(cumuEnergyMatrix, Horizontal, imageMap, rows, columns);
	}

	int maxValue = 0;
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < columns; ++j) {
			if (imageMap[i][j] > maxValue) {
				maxValue = imageMap[i][j];
			}
		}
	}

	// Open the output file
	std::ofstream seamedImage;
	std::string fileName = argv[1];
	// Remove the extension type
	fileName = fileName.substr(0, fileName.length() - 4);
	fileName += "_processed.pgm";
	seamedImage.open(fileName);
	// Make sure the file opens correctly
	if (!seamedImage.is_open()) {
		std::cerr << "Image cannot be opened\n";
		abort();
	}

	seamedImage << "P2\n";
	seamedImage << "# Edited by David Rozmajzl\n";
	seamedImage << columns << " " << rows << '\n';
	seamedImage << maxValue << '\n';
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < columns; ++j) {
			seamedImage << imageMap[i][j];
			if (j != columns - 1) {
				seamedImage << " ";
			}
		}
		seamedImage << '\n';
	}

	seamedImage.close();
}

void traceLeastPath(int **matrix, Line line, int numRows, int numColumns) {
	if (line == Vertical) {
		if (numColumns <= 0) {
			return;
		}
		int x = 0;
		int y = numRows - 1;
		// Finding smallest energy pixel coordinates first in bottom row
		for (int i = 0; i < numColumns; ++i) {
			if (matrix[y][i] < matrix[y][x]) {
				x = i;
			}
		}
		// Negative one indicates a pixel to remove
		matrix[y][x] = -1;

		if (numColumns == 1) {
			return;
		}

		// Trace path from bottom to top
		for (int i = numRows - 2; i >= 0; --i) {
			int index = x;
			if (index == 0) {
				for (int j = index; j <= index + 1; ++j) {
					if (matrix[i][j] < matrix[i][x]) {
						x = j;
					}
				}
			}

			if (index > 0 && index < numColumns - 1) {
				--index;
				--x;
				for (int j = index; j <= index + 2; ++j) {
					if (matrix[i][j] < matrix[i][x]) {
						x = j;
					}
				}
			}

			if (index == numColumns - 1) {
				--index;
				--x;
				for (int j = index; j <= index + 1; ++j) {
					if (matrix[i][j] < matrix[i][x]) {
						x = j;
					}
				}
			}

			matrix[i][x] = -1;
		}
	} else if (line == Horizontal) {
		if (numRows <= 0) {
			return;
		}
		int x = numColumns - 1;
		int y = 0;
		// Finding smallest energy pixel coordinates first in right column
		for (int i = 0; i < numRows; ++i) {
			if (matrix[i][x] < matrix[y][x]) {
				y = i;
			}
		}
		// Negative one indicates a pixel to remove
		matrix[y][x] = -1;

		if (numRows == 1) {
			return;
		}

		// Trace path right to left;
		for (int i = numColumns - 2; i >= 0; --i) {
			int index = y;
			if (index == 0) {
				for (int j = index; j <= index + 1; ++j) {
					if (matrix[j][i] < matrix[y][i]) {
						y = j;
					}
				}
			}

			if (index > 0 && index < numRows - 1) {
				--index;
				--y;
				for (int j = index; j <= index + 2; ++j) {
					if (matrix[j][i] < matrix[y][i]) {
						y = j;
					}
				}
			}

			if (index == numRows - 1) {
				--index;
				--y;
				for (int j = index; j <= index + 1; ++j) {
					if (matrix[j][i] < matrix[y][i]) {
						y = j;
					}
				}
			}

			matrix[y][i] = -1;
		}
	}

}

void adjustMatrix(int **matrix, Line line, int **secondMatrix, int &numRows, int &numColumns) {
	if (line == Vertical) {
		for (int i = 0; i < numRows; ++i) {
			for (int j = 0; j < numColumns - 1; ++j) {
				if (matrix[i][j] == -1) {
					// Pushes -1's to the right
					std::swap(secondMatrix[i][j], secondMatrix[i][j + 1]);
					std::swap(matrix[i][j], matrix[i][j + 1]);
				}
			}
		}
		// Decrease the number of columns the image contains
		--numColumns;
	} else if (line == Horizontal) {
		for (int i = 0; i < numColumns; ++i) {
			for (int j = 0; j < numRows - 1; ++j) {
				if (matrix[j][i] == -1) {
					// Pushes -1's to the bottom
					std::swap(secondMatrix[j][i], secondMatrix[j + 1][i]);
					std::swap(matrix[j][i], matrix[j + 1][i]);
				}
			}
		}
		// Decrease the number of rows the image contains
		--numRows;
	}
}

int leastEnergy(int **matrix, Line line, int row, int column, int numRows, int numColumns) {
	int energy = 0;

	if (line == Vertical) {
		// If pixel is in left column
		if (column == 0) {
			energy = std::min(matrix[row - 1][column], matrix[row - 1][column + 1]);
		}
		// If pixel is in the middle columns
		if (column > 0 && column < numColumns - 1) {
			energy = std::min(std::min(matrix[row - 1][column - 1], matrix[row - 1][column]), matrix[row - 1][column + 1]);
		}
		// If pixel is in the right column
		if (column == numColumns - 1) {
			energy = std::min(matrix[row - 1][column - 1], matrix[row - 1][column]);
		}
	} else if (line == Horizontal) {
		// If pixel is in top row
		if (row == 0) {
			energy = std::min(matrix[row][column - 1], matrix[row + 1][column - 1]);
		}
		// If pixel is in the middle rows
		if (row > 0 && row < numRows - 1) {
			energy = std::min(std::min(matrix[row - 1][column - 1], matrix[row][column - 1]), matrix[row + 1][column - 1]);
		}
		// If pixel is in the bottom row
		if (row == numRows - 1) {
			energy = std::min(matrix[row - 1][column - 1], matrix[row][column - 1]);
		}
	}

	return energy;
}


int energy(int **matrix, int row, int column, int numRows, int numColumns) {
	int energy = 0;

	// If pixel is not in top row
	if (row > 0) {
		energy += std::abs(matrix[row - 1][column] - matrix[row][column]);
	}

	// If pixel is not in bottom row
	if (row < numRows - 1) {
		energy += std::abs(matrix[row + 1][column] - matrix[row][column]);
	}

	// If pixel is not in left column
	if (column > 0) {
		energy += std::abs(matrix[row][column - 1] - matrix[row][column]);
	}

	// If pixel is not in right column
	if (column < numColumns - 1) {
		energy += std::abs(matrix[row][column + 1] - matrix[row][column]);
	}

	return energy;
}
