#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <stack>

#define internal static

struct String
{
    const char *data;
    size_t count;
};

struct Instruction
{
    int num_elements;
    int target;
    int destination;
};

typedef std::vector<std::stack<char>> multi_stack_t;
typedef std::vector<Instruction> instructions_t;

struct Stack_Inst
{
    multi_stack_t stacks;
    instructions_t instructions;
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

internal std::vector<String> split_string_to_array(String *text, char c)
{
    std::vector<String> result = {};
    
    String substr = {};
    size_t start = 0;
        
    for (size_t i = 0; i < text->count; ++i) {
        if (text->data[i] == c) {
            substr.data = text->data + start;
            substr.count = i - start;
            
            result.emplace_back(substr);
            start = i + 1;
        }
    }

    // EOF
    if (static_cast<int> (text->count - start) != 0) {
        substr.data = text->data + start;
        substr.count = text->count - start;
            
        result.emplace_back(substr);
    }
    
    return(result);
}

internal int string_to_int(const String *str)
{
    int num = 0;

    for (size_t i = 0; i < str->count; ++i) {
        num = (num * 10) + static_cast<int> (str->data[i] - '0');
    }
    
    return(num);
}

internal multi_stack_t parse_stacks(String *slurped)
{
    std::vector<String> lines = {};

    while (slurped->count != 0) {
        String line = get_line(slurped);
        if (line.count == 0) break;

        lines.emplace_back(line);
    }

    size_t stacks_num = (lines[lines.size() - 1].count + 1) / 4;
    multi_stack_t stacks = {};

    for (size_t i = 0; i < stacks_num; ++i) {
        stacks.emplace_back(std::stack<char>());
    }
    
    for (int i = static_cast<int> (lines.size() - 2); i >= 0; --i) {
        const String &current = lines[i];

        for (size_t j = 1; j < current.count; j += 4) {
            if (current.data[j] == ' ') continue;
            
            stacks[(j - 1) / 4].push(current.data[j]);
        }
    }

    return(stacks);
}

internal instructions_t parse_instructions(String *slurped)
{
    instructions_t instructions = {};
    
    while (slurped->count != 0) {
        String line = get_line(slurped);
        std::vector<String> split_array = split_string_to_array(&line, ' ');
        
        Instruction inst = {
            string_to_int(&split_array[1]),     // num_elements
            string_to_int(&split_array[3]) - 1, // target
            string_to_int(&split_array[5]) - 1  // destination
        };

        instructions.emplace_back(inst);
    }
    
    return(instructions);
}

internal Stack_Inst get_data(const char *filename)
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

    multi_stack_t stacks = parse_stacks(&slurped);
    instructions_t instructions = parse_instructions(&slurped);

    Stack_Inst data = {
        stacks,
        instructions
    };
    
    free(buff);

    return(data);
}

internal void part_1(Stack_Inst data)
{
    for (const Instruction &inst : data.instructions) {
        for (int i = 0; i < inst.num_elements; ++i) {
            data.stacks[inst.destination].push(data.stacks[inst.target].top());
            data.stacks[inst.target].pop();
        }
    }

    for (const std::stack<char> &stack : data.stacks) {
        printf("%c", stack.top());
    }

    printf("\n");
}

// NOTE(Aiden): This can be simplified if, instead of storing the data in std::stack<T>,
// we just store it in regular arrays.
internal void part_2(Stack_Inst data)
{
    for (const Instruction &inst : data.instructions) {
        std::vector<char> boxes = {};

        for (int i = 0; i < inst.num_elements; ++i) {
            char box = data.stacks[inst.target].top();
            boxes.emplace_back(box);

            data.stacks[inst.target].pop();
        }

        for (int i = static_cast<int> (boxes.size() - 1); i >= 0; --i) {
            data.stacks[inst.destination].push(boxes[i]);
        }
    }
    
    for (const std::stack<char> &stack : data.stacks) {
        printf("%c", stack.top());
    }

    printf("\n");
}

int main()
{
    Stack_Inst data = get_data("input.txt");

    part_1(data);
    part_2(data);
    
    return 0;
}
