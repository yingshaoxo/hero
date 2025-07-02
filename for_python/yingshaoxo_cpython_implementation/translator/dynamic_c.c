// idea creator: yingshaoxo
// base version: github copilot sonet3.5
// upgraded by: google gemini2.5


// This version is only working for gcc, for musl-gcc, it will hit unknown error for unknown reason


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_CMD_SIZE 4096

// Forward declarations of structs and enums
// Note: For structs defined as 'typedef struct { ... } Name;', the separate 'typedef struct Name Name;' forward declaration is redundant.
// For structs defined as 'struct Name { ... };' followed by 'typedef struct Name Name;', the forward declaration is necessary if used before definition.
// I've adjusted to the common pattern where 'typedef struct { ... } Name;' implies the forward declaration.

typedef enum {
    C_NONE,
    C_INT,
    C_LONGLONG,
    C_FLOAT,
    C_CHAR,
    C_STRING,
    C_POINTER,
    C_ARRAY,
    C_STRUCT,
    C_FUNCTION,
    C_UNION, // Added C_UNION
    C_ERROR
} CElementType;

// Forward declarations for structs that are part of recursive definitions or used before their full definition
typedef struct CElement CElement;
typedef struct SymbolTable SymbolTable;
typedef struct Lexer Lexer;
typedef struct Token Token;
typedef struct FlowResult FlowResult;
typedef struct ParserError ParserError;
typedef struct MacroDef MacroDef;

// For preprocessor directives
typedef enum {
    PP_IF,
    PP_IFDEF,
    PP_IFNDEF,
    PP_ELSE,
    PP_ELIF,
    PP_ENDIF
} PPDirectiveType;

// PPCondition struct definition
typedef struct PPCondition {
    PPDirectiveType type;
    bool is_true;
    bool condition_met_in_block; // For handling #elif and #else
    struct PPCondition* parent;
} PPCondition;

// PreprocessorState struct definition
typedef struct PreprocessorState {
    MacroDef* macros;
    PPCondition* current_condition;
} PreprocessorState;

// Define FunctionDef, StructDef, StructField, StructInstance, UnionDef, UnionField
typedef struct FunctionDef {
    char* name;
    char** params; // Array of parameter names
    int param_count;
    char* body; // Code block of the function
    SymbolTable* scope; // Scope where function was defined
} FunctionDef;

typedef struct StructField {
    char* name;
    CElementType type;
    struct StructField* next;
} StructField;

typedef struct StructDef {
    char* name;
    StructField* fields;
    struct StructDef* next;
} StructDef;

typedef struct UnionField {
    char* name;
    CElementType type;
    struct UnionField* next;
} UnionField;

typedef struct UnionDef {
    char* name;
    UnionField* fields;
    struct UnionDef* next;
} UnionDef;


typedef struct StructInstance {
    StructDef* def;
    CElement** fields; // Array of CElement* for field values
} StructInstance;

typedef struct UnionInstance {
    UnionDef* def;
    CElement* active_member; // Pointer to the currently active member
    char* active_member_name; // Name of the active member
} UnionInstance;


// ExprNodeType enum MUST be defined before ExprNode struct
typedef enum {
    EXPR_LITERAL,
    EXPR_VARIABLE,
    EXPR_UNARY_OP,
    EXPR_BINARY_OP,
    EXPR_CALL,
    EXPR_ASSIGNMENT, // For handling assignments
    EXPR_IF,         // For if statements
    EXPR_WHILE,      // For while loops
    EXPR_FOR_LOOP,   // For for loops
    EXPR_RETURN,     // For return statements
    EXPR_BREAK,      // For break statements
    EXPR_CONTINUE,   // For continue statements
    EXPR_BLOCK,      // For code blocks
    EXPR_FUNCTION_DEF, // For function definitions
    EXPR_STRUCT_DEF,   // For struct definitions
    EXPR_MEMBER_ACCESS, // For struct/union member access
    EXPR_UNION_DEF,
    EXPR_INDEX_ACCESS, // For array/pointer indexing: arr[index]
    EXPR_CAST          // For type casting: (type)expr
} ExprNodeType;

// ExprNode and its related unions/structs
typedef struct ExprNode ExprNode; // Forward declaration for self-referencing in unions

typedef struct {
    char op; // Operator character
    ExprNode* operand; // Added missing operand
} UnaryOp;

typedef struct {
    char op; // Operator character
    ExprNode* left;
    ExprNode* right;
} BinaryOp;

typedef struct {
    char* func_name;
    ExprNode** args; // Array of expressions for arguments
    int arg_count;
} CallExpr;

typedef struct {
    char* var_name;
    ExprNode* value;
} AssignmentExpr;

typedef struct {
    ExprNode* condition;
    char* if_body;
    char* else_body; // Optional else body
} IfStatement;

typedef struct {
    ExprNode* condition;
    char* body;
} WhileLoop;

typedef struct {
    char* init_stmt_str;    // String for initialization (e.g., "int i = 0;")
    ExprNode* condition_expr; // Expression for loop condition (e.g., "i < 100")
    char* increment_stmt_str; // String for increment (e.g., "i++")
    char* body_code;        // String for loop body (e.g., "{ print(i); }")
} ForLoop;


typedef struct {
    ExprNode* value;
} ReturnStatement;

typedef struct {
    ExprNode** statements;
    int count;
} Block;

typedef struct {
    ExprNode* primary; // The array/pointer being indexed (e.g., 'arr' or 'ptr')
    ExprNode* index;   // The index expression (e.g., '0' or 'i+1')
} IndexAccess;

typedef struct {
    CElementType target_type; // The type to cast to
    ExprNode* operand;        // The expression to cast
} CastExpr;


// Full ExprNode definition
struct ExprNode { // Removed redundant typedef struct ExprNode ExprNode;
    ExprNodeType type;
    union {
        CElement* literal;
        char* var_name;
        UnaryOp unary_op;
        BinaryOp binary_op;
        CallExpr call;
        AssignmentExpr assignment;
        IfStatement if_stmt;
        WhileLoop while_loop;
        ForLoop for_loop; // New for for loops
        ReturnStatement ret_stmt;
        Block block;
        FunctionDef* func_def;
        StructDef* struct_def;
        UnionDef* union_def;
        struct {
            ExprNode* primary;
            char* member_name;
        } member_access;
        IndexAccess index_access; // New for []
        CastExpr cast_expr;       // New for (type)
    } data;
};


typedef enum {
    FLOW_NORMAL,
    FLOW_RETURN,
    FLOW_BREAK,
    FLOW_CONTINUE,
    FLOW_ERROR
} FlowResultType;

struct FlowResult {
    FlowResultType type;
    CElement* value; // For return values
};

struct ParserError {
    char* message;
    int line;
    int column;
};

struct MacroDef {
    char* name;
    char* value;
    MacroDef* next;
};


// C Element structure for parsing and execution
struct CElement {
    CElementType type;
    char* name;
    union {
        int int_val;
        long long longlong_val;
        float float_val;
        char char_val;
        char* string_val;
        void* ptr_val; // For C_POINTER, points to another CElement* or raw memory
        CElement** array_val;
        FunctionDef* func_val; // Added func_val
        StructInstance* struct_val; // Added struct_val
        UnionInstance* union_val; // Added union_val
    } value;
    CElement* next;
};

// Symbol table structure to track variables and functions
struct SymbolTable {
    CElement* symbols;
    SymbolTable* parent;
};

// Token types for parsing
typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_OPERATOR,
    TOKEN_PUNCTUATOR,
    TOKEN_KEYWORD,
    TOKEN_EOF,
    TOKEN_LONGLONG, // Added TOKEN_LONGLONG
    TOKEN_TRUE,     // Added TOKEN_TRUE for boolean literal 'true'
    TOKEN_FALSE,    // Added TOKEN_FALSE for boolean literal 'false'
    TOKEN_ERROR     // Added TOKEN_ERROR
} TokenType;

struct Token {
    TokenType type;
    char* value;
    int line;
    int column;
};

// Lexer state
struct Lexer {
    const char* input;
    size_t position;
    int line;
    int column;
};

// Global symbol table
CElement* global_symbols = NULL;


// Function Prototypes (Forward Declarations)
Lexer* create_lexer(const char* input);
Token* get_next_token(Lexer* lexer);
Token* peek_token(Lexer* lexer);
void advance_lexer(Lexer* lexer);

char* parse_c_code(const char* code);
CElement* evaluate_expression(const char* expr); // This function seems unused in the main flow
void execute_statement(const char* stmt); // This function seems unused in the main flow

CElement* create_int(int val);
CElement* create_longlong(long long val);
CElement* create_float(float val);
CElement* create_char(char val);
CElement* create_string(const char* val);
CElement* create_pointer(void* ptr); // Changed to void* to point to any data
CElement* create_array(CElement** arr, int size);
CElement* create_struct_instance(StructDef* def);
CElement* create_function(const char* name, char** params, int param_count, const char* body, SymbolTable* scope);
CElement* create_union_instance(UnionDef* def);
CElement* create_error(const char* message);
CElement* create_none();

// New helper function to create a deep copy of a CElement (for return values from symbol table)
CElement* copy_celement(CElement* original);

SymbolTable* create_scope(SymbolTable* parent);
void bind_symbol(SymbolTable* scope, const char* name, CElement* value);
CElement* lookup_symbol(SymbolTable* scope, const char* name);
void free_scope(SymbolTable* scope);
void free_element(CElement* elem);
void free_struct_instance(CElement* instance);
void free_union_instance(CElement* instance);

char* safe_strdup(const char* str);
void* safe_malloc(size_t size);

ExprNode* create_literal_expr(CElement* literal);
ExprNode* create_variable_expr(const char* name);
ExprNode* create_unary_op_expr(char op, ExprNode* operand);
ExprNode* create_binary_op_expr(char op, ExprNode* left, ExprNode* right);
ExprNode* create_call_expr(const char* func_name, ExprNode** args, int arg_count);
ExprNode* create_assignment_expr(const char* var_name, ExprNode* value);
ExprNode* create_if_expr(ExprNode* condition, const char* if_body, const char* else_body);
ExprNode* create_while_expr(ExprNode* condition, const char* body);
ExprNode* create_for_loop_expr(const char* init_stmt_str, ExprNode* condition_expr, const char* increment_stmt_str, const char* body_code); // New
ExprNode* create_return_expr(ExprNode* value);
ExprNode* create_break_expr(); // New
ExprNode* create_continue_expr(); // New
ExprNode* create_block_expr(ExprNode** statements, int count);
ExprNode* create_function_def_expr(FunctionDef* func_def);
ExprNode* create_struct_def_expr(StructDef* struct_def);
ExprNode* create_union_def_expr(UnionDef* union_def);
ExprNode* create_member_access_expr(ExprNode* primary, const char* member_name);
ExprNode* create_index_access_expr(ExprNode* primary, ExprNode* index); // New for []
ExprNode* create_cast_expr(CElementType target_type, ExprNode* operand); // New for (type)

CElement* evaluate_expr(ExprNode* node, SymbolTable* scope);
FlowResult* execute_block(const char* code, SymbolTable* scope, bool in_loop); // Added in_loop parameter
FlowResult* execute_if_statement(ExprNode* node, SymbolTable* scope, bool in_loop); // Added in_loop parameter
FlowResult* execute_while_loop(ExprNode* node, SymbolTable* scope);
FlowResult* execute_for_loop(ExprNode* node, SymbolTable* scope); // New
CElement* assign_variable(ExprNode* node, SymbolTable* scope);
CElement* execute_function_call(ExprNode* node, SymbolTable* scope);
CElement* parse_function_def(Lexer* lexer, SymbolTable* scope, CElementType return_type); // Added return_type
StructDef* parse_struct_def(Lexer* lexer);
UnionDef* parse_union_def(Lexer* lexer);

ExprNode* parse_expression(Lexer* lexer);
char* parse_block(Lexer* lexer);
char* parse_statement_or_declaration(Lexer* lexer); // Helper for for loop init/increment
CElementType parse_type(Lexer* lexer); // New: parse a C type (int, char*, long long, etc.)


ParserError* create_parser_error(const char* message, int line, int column);
void print_parser_error(ParserError* error);

void synchronize(Lexer* lexer);

void define_macro(PreprocessorState* state, const char* name, const char* value);
char* expand_macros(PreprocessorState* state, const char* code);
void handle_preprocessor_directive(PreprocessorState* state, const char* line);
PreprocessorState* create_preprocessor_state();

// Built-in function type definition
typedef CElement* (*BuiltinFuncPtr)(CElement** args, int arg_count);

typedef struct {
    const char* name;
    int min_args; // Minimum number of arguments
    int max_args; // Maximum number of arguments (-1 for variable)
    BuiltinFuncPtr func_ptr;
} BuiltinFuncInfo;

// Built-in function implementations
CElement* builtin_print(CElement** args, int arg_count);
CElement* builtin_print_int(CElement** args, int arg_count);
CElement* builtin_print_float(CElement** args, int arg_count);
CElement* builtin_print_string(CElement** args, int arg_count);
CElement* builtin_malloc(CElement** args, int arg_count);
CElement* builtin_free(CElement** args, int arg_count);
CElement* builtin_len(CElement** args, int arg_count);
CElement* builtin_string_add(CElement** args, int arg_count);
CElement* builtin_type(CElement** args, int arg_count);


// Centralized list of built-in functions
static BuiltinFuncInfo builtin_functions[] = {
    {"print", 1, -1, builtin_print}, // Variable arguments
    {"print_int", 1, 1, builtin_print_int},
    {"print_float", 1, 1, builtin_print_float},
    {"print_string", 1, 1, builtin_print_string},
    {"malloc", 1, 1, builtin_malloc},
    {"free", 1, 1, builtin_free},
    {"len", 1, 1, builtin_len},
    {"string_add", 2, 2, builtin_string_add},
    {"type", 1, 1, builtin_type}
};
static const int NUM_BUILTIN_FUNCS = sizeof(builtin_functions) / sizeof(BuiltinFuncInfo);


// Function to handle built-in calls (now uses the centralized list)
CElement* handle_builtin_call(const char* func_name, CElement** arg_values, int arg_count) {
    for (int i = 0; i < NUM_BUILTIN_FUNCS; i++) {
        if (strcmp(func_name, builtin_functions[i].name) == 0) {
            if (arg_count < builtin_functions[i].min_args || (builtin_functions[i].max_args != -1 && arg_count > builtin_functions[i].max_args)) {
                char error_msg[256];
                snprintf(error_msg, sizeof(error_msg), "Built-in function '%s' expects %d to %d arguments, but got %d.",
                         func_name, builtin_functions[i].min_args, builtin_functions[i].max_args, arg_count);
                return create_error(error_msg);
            }
            return builtin_functions[i].func_ptr(arg_values, arg_count);
        }
    }
    return NULL; // Not a recognized built-in function
}

