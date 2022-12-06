#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

#define internal static

struct String
{
    const char *data;
    size_t count;
};

struct Set
{
    char data[26];
    size_t count;
};

internal bool set_contains(const Set *set, const char item)
{
    for (size_t i = 0; i < set->count; ++i) {
        if (set->data[i] == item) return(true);
    }

    return(false);
}

internal void set_append(Set *set, char item)
{
    if (!set_contains(set, item)) {
        set->data[set->count] = item;
        set->count += 1;
    }
}

internal String get_data(const char *filename)
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
    
    return(slurped);
}

internal void part_1(const String *slurped)
{
    for (size_t i = 0; i < slurped->count; ++i) {
        assert(i + 3 < slurped->count);
        
        Set set = {};

        set_append(&set, slurped->data[i + 0] - 'a');
        set_append(&set, slurped->data[i + 1] - 'a');
        set_append(&set, slurped->data[i + 2] - 'a');
        set_append(&set, slurped->data[i + 3] - 'a');

        if (set.count == 4) {
            printf("%zu\n", i + 4);
            break;
        }
    }
}

internal void part_2(const String *slurped)
{
    for (size_t i = 0; i < slurped->count; ++i) {
        assert(i + 13 < slurped->count);
        
        Set set = {};

        for (int j = 0; j < 14; ++j) {
             set_append(&set, slurped->data[i + j] - 'a');
        }

        if (set.count == 14) {
            printf("%zu\n", i + 14);
            break;
        }
    }
}

int main()
{
    String slurped = get_data("input.txt");

    part_1(&slurped);
    part_2(&slurped);
    
    free((char *) slurped.data);
    
    return 0;
}
