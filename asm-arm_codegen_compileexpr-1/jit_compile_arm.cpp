#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <stack>


const int PROGRAM_BUFFER_SIZE = 0x1000;
const int WORD_SIZE = 0x4;

typedef struct {
    const char *name;
    void *pointer;
} symbol_t;

extern "C" void jit_compile_expression_to_arm(const char *expression,
                                              const symbol_t *externs,
                                              void *out_buffer);

typedef uint32_t command_t;
typedef uint32_t addr_t;


typedef enum {
    CONSTANT,
    MATH_OPERATOR,
    FUNCTION,
    VARIABLE,
} exp_stack_type;

struct StackOperand {
    exp_stack_type type;
    int n_arguments = 0;

    StackOperand() = default;

    StackOperand(exp_stack_type type, int value) : type(type), value(value) {}

    StackOperand(exp_stack_type type, std::string name, uint8_t n_arguments = 2) : type(type), name(move(name)),
                                                                                   n_arguments(n_arguments) {}

    int value{};
    std::string name;
};

class ReversePolishNotation {
public:
    typedef enum {
        OPEN_BRACKET,
        CLOSE_BRACKET,
        OPERATION,
        NAME,
        FUNC,
        VAR,
        NUMBER,
        COMMA,
        UNDEFINED,
    } token_type_t;
private:

    struct Token {
        Token() = default;

        Token(token_type_t type) : type(type) {}

        friend std::ostream &operator<<(std::ostream &os, const Token &token) {
            switch (token.type) {
                case token_type_t::NAME:
                    os << token.name;
                    break;
                case OPERATION:
                    os << token.sym;
                    break;
                case COMMA:
                    os << ',';
                    break;
                case NUMBER:
                    os << token.value;
                    break;
                case OPEN_BRACKET:
                    os << '(';
                    break;
                case CLOSE_BRACKET:
                    os << ')';
                    break;
                case UNDEFINED:
                    os << "UNDEFINED";
                    break;
                case FUNC:
                    os << "FUNC " << token.name;
                    break;
                case VAR:
                    os << "VAR " << token.name;
                    break;
            }
            return os;
        };

        token_type_t type;
        std::string name;
        char sym{};
        int value{};
    };

    std::string expr_string;
    size_t expr_string_pointer = 0;
public:
    ReversePolishNotation() = default;

    std::vector<StackOperand> FromInfixString(const std::string &expression);

private:
    static void PushTokenToPoliz(std::vector<StackOperand> &poliz, const Token &token, int args_count = 0);

    bool ExpressionUnread() {
        return expr_string_pointer < expr_string.size();
    }

    void DecrementPointer() {
        --expr_string_pointer;
    }

    char ReadChar() {
        return expr_string[expr_string_pointer++];
    }

    std::string ReadName();

    int ReadNumber();

    Token ReadToken();

};

namespace arm_commands {
    /*
     * Соглашение следующее, аргументы функции лежат слева направо
     * в регистрах r0 - r3
     * Результат кладем в r0
     * Для математический операций r0 = r1 * r2
     * Так как умножение не может быть записано в виде r0 *= r1
     * Так что кладем слева направо в r1, r2
     * Все адреса загружаем в r4
     */

    inline command_t add() {
        return 0xe0810002; // add r0, r1, r2
    }

    inline command_t sub() {
        return 0xe0410002; // sub r0, r1, r2
    }

    inline command_t mul() {
        return 0xe0000291; // add r0, r1, r2
    }

    inline command_t push_r0() {
        return 0xe52d0004; // push {r0}
    }

    inline command_t push_r4_lr() {
        return 0xe92d4010; // push {r4, lr}
    }

    inline command_t pop_r4_lr() {
        return 0xe8bd4010; // pop {r4, lr}
    }

    inline command_t pop_r(uint32_t r_num) {
        return 0xe49d0004 + 0x1000 * r_num; // push {ri}
    }

    inline command_t mov_constant(int32_t value) {
        // mov r0 value
        if (value >= 0) {
            return 0xe3a00000 + value; // mov r0, value
        } else {
            return 0xe3e00000 - value - 1; // mvn r0, value - 1
        }
    }

    inline command_t load_constant(addr_t shift) {
        return 0xe59f0000 + shift - 2 * WORD_SIZE; // ldr r0, [pc, #shift-2]
    }

    inline command_t load_shifted_addr(addr_t shift) {
        return 0xe59f4000 + shift - 2 * WORD_SIZE; // ldr r4, [pc, #shift-2]
    }

    inline command_t load_r4() {
        return 0xe5940000; // ldr r0, [r4]
    }

    inline command_t goto_r4() {
//        return 0xebfffffe; // bl r4
        return 0xe1a0f004; // mov	pc, r4
    }

    inline command_t save_pc() {
        return 0xe1a0e00f; // mov lr, pc
    }

    inline command_t increment_lr() {
        return 0xe28ee004; // add	lr, lr, #4
    }

