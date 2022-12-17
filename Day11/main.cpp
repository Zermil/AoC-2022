#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <vector>
#include <deque>
#include <algorithm>

#define internal static

struct String
{
    const char *data;
    size_t count;
};

enum Operation_Kind
{
    NONE = 0,
    ADD,
    MUL,
    SQR,
};

struct Operation
{
    Operation_Kind kind;
    unsigned long long int argument;
};

struct Monkey
{
    std::deque<unsigned long long int> starting_items;
    Operation operation;
    unsigned long long int test;
    unsigned long long int if_true;
    unsigned long long int if_false;

    unsigned long long int inspected;
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

internal String string(const char *text)
{
    String str = {};
    str.data = text;
    str.count = strlen(text);
    
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

internal unsigned long long int string_to_int(const String *str)
{
    unsigned long long int num = 0;

    for (size_t i = 0; i < str->count; ++i) {
        num = (num * 10) + static_cast<unsigned long long int> (str->data[i] - '0');
    }
    
    return(num);
}

internal inline bool is_character_number(const char c)
{
    return(c >= '0' && c <= '9');
}

internal size_t string_index_of(const String *str, const char c)
{
    for (size_t i = 0; i < str->count; ++i) {
        if (str->data[i] == c) return(i);
    }
    
    return(str->count);
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

internal Monkey parse_monkey(const std::vector<String> &lines)
{
    Monkey monkey = {};
    
    const String lines_of_interest[] = {
        string("  Starting items: "),
        string("  Operation: new = old "),
        string("  Test: divisible by "),
        string("    If true: throw to monkey "),
        string("    If false: throw to monkey "),
    };
    
    String starting_items = substring(&lines[1], lines_of_interest[0].count, lines[1].count - 1);
    String operation = substring(&lines[2], lines_of_interest[1].count, lines[2].count - 3);
    String test = substring(&lines[3], lines_of_interest[2].count, lines[3].count - 1);
    String if_true = substring(&lines[4], lines_of_interest[3].count, lines[4].count - 1);
    String if_false = substring(&lines[5], lines_of_interest[4].count, lines[5].count - 1);

    while (starting_items.count != 0) {
        size_t comma = string_index_of(&starting_items, ',');
        String item = substring(&starting_items, 0, comma - 1);

        if (comma != starting_items.count) {
            starting_items.count -= (item.count + comma);
            starting_items.data += (item.count + comma);
        } else {
            starting_items.count -= item.count;
            starting_items.data += item.count;
        }

        monkey.starting_items.emplace_back(string_to_int(&item));
    }
    
    switch (operation.data[0]) {
        case '*': {
            monkey.operation.kind = MUL;
        } break;

        case '+': {
            monkey.operation.kind = ADD;
        } break;
    }

    if (is_character_number(operation.data[2])) {
        operation.data += 2;
        monkey.operation.argument = string_to_int(&operation);
    } else {
        monkey.operation.kind = SQR;
    }
    
    monkey.test = string_to_int(&test);
    monkey.if_true = string_to_int(&if_true);
    monkey.if_false = string_to_int(&if_false);
    monkey.inspected = 0;
    
    return(monkey);
}

internal std::vector<Monkey> parse_input(String *file_string)
{
    std::vector<String> lines = {};
    std::vector<Monkey> monkeys = {};
    
    while (file_string->count != 0) {
        String line = get_line(file_string);

        if (line.count == 0) {
            Monkey monkey = parse_monkey(lines);
            monkeys.emplace_back(monkey);

            lines.clear();
        } else {
            lines.emplace_back(line);
        }
    }

    // EOF
    if (lines.size() != 0) {
        Monkey monkey = parse_monkey(lines);
        monkeys.emplace_back(monkey);
    }

    return(monkeys);
}

internal unsigned long long int apply_operation(Operation operation, unsigned long long int item)
{
    switch (operation.kind) {
        case ADD: {
            return(item + operation.argument);
        } break;
     
        case MUL: {
            return(item * operation.argument);
        } break;
                    
        case SQR: {
            return(item * item);
        } break;
    }

    assert(false && "Unreachable");
    return(0);
}

internal void simulate_round_1(std::vector<Monkey> &monkeys)
{
    for (Monkey &monkey : monkeys) {
        while (!monkey.starting_items.empty()) {
            unsigned long long int item = apply_operation(monkey.operation, monkey.starting_items.front()) / 3;
            
            if (item % monkey.test == 0) {
                monkeys[monkey.if_true].starting_items.emplace_back(item);
            } else {
                monkeys[monkey.if_false].starting_items.emplace_back(item);
            }
            
            monkey.starting_items.pop_front();
            monkey.inspected += 1;
        }
    }
}

internal void part_1(std::vector<Monkey> monkeys)
{
    for (size_t i = 0; i < 20; ++i) {
        simulate_round_1(monkeys);
    }

    std::vector<unsigned long long int> inspected = {};
    for (const Monkey &monkey : monkeys) {
        inspected.emplace_back(monkey.inspected);
    }

    std::sort(inspected.begin(), inspected.end());
    printf("Answer Part 1: %llu\n", inspected[inspected.size() - 1] * inspected[inspected.size() - 2]);
}

internal void simulate_round_2(std::vector<Monkey> &monkeys, unsigned long long int limit)
{    
    for (Monkey &monkey : monkeys) {
        while (!monkey.starting_items.empty()) {
            unsigned long long int item = apply_operation(monkey.operation, monkey.starting_items.front()) % limit;
            
            if (item % monkey.test == 0) {
                monkeys[monkey.if_true].starting_items.emplace_back(item);
            } else {
                monkeys[monkey.if_false].starting_items.emplace_back(item);
            }
            
            monkey.starting_items.pop_front();
            monkey.inspected += 1;
        }
    }
}

internal void part_2(std::vector<Monkey> monkeys)
{
    unsigned long long int limit = 1;
    for (const Monkey &monkey : monkeys) {
        limit *= monkey.test;
    }
    
    for (size_t i = 0; i < 10000; ++i) {
        simulate_round_2(monkeys, limit);
    }

    std::vector<unsigned long long int> inspected = {};
    for (const Monkey &monkey : monkeys) {
        inspected.emplace_back(monkey.inspected);
    }

    std::sort(inspected.begin(), inspected.end());
    printf("Answer Part 2: %llu\n", inspected[inspected.size() - 1] * inspected[inspected.size() - 2]);
}

int main()
{
    const char *file_contents = get_file_contents("input.txt");
    String file_string = string(file_contents);
    
    std::vector<Monkey> monkeys = parse_input(&file_string);

    part_1(monkeys);
    part_2(monkeys);
    
    free((void *) file_contents);
    
    return 0;
}
