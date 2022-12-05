#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>

#define internal static

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

internal int string_to_int(String *str)
{
    int num = 0;
    for (size_t i = 0; i < str->count; ++i) {
        num = (num * 10) + ((int) (str->data[i] - '0'));
    }
    
    return(num);
}

internal std::vector<int> get_data(const char *filename)
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
    
    std::vector<int> data;
    int current_sum = 0;
    
    while (slurped.count != 0) {
        String line = get_line(&slurped);
        int number = string_to_int(&line);
        
        if (number == 0) {
            data.emplace_back(current_sum);
            current_sum = 0;
        } else {
            current_sum += number;
        }
    }

    // EOF
    if (current_sum > 0) data.emplace_back(current_sum);
    std::sort(data.begin(), data.end());
    
    free(buff);

    return(data);
}

int main()
{
    std::vector<int> data = get_data("input.txt");

    printf("Part 1 Answer: %d\n", data[data.size() - 1]);
    printf("Part 2 Answer: %d\n", data[data.size() - 1] + data[data.size() - 2] + data[data.size() - 3]);
    
    return 0;
}
