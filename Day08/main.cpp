#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#define internal static

typedef std::vector<int> row_t;
typedef std::vector<row_t> grid_t;

struct String
{
    const char *data;
    size_t count;
};

internal String get_line(String *text)
{
    size_t i = 0;
    while (i < text->count && text->data[i] != '\n') {
        i += 1;
    }

    String str = {};
    str.data = text->data;
    
    // NOTE(Aiden): Because I was doing this on Windows, which is backwards compatible
    // with 1900s typewriters, I had to deal with "\r\n", this was the simplest way.
    if (str.data[i - 1] == '\r') {
        str.count = i - 1;
    } else {
        str.count = i;
    }

    if (i < text->count) {
        text->count -= i + 1;
        text->data += i + 1;
    } else {
        text->count -= i;
        text->data += i;
    }
    
    return(str);
}

internal grid_t get_data(const char *filename)
{
    FILE *file = fopen(filename, "rb");

    if (file == nullptr) {
        fprintf(stderr, "[ERROR] Could not open file: %s\n", filename);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buff = (char *) malloc(size + 1);
    fread(buff, 1, size, file);
    fclose(file);
    
    buff[size] = '\0';
    
    String slurped = {
        buff,
        size
    };

    grid_t grid = {};
    
    while (slurped.count != 0) {
        String line = get_line(&slurped);

        row_t row = {};
        for (size_t i = 0; i < line.count; ++i) {
            row.emplace_back(static_cast<int> (line.data[i] - '0'));
        }
        
        grid.emplace_back(row);
    }

    free(buff);
    
    return(grid);
}

internal int check_visibility(const grid_t &grid, size_t row, size_t col)
{
    int visible = 0b1111;
    
    // Check col
    for (size_t i = 0; i < row; ++i) {
        if (grid[i][col] >= grid[row][col]) {
            visible &= 0b0111;
            break;
        }
    }

    for (size_t i = row + 1; i < grid.size(); ++i) {
        if (grid[i][col] >= grid[row][col]) {
            visible &= 0b1011;
            break;
        }
    }
    
    // Check row
    for (size_t i = 0; i < col; ++i) {
        if (grid[row][i] >= grid[row][col]) {
            visible &= 0b1101;
            break;
        }
    }

    for (size_t i = col + 1; i < grid[row].size(); ++i) {
        if (grid[row][i] >= grid[row][col]) {
            visible &= 0b1110;
            break;
        }
    }

    return(visible);
}

internal void part_1(const grid_t &grid)
{
    int ans = static_cast<int> (4 * grid.size() - 4);
    
    for (size_t i = 1; i < grid.size() - 1; ++i) {
        for (size_t j = 1; j < grid[i].size() - 1; ++j) {
            ans += (check_visibility(grid, i, j) != 0);
        }
    }

    printf("Answer: %d\n", ans);
}

internal int check_scenic(const grid_t &grid, int row, int col)
{
    int score[4] = {}; 
    
    // Check col
    for (int i = row - 1; i >= 0; --i) {
        score[0] += 1;
        if (grid[i][col] >= grid[row][col]) break;
    }
    
    for (size_t i = row + 1; i < grid.size(); ++i) {
        score[1] += 1;        
        if (grid[i][col] >= grid[row][col]) break;
    }
   
    // Check row
    for (int i = col - 1; i >= 0; --i) {
        score[2] += 1;
        if (grid[row][i] >= grid[row][col]) break;
    }
    
    for (size_t i = col + 1; i < grid[row].size(); ++i) {
        score[3] += 1;
        if (grid[row][i] >= grid[row][col]) break;        
    }
    
    return(score[0] * score[1] * score[2] * score[3]);
}

internal void part_2(const grid_t &grid)
{
    int ans = -1;
    
    for (size_t i = 0; i < grid.size(); ++i) {
        for (size_t j = 0; j < grid[i].size(); ++j) {
            int scenic_score = check_scenic(grid, static_cast<int> (i), static_cast<int> (j));
            if (ans < scenic_score) ans = scenic_score;
        }
    }

    printf("Answer: %d\n", ans);
}

int main()
{
    grid_t grid = get_data("input.txt");

    part_1(grid);
    part_2(grid);
    
    return 0;
}