// Built-in function implementations
CElement* builtin_print(CElement** args, int arg_count) {
    for (int i = 0; i < arg_count; i++) {
        CElement* arg = args[i];
        if (arg->type == C_ERROR) { // Handle error arguments gracefully
            fprintf(stderr, "Error in print argument: %s", arg->value.string_val);
            continue; // Skip to next argument or return an error for print itself
        }
        switch (arg->type) {
            case C_INT: printf("%d", arg->value.int_val); break;
            case C_LONGLONG: printf("%lld", arg->value.longlong_val); break;
            case C_FLOAT: printf("%f", arg->value.float_val); break;
            case C_CHAR: printf("%c", arg->value.char_val); break;
            case C_STRING: printf("%s", arg->value.string_val ? arg->value.string_val : "(null)"); break;
            case C_POINTER: printf("PTR:%p", arg->value.ptr_val); break; // Print pointer address
            case C_NONE: printf("NULL"); break; // Or "void"
            default: printf("[unprintable type]"); break;
        }
        if (i < arg_count - 1) {
            printf(" "); // Space between arguments
        }
    }
    printf("\n"); // Newline after print
    fflush(stdout);
    return create_none();
}

CElement* builtin_print_int(CElement** args, int arg_count) {
    if (args[0]->type == C_INT) {
        printf("%d\n", args[0]->value.int_val);
        return create_none();
    }
    return create_error("print_int expects one integer argument");
}

CElement* builtin_print_float(CElement** args, int arg_count) {
    if (args[0]->type == C_FLOAT) {
        printf("%f\n", args[0]->value.float_val);
        return create_none();
    }
    return create_error("print_float expects one float argument");
}

CElement* builtin_print_string(CElement** args, int arg_count) {
    if (args[0]->type == C_STRING) {
        printf("%s\n", args[0]->value.string_val);
        return create_none();
    }
    return create_error("print_string expects one string argument");
}

CElement* builtin_malloc(CElement** args, int arg_count) {
    if (args[0]->type != C_INT) {
        return create_error("malloc expects an integer size argument");
    }
    size_t size = (size_t)args[0]->value.int_val;
    void* ptr = malloc(size);
    if (!ptr) {
        return create_error("malloc failed to allocate memory");
    }
    return create_pointer(ptr);
}

CElement* builtin_free(CElement** args, int arg_count) {
    if (args[0]->type != C_POINTER) {
        return create_error("free expects a pointer argument");
    }
    free(args[0]->value.ptr_val);
    return create_none();
}

CElement* builtin_len(CElement** args, int arg_count) {
    if (args[0]->type == C_STRING) {
        return create_int(strlen(args[0]->value.string_val));
    }
    // Add more types for len if needed (e.g., arrays)
    return create_error("len expects a string argument");
}

CElement* builtin_string_add(CElement** args, int arg_count) {
    if (args[0]->type != C_STRING || args[1]->type != C_STRING) {
        return create_error("string_add expects two string arguments");
    }
    const char* str1 = args[0]->value.string_val;
    const char* str2 = args[1]->value.string_val;
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char* new_str = safe_malloc(len1 + len2 + 1);
    strcpy(new_str, str1);
    strcat(new_str, str2);
    CElement* result = create_string(new_str);
    free(new_str); // create_string makes a copy, so free this temp buffer
    return result;
}

CElement* builtin_type(CElement** args, int arg_count) {
    if (!args[0]) return create_error("type() received a NULL argument");
    switch (args[0]->type) {
        case C_INT: return create_string("INT");
        case C_LONGLONG: return create_string("LONGLONG");
        case C_FLOAT: return create_string("FLOAT");
        case C_CHAR: return create_string("CHAR");
        case C_STRING: return create_string("STRING");
        case C_POINTER: return create_string("POINTER");
        case C_ARRAY: return create_string("ARRAY");
        case C_STRUCT: return create_string("STRUCT");
        case C_FUNCTION: return create_string("FUNCTION");
        case C_UNION: return create_string("UNION");
        case C_NONE: return create_string("NONE");
        case C_ERROR: return create_string("ERROR");
        default: return create_string("UNKNOWN_TYPE");
    }
}


// Create new lexer
Lexer* create_lexer(const char* input) {
    Lexer* lexer = safe_malloc(sizeof(Lexer));
    lexer->input = input;
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    return lexer;
}

// Get next token from input
Token* get_next_token(Lexer* lexer) {
    Token* token = safe_malloc(sizeof(Token));

    // Skip whitespace
    while (lexer->input[lexer->position] == ' ' ||
           lexer->input[lexer->position] == '\t' ||
           lexer->input[lexer->position] == '\n') {
        if (lexer->input[lexer->position] == '\n') {
            lexer->line++;
            lexer->column = 1;
        } else {
            lexer->column++;
        }
        lexer->position++;
    }

    // Check for EOF
    if (lexer->input[lexer->position] == '\0') {
        token->type = TOKEN_EOF;
        token->value = safe_strdup(""); // Empty string for EOF
        return token;
    }

    // Identify token type
    char current = lexer->input[lexer->position];
    // Parse identifiers
    if (isalpha(current) || current == '_') {
        int start = lexer->position;
        while (isalnum(lexer->input[lexer->position]) ||
               lexer->input[lexer->position] == '_') {
            lexer->position++;
            lexer->column++;
        }
        int length = lexer->position - start;
        token->value = safe_malloc(length + 1);
        strncpy(token->value, &lexer->input[start], length);
        token->value[length] = '\0';
        token->type = TOKEN_IDENTIFIER;

        // Check if it's a keyword or boolean literal
        if (strcmp(token->value, "true") == 0) {
            token->type = TOKEN_TRUE;
        } else if (strcmp(token->value, "false") == 0) {
            token->type = TOKEN_FALSE;
        } else {
            const char* keywords[] = {"int", "char", "float", "void", "return", "if", "else", "while", "for", "function", "struct", "union", "long", "bool", "break", "continue"}; // Added "break", "continue"
            for (int i = 0; i < sizeof(keywords)/sizeof(char*); i++) {
                if (strcmp(token->value, keywords[i]) == 0) {
                    token->type = TOKEN_KEYWORD;
                    break;
                }
            }
        }
    }
    // Parse numbers
    else if (isdigit(current) || (current == '-' && isdigit(lexer->input[lexer->position + 1])) || (current == '+' && isdigit(lexer->input[lexer->position + 1]))) {
        bool is_long = false;
        bool is_float = false;
        int start = lexer->position;

        // Handle sign
        if (current == '-' || current == '+') {
            lexer->position++;
            lexer->column++;
        }

        // Parse digits before decimal
        while (isdigit(lexer->input[lexer->position])) {
            lexer->position++;
            lexer->column++;
        }

        // Check for decimal point
        if (lexer->input[lexer->position] == '.') {
            is_float = true;
            lexer->position++;
            lexer->column++;
            // Parse digits after decimal
            while (isdigit(lexer->input[lexer->position])) {
                lexer->position++;
                lexer->column++;
            }
        }

        // Check for LL or ll suffix for long long
        if (lexer->input[lexer->position] == 'l' || lexer->input[lexer->position] == 'L') {
            if (lexer->input[lexer->position + 1] == 'l' || lexer->input[lexer->position + 1] == 'L') {
                is_long = true;
                lexer->position += 2;
                lexer->column += 2;
            }
        }

        int length = lexer->position - start;
        token->value = safe_malloc(length + 1);
        strncpy(token->value, &lexer->input[start], length);
        token->value[length] = '\0';
        token->type = TOKEN_NUMBER;

        // Store info about number type
        if (is_long) {
            token->type = TOKEN_LONGLONG; // Corrected to type
        } else if (is_float) {
            // No specific TOKEN_FLOAT, TOKEN_NUMBER is fine, type can be inferred from CElement
        } else {
            // It's an integer
        }
    }

    // Parse strings
    else if (current == '"') {
        lexer->position++; // Consume '"'
        lexer->column++;
        int start = lexer->position;
        while (lexer->input[lexer->position] != '"' && lexer->input[lexer->position] != '\0') {
            lexer->position++;
            lexer->column++;
        }
        int length = lexer->position - start;
        token->value = safe_malloc(length + 1);
        strncpy(token->value, &lexer->input[start], length);
        token->value[length] = '\0';
        lexer->position++; // Consume closing '"'
        lexer->column++;
        token->type = TOKEN_STRING;
    }
    // Parse operators
    else if (strchr("+-*/%=&|!<>", current) != NULL) {
        int start = lexer->position;
        lexer->position++;
        lexer->column++;
        // Handle multi-character operators like ==, !=, <=, >=, &&, ||, +=, -=, *=, /=, %=
        char next = lexer->input[lexer->position];
        if ((current == '=' && next == '=') ||
            (current == '!' && next == '=') ||
            (current == '<' && next == '=') ||
            (current == '>' && next == '=') ||
            (current == '&' && next == '&') ||
            (current == '|' && next == '|') ||
            (current == '+' && next == '=') ||
            (current == '-' && next == '=') ||
            (current == '*' && next == '=') ||
            (current == '/' && next == '=') ||
            (current == '%' && next == '='))
        {
            lexer->position++;
            lexer->column++;
        }
        int length = lexer->position - start;
        token->value = safe_malloc(length + 1);
        strncpy(token->value, &lexer->input[start], length);
        token->value[length] = '\0';
        token->type = TOKEN_OPERATOR;
    }
    // Parse punctuators
    else if (strchr("(){}[];,.", current) != NULL) {
        token->value = safe_malloc(2);
        token->value[0] = current;
        token->value[1] = '\0';
        token->type = TOKEN_PUNCTUATOR;
        lexer->position++;
        lexer->column++;
    }
    else {
        // Unknown character, error token
        token->value = safe_malloc(2);
        token->value[0] = current;
        token->value[1] = '\0';
        token->type = TOKEN_ERROR; // Corrected to TOKEN_ERROR
        lexer->position++;
        lexer->column++;
    }
    token->line = lexer->line;
    token->column = lexer->column - strlen(token->value); // Adjust column to start of token
    return token;
}

Token* peek_token(Lexer* lexer) {
    size_t original_position = lexer->position;
    int original_line = lexer->line;
    int original_column = lexer->column;

    Token* token = get_next_token(lexer);

    lexer->position = original_position;
    lexer->line = original_line;
    lexer->column = original_column;
    return token;
}

void advance_lexer(Lexer* lexer) {
    // Free the token returned by get_next_token as it's consumed
    Token* consumed_token = get_next_token(lexer);
    if (consumed_token) {
        if (consumed_token->value) free(consumed_token->value);
        free(consumed_token);
    }
}


CElement* create_int(int val) {
    CElement* elem = safe_malloc(sizeof(CElement));
    elem->type = C_INT;
    elem->value.int_val = val;
    elem->name = NULL;
    elem->next = NULL;
    return elem;
}

CElement* create_longlong(long long val) {
    CElement* elem = safe_malloc(sizeof(CElement));
    elem->type = C_LONGLONG;
    elem->value.longlong_val = val;
    elem->name = NULL;
    elem->next = NULL;
    return elem;
}

CElement* create_float(float val) {
    CElement* elem = safe_malloc(sizeof(CElement));
    elem->type = C_FLOAT;
    elem->value.float_val = val;
    elem->name = NULL;
    elem->next = NULL;
    return elem;
}

CElement* create_char(char val) {
    CElement* elem = safe_malloc(sizeof(CElement));
    elem->type = C_CHAR;
    elem->value.char_val = val;
    elem->name = NULL;
    elem->next = NULL;
    return elem;
}

CElement* create_string(const char* val) {
    CElement* elem = safe_malloc(sizeof(CElement));
    elem->type = C_STRING;
    elem->value.string_val = safe_strdup(val);
    elem->name = NULL;
    elem->next = NULL;
    return elem;
}

CElement* create_pointer(void* ptr) {
    CElement* elem = safe_malloc(sizeof(CElement));
    elem->type = C_POINTER;
    elem->value.ptr_val = ptr;
    elem->name = NULL;
    elem->next = NULL;
    return elem;
}

CElement* create_array(CElement** arr, int size) {
    CElement* elem = safe_malloc(sizeof(CElement));
    elem->type = C_ARRAY;
    elem->value.array_val = arr;
    elem->name = NULL;
    elem->next = NULL;
    return elem;
}

CElement* create_struct_instance(StructDef* def) {
    CElement* instance = safe_malloc(sizeof(CElement));
    instance->type = C_STRUCT;
    instance->value.struct_val = safe_malloc(sizeof(StructInstance));
    instance->value.struct_val->def = def;
    // Count fields to allocate array
    int field_count = 0;
    StructField* current_field = def->fields;
    while(current_field) {
        field_count++;
        current_field = current_field->next;
    }
    instance->value.struct_val->fields = safe_malloc(sizeof(CElement*) * field_count);
    for (int i = 0; i < field_count; i++) {
        instance->value.struct_val->fields[i] = NULL; // Initialize to NULL
    }
    instance->name = NULL;
    instance->next = NULL;
    return instance;
}

CElement* create_union_instance(UnionDef* def) {
    CElement* instance = safe_malloc(sizeof(CElement));
    instance->type = C_UNION;
    instance->value.union_val = safe_malloc(sizeof(UnionInstance));
    instance->value.union_val->def = def;
    instance->value.union_val->active_member = NULL;
    instance->value.union_val->active_member_name = NULL;
    instance->name = NULL;
    instance->next = NULL;
    return instance;
}


CElement* create_function(const char* name, char** params, int param_count, const char* body, SymbolTable* scope) {
    CElement* func = safe_malloc(sizeof(CElement));
    func->type = C_FUNCTION;
    func->name = safe_strdup(name);
    func->value.func_val = safe_malloc(sizeof(FunctionDef));
    func->value.func_val->name = safe_strdup(name);
    func->value.func_val->params = params;
    func->value.func_val->param_count = param_count;
    func->value.func_val->body = safe_strdup(body);
    func->value.func_val->scope = scope; // Capture the scope where the function is defined
    func->next = NULL;
    return func;
}


CElement* create_error(const char* message) {
    CElement* error = safe_malloc(sizeof(CElement));
    error->type = C_ERROR;
    error->value.string_val = safe_strdup(message);
    error->name = NULL;
    error->next = NULL;
    return error;
}

CElement* create_none() {
    CElement* none = safe_malloc(sizeof(CElement));
    none->type = C_NONE;
    none->name = NULL;
    none->next = NULL;
    return none;
}

