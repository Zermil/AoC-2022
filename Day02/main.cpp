#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#define internal static

enum Target {
    A = 0,
    B,
    C,
};

enum Response {
    X = 0,
    Y,
    Z,
};

struct String
{
    const char *data;
    size_t count;
};

struct Move {
    Target target;
    Response response;
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

    char *buff = (char *) malloc(size);
    fread(buff, 1, size, file);
    fclose(file);
    
    String slurped = {
        buff,
        strlen(buff)
    };

    std::vector<Move> data;
    
    while (slurped.count != 0) {
        String line = get_line(&slurped);

        Target target = static_cast<Target> (line.data[0] - 'A');
        Response response = static_cast<Response> (line.data[2] - 'X');

        Move move = {
            target,
            response
        };
        
        data.emplace_back(move);
    }
    
    free(buff);

    return(data);
}

internal void part_1(const std::vector<Move> &data)
{
    int points_table[3][3] = {
        { 4, 8, 3 },
        { 1, 5, 9 },
        { 7, 2, 6 }
    };
    
    int points_total = 0;
    
    for (size_t i = 0; i < data.size(); ++i) {
        Target target = data[i].target;
        Response response = data[i].response;
        
        points_total += points_table[target][response];
    }
    
    printf("Part 1 Answer: %d\n", points_total);
}

internal void part_2(const std::vector<Move> &data)
{
    int points_table[3][3] = {
        { 3, 4, 8 },
        { 1, 5, 9 },
        { 2, 6, 7 }
    };
    
    int points_total = 0;
    
    for (size_t i = 0; i < data.size(); ++i) {
        Target target = data[i].target;
        Response response = data[i].response;
        
        points_total += points_table[target][response];
    }
    
    printf("Part 2 Answer: %d\n", points_total);
}

int main()
{
    std::vector<Move> data = get_data("input.txt");

    part_1(data);
    part_2(data);
    
    return 0;
}