    inline command_t exit() {
        return 0xe12fff1e; // bx lr
    }
}

class ExpressionFuncGenerator {

    std::vector<StackOperand> expression;
    std::map<std::string, addr_t> symbols_ptr;
    std::vector<command_t> program_commands;
    // The addresses of functions and variables will lie at the end of the buffer under the program in reverse order
    // data_section[0] - at the very end of the buffer
    std::vector<command_t> data_section;
    size_t program_buffer_size;
public:
    explicit ExpressionFuncGenerator(std::vector<StackOperand> expression) : expression(move(expression)) {}

    void SetSymbols(const symbol_t *externs, uint32_t buffer_size) {
        program_buffer_size = buffer_size;
        for (size_t i = 0; externs[i].name; ++i) {
            // In the development environment it writes a mistake because void* occupies 64 bits, but in arm 32
            addr_t ptr = reinterpret_cast<addr_t>(externs[i].pointer);
            symbols_ptr[externs[i].name] = GetNextDataWordShift();
            data_section.push_back(ptr);

        }
    }

    void GenerateMachineCommands() {
        PushCommand(arm_commands::push_r4_lr());
        for (StackOperand &operand: expression) {
            ProcessStackOperand(operand);
        }
        PushCommand(arm_commands::pop_r(0));
        PushCommand(arm_commands::pop_r4_lr());
        PushCommand(arm_commands::exit());
    }

    void WriteCommandsByPtr(void *out_buffer);

private:

    void PushCommand(command_t cmd) {
        program_commands.emplace_back(cmd);
    }

    void ProcessStackOperand(const StackOperand &operand);

    addr_t GetNextDataWordShift() const {
        return program_buffer_size - WORD_SIZE * (data_section.size() + 1);
    }
};


void jit_compile_expression_to_arm(const char *expression,
                                   const symbol_t *externs,
                                   void *out_buffer) {
    auto parser = ReversePolishNotation();
    auto polishStack = parser.FromInfixString(expression);

    ExpressionFuncGenerator func_generator(polishStack);
    func_generator.SetSymbols(externs, PROGRAM_BUFFER_SIZE);
    func_generator.GenerateMachineCommands();

    func_generator.WriteCommandsByPtr(out_buffer);
}


void ExpressionFuncGenerator::ProcessStackOperand(const StackOperand &operand) {
    addr_t addr_shift;
    switch (operand.type) {
        case CONSTANT:
            addr_shift = GetNextDataWordShift();
            data_section.push_back(static_cast<command_t>(operand.value));
            PushCommand(arm_commands::load_constant(addr_shift - program_commands.size() * WORD_SIZE));
            break;
        case VARIABLE:
            addr_shift = symbols_ptr[operand.name];
            PushCommand(arm_commands::load_shifted_addr(addr_shift - program_commands.size() * WORD_SIZE));
            PushCommand(arm_commands::load_r4());
            break;
        case FUNCTION:
            // Function accepts <= 4 arguments
            // Put them in r0, r1, r2, r3 respectively
            // In the stack they lie in reverse order, i.e. at the top of the last argument
            for (int i = operand.n_arguments - 1; i >= 0; --i) {
                PushCommand(arm_commands::pop_r(i));
            }

            addr_shift = symbols_ptr[operand.name];
            PushCommand(arm_commands::load_shifted_addr(addr_shift - program_commands.size() * WORD_SIZE));
            PushCommand(arm_commands::save_pc());
            PushCommand(arm_commands::increment_lr());
            PushCommand(arm_commands::goto_r4());
            break;
        case MATH_OPERATOR:
            PushCommand(arm_commands::pop_r(2));
            PushCommand(arm_commands::pop_r(1));
            switch (operand.name[0]) {
                case '*':
                    PushCommand(arm_commands::mul());
                    break;
                case '+':
                    PushCommand(arm_commands::add());
                    break;
                case '-':
                    PushCommand(arm_commands::sub());
                    break;
            }
            break;
        default:
            break;
    }
    PushCommand(arm_commands::push_r0());
}

void ExpressionFuncGenerator::WriteCommandsByPtr(void *out_buffer) {
    // Write commands from the beginning of the buffer
    auto *buffer_begin = static_cast<command_t *>(out_buffer);
    addr_t *ptr = buffer_begin;
    for (const command_t cmd: program_commands) {
        *ptr = cmd;
        ++ptr;
    }
    // Write addresses of variables and functions at the end of the buffer
    // Hope there is enough room, otherwise everything will break :)
    addr_t *buffer_back_ptr = buffer_begin + program_buffer_size / WORD_SIZE - 1;
    for (const addr_t addr: data_section) {
        *buffer_back_ptr = addr;
        --buffer_back_ptr;
    }
}