// New helper function to create a deep copy of a CElement (for return values from symbol table)
CElement* copy_celement(CElement* original) {
    if (!original) return NULL;
    CElement* copy = safe_malloc(sizeof(CElement));
    copy->type = original->type;
    copy->name = NULL; // Copy should not have a name for symbol table binding
    copy->next = NULL;

    switch (original->type) {
        case C_INT: copy->value.int_val = original->value.int_val; break;
        case C_LONGLONG: copy->value.longlong_val = original->value.longlong_val; break;
        case C_FLOAT: copy->value.float_val = original->value.float_val; break;
        case C_CHAR: copy->value.char_val = original->value.char_val; break;
        case C_STRING: copy->value.string_val = safe_strdup(original->value.string_val); break;
        case C_POINTER: copy->value.ptr_val = original->value.ptr_val; break; // Shallow copy of pointer value
        case C_FUNCTION: copy->value.func_val = original->value.func_val; break; // Shallow copy of function def
        case C_STRUCT: copy->value.struct_val = original->value.struct_val; break; // Shallow copy of struct instance (complex, might need deep copy later)
        case C_UNION: copy->value.union_val = original->value.union_val; break; // Shallow copy of union instance
        case C_ERROR: copy->value.string_val = safe_strdup(original->value.string_val); break;
        case C_NONE: break;
        default: break;
    }
    return copy;
}


SymbolTable* create_scope(SymbolTable* parent) {
    SymbolTable* scope = safe_malloc(sizeof(SymbolTable));
    scope->symbols = NULL;
    scope->parent = parent;
    return scope;
}

void bind_symbol(SymbolTable* scope, const char* name, CElement* value) {
    CElement* current = scope->symbols;
    while (current) {
        if (current->name && strcmp(current->name, name) == 0) {
            // Found existing symbol in the current scope. Update its content.
            // Free old dynamically allocated content if necessary
            if (current->type == C_STRING && current->value.string_val) {
                free(current->value.string_val);
            }
            // Copy new value's content
            current->type = value->type;
            current->value = value->value; // This copies the union contents directly

            // The value CElement passed to bind_symbol is a temporary, so free its container
            if (value->name) free(value->name); // Free the name if it was duplicated for the temporary
            free(value); // Free the temporary CElement container
            return;
        }
        current = current->next;
    }

    // If not found in the current scope, check parent scopes.
    // If found in a parent scope, we should not re-bind it in the current scope.
    // C's assignment to an existing variable modifies the original.
    // However, this `bind_symbol` is used for *declarations* and *initial assignments*.
    // For assignments to existing variables, `assign_variable` handles it by looking up.

    // Add new symbol if not found in current scope
    value->name = safe_strdup(name); // Duplicate name for the new symbol table entry
    value->next = scope->symbols;
    scope->symbols = value;
}


CElement* lookup_symbol(SymbolTable* scope, const char* name) {
    SymbolTable* current_scope = scope;
    while (current_scope) {
        CElement* current_symbol = current_scope->symbols;
        while (current_symbol) {
            if (current_symbol->name && strcmp(current_symbol->name, name) == 0) {
                return current_symbol;
            }
            current_symbol = current_symbol->next;
        }
        current_scope = current_scope->parent;
    }
    return NULL; // Symbol not found
}

// Function to evaluate an expression node
CElement* evaluate_expr(ExprNode* node, SymbolTable* scope) {
    if (!node) return create_error("Expression node is NULL"); // Return error for null node

    switch (node->type) {
        case EXPR_LITERAL:
            return node->data.literal;
        case EXPR_VARIABLE: {
            CElement* var = lookup_symbol(scope, node->data.var_name);
            if (!var) {
                return create_error("Undefined variable");
            }
            return copy_celement(var); // Return a copy to prevent premature freeing
        }
        case EXPR_UNARY_OP: {
            if (!node->data.unary_op.operand) return create_error("Unary operand is NULL");
            CElement* operand = evaluate_expr(node->data.unary_op.operand, scope);
            if (operand->type == C_ERROR) return operand;

            CElement* result = NULL;
            if (node->data.unary_op.op == '-') {
                if (operand->type == C_INT) result = create_int(-operand->value.int_val);
                else if (operand->type == C_LONGLONG) result = create_longlong(-operand->value.longlong_val);
                else if (operand->type == C_FLOAT) result = create_float(-operand->value.float_val);
                else result = create_error("Unsupported type for unary '-' operator");
            } else if (node->data.unary_op.op == '!') {
                if (operand->type == C_INT) result = create_int(!operand->value.int_val);
                else result = create_error("Unsupported type for unary '!' operator");
            } else if (node->data.unary_op.op == '&') { // Address-of operator
                // For '&var', return a C_POINTER to the CElement representing 'var'
                // The operand must be a variable (not a literal or temporary result)
                // We need to look up the original CElement in the symbol table
                if (node->data.unary_op.operand->type == EXPR_VARIABLE) {
                    CElement* var_elem = lookup_symbol(scope, node->data.unary_op.operand->data.var_name);
                    if (var_elem) {
                        result = create_pointer(var_elem); // Pointer to the actual CElement in symbol table
                    } else {
                        result = create_error("Cannot take address of non-existent variable");
                    }
                } else {
                    result = create_error("Cannot take address of non-variable or temporary expression");
                }
            } else if (node->data.unary_op.op == '*') { // Dereference operator
                // For '*ptr', dereference the pointer to get the value it points to
                if (operand->type == C_POINTER && operand->value.ptr_val) {
                    CElement* pointed_to_elem = (CElement*)operand->value.ptr_val;
                    if (pointed_to_elem) {
                        // Return a copy of the pointed-to element's value
                        result = copy_celement(pointed_to_elem);
                    } else {
                        result = create_error("Attempted to dereference NULL pointer");
                    }
                } else {
                    result = create_error("Cannot dereference non-pointer type");
                }
            } else {
                result = create_error("Unsupported unary operator");
            }
            free_element(operand); // Free the temporary operand
            return result;
        }
        case EXPR_BINARY_OP: {
            if (!node->data.binary_op.left) return create_error("Binary left operand is NULL");
            CElement* left = evaluate_expr(node->data.binary_op.left, scope);
            if (left->type == C_ERROR) return left;

            if (!node->data.binary_op.right) {
                free_element(left); // Clean up left operand
                return create_error("Binary right operand is NULL");
            }
            CElement* right = evaluate_expr(node->data.binary_op.right, scope);
            if (right->type == C_ERROR) {
                free_element(left); // Clean up left operand
                return right;
            }

            CElement* result_elem = NULL;

            // Handle type promotion for binary operations (simplified)
            if (left->type == C_FLOAT || right->type == C_FLOAT) {
                float l_val = (left->type == C_FLOAT) ? left->value.float_val :
                              (left->type == C_INT) ? (float)left->value.int_val :
                              (left->type == C_LONGLONG) ? (float)left->value.longlong_val : 0.0f;
                float r_val = (right->type == C_FLOAT) ? right->value.float_val :
                              (right->type == C_INT) ? (float)right->value.int_val :
                              (right->type == C_LONGLONG) ? (float)right->value.longlong_val : 0.0f;

                switch (node->data.binary_op.op) {
                    case '+': result_elem = create_float(l_val + r_val); break;
                    case '-': result_elem = create_float(l_val - r_val); break;
                    case '*': result_elem = create_float(l_val * r_val); break;
                    case '/':
                        if (r_val == 0.0f) result_elem = create_error("Division by zero");
                        else result_elem = create_float(l_val / r_val);
                        break;
                    case '<': result_elem = create_int(l_val < r_val); break;
                    case '>': result_elem = create_int(l_val > r_val); break;
                    case '=': result_elem = create_int(l_val == r_val); break;
                    case '!': result_elem = create_int(l_val != r_val); break;
                    default: result_elem = create_error("Unsupported float binary operator"); break;
                }
            } else if (left->type == C_LONGLONG || right->type == C_LONGLONG) {
                long long l_val = (left->type == C_LONGLONG) ? left->value.longlong_val :
                                  (left->type == C_INT) ? (long long)left->value.int_val : 0LL;
                long long r_val = (right->type == C_LONGLONG) ? right->value.longlong_val :
                                  (right->type == C_INT) ? (long long)right->value.int_val : 0LL;

                switch (node->data.binary_op.op) {
                    case '+': result_elem = create_longlong(l_val + r_val); break;
                    case '-': result_elem = create_longlong(l_val - r_val); break;
                    case '*': result_elem = create_longlong(l_val * r_val); break;
                    case '/':
                        if (r_val == 0) result_elem = create_error("Division by zero");
                        else result_elem = create_longlong(l_val / r_val);
                        break;
                    case '%':
                        if (r_val == 0) result_elem = create_error("Modulo by zero");
                        else result_elem = create_longlong(l_val % r_val);
                        break;
                    case '<': result_elem = create_int(l_val < r_val); break;
                    case '>': result_elem = create_int(l_val > r_val); break;
                    case '=': result_elem = create_int(l_val == r_val); break;
                    case '!': result_elem = create_int(l_val != r_val); break;
                    case '&': result_elem = create_int(l_val && r_val); break;
                    case '|': result_elem = create_int(l_val || r_val); break;
                    default: result_elem = create_error("Unsupported long long binary operator"); break;
                }
            } else if (left->type == C_INT && right->type == C_INT) {
                switch (node->data.binary_op.op) {
                    case '+': result_elem = create_int(left->value.int_val + right->value.int_val); break;
                    case '-': result_elem = create_int(left->value.int_val - right->value.int_val); break;
                    case '*': result_elem = create_int(left->value.int_val * right->value.int_val); break;
                    case '/':
                        if (right->value.int_val == 0) result_elem = create_error("Division by zero");
                        else result_elem = create_int(left->value.int_val / right->value.int_val);
                        break;
                    case '%':
                        if (right->value.int_val == 0) result_elem = create_error("Modulo by zero");
                        else result_elem = create_int(left->value.int_val % right->value.int_val);
                        break;
                    case '<': result_elem = create_int(left->value.int_val < right->value.int_val); break;
                    case '>': result_elem = create_int(left->value.int_val > right->value.int_val); break;
                    case '=': result_elem = create_int(left->value.int_val == right->value.int_val); break; // ==
                    case '!': result_elem = create_int(left->value.int_val != right->value.int_val); break; // !=
                    case '&': result_elem = create_int(left->value.int_val && right->value.int_val); break; // &&
                    case '|': result_elem = create_int(left->value.int_val || right->value.int_val); break; // ||
                    default: result_elem = create_error("Unsupported integer binary operator"); break;
                }
            } else if (left->type == C_STRING && right->type == C_STRING) {
                if (node->data.binary_op.op == '=') { // String equality
                    result_elem = create_int(strcmp(left->value.string_val, right->value.string_val) == 0);
                } else {
                    result_elem = create_error("Unsupported binary operation for strings (C does not concatenate with '+'). Use string_add().");
                }
            } else {
                result_elem = create_error("Invalid types for binary operation");
            }

            free_element(left);
            free_element(right);
            return result_elem;
        }
        case EXPR_ASSIGNMENT:
            return assign_variable(node, scope);
        case EXPR_CALL:
            return execute_function_call(node, scope);
        case EXPR_MEMBER_ACCESS: {
            if (!node->data.member_access.primary) return create_error("Member access primary is NULL");
            CElement* primary_val = evaluate_expr(node->data.member_access.primary, scope);
            if (primary_val->type == C_ERROR) return primary_val;

            if (primary_val->type == C_STRUCT) {
                StructInstance* struct_instance = primary_val->value.struct_val;
                StructDef* struct_def = struct_instance->def;
                StructField* current_field = struct_def->fields;
                int i = 0;
                while (current_field) {
                    if (strcmp(current_field->name, node->data.member_access.member_name) == 0) {
                        CElement* result = copy_celement(struct_instance->fields[i]); // Return a copy
                        free_element(primary_val); // Free the temporary struct instance
                        return result;
                    }
                    current_field = current_field->next;
                    i++;
                }
                free_element(primary_val);
                return create_error("Struct member not found");
            } else if (primary_val->type == C_UNION) {
                UnionInstance* union_instance = primary_val->value.union_val;
                if (union_instance->active_member_name && strcmp(union_instance->active_member_name, node->data.member_access.member_name) == 0) {
                    CElement* result = copy_celement(union_instance->active_member); // Return a copy
                    free_element(primary_val);
                    return result;
                }
                free_element(primary_val);
                return create_error("Union member not active or not found");
            }
            free_element(primary_val);
            return create_error("Member access on non-struct/union type");
        }
        case EXPR_INDEX_ACCESS: {
            if (!node->data.index_access.primary) return create_error("Index access primary is NULL");
            if (!node->data.index_access.index) return create_error("Index access index is NULL");

            CElement* primary_val = evaluate_expr(node->data.index_access.primary, scope);
            if (primary_val->type == C_ERROR) return primary_val;

            CElement* index_val = evaluate_expr(node->data.index_access.index, scope);
            if (index_val->type == C_ERROR) { free_element(primary_val); return index_val; }
            if (index_val->type != C_INT) {
                free_element(primary_val); free_element(index_val);
                return create_error("Array/pointer index must be an integer");
            }
            int index = index_val->value.int_val;

            CElement* result = NULL;
            if (primary_val->type == C_STRING) {
                if (index >= 0 && index < strlen(primary_val->value.string_val)) {
                    result = create_char(primary_val->value.string_val[index]);
                } else {
                    result = create_error("String index out of bounds");
                }
            } else if (primary_val->type == C_POINTER) {
                // This is a simplified approach. A real C interpreter would need to know
                // the type the pointer points to to calculate correct byte offset.
                // For now, assuming char* or int* based on context.
                // If it points to a CElement*, then it's an array of CElements.
                if (primary_val->value.ptr_val && index >= 0) {
                    // Assuming it's a pointer to a CElement (like from &var)
                    CElement* base_elem = (CElement*)primary_val->value.ptr_val;
                    if (base_elem->type == C_STRING) { // Pointer to a string
                        if (index < strlen(base_elem->value.string_val)) {
                            result = create_char(base_elem->value.string_val[index]);
                        } else {
                            result = create_error("Pointer to string index out of bounds");
                        }
                    } else if (base_elem->type == C_INT) { // Pointer to an int
                        // Here, if it's `&i`, `ptr_val` is `&i_CElement`.
                        // `*(&i)` should return `i`'s value. `(&i)[0]` should return `i`'s value.
                        // `(&i)[1]` is undefined.
                        // For now, if it's a pointer to a single CElement, [0] returns that element.
                        if (index == 0) {
                            result = copy_celement(base_elem);
                        } else {
                            result = create_error("Pointer to single element accessed with non-zero index");
                        }
                    } else {
                        result = create_error("Unsupported pointer type for indexing");
                    }
                } else {
                    result = create_error("Invalid pointer or index for access");
                }
            } else if (primary_val->type == C_ARRAY) {
                // Needs array size tracking
                result = create_error("Array indexing not fully implemented (size tracking)");
            } else {
                result = create_error("Cannot index non-string, non-pointer, non-array type");
            }
            free_element(primary_val);
            free_element(index_val);
            return result;
        }
        case EXPR_CAST: {
            if (!node->data.cast_expr.operand) return create_error("Cast operand is NULL");
            CElement* operand = evaluate_expr(node->data.cast_expr.operand, scope);
            if (operand->type == C_ERROR) return operand;

            CElementType target_type = node->data.cast_expr.target_type;
            CElement* result = NULL;

            // Simplified casting rules
            if (target_type == operand->type) {
                result = operand; // No change needed, return original
            } else if (target_type == C_INT) {
                if (operand->type == C_FLOAT) result = create_int((int)operand->value.float_val);
                else if (operand->type == C_LONGLONG) result = create_int((int)operand->value.longlong_val);
                else if (operand->type == C_CHAR) result = create_int((int)operand->value.char_val);
                else if (operand->type == C_POINTER) result = create_int((int)(long)operand->value.ptr_val); // Pointer to int cast
                else result = create_error("Invalid cast to int");
            } else if (target_type == C_FLOAT) {
                if (operand->type == C_INT) result = create_float((float)operand->value.int_val);
                else if (operand->type == C_LONGLONG) result = create_float((float)operand->value.longlong_val);
                else result = create_error("Invalid cast to float");
            } else if (target_type == C_CHAR) {
                if (operand->type == C_INT) result = create_char((char)operand->value.int_val);
                else result = create_error("Invalid cast to char");
            } else if (target_type == C_POINTER) {
                // Casting any value to a pointer. This is a very unsafe C operation.
                // For now, just store the raw value as a pointer.
                if (operand->type == C_INT) result = create_pointer((void*)(long)operand->value.int_val);
                else if (operand->type == C_LONGLONG) result = create_pointer((void*)(long)operand->value.longlong_val);
                else if (operand->type == C_POINTER) result = create_pointer(operand->value.ptr_val); // Cast pointer to pointer
                else result = create_error("Invalid cast to pointer");
            } else {
                result = create_error("Unsupported cast operation");
            }
            free_element(operand);
            return result;
        }
        default:
            return create_error("Invalid expression type");
    }
}


