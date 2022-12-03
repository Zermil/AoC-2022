#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <vector>

#define internal static

struct String
{
    const char *data;
    size_t count;
};

// NOTE(Aiden): I wish C/C++ had multi-return functions by default.
struct Pair
{
    void *ptr;
    std::vector<String> lines;
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

internal Pair get_data(const char *filename)
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

    std::vector<String> lines;

    while (slurped.count != 0) {
        String line = get_line(&slurped);
        lines.emplace_back(line);
    }
    
    Pair data = {
        buff,
        lines
    };
    
    return(data);
}

internal int find_index_of(const String *str, const char c)
{
    for (size_t i = (str->count / 2); i < str->count; ++i) {
        if (c == str->data[i]) {
            return(static_cast<int> (i));
        }
    }
    
    return(-1);
}

internal inline int get_points(const char c)
{
    if (c >= 'a' && c <= 'z') return((c - 'a') + 1);
    if (c >= 'A' && c <= 'Z') return((c - 'A') + 27);
    
    return(-1);
}

internal void part_1(const std::vector<String> &lines)
{
    int sum = 0;
    
    for (const String &line : lines) {
        for (size_t i = 0; i < (line.count / 2); ++i) {
            int idx = find_index_of(&line, line.data[i]);
            
            if (idx != -1) {
                sum += get_points(line.data[i]);
                break;
            }
        }
    }

    printf("Part 1 Answer: %d\n", sum);
}

internal std::vector<int> get_chars_table(const String *line)
{
    std::vector<int> table(52);
    
    for (size_t i = 0; i < line->count; ++i) {
        if (isupper(line->data[i])) table[(line->data[i] - 'A') + 26] += 1;
        else table[line->data[i] - 'a'] += 1;
    }

    return(table);
}

internal void part_2(const std::vector<String> &lines)
{
    int sum = 0;
    
    for (size_t i = 0; i < lines.size(); i += 3) {
        std::vector<int> table_first = get_chars_table(&lines[i]);
        std::vector<int> table_second = get_chars_table(&lines[i + 1]);
        std::vector<int> table_third = get_chars_table(&lines[i + 2]);

        for (size_t j = 0; j < 52; ++j) {
            if (table_first[j] > 0 && table_second[j] > 0 && table_third[j] > 0) {
                sum += static_cast<int> (j) + 1;
                break;
            }
        }
    }

    printf("Part 2 Answer: %d\n", sum);
}

int main()
{
    Pair data = get_data("input.txt");

    part_1(data.lines);
    part_2(data.lines);
    
    free(data.ptr);
    
    return 0;
}
