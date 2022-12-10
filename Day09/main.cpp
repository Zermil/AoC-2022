#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <vector>
#include <set>

#define internal static

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define SIGN(x) ((x) < 0 ? -1 : 1)

struct String
{
    const char *data;
    size_t count;
};

enum Direction
{
    UP = 0,
    DOWN,
    LEFT,
    RIGHT,
};

struct Move
{
    Direction dir;
    unsigned int count;
};

struct Point
{
    int row;
    int col;

    // NOTE(Aiden): Not thrilled
    bool operator==(const Point &rhs) const { return(row == rhs.row && col == rhs.col); }
    bool operator<(const Point &rhs) const { return(row < rhs.row || (row == rhs.row && col < rhs.col)); }
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

internal String substring(const String *str, size_t start, size_t end)
{
    assert(start < str->count && end < str->count);
    assert(end >= start);
    
    String substr = {};
    substr.data = str->data + start;
    substr.count = end - start + 1;
    
    return(substr);
}

internal unsigned int string_to_int(const String *str)
{
    unsigned int num = 0;

    for (size_t i = 0; i < str->count; ++i) {
        num = (num * 10) + static_cast<unsigned int> (str->data[i] - '0');
    }
    
    return(num);
}

internal std::vector<Move> get_data(const char *filename)
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

    std::vector<Move> data = {};
    
    while (slurped.count != 0) {
        String line = get_line(&slurped);
        String count_str = substring(&line, 2, line.count - 1);

        Move move = {};
        move.count = string_to_int(&count_str);

        switch (line.data[0]) {
            case 'U': {
                move.dir = UP;
            } break;

            case 'D': {
                move.dir = DOWN;
            } break;
                
            case 'L': {
                move.dir = LEFT;
            } break;
                
            case 'R': {
                move.dir = RIGHT;
            } break;
        }

        data.emplace_back(move);
    }
    
    free(buff);

    return(data);
}

internal void adjust_tail(const Point *head, Point *tail)
{
    int drow = head->row - tail->row;
    int dcol = head->col - tail->col;
    
    if (ABS(dcol) > 1 || ABS(drow) > 1) {
        if (ABS(dcol) > ABS(drow)) {
            tail->row = head->row;
            tail->col = head->col - SIGN(dcol);
        } else if (ABS(dcol) < ABS(drow)) {
            tail->col = head->col;
            tail->row = head->row - SIGN(drow);
        } else {
            tail->row = head->row - SIGN(drow);
            tail->col = head->col - SIGN(dcol);
        }
    }
}

internal void apply_move(const Direction direction, Point *rope, size_t rope_size)
{
    switch (direction) {
        case UP: {
            rope[0].row += 1;
        } break;

        case DOWN: {
            rope[0].row -= 1;                        
        } break;

        case LEFT: {
            rope[0].col -= 1;
        } break;

        case RIGHT: {
            rope[0].col += 1;
        } break;
    }

    for (size_t i = 1; i < rope_size; ++i) {
        adjust_tail(&rope[i - 1], &rope[i]);
    }
}

internal void part_1(const std::vector<Move> &data)
{
    Point rope[2] = {};
    std::set<Point> visited = {};
    
    for (const Move &move : data) {
        for (unsigned int i = 0; i < move.count; ++i) {
            apply_move(move.dir, rope, 2);
            visited.emplace(rope[1]);
        }
    }

    printf("Answer Part 1: %zu\n", visited.size());
}

internal void part_2(const std::vector<Move> &data)
{
    Point rope[10] = {};
    std::set<Point> visited = {};
    
    for (const Move &move : data) {
        for (unsigned int i = 0; i < move.count; ++i) {
            apply_move(move.dir, rope, 10);
            visited.emplace(rope[9]);
        }
    }
    
    printf("Answer Part 2: %zu\n", visited.size());
}

int main()
{
    std::vector<Move> data = get_data("input.txt");

    part_1(data);
    part_2(data);
    
    return 0;
}