// Function to execute a block of statements
FlowResult* execute_block(const char* code, SymbolTable* scope, bool in_loop) { // Added in_loop parameter
    Lexer* lexer = create_lexer(code);
    FlowResult* result = safe_malloc(sizeof(FlowResult));
    result->type = FLOW_NORMAL;
    result->value = create_none(); // Default return value

    while (peek_token(lexer)->type != TOKEN_EOF) {
        Token* current_token = peek_token(lexer);
        ExprNode* expr_node = NULL;

        // Determine if the current token sequence is a type declaration (for var or func)
        bool is_type_declaration_start = false;
        CElementType declared_type = C_NONE; // Will hold the base type (int, char, void, etc.)

        // Check for a type keyword at the beginning of a potential declaration
        if (current_token->type == TOKEN_KEYWORD) {
            CElementType temp_type = parse_type(lexer); // Try parsing a type
            if (temp_type != C_ERROR) { // If a valid type was parsed
                is_type_declaration_start = true;
                declared_type = temp_type;
            }
            // Rewind lexer to original position if it wasn't a type declaration start
            // This is a simplification; a real parser would manage token streams better.
            // For now, we'll just let the next `if` handle it.
            lexer->position = current_token->column -1; // Reset position to start of current token
            lexer->line = current_token->line;
            free(current_token->value); free(current_token); // Free the peeked token
            current_token = peek_token(lexer); // Re-peek the token
        }


        if (is_type_declaration_start) {
            // Consume the base type keyword(s) and potential pointer '*'
            declared_type = parse_type(lexer); // Re-parse the type to advance lexer

            Token* identifier_token = peek_token(lexer);
            if (!identifier_token || identifier_token->type != TOKEN_IDENTIFIER) {
                result->type = FLOW_ERROR;
                result->value = create_error("Expected identifier after type declaration");
                return result;
            }

            // Peek ahead to distinguish variable declaration from function definition
            Lexer* temp_lexer_for_func_check = create_lexer(lexer->input + lexer->position);
            advance_lexer(temp_lexer_for_func_check); // Consume identifier
            Token* next_after_identifier = peek_token(temp_lexer_for_func_check);

            if (next_after_identifier && strcmp(next_after_identifier->value, "(") == 0) {
                // It's a function definition
                free(temp_lexer_for_func_check);
                if (next_after_identifier->value) free(next_after_identifier->value);
                free(next_after_identifier); // Clean up temp tokens

                CElement* func = parse_function_def(lexer, scope, declared_type); // Pass the return type
                if (func->type == C_ERROR) {
                    result->type = FLOW_ERROR;
                    result->value = func;
                    return result;
                }
                bind_symbol(scope, func->name, func);
                continue; // Move to next statement after function definition
            }
            free(temp_lexer_for_func_check);
            if (next_after_identifier->value) free(next_after_identifier->value);
            free(next_after_identifier); // Clean up temp tokens

            // If not a function definition, it's a variable declaration
            advance_lexer(lexer); // Consume the variable name
            char* var_name = safe_strdup(identifier_token->value);

            CElement* initial_value = NULL;
            Token* assignment_op = peek_token(lexer);
            if (assignment_op && strcmp(assignment_op->value, "=") == 0) {
                advance_lexer(lexer); // Consume '='
                ExprNode* value_expr = parse_expression(lexer);
                if (!value_expr) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Expected expression after '=' in declaration");
                    free(var_name);
                    return result;
                }
                initial_value = evaluate_expr(value_expr, scope);
                if (initial_value->type == C_ERROR) {
                    result->type = FLOW_ERROR;
                    result->value = initial_value; // Propagate error
                    free(var_name);
                    return result;
                }
                // Type check: ensure initial_value type matches declared_type
                bool type_compatible = false;
                if (declared_type == initial_value->type) type_compatible = true;
                else if (declared_type == C_INT && (initial_value->type == C_FLOAT || initial_value->type == C_LONGLONG)) type_compatible = true;
                else if (declared_type == C_FLOAT && (initial_value->type == C_INT || initial_value->type == C_LONGLONG)) type_compatible = true;
                else if (declared_type == C_LONGLONG && (initial_value->type == C_INT || initial_value->type == C_FLOAT)) type_compatible = true;
                else if (declared_type == C_STRING && initial_value->type == C_STRING) type_compatible = true;
                else if (declared_type == C_POINTER && (initial_value->type == C_POINTER || initial_value->type == C_NONE)) type_compatible = true; // Allow NULL to pointer

                if (!type_compatible) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Type mismatch in declaration assignment");
                    free(var_name);
                    free_element(initial_value);
                    return result;
                }
            } else {
                // No initial assignment, initialize to default zero value
                if (declared_type == C_INT) initial_value = create_int(0);
                else if (declared_type == C_LONGLONG) initial_value = create_longlong(0LL);
                else if (declared_type == C_FLOAT) initial_value = create_float(0.0f);
                else if (declared_type == C_CHAR) initial_value = create_char('\0');
                else if (declared_type == C_STRING) initial_value = create_string(""); // Initialize string pointers to empty string
                else if (declared_type == C_POINTER) initial_value = create_pointer(NULL);
                else initial_value = create_none(); // For void or other types without a clear zero
            }

            bind_symbol(scope, var_name, initial_value);
            free(var_name); // Free the duplicated name

            Token* semicolon = get_next_token(lexer);
            if (!semicolon || strcmp(semicolon->value, ";") != 0) {
                result->type = FLOW_ERROR;
                result->value = create_error("Expected ';' after variable declaration");
            }
            if (semicolon) { free(semicolon->value); free(semicolon); }
            continue; // Move to next statement after declaration
        }


        if (current_token->type == TOKEN_KEYWORD) {
            if (strcmp(current_token->value, "if") == 0) {
                advance_lexer(lexer); // Consume "if"
                expr_node = safe_malloc(sizeof(ExprNode));
                expr_node->type = EXPR_IF;
                // Parse condition
                Token* open_paren_if = get_next_token(lexer);
                if (!open_paren_if || strcmp(open_paren_if->value, "(") != 0) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Expected '(' after if");
                    if (open_paren_if) { free(open_paren_if->value); free(open_paren_if); }
                    return result;
                }
                expr_node->data.if_stmt.condition = parse_expression(lexer);
                Token* close_paren_if = get_next_token(lexer);
                if (!close_paren_if || strcmp(close_paren_if->value, ")") != 0) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Expected ')' after if condition");
                    if (close_paren_if) { free(close_paren_if->value); free(close_paren_if); }
                    return result;
                }
                if (open_paren_if) { free(open_paren_if->value); free(open_paren_if); }
                if (close_paren_if) { free(close_paren_if->value); free(close_paren_if); }

                expr_node->data.if_stmt.if_body = parse_block(lexer); // Parse if body
                Token* next = peek_token(lexer);
                if (next->type == TOKEN_KEYWORD && strcmp(next->value, "else") == 0) {
                    advance_lexer(lexer); // Consume "else"
                    expr_node->data.if_stmt.else_body = parse_block(lexer); // Parse else body
                } else {
                    expr_node->data.if_stmt.else_body = NULL;
                }
                FlowResult* if_result = execute_if_statement(expr_node, scope, in_loop); // Pass in_loop
                if (if_result->type != FLOW_NORMAL) {
                    // Propagate return, break, continue
                    free(result->value);
                    free(result);
                    return if_result;
                }
            } else if (strcmp(current_token->value, "while") == 0) {
                advance_lexer(lexer); // Consume "while"
                expr_node = safe_malloc(sizeof(ExprNode));
                expr_node->type = EXPR_WHILE;
                Token* open_paren_while = get_next_token(lexer);
                if (!open_paren_while || strcmp(open_paren_while->value, "(") != 0) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Expected '(' after while");
                    if (open_paren_while) { free(open_paren_while->value); free(open_paren_while); }
                    return result;
                }
                expr_node->data.while_loop.condition = parse_expression(lexer);
                Token* close_paren_while = get_next_token(lexer);
                if (!close_paren_while || strcmp(close_paren_while->value, ")") != 0) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Expected ')' after while condition");
                    if (close_paren_while) { free(close_paren_while->value); free(close_paren_while); }
                    return result;
                }
                if (open_paren_while) { free(open_paren_while->value); free(open_paren_while); }
                if (close_paren_while) { free(close_paren_while->value); free(close_paren_while); }

                expr_node->data.while_loop.body = parse_block(lexer);
                FlowResult* while_result = execute_while_loop(expr_node, scope); // This function sets in_loop internally
                if (while_result->type != FLOW_NORMAL) {
                    free(result->value);
                    *result = *while_result; // Copy result (return/break/continue/error)
                    free(while_result);
                    return result;
                }
            } else if (strcmp(current_token->value, "for") == 0) { // New for loop parsing
                advance_lexer(lexer); // Consume "for"
                expr_node = safe_malloc(sizeof(ExprNode));
                expr_node->type = EXPR_FOR_LOOP;

                Token* open_paren_for = get_next_token(lexer);
                if (!open_paren_for || strcmp(open_paren_for->value, "(") != 0) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Expected '(' after for");
                    if (open_paren_for) { free(open_paren_for->value); free(open_paren_for); }
                    return result;
                }
                if (open_paren_for) { free(open_paren_for->value); free(open_paren_for); }

                // Parse init statement
                expr_node->data.for_loop.init_stmt_str = parse_statement_or_declaration(lexer); // Can be declaration or expression
                if (!expr_node->data.for_loop.init_stmt_str) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Failed to parse for loop initialization");
                    return result;
                }

                Token* semicolon1 = get_next_token(lexer);
                if (!semicolon1 || strcmp(semicolon1->value, ";") != 0) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Expected ';' after for loop initialization");
                    if (semicolon1) { free(semicolon1->value); free(semicolon1); }
                    return result;
                }
                if (semicolon1) { free(semicolon1->value); free(semicolon1); }

                // Parse condition expression
                expr_node->data.for_loop.condition_expr = parse_expression(lexer);
                if (!expr_node->data.for_loop.condition_expr) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Failed to parse for loop condition");
                    return result;
                }

                Token* semicolon2 = get_next_token(lexer);
                if (!semicolon2 || strcmp(semicolon2->value, ";") != 0) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Expected ';' after for loop condition");
                    if (semicolon2) { free(semicolon2->value); free(semicolon2); }
                    return result;
                }
                if (semicolon2) { free(semicolon2->value); free(semicolon2); }

                // Parse increment statement
                expr_node->data.for_loop.increment_stmt_str = parse_statement_or_declaration(lexer); // Can be expression
                if (!expr_node->data.for_loop.increment_stmt_str) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Failed to parse for loop increment");
                    return result;
                }

                Token* close_paren_for = get_next_token(lexer);
                if (!close_paren_for || strcmp(close_paren_for->value, ")") != 0) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Expected ')' after for loop increment");
                    if (close_paren_for) { free(close_paren_for->value); free(close_paren_for); }
                    return result;
                }
                if (close_paren_for) { free(close_paren_for->value); free(close_paren_for); }

                // Parse for loop body
                expr_node->data.for_loop.body_code = parse_block(lexer);
                if (!expr_node->data.for_loop.body_code) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Expected for loop body enclosed in {}");
                    return result;
                }

                FlowResult* for_result = execute_for_loop(expr_node, scope); // This function sets in_loop internally
                if (for_result->type != FLOW_NORMAL) {
                    free(result->value);
                    *result = *for_result; // Copy result (return/break/continue/error)
                    free(for_result);
                    return result;
                }
            } else if (strcmp(current_token->value, "return") == 0) {
                advance_lexer(lexer); // Consume "return"
                ExprNode* return_val_expr = parse_expression(lexer);
                result->type = FLOW_RETURN;
                free(result->value);
                result->value = evaluate_expr(return_val_expr, scope);
                // Ensure semicolon
                Token* semicolon_return = get_next_token(lexer);
                if (!semicolon_return || strcmp(semicolon_return->value, ";") != 0) {
                    result->type = FLOW_ERROR; // Set to error if semicolon is missing
                    result->value = create_error("Expected ';' after return statement");
                }
                if (semicolon_return) { free(semicolon_return->value); free(semicolon_return); }
                return result; // Exit block execution
            } else if (strcmp(current_token->value, "break") == 0) { // New break statement
                advance_lexer(lexer); // Consume "break"
                Token* semicolon_break = get_next_token(lexer);
                if (!semicolon_break || strcmp(semicolon_break->value, ";") != 0) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Expected ';' after break statement");
                } else if (!in_loop) { // Check if inside a loop
                    result->type = FLOW_ERROR;
                    result->value = create_error("Cannot use 'break' outside of a loop");
                } else {
                    result->type = FLOW_BREAK;
                }
                if (semicolon_break) { free(semicolon_break->value); free(semicolon_break); }
                return result; // Exit block execution
            } else if (strcmp(current_token->value, "continue") == 0) { // New continue statement
                advance_lexer(lexer); // Consume "continue"
                Token* semicolon_continue = get_next_token(lexer);
                if (!semicolon_continue || strcmp(semicolon_continue->value, ";") != 0) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Expected ';' after continue statement");
                } else if (!in_loop) { // Check if inside a loop
                    result->type = FLOW_ERROR;
                    result->value = create_error("Cannot use 'continue' outside of a loop");
                } else {
                    result->type = FLOW_CONTINUE;
                }
                if (semicolon_continue) { free(semicolon_continue->value); free(semicolon_continue); }
                return result; // Exit block execution
            }
            else if (strcmp(current_token->value, "struct") == 0) {
                advance_lexer(lexer); // Consume "struct"
                StructDef* struct_def = parse_struct_def(lexer);
                if (!struct_def) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Failed to parse struct definition");
                    return result;
                }
                // Store struct definition in the symbol table, e.g., under a specific type or a global list
                // For simplicity, binding it as a C_STRUCT type with its definition for now.
                CElement* struct_elem = safe_malloc(sizeof(CElement));
                struct_elem->type = C_STRUCT; // Re-using C_STRUCT type for the definition itself
                struct_elem->name = safe_strdup(struct_def->name);
                struct_elem->value.ptr_val = struct_def; // Store the definition pointer
                bind_symbol(scope, struct_elem->name, struct_elem);

            } else if (strcmp(current_token->value, "union") == 0) {
                advance_lexer(lexer); // Consume "union"
                UnionDef* union_def = parse_union_def(lexer);
                if (!union_def) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Failed to parse union definition");
                    return result;
                }
                CElement* union_elem = safe_malloc(sizeof(CElement));
                union_elem->type = C_UNION;
                union_elem->name = safe_strdup(union_def->name);
                union_elem->value.ptr_val = union_def;
                bind_symbol(scope, union_elem->name, union_elem);

            } else {
                // If it's another keyword not handled as a declaration or control flow
                expr_node = parse_expression(lexer);
                if (expr_node) {
                    CElement* eval_result = evaluate_expr(expr_node, scope);
                    if (eval_result->type == C_ERROR) {
                        result->type = FLOW_ERROR;
                        result->value = eval_result;
                        return result;
                    }
                    // IMPORTANT FIX: Do NOT free eval_result if it's an assignment that updated a symbol table entry.
                    // Only free temporary results of expressions that don't bind to a variable.
                    // For now, we assume all results of top-level expressions are temporary unless it's an assignment.
                    if (expr_node->type != EXPR_ASSIGNMENT) {
                        free_element(eval_result); // Free temporary result
                    }
                }
                Token* semicolon_stmt = get_next_token(lexer);
                if (!semicolon_stmt || strcmp(semicolon_stmt->value, ";") != 0) {
                    result->type = FLOW_ERROR;
                    result->value = create_error("Expected ';' after statement");
                }
                if (semicolon_stmt) { free(semicolon_stmt->value); free(semicolon_stmt); }
            }
        } else { // Not a keyword, assume it's an expression statement
            expr_node = parse_expression(lexer);
            if (expr_node) {
                CElement* eval_result = evaluate_expr(expr_node, scope);
                if (eval_result->type == C_ERROR) {
                    result->type = FLOW_ERROR;
                    result->value = eval_result;
                    return result;
                }
                // IMPORTANT FIX: Do NOT free eval_result if it's an assignment that updated a symbol table entry.
                if (expr_node->type != EXPR_ASSIGNMENT) {
                    free_element(eval_result); // Free temporary result
                }
            }
            Token* semicolon_expr_stmt = get_next_token(lexer);
            if (!semicolon_expr_stmt || strcmp(semicolon_expr_stmt->value, ";") != 0) {
                result->type = FLOW_ERROR;
                result->value = create_error("Expected ';' after expression statement");
            }
            if (semicolon_expr_stmt) { free(semicolon_expr_stmt->value); free(semicolon_expr_stmt); }
        }
    }
    free(lexer); // Free the lexer for this block
    return result;
}

