#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <vector>

#define internal static

#define CRT_WIDTH 40
#define CRT_HEIGHT 6

struct String
{
    const char *data;
    size_t count;
};

enum Instruction
{
    NOOP = 0,
    ADDX,
};

struct Operation
{
    Instruction inst;
    int value;
};

struct CPU
{
    unsigned int current_cycle;
    int register_value;
    
    char display[CRT_HEIGHT * CRT_WIDTH];
};

internal inline unsigned int index(int row, int col)
{
    return(col + CRT_WIDTH * row);
}

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

internal int string_to_int(const String *str)
{
    bool negative = (str->data[0] == '-');
    
    int num = 0;
    for (size_t i = negative; i < str->count; ++i) {
        num = (num * 10) + static_cast<int> (str->data[i] - '0');
    }
    
    return(negative ? -num : num);
}

internal std::vector<Operation> get_data(const char *filename)
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

    std::vector<Operation> data = {};
    
    while (slurped.count != 0) {
        String line = get_line(&slurped);
        
        Operation op = {};
        op.inst = static_cast<Instruction> (line.data[0] == 'a');

        if (op.inst == ADDX) {
            String value_str = substring(&line, 5, line.count - 1);
            op.value = string_to_int(&value_str);
        }
        
        data.emplace_back(op);
    }
    
    free(buff);

    return(data);
}

internal void execute_cycles(unsigned int cycle_amount, CPU *cpu_unit, int &sum)
{
    for (unsigned int i = 0; i < cycle_amount; ++i) {
        if (cpu_unit->current_cycle > 20 && (cpu_unit->current_cycle - 20) % 40 == 0) {
            sum += static_cast<int> (cpu_unit->current_cycle) * cpu_unit->register_value;
        } else if (cpu_unit->current_cycle == 20) {
            sum += static_cast<int> (cpu_unit->current_cycle) * cpu_unit->register_value;
        }
        
        cpu_unit->current_cycle += 1;
    }
}

internal void part_1(const std::vector<Operation> &data)
{
    int sum = 0;
    CPU cpu_unit = { 1, 1, {} };
    
    for (const Operation &op : data) {
        switch (op.inst) {
            case NOOP: {
                execute_cycles(1, &cpu_unit, sum);
            } break;

            case ADDX: {
                execute_cycles(2, &cpu_unit, sum);
                cpu_unit.register_value += op.value;
            } break;
        }
    }

    printf("Answer Part 1: %d\n", sum);
}

internal inline bool is_between(int val, int low, int high)
{
    return(val >= low && val <= high);
}

internal void draw_sprite_in_cycles(unsigned int cycle_amount, CPU *cpu_unit)
{
    for (unsigned int i = 0; i < cycle_amount; ++i) {
        int row = (cpu_unit->current_cycle / 40);
        int col = (cpu_unit->current_cycle % 40);

        if (is_between(col, cpu_unit->register_value - 1, cpu_unit->register_value + 1)) {
            cpu_unit->display[index(row, col)] = '#';
        } else {
            cpu_unit->display[index(row, col)] = '.';
        }

        cpu_unit->current_cycle += 1;
    }
}

internal void part_2(const std::vector<Operation> &data)
{
    CPU cpu_unit = {};
    cpu_unit.register_value = 1;
    
    for (const Operation &op : data) {
        switch (op.inst) {
            case NOOP: {
                draw_sprite_in_cycles(1, &cpu_unit);
            } break;

            case ADDX: {
                draw_sprite_in_cycles(2, &cpu_unit);
                cpu_unit.register_value += op.value;
            } break;
        }
    }
    
    for (int i = 0; i < CRT_HEIGHT; ++i) {
        for (int j = 0; j < CRT_WIDTH; ++j) {
            printf("%c", cpu_unit.display[index(i, j)]);
        }

        printf("\n");
    }
}

int main()
{
    std::vector<Operation> data = get_data("input.txt");

    part_1(data);
    part_2(data);
    
    return 0;
}
