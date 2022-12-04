#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cassert>

#define internal static

struct String
{
    const char *data;
    size_t count;
};

struct Range
{
    int start;
    int end;
};

struct Range_Pair
{
    Range first;
    Range second;
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

internal int find_index(const String *str, const char c)
{
    for (size_t i = 0; i < str->count; ++i) {
        if (str->data[i] == c) {
            return(static_cast<int> (i));
        }
    }
    
    return(-1);
}

internal String get_substring(const String *str, int start, int end)
{
    assert(start >= 0 && start <= str->count);
    assert(end >= 0 && end <= str->count);
    assert(end >= start);
    
    String substr = {};
    substr.data = str->data + start;
    substr.count = end - start;
    
    return(substr);
}

internal int string_to_int(const String *str)
{
    int ans = 0;
    for (size_t i = 0; i < str->count; ++i) {
        ans = (ans * 10) + static_cast<int> (str->data[i] - '0');
    }

    return(ans);
}

internal Range get_range_values(const String *range)
{
    int numbers_split = find_index(range, '-');
    String start = get_substring(range, 0, numbers_split);
    String end = get_substring(range, numbers_split + 1, static_cast<int> (range->count));

    Range values = {};
    values.start = string_to_int(&start);
    values.end = string_to_int(&end);
    
    return(values);
}

internal Range_Pair get_range_pairs(const String *line)
{
    int range_split = find_index(line, ',');
    String r1 = get_substring(line, 0, range_split);
    String r2 = get_substring(line, range_split + 1, static_cast<int> (line->count));

    Range first = get_range_values(&r1);
    Range second = get_range_values(&r2);
    
    Range_Pair range_pair = {};

    range_pair.first = first;
    range_pair.second = second;
    
    return(range_pair);
}

internal std::vector<Range_Pair> get_data(const char *filename)
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

    std::vector<Range_Pair> data;
    
    while (slurped.count != 0) {
        String line = get_line(&slurped);
        Range_Pair pair = get_range_pairs(&line);
        
        data.emplace_back(pair);
    }
    
    free(buff);

    return(data);
}

internal inline bool range_is_subset(Range r1, Range r2)
{
    return((r1.start >= r2.start && r1.start <= r2.end) &&
           (r1.end >= r2.start && r1.end <= r2.end));
}

internal void part_1(const std::vector<Range_Pair> &data)
{
    int ans = 0;
    
    for (const Range_Pair &pair : data) {
        if (range_is_subset(pair.first, pair.second) || range_is_subset(pair.second, pair.first)) {
            ans += 1;
        }
    }

    printf("Part 1 Answer: %d\n", ans);
}

internal inline bool range_contains_one(Range r1, Range r2)
{
    return((r1.start >= r2.start && r1.start <= r2.end) ||
           (r1.end >= r2.start && r1.end <= r2.end));
}

internal void part_2(const std::vector<Range_Pair> &data)
{
    int ans = 0;
    
    for (const Range_Pair &pair : data) {
        if (range_contains_one(pair.first, pair.second) || range_contains_one(pair.second, pair.first)) {
            ans += 1;
        }
    }

    printf("Part 2 Answer: %d\n", ans);
}

int main()
{
    std::vector<Range_Pair> data = get_data("input.txt");

    part_1(data);
    part_2(data);
    
    return 0;
}