std::vector<StackOperand> ReversePolishNotation::FromInfixString(const std::string &expression) {
    // Uses the "sorting station" algorithm
    expr_string = expression + " ";

    std::stack<Token> sorting_stack;

    // Put a plug at the beginning of the stack to avoid checks for !stack.empty().
    // You can always refer to stack.top() instead.
    // A sign of the end is stack.top().type == UNDEFINED
    sorting_stack.push(Token(UNDEFINED));

    std::vector<StackOperand> poliz;
    Token last_read_token(UNDEFINED);
    bool unary_minus = false;
    // Variable for storing the number of arguments of the function
    int args_count = 0;

    while (ExpressionUnread()) {
        Token token = ReadToken();
        if (last_read_token.type == NAME) {
            if (token.type == OPEN_BRACKET) {
                last_read_token.type = FUNC;
                sorting_stack.push(last_read_token);
            } else {
                last_read_token.type = VAR;
                PushTokenToPoliz(poliz, last_read_token);
            }
        }

        switch (token.type) {
            case NUMBER:
                if (unary_minus) {
                    // If there was a minus before the number. In this problem, the unary minus can be only before the constant
                    token.value *= -1;
                    unary_minus = false;
                }
                PushTokenToPoliz(poliz, token);
                break;
            case COMMA:
                while (sorting_stack.top().type != OPEN_BRACKET) {
                    PushTokenToPoliz(poliz, sorting_stack.top());
                    sorting_stack.pop();
                }
                ++sorting_stack.top().value;
                break;
            case OPEN_BRACKET:
                token.value = 1;
                sorting_stack.push(token);
                break;
            case CLOSE_BRACKET:
                while (sorting_stack.top().type != OPEN_BRACKET) {
                    PushTokenToPoliz(poliz, sorting_stack.top());
                    sorting_stack.pop();
                }
                args_count = sorting_stack.top().value;
                sorting_stack.pop();
                if (sorting_stack.top().type == FUNC) {
                    PushTokenToPoliz(poliz, sorting_stack.top(), args_count);
                    sorting_stack.pop();
                }
                break;
            case OPERATION:
                if (token.sym == '-' && (last_read_token.type == OPEN_BRACKET || last_read_token.type == COMMA ||
                                         last_read_token.type == UNDEFINED || last_read_token.type == OPERATION)) {
                    unary_minus = true;
                } else {
                    // Unary minus test. It will be used when processing a number (NUMBER) -> num *= -1.
                    while (sorting_stack.top().type == OPERATION &&
                           (token.sym != '*' || sorting_stack.top().sym == '*')) {
                        PushTokenToPoliz(poliz, sorting_stack.top());
                        sorting_stack.pop();
                    }
                    sorting_stack.push(token);
                }
                break;
            case NAME:
                // Here we will use last_read_token on the next iteration of the loop
                break;
            default:
                break;
        }
        last_read_token = token;
    }
    while (sorting_stack.top().type != UNDEFINED) {
        PushTokenToPoliz(poliz, sorting_stack.top());
        sorting_stack.pop();
    }

    return poliz;
}

ReversePolishNotation::Token ReversePolishNotation::ReadToken() {
    Token token;

    if (!ExpressionUnread()) {
        token.type = UNDEFINED;
        return token;
    }

    char c = ReadChar();
    if (c == '+' || c == '-' || c == '*') {
        token.type = OPERATION;
        token.sym = c;
    } else if (c == '(') {
        token.type = OPEN_BRACKET;
    } else if (c == ')') {
        token.type = CLOSE_BRACKET;
    } else if (c == ',') {
        token.type = COMMA;
    } else if (isdigit(c)) {
        DecrementPointer();
        token.type = NUMBER;
        token.value = ReadNumber();
    } else if (isalpha(c)) {
        DecrementPointer();
        token.type = NAME;
        token.name = ReadName();
    } else {
        // Whitespace or other invalid character, skip
        token = ReadToken();
    }
    return token;
}

std::string ReversePolishNotation::ReadName() {
    char c;
    std::string name;
    while (ExpressionUnread()) {
        c = ReadChar();
        if (std::isalpha(c) || std::isdigit(c)) {
            name += c;
        } else {
            DecrementPointer();
            break;
        }
    }
    return name;
}

int ReversePolishNotation::ReadNumber() {
    int value = 0;
    char c;
    while (ExpressionUnread()) {
        c = ReadChar();
        if (isdigit(c)) {
            value *= 10;
            value += c - '0';
        } else {
            DecrementPointer();
            break;
        }
    }
    return value;
}

void
ReversePolishNotation::PushTokenToPoliz(std::vector<StackOperand> &poliz, const ReversePolishNotation::Token &token,
                                        int args_count) {
    switch (token.type) {
        case NUMBER:
            poliz.emplace_back(CONSTANT, token.value);
            break;
        case FUNC:
            poliz.emplace_back(FUNCTION, token.name, args_count);
            break;
        case VAR:
            poliz.emplace_back(VARIABLE, token.name);
            break;
        case OPERATION:
            poliz.emplace_back(MATH_OPERATOR, std::string(1, token.sym));
            break;
        default:
            break;
    }
}
