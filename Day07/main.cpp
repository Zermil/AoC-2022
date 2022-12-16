#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <vector>
#include <algorithm>

#define internal static

#define NODE_CAP 512
#define FILESYSTEM_SPACE 70000000
#define UPDATE_SPACE 30000000

struct String
{
    const char *data;
    size_t count;
};

enum File_Type
{
    DIRECTORY = 0,
    BINARY,
};

struct Node
{
    File_Type type;
    unsigned int size;
    String name;
    Node *parent;
    
    std::vector<Node *> content;
};

struct Arena
{
    Node *pool;
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

internal String substring(const String *str, size_t start, size_t end)
{
    assert(start < str->count && end < str->count);
    assert(end >= start);
    
    String substr = {};
    substr.data = str->data + start;
    substr.count = end - start + 1;
    
    return(substr);
}

internal bool string_compare(const String *str, const String *cmp)
{
    for (size_t i = 0; i < str->count; ++i) {
        if (str->data[i] != cmp->data[i]) return(false);
    }
    
    return(true);
}

internal String string(const char *text)
{
    String str = {};
    str.data = text;
    str.count = strlen(text);
    
    return(str);
}

internal size_t index_of(const String *str, const char c)
{
    for (size_t i = 0; i < str->count; ++i) {
        if (str->data[i] == c) return(i);
    }
    
    return(0);
}

internal unsigned int string_to_int(const String *str)
{
    unsigned int num = 0;
    for (size_t i = 0; i < str->count; ++i) {
        num = (num * 10) + static_cast<unsigned int> (str->data[i] - '0');
    }
    
    return(num);
}

internal Node *arena_get(Arena *memory_arena)
{
    assert(memory_arena->count < NODE_CAP);
    return(&memory_arena->pool[memory_arena->count++]); 
}

internal void arena_free(Arena *memory_arena)
{
    assert(memory_arena->pool != NULL);
    
    free(memory_arena->pool);
    memory_arena->count = 0;
}

internal Node* find_directory(const Arena *memory_arena, const Node *current_dir, const String *str)
{
    for (size_t i = 0; i < memory_arena->count; ++i) {
        String *filename = &memory_arena->pool[i].name;
        
        if (string_compare(filename, str) && memory_arena->pool[i].parent == current_dir) {
            return(&memory_arena->pool[i]);
        }
    }

    return(NULL);
}

internal void parse_input(String *slurped, Arena *arena)
{
    Node *current_directory = NULL;
    
    while (slurped->count != 0) {
        String line = get_line(slurped);
        
        if (line.data[0] == '$') {
            String command = substring(&line, 2, 3);
            String cd = string("cd");
            
            if (string_compare(&command, &cd)) {
                String arg = substring(&line, 5, line.count - 1);
                String empty = string("..");
                
                if (string_compare(&arg, &empty)) {
                    current_directory = current_directory->parent;
                    continue;
                }
                
                Node *current_file = find_directory(arena, current_directory, &arg);
                
                if (current_file == NULL) {
                    current_file = arena_get(arena);
                    
                    current_file->type = DIRECTORY;
                    current_file->size = 0;
                    current_file->name = arg;
                    current_file->parent = current_directory;
                }
                
                current_directory = current_file;
            }
        } else {
            size_t space = index_of(&line, ' ');
            String dir = string("dir");
            
            String filename = substring(&line, space + 1, line.count - 1);
            String prefix = substring(&line, 0, space - 1);

            Node *directory_file = arena_get(arena);

            if (string_compare(&prefix, &dir)) {
                directory_file->type = DIRECTORY;
                directory_file->size = 0;
            } else {
                directory_file->type = BINARY;
                directory_file->size = string_to_int(&prefix);
            }
            
            directory_file->name = filename;
            directory_file->parent = current_directory;
            
            current_directory->content.push_back(directory_file);
        }
    }
}

internal const char *get_file_contents(const char *filename)
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
    
    return(buff);
}

internal void calculate_directory_size(Node *node)
{
    for (Node *file : node->content) {
        if (file->type == BINARY) {
            node->size += file->size;
        } else {
            calculate_directory_size(file);
            node->size += file->size;
        }
    }
}

internal void part_1(const Arena *arena)
{
    unsigned int ans = 0;
    
    for (size_t i = 0; i < arena->count; ++i) {
        const Node &node = arena->pool[i];
        if (node.type == BINARY) continue;
        
        if (node.size <= 100000) {
            ans += node.size;
        }
    }

    printf("Answer part 1: %u\n", ans);
}

internal void part_2(const Arena *arena)
{
    unsigned int unused_space = FILESYSTEM_SPACE - arena->pool[0].size;
    std::vector<unsigned int> directories = {};
    
    for (size_t i = 0; i < arena->count; ++i) {
        const Node &node = arena->pool[i];
        if (node.type == BINARY) continue;

        unsigned int space_after_delete = unused_space + arena->pool[i].size;
        
        if (space_after_delete >= UPDATE_SPACE) {
            directories.emplace_back(arena->pool[i].size);
        }
    }

    std::sort(directories.begin(), directories.end());
    printf("Answer part 2: %u\n", directories[0]);
}

int main()
{
    const char *file_contents = get_file_contents("input.txt");
    String slurped = string(file_contents);
    
    Arena arena = {};
    arena.pool = (Node *) malloc(NODE_CAP * sizeof(Node));
    memset(arena.pool, 0, NODE_CAP * sizeof(Node));
    
    parse_input(&slurped, &arena);
    calculate_directory_size(&arena.pool[0]);
    
    part_1(&arena);
    part_2(&arena);

    arena_free(&arena);
    free((void *) file_contents);
        
    return 0;
}