FlowResult* execute_if_statement(ExprNode* node, SymbolTable* scope, bool in_loop) { // Pass in_loop
    CElement* condition_val = evaluate_expr(node->data.if_stmt.condition, scope);
    if (condition_val->type == C_ERROR) {
        FlowResult* res = safe_malloc(sizeof(FlowResult));
        res->type = FLOW_ERROR;
        res->value = condition_val;
        return res;
    }

    SymbolTable* if_scope = create_scope(scope);
    FlowResult* result;

    if ((condition_val->type == C_INT && condition_val->value.int_val != 0) ||
        (condition_val->type == C_LONGLONG && condition_val->value.longlong_val != 0) ||
        (condition_val->type == C_FLOAT && condition_val->value.float_val != 0.0f)) {
        result = execute_block(node->data.if_stmt.if_body, if_scope, in_loop); // Pass in_loop
    } else if (node->data.if_stmt.else_body) {
        SymbolTable* else_scope = create_scope(scope);
        result = execute_block(node->data.if_stmt.else_body, else_scope, in_loop); // Pass in_loop
        free_scope(else_scope);
    } else {
        result = safe_malloc(sizeof(FlowResult));
        result->type = FLOW_NORMAL;
        result->value = create_none();
    }
    free_scope(if_scope);
    free_element(condition_val);
    return result;
}

FlowResult* execute_while_loop(ExprNode* node, SymbolTable* scope) {
    FlowResult* result = safe_malloc(sizeof(FlowResult));
    result->type = FLOW_NORMAL;
    result->value = create_none();

    while (true) {
        CElement* condition_val = evaluate_expr(node->data.while_loop.condition, scope);
        if (condition_val->type == C_ERROR) {
            result->type = FLOW_ERROR;
            result->value = condition_val;
            return result;
        }

        if ((condition_val->type == C_INT && condition_val->value.int_val == 0) || // Use OR for condition check
            (condition_val->type == C_LONGLONG && condition_val->value.longlong_val == 0) ||
            (condition_val->type == C_FLOAT && condition_val->value.float_val == 0.0f)) {
            free_element(condition_val);
            break; // Condition is false, exit loop
        }
        free_element(condition_val);

        SymbolTable* loop_body_scope = create_scope(scope); // New scope for each iteration of body
        FlowResult* block_result = execute_block(node->data.while_loop.body, loop_body_scope, true); // Set in_loop to true
        free_scope(loop_body_scope);

        if (block_result->type == FLOW_RETURN) {
            free(result->value);
            *result = *block_result; // Copy return value/type
            free(block_result);
            return result;
        } else if (block_result->type == FLOW_BREAK) {
            free(block_result->value);
            free(block_result);
            break; // Exit loop
        } else if (block_result->type == FLOW_CONTINUE) {
            free(block_result->value);
            free(block_result);
            continue; // Continue to next iteration
        } else if (block_result->type == FLOW_ERROR) {
            free(result->value);
            *result = *block_result; // Propagate error
            free(block_result);
            return result;
        }
        free(block_result->value);
        free(block_result);
    }
    return result;
}

FlowResult* execute_for_loop(ExprNode* node, SymbolTable* scope) {
    FlowResult* result = safe_malloc(sizeof(FlowResult));
    result->type = FLOW_NORMAL;
    result->value = create_none();

    SymbolTable* for_loop_scope = create_scope(scope); // Scope for for loop variables (init, increment)

    // 1. Execute Initialization
    FlowResult* init_res = execute_block(node->data.for_loop.init_stmt_str, for_loop_scope, true); // in_loop true for init
    if (init_res->type == FLOW_ERROR) {
        free_scope(for_loop_scope);
        free(result->value);
        *result = *init_res;
        free(init_res);
        return result;
    }
    free(init_res->value);
    free(init_res);

    while (true) {
        // 2. Evaluate Condition
        CElement* condition_val = evaluate_expr(node->data.for_loop.condition_expr, for_loop_scope);
        if (condition_val->type == C_ERROR) {
            free_scope(for_loop_scope);
            result->type = FLOW_ERROR;
            result->value = condition_val;
            return result;
        }

        if ((condition_val->type == C_INT && condition_val->value.int_val == 0) ||
            (condition_val->type == C_LONGLONG && condition_val->value.longlong_val == 0) ||
            (condition_val->type == C_FLOAT && condition_val->value.float_val == 0.0f)) {
            free_element(condition_val);
            break; // Condition is false, exit loop
        }
        free_element(condition_val);

        // 3. Execute Body
        SymbolTable* loop_body_scope = create_scope(for_loop_scope); // New scope for each iteration of body
        FlowResult* body_res = execute_block(node->data.for_loop.body_code, loop_body_scope, true); // Set in_loop to true
        free_scope(loop_body_scope);

        if (body_res->type == FLOW_RETURN) {
            free(result->value);
            *result = *body_res; // Copy return value/type
            free(body_res);
            free_scope(for_loop_scope);
            return result;
        } else if (body_res->type == FLOW_BREAK) {
            free(body_res->value);
            free(body_res);
            break; // Exit loop
        } else if (body_res->type == FLOW_CONTINUE) {
            free(body_res->value);
            free(body_res);
            // Skip increment, continue to next iteration
        } else if (body_res->type == FLOW_ERROR) {
            free(result->value);
            *result = *body_res; // Propagate error
            free(body_res);
            free_scope(for_loop_scope);
            return result;
        }
        free(body_res->value);
        free(body_res);

        // 4. Execute Increment (if not a continue)
        if (body_res->type != FLOW_CONTINUE) {
            FlowResult* increment_res = execute_block(node->data.for_loop.increment_stmt_str, for_loop_scope, true); // in_loop true for increment
            if (increment_res->type == FLOW_ERROR) {
                free_scope(for_loop_scope);
                free(result->value);
                *result = *increment_res;
                free(increment_res);
                return result;
            }
            free(increment_res->value);
            free(increment_res);
        }
    }
    free_scope(for_loop_scope);
    return result;
}


// Function to assign value to a variable
CElement* assign_variable(ExprNode* node, SymbolTable* scope) {
    CElement* var_to_assign = lookup_symbol(scope, node->data.assignment.var_name);
    CElement* value = evaluate_expr(node->data.assignment.value, scope);

    if (value->type == C_ERROR) {
        // If the value to assign is an error, propagate it
        return value;
    }

    if (!var_to_assign) {
        // This path should ideally only be taken for implicit declarations in REPL mode.
        // In strict C, assignment to undeclared variable is an error.
        // For now, allow it for convenience in REPL, declaring in current scope.
        CElement* new_var = safe_malloc(sizeof(CElement));
        new_var->type = value->type;
        new_var->name = safe_strdup(node->data.assignment.var_name);
        new_var->value = value->value; // Copy union content
        new_var->next = NULL;
        bind_symbol(scope, new_var->name, new_var); // Bind the new variable
        return new_var;
    }

    // If variable exists, update its value
    // Handle type conversions or throw an error for mismatch
    // For simplicity, allow some implicit conversions (e.g., int to float)
    bool compatible = false;
    if (var_to_assign->type == value->type) compatible = true;
    else if ((var_to_assign->type == C_INT && (value->type == C_FLOAT || value->type == C_LONGLONG))) compatible = true;
    else if ((var_to_assign->type == C_FLOAT && (value->type == C_INT || value->type == C_LONGLONG))) compatible = true;
    else if ((var_to_assign->type == C_LONGLONG && (value->type == C_INT || value->type == C_FLOAT))) compatible = true;
    else if (var_to_assign->type == C_STRING && value->type == C_STRING) compatible = true; // Allow string to string
    else if (var_to_assign->type == C_POINTER && (value->type == C_POINTER || value->type == C_NONE)) compatible = true; // Allow NULL to pointer

    if (!compatible) {
        free_element(value); // Free the temporary value
        return create_error("Type mismatch in assignment");
    }

    // Free old dynamically allocated content if necessary (e.g., old string value)
    if (var_to_assign->type == C_STRING && var_to_assign->value.string_val) {
        free(var_to_assign->value.string_val);
    }

    // Assign value based on type
    var_to_assign->type = value->type; // Update type if conversion happened
    var_to_assign->value = value->value;
    free(value); // Free the temporary value container

    return var_to_assign;
}

CElement* execute_function_call(ExprNode* node, SymbolTable* scope) {
    // 1. Evaluate arguments in the caller's scope
    CElement** arg_values_array = safe_malloc(sizeof(CElement*) * node->data.call.arg_count);
    for (int i = 0; i < node->data.call.arg_count; i++) {
        arg_values_array[i] = evaluate_expr(node->data.call.args[i], scope);
        if (arg_values_array[i]->type == C_ERROR) {
            // If any argument evaluation fails, clean up and propagate error
            for (int j = 0; j <= i; j++) {
                if (arg_values_array[j]) free_element(arg_values_array[j]);
            }
            free(arg_values_array);
            return create_error("Error evaluating function argument"); // More specific error
        }
    }

    // 2. Check for built-in functions
    CElement* builtin_result = handle_builtin_call(node->data.call.func_name, arg_values_array, node->data.call.arg_count);
    if (builtin_result) {
        // If it was a built-in, free the argument CElement*s and the array, then return the result
        for (int i = 0; i < node->data.call.arg_count; i++) {
            if (arg_values_array[i]) free_element(arg_values_array[i]);
        }
        free(arg_values_array);
        return builtin_result;
    }

    // 3. If not built-in, lookup user-defined function
    CElement* func = lookup_symbol(scope, node->data.call.func_name);
    if (!func || func->type != C_FUNCTION) {
        // If not found or not a function, free arguments and return error
        for (int i = 0; i < node->data.call.arg_count; i++) {
            if (arg_values_array[i]) free_element(arg_values_array[i]);
        }
        free(arg_values_array);
        return create_error("Function not found or not a function");
    }

    FunctionDef* func_def = func->value.func_val;

    if (node->data.call.arg_count != func_def->param_count) {
        // Argument count mismatch for user-defined function
        for (int i = 0; i < node->data.call.arg_count; i++) {
            if (arg_values_array[i]) free_element(arg_values_array[i]);
        }
        free(arg_values_array);
        return create_error("Argument count mismatch for user-defined function");
    }

    SymbolTable* func_scope = create_scope(func_def->scope ? func_def->scope : scope);

    // Bind arguments to parameters in the function's new scope
    for (int i = 0; i < node->data.call.arg_count; i++) {
        bind_symbol(func_scope, func_def->params[i], arg_values_array[i]); // Ownership of arg_values_array[i] transferred to func_scope
    }
    free(arg_values_array); // Free the array of pointers, as elements are now owned by func_scope

    FlowResult* result = execute_block(func_def->body, func_scope, false); // Functions are not loops, so in_loop is false
    CElement* return_value = result->value;
    free(result);

    free_scope(func_scope);

    return return_value;
}


// Parser functions (simplified)
ExprNode* parse_expression(Lexer* lexer) {
    Token* token = get_next_token(lexer);
    ExprNode* node = NULL;

    // IMPORTANT: Handle unexpected EOF or NULL token at the start of expression parsing
    if (!token || token->type == TOKEN_EOF) {
        if (token) { free(token->value); free(token); } // Free the EOF token if it's not NULL
        return NULL; // Indicates an incomplete expression or unexpected end of input
    }

    // Handle type cast: (type)expr
    if (token->type == TOKEN_PUNCTUATOR && strcmp(token->value, "(") == 0) {
        // Attempt to parse a type
        CElementType cast_type = parse_type(lexer);
        if (cast_type != C_ERROR) { // If a valid type was parsed
            Token* close_paren_cast = get_next_token(lexer);
            if (!close_paren_cast || strcmp(close_paren_cast->value, ")") != 0) {
                if (close_paren_cast) { free(close_paren_cast->value); free(close_paren_cast); }
                if (token) { free(token->value); free(token); }
                return NULL; // Error: Malformed cast
            }
            if (close_paren_cast) { free(close_paren_cast->value); free(close_paren_cast); }

            ExprNode* operand_expr = parse_expression(lexer);
            if (!operand_expr) {
                if (token) { free(token->value); free(token); }
                return NULL; // Error: Missing operand for cast
            }
            node = create_cast_expr(cast_type, operand_expr);
            if (token) { free(token->value); free(token); } // Free the initial '(' token
            return node; // Return the cast expression
        }
        // If not a cast, put the '(' back and proceed as normal
        lexer->position = token->column -1; // Reset position to start of current token
        lexer->line = token->line;
        free(token->value); free(token); // Free the token
        token = get_next_token(lexer); // Re-get the token, which should be '('
    }


    if (token->type == TOKEN_NUMBER) {
        if (token->type == TOKEN_LONGLONG) { // Check for LONGLONG type directly
            node = create_literal_expr(create_longlong(atoll(token->value)));
        } else {
            // Assume it's an int for now, or float if it has a decimal
            if (strchr(token->value, '.') != NULL) {
                node = create_literal_expr(create_float(atof(token->value)));
            } else {
                node = create_literal_expr(create_int(atoi(token->value)));
            }
        }
    } else if (token->type == TOKEN_STRING) {
        node = create_literal_expr(create_string(token->value));
    } else if (token->type == TOKEN_TRUE) {
        node = create_literal_expr(create_int(1)); // 'true' maps to integer 1
    } else if (token->type == TOKEN_FALSE) {
        node = create_literal_expr(create_int(0)); // 'false' maps to integer 0
    }
    else if (token->type == TOKEN_IDENTIFIER) {
        Token* next_token = peek_token(lexer);
        if (next_token && strcmp(next_token->value, "(") == 0) {
            // Function call
            advance_lexer(lexer); // Consume '('
            CallExpr call;
            call.func_name = safe_strdup(token->value);
            call.args = NULL;
            call.arg_count = 0;

            if (strcmp(peek_token(lexer)->value, ")") != 0) {
                // Parse arguments
                int capacity = 2;
                call.args = safe_malloc(sizeof(ExprNode*) * capacity);
                while (strcmp(peek_token(lexer)->value, ")") != 0 && peek_token(lexer)->type != TOKEN_EOF) {
                    if (call.arg_count == capacity) {
                        capacity *= 2;
                        call.args = realloc(call.args, sizeof(ExprNode*) * capacity);
                    }
                    ExprNode* arg_expr = parse_expression(lexer);
                    if (!arg_expr) { // If parsing an argument fails
                        // Clean up previously allocated args
                        for (int i = 0; i < call.arg_count; i++) free(call.args[i]);
                        free(call.args);
                        free(call.func_name);
                        if (token) { free(token->value); free(token); }
                        return NULL; // Indicate error
                    }
                    call.args[call.arg_count++] = arg_expr;

                    if (strcmp(peek_token(lexer)->value, ",") == 0) {
                        advance_lexer(lexer); // Consume ','
                    }
                }
            }
            Token* close_paren_call = get_next_token(lexer);
            if (!close_paren_call || strcmp(close_paren_call->value, ")") != 0) {
                // Clean up if closing paren is missing
                free(call.func_name);
                if (call.args) {
                    for (int i = 0; i < call.arg_count; i++) free(call.args[i]);
                    free(call.args);
                }
                if (close_paren_call) { free(close_paren_call->value); free(close_paren_call); }
                if (token) { free(token->value); free(token); }
                return NULL; // Indicate error, parser will handle
            }
            if (close_paren_call) { free(close_paren_call->value); free(close_paren_call); }
            node = create_call_expr(call.func_name, call.args, call.arg_count);
        } else if (next_token && (strcmp(next_token->value, "=") == 0 ||
                                  strcmp(next_token->value, "+=") == 0 ||
                                  strcmp(next_token->value, "-=") == 0 ||
                                  strcmp(next_token->value, "*=") == 0 ||
                                  strcmp(next_token->value, "/=") == 0 ||
                                  strcmp(next_token->value, "%=") == 0)) {
            // Assignment or Compound Assignment
            char* op_str = safe_strdup(next_token->value);
            advance_lexer(lexer); // Consume operator

            ExprNode* right_expr = parse_expression(lexer);
            if (!right_expr) {
                free(op_str);
                if (token) { free(token->value); free(token); }
                return NULL; // Error in parsing right-hand side
            }

            if (strcmp(op_str, "=") == 0) {
                node = create_assignment_expr(token->value, right_expr);
            } else {
                // Handle compound assignment: a op= b  =>  a = a op b
                char simple_op = op_str[0]; // Get the base operator (+, -, *, /, %)
                ExprNode* left_var_expr = create_variable_expr(token->value);
                ExprNode* binary_op_expr = create_binary_op_expr(simple_op, left_var_expr, right_expr);
                node = create_assignment_expr(token->value, binary_op_expr);
            }
            free(op_str); // Free the duplicated operator string
        } else if (next_token && strcmp(next_token->value, ".") == 0) {
            // Member access (struct/union)
            advance_lexer(lexer); // Consume '.'
            Token* member_name_token = get_next_token(lexer);
            if (!member_name_token || member_name_token->type != TOKEN_IDENTIFIER) {
                if (member_name_token) { free(member_name_token->value); free(member_name_token); }
                if (token) { free(token->value); free(token); }
                return NULL; // Indicate error, parser will handle
            }
            node = create_member_access_expr(create_variable_expr(token->value), member_name_token->value);
            if (member_name_token) { free(member_name_token->value); free(member_name_token); }

        } else if (next_token && strcmp(next_token->value, "[") == 0) { // Array/pointer indexing
            ExprNode* primary = create_variable_expr(token->value); // The variable being indexed
            advance_lexer(lexer); // Consume '['
            ExprNode* index_expr = parse_expression(lexer); // Parse the index expression
            Token* close_bracket = get_next_token(lexer);
            if (!close_bracket || strcmp(close_bracket->value, "]") != 0) {
                if (close_bracket) { free(close_bracket->value); free(close_bracket); }
                // Free primary and index_expr if error
                return NULL; // Error
            }
            if (close_bracket) { free(close_bracket->value); free(close_bracket); }
            node = create_index_access_expr(primary, index_expr);

        } else {
            node = create_variable_expr(token->value);
        }
    } else if (strcmp(token->value, "(") == 0) {
        node = parse_expression(lexer); // Parse sub-expression
        Token* close_paren_expr = get_next_token(lexer);
        if (!close_paren_expr || strcmp(close_paren_expr->value, ")") != 0) {
            if (close_paren_expr) { free(close_paren_expr->value); free(close_paren_expr); }
            if (token) { free(token->value); free(token); }
            return NULL; // Indicate error, parser will handle
        }
        if (close_paren_expr) { free(close_paren_expr->value); free(close_paren_expr); }
    } else if (strchr("+-!*&", token->value[0]) != NULL && strlen(token->value) == 1) { // Unary operators: +, -, !, *, &
        node = create_unary_op_expr(token->value[0], parse_expression(lexer));
    }
    // Handle binary operators after parsing primary expressions
    Token* next_op = peek_token(lexer);
    if (next_op && strchr("+-*/%<>=!&|", next_op->value[0]) != NULL && node) {
        // This part needs more robust precedence handling for a full C parser.
        // For now, it's a simple left-to-right association.
        // If the current operator is a compound assignment, it's already handled above.
        if (! (strcmp(next_op->value, "+=") == 0 ||
               strcmp(next_op->value, "-=") == 0 ||
               strcmp(next_op->value, "*=") == 0 ||
               strcmp(next_op->value, "/=") == 0 ||
               strcmp(next_op->value, "%=") == 0) ) {

            advance_lexer(lexer); // Consume operator
            node = create_binary_op_expr(next_op->value[0], node, parse_expression(lexer));
        }
    }
    if (token) { free(token->value); free(token); } // Free the initial token
    return node;
}

char* parse_block(Lexer* lexer) {
    Token* brace = get_next_token(lexer);
    if (!brace || strcmp(brace->value, "{") != 0) {
        // If it's not an opening brace, it's a single statement.
        // This simplified parser expects a block for control flow bodies.
        // For now, we'll return NULL to indicate a parsing error.
        // A more robust parser would rewind the lexer and parse a single statement.
        if (brace) { free(brace->value); free(brace); }
        return NULL; // Indicates an error or unexpected token
    }
    if (brace) { free(brace->value); free(brace); }

    int brace_count = 1;
    int start = lexer->position;
    while (brace_count > 0 && lexer->input[lexer->position] != '\0') {
        if (lexer->input[lexer->position] == '{') {
            brace_count++;
        } else if (lexer->input[lexer->position] == '}') {
            brace_count--;
        }
        lexer->position++;
        lexer->column++;
    }
    if (brace_count != 0) {
        return NULL; // Error: Mismatched braces
    }
    int length = lexer->position - start - 1; // Exclude the last '}'
    char* block_content = safe_malloc(length + 1);
    strncpy(block_content, &lexer->input[start], length);
    block_content[length] = '\0';
    return block_content;
}

// Helper function to parse a single statement or declaration for for-loop init/increment
char* parse_statement_or_declaration(Lexer* lexer) {
    size_t original_pos = lexer->position;
    int original_line = lexer->line;
    int original_col = lexer->column;

    // Try to find the next semicolon or closing parenthesis
    int semicolon_pos = -1;
    int paren_pos = -1;
    int current_pos = lexer->position;
    int balance = 0; // For nested parentheses

    while (lexer->input[current_pos] != '\0') {
        if (lexer->input[current_pos] == '(') {
            balance++;
        } else if (lexer->input[current_pos] == ')') {
            balance--;
        } else if (lexer->input[current_pos] == ';' && balance == 0) {
            semicolon_pos = current_pos;
            break;
        } else if (lexer->input[current_pos] == ')' && balance == -1) { // Closing paren for for loop
            paren_pos = current_pos;
            break;
        }
        current_pos++;
    }

    int end_pos = -1;
    if (semicolon_pos != -1) {
        end_pos = semicolon_pos;
    } else if (paren_pos != -1) {
        end_pos = paren_pos;
    }

    if (end_pos == -1) {
        // No semicolon or closing parenthesis found, probably an incomplete statement
        return NULL;
    }

    int length = end_pos - original_pos;
    char* stmt_str = safe_malloc(length + 1);
    strncpy(stmt_str, &lexer->input[original_pos], length);
    stmt_str[length] = '\0';

    // Advance lexer past the parsed statement/declaration
    lexer->position = end_pos;
    lexer->line = original_line; // Simplified line/column tracking for this helper
    lexer->column = original_col + length;

    return stmt_str;
}

// New function to parse a C type (e.g., "int", "char*", "long long")
CElementType parse_type(Lexer* lexer) {
    Token* type_token = get_next_token(lexer);
    if (!type_token || type_token->type != TOKEN_KEYWORD) {
        if (type_token) { free(type_token->value); free(type_token); }
        return C_ERROR; // Not a type keyword
    }

    CElementType type = C_ERROR;
    if (strcmp(type_token->value, "int") == 0) {
        type = C_INT;
    } else if (strcmp(type_token->value, "char") == 0) {
        type = C_CHAR;
    } else if (strcmp(type_token->value, "float") == 0) {
        type = C_FLOAT;
    } else if (strcmp(type_token->value, "void") == 0) {
        type = C_NONE;
    } else if (strcmp(type_token->value, "bool") == 0) {
        type = C_INT; // bool maps to int
    } else if (strcmp(type_token->value, "long") == 0) {
        Token* next_long = peek_token(lexer);
        if (next_long && strcmp(next_long->value, "long") == 0) {
            advance_lexer(lexer); // Consume second "long"
            type = C_LONGLONG;
        } else {
            type = C_INT; // Just "long" without "long long" is often treated as int or long int
        }
        if (next_long) { free(next_long->value); free(next_long); }
    } else {
        // Handle struct/union types later if needed
        type = C_ERROR;
    }
    if (type_token) { free(type_token->value); free(type_token); }

    // Check for pointer '*'
    Token* pointer_token = peek_token(lexer);
    if (pointer_token && strcmp(pointer_token->value, "*") == 0) {
        advance_lexer(lexer); // Consume '*'
        if (type == C_CHAR) {
            type = C_STRING; // Treat char* as C_STRING for simplified string handling
        } else {
            type = C_POINTER; // Generic pointer type
        }
    }
    if (pointer_token) { free(pointer_token->value); free(pointer_token); }

    return type;
}


CElement* parse_function_def(Lexer* lexer, SymbolTable* scope, CElementType return_type) {
    // Expecting: func_name ( type param1, type param2 ) { body }
    // The return type keyword (int, void, etc.) has already been consumed by execute_block.

    Token* func_name_token = get_next_token(lexer);
    if (!func_name_token || func_name_token->type != TOKEN_IDENTIFIER) {
        if (func_name_token) { free(func_name_token->value); free(func_name_token); }
        return create_error("Expected function name after return type in function definition");
    }
    char* func_name = safe_strdup(func_name_token->value);
    if (func_name_token) { free(func_name_token->value); free(func_name_token); }

    Token* open_paren = get_next_token(lexer);
    if (!open_paren || strcmp(open_paren->value, "(") != 0) {
        free(func_name);
        if (open_paren) { free(open_paren->value); free(open_paren); }
        return create_error("Expected '(' after function name");
    }
    if (open_paren) { free(open_paren->value); free(open_paren); }

    char** params = NULL;
    int param_count = 0;
    int param_capacity = 2;
    params = safe_malloc(sizeof(char*) * param_capacity);

    Token* next_token = peek_token(lexer);
    if (strcmp(next_token->value, ")") != 0) {
        while (next_token && strcmp(next_token->value, ")") != 0) {
            // Parse parameter type
            CElementType param_type = parse_type(lexer);
            if (param_type == C_ERROR) {
                for (int i = 0; i < param_count; i++) free(params[i]);
                free(params);
                free(func_name);
                return create_error("Expected parameter type");
            }

            // Parse parameter name
            Token* param_name_token = get_next_token(lexer);
            if (!param_name_token || param_name_token->type != TOKEN_IDENTIFIER) {
                for (int i = 0; i < param_count; i++) free(params[i]);
                free(params);
                free(func_name);
                if (param_name_token) { free(param_name_token->value); free(param_name_token); }
                return create_error("Expected parameter name after type");
            }

            if (param_count == param_capacity) {
                param_capacity *= 2;
                params = realloc(params, sizeof(char*) * param_capacity);
            }
            params[param_count++] = safe_strdup(param_name_token->value);
            if (param_name_token) { free(param_name_token->value); free(param_name_token); }

            next_token = peek_token(lexer);
            if (strcmp(next_token->value, ",") == 0) {
                advance_lexer(lexer); // Consume ','
                next_token = peek_token(lexer);
            }
        }
    }

    Token* close_paren = get_next_token(lexer);
    if (!close_paren || strcmp(close_paren->value, ")") != 0) {
        // Free previously allocated params on error
        for (int i = 0; i < param_count; i++) free(params[i]);
        free(params);
        free(func_name);
        if (close_paren) { free(close_paren->value); free(close_paren); }
        return create_error("Expected ')' after function parameters");
    }
    if (close_paren) { free(close_paren->value); free(close_paren); }

    // Function body
    char* body = parse_block(lexer);
    if (!body) {
        // Free previously allocated params and func_name on error
        for (int i = 0; i < param_count; i++) free(params[i]);
        free(params);
        free(func_name);
        return create_error("Expected function body enclosed in {}");
    }

    return create_function(func_name, params, param_count, body, scope);
}

StructDef* parse_struct_def(Lexer* lexer) {
    // Expecting: struct StructName { type field1; type field2; };
    Token* struct_name_token = get_next_token(lexer);
    if (!struct_name_token || struct_name_token->type != TOKEN_IDENTIFIER) {
        create_parser_error("Expected struct name after 'struct' keyword", lexer->line, lexer->column);
        if (struct_name_token) { free(struct_name_token->value); free(struct_name_token); }
        return NULL;
    }
    StructDef* new_struct = safe_malloc(sizeof(StructDef));
    new_struct->name = safe_strdup(struct_name_token->value);
    new_struct->fields = NULL;
    new_struct->next = NULL;
    if (struct_name_token) { free(struct_name_token->value); free(struct_name_token); }

    Token* open_brace = get_next_token(lexer);
    if (!open_brace || strcmp(open_brace->value, "{") != 0) {
        create_parser_error("Expected '{' after struct name", lexer->line, lexer->column);
        free(new_struct->name);
        free(new_struct);
        if (open_brace) { free(open_brace->value); free(open_brace); }
        return NULL;
    }
    if (open_brace) { free(open_brace->value); free(open_brace); }

    StructField* current_field = NULL;
    while (true) {
        Token* field_type_token_peek = peek_token(lexer);
        if (!field_type_token_peek || strcmp(field_type_token_peek->value, "}") == 0) {
            if (field_type_token_peek) { free(field_type_token_peek->value); free(field_type_token_peek); }
            break; // End of struct definition
        }
        
        CElementType field_type = parse_type(lexer);
        if (field_type == C_ERROR) {
            create_parser_error("Unknown field type in struct definition", lexer->line, lexer->column);
            return NULL;
        }

        Token* field_name_token = get_next_token(lexer);
        if (!field_name_token || field_name_token->type != TOKEN_IDENTIFIER) {
            create_parser_error("Expected field name in struct definition", lexer->line, lexer->column);
            if (field_name_token) { free(field_name_token->value); free(field_name_token); }
            return NULL;
        }

        StructField* new_field = safe_malloc(sizeof(StructField));
        new_field->name = safe_strdup(field_name_token->value);
        new_field->type = field_type;
        new_field->next = NULL;
        if (field_name_token) { free(field_name_token->value); free(field_name_token); }

        if (new_struct->fields == NULL) {
            new_struct->fields = new_field;
        } else {
            current_field->next = new_field;
        }
        current_field = new_field;

        Token* semicolon = get_next_token(lexer);
        if (!semicolon || strcmp(semicolon->value, ";") != 0) {
            create_parser_error("Expected ';' after struct field definition", lexer->line, lexer->column);
            if (semicolon) { free(semicolon->value); free(semicolon); }
            return NULL;
        }
        if (semicolon) { free(semicolon->value); free(semicolon); }
    }

    Token* close_brace = get_next_token(lexer);
    if (!close_brace || strcmp(close_brace->value, "}") != 0) {
        create_parser_error("Expected '}' at end of struct definition", lexer->line, lexer->column);
        if (close_brace) { free(close_brace->value); free(close_brace); }
        return NULL;
    }
    if (close_brace) { free(close_brace->value); free(close_brace); }

    Token* semicolon_after_struct = get_next_token(lexer);
    if (!semicolon_after_struct || strcmp(semicolon_after_struct->value, ";") != 0) {
        create_parser_error("Expected ';' after struct definition", lexer->line, lexer->column);
        if (semicolon_after_struct) { free(semicolon_after_struct->value); free(semicolon_after_struct); }
        return NULL;
    }
    if (semicolon_after_struct) { free(semicolon_after_struct->value); free(semicolon_after_struct); }

    return new_struct;
}

UnionDef* parse_union_def(Lexer* lexer) {
    // Similar to parse_struct_def
    Token* union_name_token = get_next_token(lexer);
    if (!union_name_token || union_name_token->type != TOKEN_IDENTIFIER) {
        create_parser_error("Expected union name after 'union' keyword", lexer->line, lexer->column);
        if (union_name_token) { free(union_name_token->value); free(union_name_token); }
        return NULL;
    }
    UnionDef* new_union = safe_malloc(sizeof(UnionDef));
    new_union->name = safe_strdup(union_name_token->value);
    new_union->fields = NULL;
    new_union->next = NULL;
    if (union_name_token) { free(union_name_token->value); free(union_name_token); }


    Token* open_brace = get_next_token(lexer);
    if (!open_brace || strcmp(open_brace->value, "{") != 0) {
        create_parser_error("Expected '{' after union name", lexer->line, lexer->column);
        free(new_union->name);
        free(new_union);
        if (open_brace) { free(open_brace->value); free(open_brace); }
        return NULL;
    }
    if (open_brace) { free(open_brace->value); free(open_brace); }


    UnionField* current_field = NULL;
    while (true) {
        Token* field_type_token_peek = peek_token(lexer);
        if (!field_type_token_peek || strcmp(field_type_token_peek->value, "}") == 0) {
            if (field_type_token_peek) { free(field_type_token_peek->value); free(field_type_token_peek); }
            break; // End of union definition
        }
        
        CElementType field_type = parse_type(lexer);
        if (field_type == C_ERROR) {
            create_parser_error("Unknown field type in union definition", lexer->line, lexer->column);
            return NULL;
        }


        Token* field_name_token = get_next_token(lexer);
        if (!field_name_token || field_name_token->type != TOKEN_IDENTIFIER) {
            create_parser_error("Expected field name in union definition", lexer->line, lexer->column);
            if (field_name_token) { free(field_name_token->value); free(field_name_token); }
            return NULL;
        }

        UnionField* new_field = safe_malloc(sizeof(UnionField));
        new_field->name = safe_strdup(field_name_token->value);
        new_field->type = field_type;
        new_field->next = NULL;
        if (field_name_token) { free(field_name_token->value); free(field_name_token); }


        if (new_union->fields == NULL) {
            new_union->fields = new_field;
        } else {
            current_field->next = new_field;
        }
        current_field = new_field;

        Token* semicolon = get_next_token(lexer);
        if (!semicolon || strcmp(semicolon->value, ";") != 0) {
            create_parser_error("Expected ';' after union field definition", lexer->line, lexer->column);
            if (semicolon) { free(semicolon->value); free(semicolon); }
            return NULL;
        }
        if (semicolon) { free(semicolon->value); free(semicolon); }
    }

    Token* close_brace = get_next_token(lexer);
    if (!close_brace || strcmp(close_brace->value, "}") != 0) {
        create_parser_error("Expected '}' at end of union definition", lexer->line, lexer->column);
        if (close_brace) { free(close_brace->value); free(close_brace); }
        return NULL;
    }
    if (close_brace) { free(close_brace->value); free(close_brace); }

    Token* semicolon_after_union = get_next_token(lexer);
    if (!semicolon_after_union || strcmp(semicolon_after_union->value, ";") != 0) {
        create_parser_error("Expected ';' after union definition", lexer->line, lexer->column);
        if (semicolon_after_union) { free(semicolon_after_union->value); free(semicolon_after_union); }
        return NULL;
    }
    if (semicolon_after_union) { free(semicolon_after_union->value); free(semicolon_after_union); }

    return new_union;
}


// Error handling for parser
ParserError* create_parser_error(const char* message, int line, int column) {
    ParserError* error = safe_malloc(sizeof(ParserError));
    error->message = safe_strdup(message);
    error->line = line;
    error->column = column;
    return error;
}

void print_parser_error(ParserError* error) {
    fprintf(stderr, "Parser Error at line %d, column %d: %s\n", error->line, error->column, error->message);
    free(error->message);
    free(error);
}

// Function to synchronize lexer after error
void synchronize(Lexer* lexer) {
    // Consume tokens until a known statement boundary or end of file
    while (peek_token(lexer)->type != TOKEN_EOF) {
        Token* token = peek_token(lexer);
        // Look for semicolons or keywords that mark the start of a new statement/declaration
        if (strcmp(token->value, ";") == 0 ||
            (token->type == TOKEN_KEYWORD &&
             (strcmp(token->value, "if") == 0 || strcmp(token->value, "while") == 0 ||
              strcmp(token->value, "return") == 0 || strcmp(token->value, "function") == 0 ||
              strcmp(token->value, "int") == 0 || strcmp(token->value, "char") == 0 ||
              strcmp(token->value, "float") == 0 || strcmp(token->value, "void") == 0 ||
              strcmp(token->value, "struct") == 0 || strcmp(token->value, "union") == 0 ||
              strcmp(token->value, "bool") == 0 || strcmp(token->value, "for") == 0 ||
              strcmp(token->value, "break") == 0 || strcmp(token->value, "continue") == 0)))
        {
            break; // Found a synchronization point
        }
        advance_lexer(lexer);
    }
}


// Memory management
void free_scope(SymbolTable* scope) {
    CElement* current = scope->symbols;
    while (current) {
        CElement* next = current->next;
        // Only free the content if it's dynamically allocated and not a pointer to another managed element
        if (current->type == C_STRING && current->value.string_val) {
            free(current->value.string_val);
        }
        if (current->name) {
            free(current->name);
        }
        // For C_POINTER, if it points to memory allocated by malloc, free that memory here.
        // This is a crucial part of memory management for the interpreter.
        // We'll rely on the 'free' builtin for explicit freeing of malloc'd blocks.
        // So, for now, don't free ptr_val here unless we explicitly track ownership.
        // This means, if a C_POINTER is holding a malloc'd block, it needs to be freed via `free()` builtin.
        // If it's a pointer to another CElement in the symbol table, that CElement is freed when its scope is freed.
        free(current); // Free the CElement container itself
        current = next;
    }
    free(scope);
}

void free_struct_instance(CElement* instance) {
    if (!instance || instance->type != C_STRUCT || !instance->value.struct_val) return;

    StructInstance* struct_data = instance->value.struct_val;
    StructDef* def = struct_data->def; // The definition itself is not freed here, it's shared

    // Free each field's CElement if it was allocated
    int field_count = 0;
    StructField* current_field = def->fields;
    while(current_field) {
        field_count++;
        current_field = current_field->next;
    }

    for (int i = 0; i < field_count; i++) {
        if (struct_data->fields[i]) {
            free_element(struct_data->fields[i]);
        }
    }
    free(struct_data->fields);
    free(struct_data);
}

void free_union_instance(CElement* instance) {
    if (!instance || instance->type != C_UNION || !instance->value.union_val) return;

    UnionInstance* union_data = instance->value.union_val;
    if (union_data->active_member) {
        free_element(union_data->active_member); // Free the active member's content
    }
    if (union_data->active_member_name) {
        free(union_data->active_member_name);
    }
    // The UnionDef itself is not freed here, it's shared
    free(union_data);
}


void free_element(CElement* elem) {
    if (!elem) return;

    if (elem->name) {
        free(elem->name);
    }

    switch (elem->type) {
        case C_STRING:
            if (elem->value.string_val) free(elem->value.string_val);
            break;
        case C_POINTER:
            // Don't free the pointer itself (elem->value.ptr_val) here.
            // Memory pointed to by ptr_val should be explicitly freed by `free()` builtin
            // or by the owning structure/scope.
            break;
        case C_ARRAY:
            if (elem->value.array_val) {
                // Assuming array_val is a NULL-terminated list or size is known elsewhere
                // For now, iterate and free elements if they are allocated
                // This requires careful handling as we don't have size info here
                // For simplicity, a shallow free for the array pointer itself
                free(elem->value.array_val);
            }
            break;
        case C_FUNCTION:
            if (elem->value.func_val) {
                if (elem->value.func_val->name) free(elem->value.func_val->name);
                if (elem->value.func_val->params) {
                    for (int i = 0; i < elem->value.func_val->param_count; i++) {
                        free(elem->value.func_val->params[i]);
                    }
                    free(elem->value.func_val->params);
                }
                if (elem->value.func_val->body) free(elem->value.func_val->body);
                // Don't free func_val->scope here, as it's typically managed by parent scope
                free(elem->value.func_val);
            }
            break;
        case C_STRUCT:
            free_struct_instance(elem);
            break;
        case C_UNION:
            free_union_instance(elem);
            break;
        case C_ERROR:
            if (elem->value.string_val) free(elem->value.string_val);
            break;
        default:
            // For C_INT, C_LONGLONG, C_FLOAT, C_CHAR, C_NONE, no extra memory to free
            break;
    }
    free(elem);
}


char* safe_strdup(const char* str) {
    if (!str) return NULL;
    char* new_str = strdup(str);
    if (!new_str) {
        fprintf(stderr, "Memory allocation failed for strdup!\n");
        exit(EXIT_FAILURE);
    }
    return new_str;
}

void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "Memory allocation failed for malloc of size %zu!\n", size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}


// Expression Node creation helpers
ExprNode* create_literal_expr(CElement* literal) {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_LITERAL;
    node->data.literal = literal;
    return node;
}

ExprNode* create_variable_expr(const char* name) {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_VARIABLE;
    node->data.var_name = safe_strdup(name);
    return node;
}

ExprNode* create_unary_op_expr(char op, ExprNode* operand) {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_UNARY_OP;
    node->data.unary_op.op = op;
    node->data.unary_op.operand = operand;
    return node;
}

ExprNode* create_binary_op_expr(char op, ExprNode* left, ExprNode* right) {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_BINARY_OP;
    node->data.binary_op.op = op;
    node->data.binary_op.left = left;
    node->data.binary_op.right = right;
    return node;
}

ExprNode* create_call_expr(const char* func_name, ExprNode** args, int arg_count) {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_CALL;
    node->data.call.func_name = safe_strdup(func_name);
    node->data.call.args = args;
    node->data.call.arg_count = arg_count;
    return node;
}

ExprNode* create_assignment_expr(const char* var_name, ExprNode* value) {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_ASSIGNMENT;
    node->data.assignment.var_name = safe_strdup(var_name);
    node->data.assignment.value = value;
    return node;
}

ExprNode* create_if_expr(ExprNode* condition, const char* if_body, const char* else_body) {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_IF;
    node->data.if_stmt.condition = condition;
    node->data.if_stmt.if_body = safe_strdup(if_body);
    node->data.if_stmt.else_body = safe_strdup(else_body);
    return node;
}

ExprNode* create_while_expr(ExprNode* condition, const char* body) {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_WHILE;
    node->data.while_loop.condition = condition;
    node->data.while_loop.body = safe_strdup(body);
    return node;
}

ExprNode* create_for_loop_expr(const char* init_stmt_str, ExprNode* condition_expr, const char* increment_stmt_str, const char* body_code) {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_FOR_LOOP;
    node->data.for_loop.init_stmt_str = safe_strdup(init_stmt_str);
    node->data.for_loop.condition_expr = condition_expr;
    node->data.for_loop.increment_stmt_str = safe_strdup(increment_stmt_str);
    node->data.for_loop.body_code = safe_strdup(body_code);
    return node;
}


ExprNode* create_return_expr(ExprNode* value) {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_RETURN;
    node->data.ret_stmt.value = value;
    return node;
}

ExprNode* create_break_expr() {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_BREAK;
    return node;
}

ExprNode* create_continue_expr() {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_CONTINUE;
    return node;
}


ExprNode* create_block_expr(ExprNode** statements, int count) {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_BLOCK;
    node->data.block.statements = statements;
    node->data.block.count = count;
    return node;
}

ExprNode* create_function_def_expr(FunctionDef* func_def) {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_FUNCTION_DEF;
    node->data.func_def = func_def;
    return node;
}

ExprNode* create_struct_def_expr(StructDef* struct_def) {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_STRUCT_DEF;
    node->data.struct_def = struct_def;
    return node;
}

ExprNode* create_union_def_expr(UnionDef* union_def) {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_UNION_DEF;
    node->data.union_def = union_def;
    return node;
}

ExprNode* create_member_access_expr(ExprNode* primary, const char* member_name) {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_MEMBER_ACCESS;
    node->data.member_access.primary = primary;
    node->data.member_access.member_name = safe_strdup(member_name);
    return node;
}

ExprNode* create_index_access_expr(ExprNode* primary, ExprNode* index) {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_INDEX_ACCESS;
    node->data.index_access.primary = primary;
    node->data.index_access.index = index;
    return node;
}

ExprNode* create_cast_expr(CElementType target_type, ExprNode* operand) {
    ExprNode* node = safe_malloc(sizeof(ExprNode));
    node->type = EXPR_CAST;
    node->data.cast_expr.target_type = target_type;
    node->data.cast_expr.operand = operand;
    return node;
}

// Preprocessor functions
void define_macro(PreprocessorState* state, const char* name, const char* value) {
    MacroDef* new_macro = safe_malloc(sizeof(MacroDef));
    new_macro->name = safe_strdup(name);
    new_macro->value = safe_strdup(value);
    new_macro->next = state->macros;
    state->macros = new_macro;
}

char* expand_macros(PreprocessorState* state, const char* code) {
    // This is a very simplified macro expansion.
    // A real preprocessor would handle tokenization,
    // recursive expansion, argument substitution, etc.
    // For now, it's a simple string replace.
    char* expanded_code = safe_strdup(code);
    MacroDef* current = state->macros;
    while (current) {
        // Find and replace current->name with current->value in expanded_code
        // This simple string replacement is prone to bugs for complex cases
        // but serves as a placeholder.
        char* pos;
        while ((pos = strstr(expanded_code, current->name)) != NULL) {
            char* before = strndup(expanded_code, pos - expanded_code);
            char* after = safe_strdup(pos + strlen(current->name));
            char* temp = safe_malloc(strlen(before) + strlen(current->value) + strlen(after) + 1);
            strcpy(temp, before);
            strcat(temp, current->value);
            strcat(temp, after);
            free(expanded_code);
            free(before);
            free(after);
            expanded_code = temp;
        }
        current = current->next;
    }
    return expanded_code;
}


void handle_preprocessor_directive(PreprocessorState* state, const char* line) {
    // This function needs to parse directives like #define, #ifdef, #ifndef, #else, #endif
    // For now, a very basic implementation.
    if (strncmp(line, "#define", 7) == 0) {
        char name[256], value[MAX_CMD_SIZE];
        if (sscanf(line, "#define %s %[^\n]", name, value) == 2) {
            define_macro(state, name, value);
        } else if (sscanf(line, "#define %s", name) == 1) {
            define_macro(state, name, ""); // Define with empty value
        }
    } else if (strncmp(line, "#ifdef", 6) == 0) {
        char name[256];
        if (sscanf(line, "#ifdef %s", name) == 1) {
            PPCondition* cond = safe_malloc(sizeof(PPCondition));
            cond->type = PP_IFDEF;
            cond->parent = state->current_condition;
            // Check if macro is defined
            bool is_defined = false;
            MacroDef* current_macro = state->macros;
            while(current_macro) {
                if (strcmp(current_macro->name, name) == 0) {
                    is_defined = true;
                    break;
                }
                current_macro = current_macro->next;
            }
            cond->is_true = is_defined;
            cond->condition_met_in_block = is_defined; // True if this #ifdef condition is met
            state->current_condition = cond;
        }
    } else if (strncmp(line, "#ifndef", 7) == 0) {
        char name[256];
        if (sscanf(line, "#ifndef %s", name) == 1) {
            PPCondition* cond = safe_malloc(sizeof(PPCondition));
            cond->type = PP_IFNDEF;
            cond->parent = state->current_condition;
            // Check if macro is defined
            bool is_defined = false;
            MacroDef* current_macro = state->macros;
            while(current_macro) {
                if (strcmp(current_macro->name, name) == 0) {
                    is_defined = true;
                    break;
                }
                current_macro = current_macro->next;
            }
            cond->is_true = !is_defined; // True if macro is NOT defined
            cond->condition_met_in_block = !is_defined; // True if this #ifndef condition is met
            state->current_condition = cond;
        }
    } else if (strncmp(line, "#else", 5) == 0) {
        if (state->current_condition) {
            // #else negates the previous condition if it hasn't been met yet
            state->current_condition->is_true = !state->current_condition->condition_met_in_block;
            state->current_condition->condition_met_in_block = true; // Mark that a condition in this block has been met
        }
    } else if (strncmp(line, "#endif", 6) == 0) {
        if (state->current_condition) {
            PPCondition* parent = state->current_condition->parent;
            free(state->current_condition);
            state->current_condition = parent;
        }
    }
}

PreprocessorState* create_preprocessor_state() {
    PreprocessorState* state = safe_malloc(sizeof(PreprocessorState));
    state->macros = NULL;
    state->current_condition = NULL;
    return state;
}


char* parse_c_code(const char* code) {
    PreprocessorState* pp_state = create_preprocessor_state();
    char* processed_code = safe_strdup(code); // Start with original code

    // First pass for preprocessor directives
    char* line = strtok(processed_code, "\n");
    char* temp_code_buffer = safe_malloc(MAX_CMD_SIZE * 10); // Large enough buffer
    temp_code_buffer[0] = '\0'; // Initialize empty

    while (line != NULL) {
        // Check preprocessor conditions
        bool include_line = true;
        if (pp_state->current_condition) {
            include_line = pp_state->current_condition->is_true;
        }

        if (line[0] == '#') {
            handle_preprocessor_directive(pp_state, line);
        } else if (include_line) {
            strcat(temp_code_buffer, line);
            strcat(temp_code_buffer, "\n"); // Add newline back
        }
        line = strtok(NULL, "\n");
    }
    free(processed_code); // Free the strtok'd copy
    processed_code = expand_macros(pp_state, temp_code_buffer);
    free(temp_code_buffer); // Free the temporary buffer

    // Here, you would parse and execute the C code
    // For this example, we just return the preprocessed code.
    return processed_code;
}


// Main REPL loop and file execution
void run_enhanced_repl() {
    char input[MAX_CMD_SIZE];
    SymbolTable* global_scope = create_scope(NULL); // Global scope

    // Bind built-in functions to the global scope
    for (int i = 0; i < NUM_BUILTIN_FUNCS; i++) {
        // Create a CElement for each built-in function and bind it
        CElement* func_elem = safe_malloc(sizeof(CElement));
        func_elem->type = C_FUNCTION;
        func_elem->name = safe_strdup(builtin_functions[i].name);
        func_elem->value.func_val = safe_malloc(sizeof(FunctionDef));
        func_elem->value.func_val->name = safe_strdup(builtin_functions[i].name);
        func_elem->value.func_val->params = NULL; // Built-ins don't have parsable params
        func_elem->value.func_val->param_count = 0; // Handled by min/max_args in BuiltinFuncInfo
        func_elem->value.func_val->body = NULL; // No body for built-ins
        func_elem->value.func_val->scope = NULL; // No scope for built-ins
        func_elem->next = NULL;
        // Store the function pointer in a way that handle_builtin_call can retrieve it.
        // For simplicity, we'll rely on handle_builtin_call's direct lookup.
        // The CElement itself just serves as a placeholder in the symbol table.
        bind_symbol(global_scope, func_elem->name, func_elem);
    }


    printf("C Interpreter REPL. Type 'exit()' to quit.\n");
    while (true) {
        printf(">>> ");
        fflush(stdout);
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        input[strcspn(input, "\n")] = 0; // Remove newline

        if (strcmp(input, "exit()") == 0) {
            break;
        }

        char* processed_input = parse_c_code(input); // Apply preprocessing

        Lexer* lexer = create_lexer(processed_input);
        FlowResult* result = execute_block(processed_input, global_scope, false); // REPL is not in a loop

        if (result->type == FLOW_RETURN) {
            printf("Returned: ");
            if (result->value->type == C_INT) {
                printf("%d\n", result->value->value.int_val);
            } else if (result->value->type == C_LONGLONG) {
                printf("%lld\n", result->value->value.longlong_val);
            } else if (result->value->type == C_FLOAT) {
                printf("%f\n", result->value->value.float_val);
            } else if (result->value->type == C_STRING) {
                printf("\"%s\"\n", result->value->value.string_val);
            } else if (result->value->type == C_ERROR) {
                fprintf(stderr, "Runtime Error: %s\n", result->value->value.string_val);
            } else {
                printf("None\n");
            }
        } else if (result->type == FLOW_ERROR) {
            fprintf(stderr, "Runtime Error: %s\n", result->value->value.string_val);
        } else if (result->type == FLOW_BREAK || result->type == FLOW_CONTINUE) {
            fprintf(stderr, "Runtime Error: Cannot use '%s' outside of a loop\n",
                    result->type == FLOW_BREAK ? "break" : "continue");
        }


        free(processed_input);
        free_element(result->value);
        free(result);
        // Lexer is freed inside execute_block
    }
    free_scope(global_scope);
}


int main(int argc, char* argv[]) {
    int exit_code = 0;

    // The main_module_scope acts as the top-level scope for file execution.
    // Any global variables or functions defined in the file will reside here.
    SymbolTable* main_module_scope = create_scope(NULL);
    
    // main_module is a placeholder CElement to represent the module itself,
    // though its direct use is limited in this simplified interpreter.
    CElement* main_module = safe_malloc(sizeof(CElement));
    main_module->type = C_NONE; // Placeholder type
    main_module->name = safe_strdup("main_module");
    main_module->value.ptr_val = NULL; // No specific value
    main_module->next = NULL;

    // Initialize preprocessor state for file processing
    PreprocessorState* pp_state = create_preprocessor_state();

    // Bind built-in functions to the main module's scope for file execution
    for (int i = 0; i < NUM_BUILTIN_FUNCS; i++) {
        CElement* func_elem = safe_malloc(sizeof(CElement));
        func_elem->type = C_FUNCTION;
        func_elem->name = safe_strdup(builtin_functions[i].name);
        func_elem->value.func_val = safe_malloc(sizeof(FunctionDef));
        func_elem->value.func_val->name = safe_strdup(builtin_functions[i].name);
        func_elem->value.func_val->params = NULL;
        func_elem->value.func_val->param_count = 0;
        func_elem->value.func_val->body = NULL;
        func_elem->value.func_val->scope = NULL;
        func_elem->next = NULL;
        bind_symbol(main_module_scope, func_elem->name, func_elem);
    }


    if (argc > 1) {
        // File execution mode
        FILE* file = fopen(argv[1], "r");
        if (file) {
            fseek(file, 0, SEEK_END);
            long length = ftell(file);
            fseek(file, 0, SEEK_SET);
            char* code = safe_malloc(length + 1);
            if (fread(code, 1, length, file) > 0) {
                code[length] = '\0';
                char* expanded = expand_macros(pp_state, code); // Apply preprocessor macros
                FlowResult* result = execute_block(expanded, main_module_scope, false); // File execution is not in a loop

                if (result->type == FLOW_ERROR) {
                    fprintf(stderr, "Execution Error: %s\n", result->value->value.string_val);
                    exit_code = 1;
                } else if (result->type == FLOW_RETURN) {
                    // If the top-level execution returns a value, use it as exit code
                    if (result->value->type == C_INT) {
                        exit_code = result->value->value.int_val;
                    }
                } else if (result->type == FLOW_BREAK || result->type == FLOW_CONTINUE) {
                    fprintf(stderr, "Execution Error: Cannot use '%s' outside of a loop\n",
                            result->type == FLOW_BREAK ? "break" : "continue");
                    exit_code = 1;
                }
                free(expanded);
                if (result) {
                    if (result->value) free_element(result->value);
                    free(result);
                }
            }
            fclose(file);
            free(code);
        } else {
            fprintf(stderr, "Error: Could not read file '%s'\n", argv[1]);
            exit_code = 1;
        }
    } else {
        // Interactive REPL mode
        run_enhanced_repl();
    }

    // Final cleanup for preprocessor state
    if (pp_state) {
        MacroDef* current = pp_state->macros;
        while (current) {
            MacroDef* next = current->next;
            free(current->name);
            free(current->value);
            free(current);
            current = next;
        }
        if (pp_state->current_condition) {
            PPCondition* cond = pp_state->current_condition;
            while (cond) {
                PPCondition* parent = cond->parent;
                free(cond);
                cond = parent;
            }
        }
        free(pp_state);
    }

    // Free the main module's scope and its placeholder element
    if (main_module_scope) {
        free_scope(main_module_scope);
    }
    if (main_module) {
        if (main_module->name) free(main_module->name);
        free(main_module);
    }

    return exit_code;
}
