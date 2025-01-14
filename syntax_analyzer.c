#include <dirent.h> // For directory operations
#include <string.h> // For string operations
#include "syntax_analyzer.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_TOKENS 1000 // Adjust size based on expected token count


// Global Variables - Store tokens and tracking variables
Token tokens[1000]; // Array of tokens
int currentTokenIndex = 0; // Tracks the current token index
int totalTokens = 0; // Total number of tokens

// Function prototypes for parse tree handling
void writeParseTreeToFile(ParseTreeNode* node, FILE* file);
void freeParseTree(ParseTreeNode* node);
void syntaxError(const char* message);



// Utility Functions - Token management
Token* getNextToken() {
    if (currentTokenIndex < totalTokens) {
        Token* token = &tokens[currentTokenIndex++];
        printf("DEBUG: getNextToken: Retrieved Token[%d]: Type='%s', Value='%s', Line=%d\n",
               currentTokenIndex - 1, token->type, token->value, token->lineNumber);
        return token;
    }
    printf("DEBUG: getNextToken: No more tokens. Current Index=%d, Total Tokens=%d\n",
           currentTokenIndex, totalTokens);
    return NULL;
}

Token* peekToken() {
    if (currentTokenIndex < totalTokens) {
        printf("DEBUG: peekToken: Current Token[%d]: Type='%s', Value='%s', Line=%d\n",
               currentTokenIndex, tokens[currentTokenIndex].type, tokens[currentTokenIndex].value,
               tokens[currentTokenIndex].lineNumber);
        return &tokens[currentTokenIndex];
    }
    printf("DEBUG: peekToken: No more tokens. Current Index=%d, Total Tokens=%d\n",
           currentTokenIndex, totalTokens);
    return NULL;
}






// Function to trim leading and trailing whitespace from a string
void trimWhitespace(char *str) {
    if (!str || !*str) return;

    // Trim leading whitespace
    char *start = str;
    while (isspace((unsigned char)*start)) {
        start++;
    }

    // Move trimmed string to the beginning
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    // Trim trailing whitespace
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
}



void mapToken(Token* token) {
    if (strcmp(token->type, "Right Parenthesis") == 0) {
        strcpy(token->type, "Delimiter");
        strcpy(token->value, ")");
    } else if (strcmp(token->type, "Left Parenthesis") == 0) {
        strcpy(token->type, "Delimiter");
        strcpy(token->value, "(");
    } else if (strcmp(token->type, "Left Curly Brace") == 0) {
        strcpy(token->type, "Delimiter");
        strcpy(token->value, "{");
    } else if (strcmp(token->type, "Right Curly Brace") == 0) {
        strcpy(token->type, "Delimiter");
        strcpy(token->value, "}");
    } else if (strcmp(token->type, "Left Bracket") == 0) {
        strcpy(token->type, "Delimiter");
        strcpy(token->value, "[");
    } else if (strcmp(token->type, "Right Bracket") == 0) {
        strcpy(token->type, "Delimiter");
        strcpy(token->value, "]");
    } else if (strcmp(token->type, "Semicolon") == 0) {
        strcpy(token->type, "Delimiter");
        strcpy(token->value, ";");
    } else if (strcmp(token->type, "Colon") == 0) {
        strcpy(token->type, "Delimiter");
        strcpy(token->value, ":");
    } else if (strcmp(token->type, "Comma") == 0) {
        strcpy(token->type, "Delimiter");
        strcpy(token->value, ",");
    } else if (strcmp(token->type, "Arithmetic Operator (Addition)") == 0) {
        strcpy(token->type, "ArithmeticOperator");
        strcpy(token->value, "+");
    } else if (strcmp(token->type, "Arithmetic Operator (Subtraction)") == 0) {
        strcpy(token->type, "ArithmeticOperator");
        strcpy(token->value, "-");
    } else if (strcmp(token->type, "Arithmetic Operator (Multiplication)") == 0) {
        strcpy(token->type, "ArithmeticOperator");
        strcpy(token->value, "*");
    } else if (strcmp(token->type, "Arithmetic Operator (Division)") == 0) {
        strcpy(token->type, "ArithmeticOperator");
        strcpy(token->value, "/");
    } else if (strcmp(token->type, "Arithmetic Operator (Modulo)") == 0) {
        strcpy(token->type, "ArithmeticOperator");
        strcpy(token->value, "%");
    } else if (strcmp(token->type, "Arithmetic Operator (Exponentiation)") == 0) {
        strcpy(token->type, "ArithmeticOperator");
        strcpy(token->value, "^");
    } else if (strcmp(token->type, "Logical Operator (NOT)") == 0) {
        strcpy(token->type, "LogicalOperator");
        strcpy(token->value, "!");
    } else if (strcmp(token->type, "Logical Operator (OR)") == 0) {
        strcpy(token->type, "LogicalOperator");
        strcpy(token->value, "||");
    } else if (strcmp(token->type, "Logical Operator (AND)") == 0) {
        strcpy(token->type, "LogicalOperator");
        strcpy(token->value, "&&");
    } else if (strcmp(token->type, "Relational Operator (Equal To)") == 0) {
        strcpy(token->type, "RelationalOperator");
        strcpy(token->value, "==");
    } else if (strcmp(token->type, "Relational Operator (Not Equal To)") == 0) {
        strcpy(token->type, "RelationalOperator");
        strcpy(token->value, "!=");
    } else if (strcmp(token->type, "Relational Operator (Greater Than)") == 0) {
        strcpy(token->type, "RelationalOperator");
        strcpy(token->value, ">");
    } else if (strcmp(token->type, "Relational Operator (Less Than)") == 0) {
        strcpy(token->type, "RelationalOperator");
        strcpy(token->value, "<");
    } else if (strcmp(token->type, "Relational Operator (Greater Than or Equal To)") == 0) {
        strcpy(token->type, "RelationalOperator");
        strcpy(token->value, ">=");
    } else if (strcmp(token->type, "Relational Operator (Less Than or Equal To)") == 0) {
        strcpy(token->type, "RelationalOperator");
        strcpy(token->value, "<=");
    } else if (strcmp(token->type, "Integer Literal") == 0) {
        strcpy(token->type, "INT_LITERAL");
    } else if (strcmp(token->type, "Float Literal") == 0) {
        strcpy(token->type, "FLOAT_LITERAL");
    } else if (strcmp(token->type, "Character Literal") == 0) {
        strcpy(token->type, "CHAR_LITERAL");
    } else if (strcmp(token->type, "String Literal") == 0) {
        strcpy(token->type, "STRING_LITERAL");
    } else if (strcmp(token->type, "Unary Operator (Increment)") == 0) {
        strcpy(token->type, "UnaryOperator");
        strcpy(token->value, "++");
    } else if (strcmp(token->type, "Unary Operator (Decrement)") == 0) {
        strcpy(token->type, "UnaryOperator");
        strcpy(token->value, "--");
    } else if (strcmp(token->type, "Assignment Operator") == 0) {
        strcpy(token->type, "AssignmentOperator");
        strcpy(token->value, "=");
    } else if (strcmp(token->type, "Reserved Word") == 0) {
        strcpy(token->type, "ReservedWord");
    } else if (strcmp(token->type, "Noise Word") == 0) {
        strcpy(token->type, "NoiseWord");
    } else if (strstr(token->type, "Comment") != NULL) {
        strcpy(token->type, "Comment");
    }
}



// Function to load tokens from a file
int loadTokensFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return 0;
    }

    char line[256];
    int tokenCount = 0;

    printf("\nLoading tokens from %s...\n", filename);

    while (fgets(line, sizeof(line), file)) {
        // Debug: Print each line read
        printf("DEBUG: Processing line: %s", line);

        // Trim whitespace from the line
        trimWhitespace(line);

        // Skip empty lines
        if (strlen(line) == 0) {
            printf("DEBUG: Skipping empty line.\n");
            continue;
        }

        // Parse the line into token fields (comma-separated)
        Token token;
        char type[50] = {0}, value[50] = {0};
        int lineNumber = 0;

        // Extract fields using comma as a delimiter
        char *firstComma = strchr(line, ',');
        char *lastComma = strrchr(line, ',');

        if (!firstComma || !lastComma || firstComma == lastComma) {
            printf("DEBUG: Invalid or unparsed line: %s\n", line);
            continue; // Skip invalid lines
        }

        // Extract token type
        *firstComma = '\0';
        strncpy(type, line, sizeof(type) - 1);
        type[sizeof(type) - 1] = '\0';

        // Extract token value
        *lastComma = '\0';
        strncpy(value, firstComma + 1, sizeof(value) - 1);
        value[sizeof(value) - 1] = '\0';

        // Extract line number
        lineNumber = atoi(lastComma + 1);

        // Populate the token
        strncpy(token.type, type, sizeof(token.type) - 1);
        token.type[sizeof(token.type) - 1] = '\0';
        strncpy(token.value, value, sizeof(token.value) - 1);
        token.value[sizeof(token.value) - 1] = '\0';
        token.lineNumber = lineNumber;

        // Debug: Print the extracted token fields
        printf("DEBUG: Extracted Token: Type='%s', Value='%s', Line=%d\n", token.type, token.value, token.lineNumber);

        // Add the token to the list
        if (tokenCount < MAX_TOKENS) {
            tokens[tokenCount++] = token;
        } else {
            printf("Error: Token limit exceeded. Increase MAX_TOKENS.\n");
            break;
        }
    }

    fclose(file);

    // Update totalTokens and print all loaded tokens
    totalTokens = tokenCount;
    printf("DEBUG: Total Tokens Loaded: %d\n", totalTokens);
    for (int i = 0; i < totalTokens; i++) {
        printf("DEBUG: Token[%d]: Type='%s', Value='%s', Line=%d\n",
               i, tokens[i].type, tokens[i].value, tokens[i].lineNumber);
    }

    printf("DEBUG: Completed loading tokens. Total loaded: %d from %s.\n", tokenCount, filename);
    return totalTokens;
}













void syntaxError(const char* message) {
    Token* token = peekToken();
    if (token) {
        printf("Syntax Error at line %d: %s\n", token->lineNumber, message);
    } else {
        printf("Syntax Error: %s\n", message);
    }
    exit(EXIT_FAILURE);
}








// Recursive Descent Parsing Functions
ParseTreeNode* parseProgram() {
    printf("DEBUG: Starting parseProgram...\n");

    // Peek at the first token
    Token* token = peekToken();
    if (!token) {
        printf("DEBUG: No tokens available at the start of parseProgram.\n");
        return NULL;
    }

    printf("DEBUG: First token in program: Type='%s', Value='%s', Line=%d\n",
           token->type, token->value, token->lineNumber);

    // Check for 'void' keyword
    if (strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "void") != 0) {
        printf("Syntax Error: Expected 'void' keyword but got Type='%s', Value='%s', Line=%d\n",
               token->type, token->value, token->lineNumber);
        return NULL;
    }

    // Consume 'void' keyword
    getNextToken();

    printf("DEBUG: Successfully matched 'void' keyword.\n");

    // Continue parsing the main function
    ParseTreeNode* mainFunctionNode = parseMainFunction();
    if (!mainFunctionNode) {
        printf("DEBUG: Failed to parse main function.\n");
        return NULL;
    }

    printf("DEBUG: Completed parseProgram.\n");
    return mainFunctionNode;
}














ParseTreeNode* matchToken(const char* expectedType, const char* expectedValue) {
    // Debug: Check the token before fetching
    printf("DEBUG: Checking token before fetching: Type='%s', Value='%s', Line=%d\n",
           peekToken() ? peekToken()->type : "NULL",
           peekToken() ? peekToken()->value : "NULL",
           peekToken() ? peekToken()->lineNumber : -1);

    // Fetch the next token
    Token* token = getNextToken();
    if (!token) {
        printf("DEBUG: No more tokens available when trying to match. Expected Type='%s', Value='%s'.\n",
               expectedType, expectedValue);
    } else {
        printf("DEBUG: Fetching token for match. Got Type='%s', Value='%s', Line=%d. Expected Type='%s', Value='%s'.\n",
               token->type, token->value, token->lineNumber, expectedType, expectedValue);
    }

    // Check if the token matches the expected type and value
    if (!token || strcmp(token->type, expectedType) != 0 || strcmp(token->value, expectedValue) != 0) {
        char errorMessage[200];
        snprintf(errorMessage, sizeof(errorMessage),
                 "Syntax Error: Expected token Type='%s', Value='%s', but got Type='%s', Value='%s' at line %d.",
                 expectedType, expectedValue,
                 token ? token->type : "EOF",
                 token ? token->value : "EOF",
                 token ? token->lineNumber : -1);
        printf("DEBUG: Token mismatch error: %s\n", errorMessage);

        // Debug remaining tokens on mismatch
        printf("DEBUG: Remaining Tokens After Mismatch:\n");
        for (int i = currentTokenIndex; i < totalTokens; i++) {
            printf("  Token %d: Type='%s', Value='%s', Line=%d\n",
                   i + 1, tokens[i].type, tokens[i].value, tokens[i].lineNumber);
        }

        syntaxError(errorMessage);
        return NULL; // Return NULL on mismatch
    }

    // Debug: Successful match
    printf("DEBUG: Matched token successfully: Type='%s', Value='%s', Line=%d\n",
           token->type, token->value, token->lineNumber);

    // Create a parse tree node for the matched token
    ParseTreeNode* node = createParseTreeNode(token->type, token->value);

    // Debug: ParseTreeNode creation
    printf("DEBUG: Created ParseTreeNode: Type='%s', Value='%s'\n", token->type, token->value);

    // Debug: Verify remaining tokens
    printf("DEBUG: Remaining Tokens After Match:\n");
    for (int i = currentTokenIndex; i < totalTokens; i++) {
        printf("  Token %d: Type='%s', Value='%s', Line=%d\n",
               i + 1, tokens[i].type, tokens[i].value, tokens[i].lineNumber);
    }

    return node;
}









ParseTreeNode* parseBlock() {
    printf("Parsing Block...\n");

    ParseTreeNode* blockNode = createParseTreeNode("Block", "");

    // Match the opening curly brace "{"
    Token* token = peekToken();
    printf("DEBUG: Checking for '{'. Current Token: Type='%s', Value='%s', Line=%d\n",
           token ? token->type : "NULL",
           token ? token->value : "NULL",
           token ? token->lineNumber : -1);

    if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "{") == 0) {
        printf("DEBUG: Matched '{'.\n");
        addChild(blockNode, matchToken("Delimiter", "{"));
    } else {
        syntaxError("Expected '{' at the start of block.");
        return blockNode; // Return with partial parse tree
    }

    // Parse statements within the block
    printf("DEBUG: Parsing statements inside block...\n");
    while ((token = peekToken()) &&
           !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "}"))) {
        printf("DEBUG: Parsing statement. Current Token: Type='%s', Value='%s', Line=%d\n",
               token->type, token->value, token->lineNumber);

        ParseTreeNode* statementNode = parseStatement();
        if (statementNode) {
            addChild(blockNode, statementNode);
        } else {
            syntaxError("Error parsing statement inside block.");
            break; // Exit the loop on error
        }
    }

    // Match the closing curly brace "}"
    token = peekToken();
    printf("DEBUG: Checking for '}'. Current Token: Type='%s', Value='%s', Line=%d\n",
           token ? token->type : "NULL",
           token ? token->value : "NULL",
           token ? token->lineNumber : -1);

    if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "}") == 0) {
        printf("DEBUG: Matched '}'.\n");
        addChild(blockNode, matchToken("Delimiter", "}"));
    } else {
        syntaxError("Expected '}' at the end of block.");
    }

    printf("DEBUG: Completed parsing Block.\n");
    return blockNode;
}















ParseTreeNode* parseStatementList() {
    printf("Parsing Statement List...\n");

    // Create a node for the statement list
    ParseTreeNode* statementListNode = createParseTreeNode("StatementList", "");

    Token* token = peekToken();
    while (token && 
           !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "}")) && 
           !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ";"))) {
        // Debug: Current token being parsed
        printf("DEBUG: Parsing statement in list. Current Token: Type='%s', Value='%s', Line=%d\n",
               token->type, token->value, token->lineNumber);

        // Parse each statement and add it as a child
        ParseTreeNode* statementNode = parseStatement();
        if (statementNode) {
            addChild(statementListNode, statementNode);
        } else {
            syntaxError("Error parsing statement in statement list.");
            break; // Exit the loop on error
        }

        token = peekToken(); // Update the token to check for more statements
    }

    printf("DEBUG: Completed parsing Statement List.\n");
    return statementListNode;
}








ParseTreeNode* parseDeclarationStatement() {
    printf("Parsing Declaration Statement...\n");

    ParseTreeNode* declarationNode = createParseTreeNode("DeclarationStatement", "");

    // Match a type specifier to begin the variable declaration
    while (peekToken() &&
           (strcmp(peekToken()->type, "Keyword") == 0) && 
           (strcmp(peekToken()->value, "int") == 0 || 
            strcmp(peekToken()->value, "float") == 0 || 
            strcmp(peekToken()->value, "char") == 0 || 
            strcmp(peekToken()->value, "bool") == 0 || 
            strcmp(peekToken()->value, "string") == 0)) {
        addChild(declarationNode, parseVariableDeclaration());
    }

    return declarationNode;
}







ParseTreeNode* parseExpression() {
    printf("Parsing Expression...\n");

    ParseTreeNode* exprNode = createParseTreeNode("Expression","");

    addChild(exprNode, parseLogicalOrExpr());

    return exprNode;
}



ParseTreeNode* parseTypeSpecifier() {
    printf("Parsing Type Specifier...\n");

    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Keyword") != 0 ||
        !(strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
          strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
          strcmp(token->value, "string") == 0)) {
        syntaxError("Expected a type specifier (int, float, char, bool, or string).");
    }

    // Debug: Log the matched type specifier
    printf("DEBUG: Matched Type Specifier: %s\n", token->value);

    // Create a parse tree node for the type specifier
    return createParseTreeNode("TypeSpecifier", token->value);
}


ParseTreeNode* parseVariableDeclaration() {
    printf("Parsing Variable Declaration...\n");

    // Create a node for the variable declaration
    ParseTreeNode* variableDeclarationNode = createParseTreeNode("VariableDeclaration", "");

    // Parse the type specifier and add it as a child
    addChild(variableDeclarationNode, parseTypeSpecifier());

    Token* token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier in variable declaration.");
    }

    // Add the identifier as a child
    printf("DEBUG: Parsed identifier in variable declaration: %s\n", token->value);
    addChild(variableDeclarationNode, createParseTreeNode("Identifier", token->value));

    // Check for optional assignment
    token = peekToken();
    if (token && strcmp(token->type, "AssignmentOperator") == 0) {
        getNextToken(); // Consume '='
        printf("DEBUG: Assignment operator detected.\n");
        // Parse the initializer and add it as a child
        addChild(variableDeclarationNode, parseExpression());
    }

    // Handle multiple declarations separated by commas
    while (peekToken() && strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, ",") == 0) {
        getNextToken(); // Consume ','

        token = getNextToken();
        if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
            syntaxError("Expected an identifier after ',' in variable declaration.");
        }

        // Add the identifier as a child
        printf("DEBUG: Parsed identifier after ',' in variable declaration: %s\n", token->value);
        addChild(variableDeclarationNode, createParseTreeNode("Identifier", token->value));

        // Check for optional assignment
        token = peekToken();
        if (token && strcmp(token->type, "AssignmentOperator") == 0) {
            getNextToken(); // Consume '='
            printf("DEBUG: Assignment operator detected after ','.\n");
            // Parse the initializer and add it as a child
            addChild(variableDeclarationNode, parseExpression());
        }
    }

    // Ensure the statement ends with a semicolon
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        syntaxError("Expected ';' at the end of variable declaration.");
    }

    printf("DEBUG: Completed parsing variable declaration.\n");
    return variableDeclarationNode;
}









ParseTreeNode* parseStatement() {
    printf("Parsing Statement...\n");
    Token* token = peekToken();

    if (!token) {
        syntaxError("Unexpected end of input while parsing statement.");
    }

    // Debug: Log the current token
    printf("DEBUG: Current Token in parseStatement: Type='%s', Value='%s', Line=%d\n",
           token->type, token->value, token->lineNumber);

    // Create a parse tree node for the statement
    ParseTreeNode* statementNode = createParseTreeNode("Statement", "");

    // Match based on the token's value or type
    if (strcmp(token->type, "Identifier") == 0) {
        addChild(statementNode, parseAssignmentStatement());
    } else if (strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "input") == 0) {
        addChild(statementNode, parseInputStatement());
    } else if (strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "printf") == 0) {
        addChild(statementNode, parseOutputStatement());
    } else if (strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "if") == 0) {
        addChild(statementNode, parseConditionalStatement());
    } else if (strcmp(token->type, "Keyword") == 0 && 
               (strcmp(token->value, "for") == 0 || strcmp(token->value, "while") == 0 || strcmp(token->value, "do") == 0)) {
        addChild(statementNode, parseIterativeStatement());
    } else if (strcmp(token->type, "Keyword") == 0 && 
               (strcmp(token->value, "return") == 0 || strcmp(token->value, "break") == 0 || strcmp(token->value, "continue") == 0)) {
        addChild(statementNode, parseJumpStatement());
    } else {
        // Default to parsing an expression statement
        addChild(statementNode, parseExpressionStatement());
    }

    return statementNode;
}






ParseTreeNode* parseLogicalOrExpr() {
    printf("Parsing Logical OR Expression...\n");

    // Create a parse tree node for the Logical OR expression
    ParseTreeNode* logicalOrNode = createParseTreeNode("LogicalOrExpr", "");

    // Parse the left-hand side as a Logical AND expression
    addChild(logicalOrNode, parseLogicalAndExpr());

    // Check for multiple '||' operators
    while (peekToken() && strcmp(peekToken()->type, "LogicalOperator") == 0 && strcmp(peekToken()->value, "||") == 0) {
        Token* operatorToken = getNextToken(); // Consume the '||' operator
        printf("DEBUG: Logical OR operator detected: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(logicalOrNode, createParseTreeNode("LogicalOperator", operatorToken->value));

        // Parse the right-hand side as another Logical AND expression
        addChild(logicalOrNode, parseLogicalAndExpr());
    }

    return logicalOrNode;
}









ParseTreeNode* parseLogicalAndExpr() {
    printf("Parsing Logical AND Expression...\n");

    // Create a parse tree node for the Logical AND expression
    ParseTreeNode* logicalAndNode = createParseTreeNode("LogicalAndExpr", "");

    // Parse the left-hand side as an Equality expression
    addChild(logicalAndNode, parseEqualityExpr());

    // Check for multiple '&&' operators
    while (peekToken() && strcmp(peekToken()->type, "LogicalOperator") == 0 && strcmp(peekToken()->value, "&&") == 0) {
        Token* operatorToken = getNextToken(); // Consume the '&&' operator
        printf("DEBUG: Logical AND operator detected: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(logicalAndNode, createParseTreeNode("LogicalOperator", operatorToken->value));

        // Parse the right-hand side as another Equality expression
        addChild(logicalAndNode, parseEqualityExpr());
    }

    return logicalAndNode;
}



ParseTreeNode* parseRelationalExpr() {
    printf("Parsing Relational Expression...\n");

    // Create a parse tree node for the Relational expression
    ParseTreeNode* relationalNode = createParseTreeNode("RelationalExpr", "");

    // Parse the first operand (Additive expression)
    addChild(relationalNode, parseAdditiveExpr());

    // Check for relational operators (<, >, <=, >=)
    while (peekToken() && strcmp(peekToken()->type, "RelationalOperator") == 0 &&
           (strcmp(peekToken()->value, "<") == 0 || strcmp(peekToken()->value, ">") == 0 ||
            strcmp(peekToken()->value, "<=") == 0 || strcmp(peekToken()->value, ">=") == 0)) {
        // Consume the operator
        Token* operatorToken = getNextToken();
        printf("DEBUG: Matched Relational Operator: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(relationalNode, createParseTreeNode("RelationalOperator", operatorToken->value));

        // Parse the next operand (Additive expression)
        addChild(relationalNode, parseAdditiveExpr());
    }

    return relationalNode;
}



ParseTreeNode* parseEqualityExpr() {
    printf("Parsing Equality Expression...\n");

    // Create a parse tree node for the Equality expression
    ParseTreeNode* equalityNode = createParseTreeNode("EqualityExpr", "");

    // Parse the first operand (Relational expression)
    addChild(equalityNode, parseRelationalExpr());

    // Check for equality operators (== or !=)
    while (peekToken() && strcmp(peekToken()->type, "RelationalOperator") == 0 &&
           (strcmp(peekToken()->value, "==") == 0 || strcmp(peekToken()->value, "!=") == 0)) {
        // Consume the operator
        Token* operatorToken = getNextToken();
        printf("DEBUG: Matched Equality Operator: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(equalityNode, createParseTreeNode("RelationalOperator", operatorToken->value));

        // Parse the next operand (Relational expression)
        addChild(equalityNode, parseRelationalExpr());
    }

    return equalityNode;
}



ParseTreeNode* parseAdditiveExpr() {
    printf("Parsing Additive Expression...\n");

    // Create a parse tree node for the Additive expression
    ParseTreeNode* additiveNode = createParseTreeNode("AdditiveExpr", "");

    // Parse the first operand (Multiplicative expression)
    addChild(additiveNode, parseMultiplicativeExpr());

    // Check for additive operators ('+', '-')
    while (peekToken() && strcmp(peekToken()->type, "ArithmeticOperator") == 0 &&
           (strcmp(peekToken()->value, "+") == 0 || strcmp(peekToken()->value, "-") == 0)) {
        // Consume the operator
        Token* operatorToken = getNextToken();
        printf("DEBUG: Matched Additive Operator: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(additiveNode, createParseTreeNode("ArithmeticOperator", operatorToken->value));

        // Parse the next operand (Multiplicative expression)
        addChild(additiveNode, parseMultiplicativeExpr());
    }

    return additiveNode;
}




ParseTreeNode* parseMultiplicativeExpr() {
    printf("Parsing Multiplicative Expression...\n");

    // Create a parse tree node for the Multiplicative expression
    ParseTreeNode* multiplicativeNode = createParseTreeNode("MultiplicativeExpr", "");

    // Parse the first operand (Exponential expression)
    addChild(multiplicativeNode, parseExponentialExpr());

    // Check for multiplicative operators ('*', '/', '%', '//')
    while (peekToken() && strcmp(peekToken()->type, "ArithmeticOperator") == 0 &&
           (strcmp(peekToken()->value, "*") == 0 || 
            strcmp(peekToken()->value, "/") == 0 || 
            strcmp(peekToken()->value, "%") == 0 || 
            strcmp(peekToken()->value, "//") == 0)) {
        // Consume the operator
        Token* operatorToken = getNextToken();
        printf("DEBUG: Matched Multiplicative Operator: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(multiplicativeNode, createParseTreeNode("ArithmeticOperator", operatorToken->value));

        // Parse the next operand (Exponential expression)
        addChild(multiplicativeNode, parseExponentialExpr());
    }

    return multiplicativeNode;
}








ParseTreeNode* parseExponentialExpr() {
    printf("Parsing Exponential Expression...\n");

    // Create a parse tree node for the Exponential expression
    ParseTreeNode* exponentialNode = createParseTreeNode("ExponentialExpr", "");

    // Parse the first operand (Unary expression)
    addChild(exponentialNode, parseUnaryExpr());

    // Check for the exponential operator ('^')
    while (peekToken() && strcmp(peekToken()->type, "ArithmeticOperator") == 0 &&
           strcmp(peekToken()->value, "^") == 0) {
        // Consume the operator
        Token* operatorToken = getNextToken();
        printf("DEBUG: Matched Exponential Operator: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(exponentialNode, createParseTreeNode("ArithmeticOperator", operatorToken->value));

        // Parse the next operand (Unary expression)
        addChild(exponentialNode, parseUnaryExpr());
    }

    return exponentialNode;
}








ParseTreeNode* parseUnaryExpr() {
    printf("Parsing Unary Expression...\n");

    // Create a parse tree node for the Unary Expression
    ParseTreeNode* unaryNode = createParseTreeNode("UnaryExpr", "");

    Token* token = peekToken();

    if (!token) {
        syntaxError("Unexpected end of input while parsing unary expression");
        return NULL; // Return NULL in case of an error
    }

    // Check for unary operators (Logical NOT `!`, Subtraction `-`, Increment `++`, Decrement `--`)
    if ((strcmp(token->type, "LogicalOperator") == 0 && strcmp(token->value, "!") == 0) ||
        (strcmp(token->type, "ArithmeticOperator") == 0 && strcmp(token->value, "-") == 0) ||
        (strcmp(token->type, "UnaryOperator") == 0 && 
         (strcmp(token->value, "++") == 0 || strcmp(token->value, "--") == 0))) {
        token = getNextToken(); // Consume the unary operator
        printf("DEBUG: Matched Unary Operator: %s\n", token->value);

        // Add the operator as a child node
        addChild(unaryNode, createParseTreeNode("UnaryOperator", token->value));

        // Parse the operand (recursively handle unary expressions)
        addChild(unaryNode, parseUnaryExpr());
    } else {
        // If no unary operator, parse as a postfix expression
        addChild(unaryNode, parsePostfixExpr());
    }

    return unaryNode;
}







ParseTreeNode* parsePostfixExpr() {
    printf("Parsing Postfix Expression...\n");

    // Create a parse tree node for the Postfix Expression
    ParseTreeNode* postfixNode = createParseTreeNode("PostfixExpr", "");

    // Parse the primary expression and add it as a child
    addChild(postfixNode, parsePrimaryExpr());

    // Handle optional postfix operations: function calls, array accesses, or unary operators
    Token* token = peekToken();
    while (token && 
           (strcmp(token->type, "Delimiter") == 0 && 
           (strcmp(token->value, "(") == 0 || strcmp(token->value, "[") == 0)) || 
           (strcmp(token->type, "UnaryOperator") == 0)) {
        if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "(") == 0) {
            printf("Parsing Function Call...\n");

            // Create a node for the function call
            ParseTreeNode* funcCallNode = createParseTreeNode("FunctionCall", "");

            getNextToken(); // Consume '('

            // Parse the argument list and add it as a child
            addChild(funcCallNode, parseArgumentList());

            token = getNextToken();
            if (!token || !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ")") == 0)) {
                syntaxError("Expected ')' after function arguments");
            }

            // Add the function call node as a child of the postfix node
            addChild(postfixNode, funcCallNode);

        } else if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "[") == 0) {
            printf("Parsing Array Access...\n");

            // Create a node for the array access
            ParseTreeNode* arrayAccessNode = createParseTreeNode("ArrayAccess", "");

            getNextToken(); // Consume '['

            // Parse the index expression and add it as a child
            addChild(arrayAccessNode, parseExpression());

            token = getNextToken();
            if (!token || !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "]") == 0)) {
                syntaxError("Expected ']' after array index expression");
            }

            // Add the array access node as a child of the postfix node
            addChild(postfixNode, arrayAccessNode);

        } else if (strcmp(token->type, "UnaryOperator") == 0) {
            printf("Parsing Unary Operator...\n");

            // Create a node for the unary operator
            ParseTreeNode* unaryOpNode = createParseTreeNode("UnaryOperator", token->value);

            getNextToken(); // Consume the unary operator

            // Add the unary operator node as a child of the postfix node
            addChild(postfixNode, unaryOpNode);
        }

        token = peekToken(); // Check for more postfix operations
    }

    return postfixNode;
}







ParseTreeNode* parseLiteral() {
    printf("Parsing Literal...\n");

    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing literal.");
    }

    // Create a node for the literal
    ParseTreeNode* literalNode = createParseTreeNode("Literal", "");

    if (strcmp(token->type, "INT_LITERAL") == 0 ||
        strcmp(token->type, "FLOAT_LITERAL") == 0 ||
        strcmp(token->type, "CHAR_LITERAL") == 0 ||
        strcmp(token->type, "STRING_LITERAL") == 0) {
        printf("Matched Literal: Type='%s', Value='%s'\n", token->type, token->value);

        // Add the literal value to the node
        setNodeValue(literalNode, token->value);

        getNextToken(); // Consume the literal token
    } else if (strcmp(token->type, "BOOLEAN_LITERAL") == 0) {
        printf("Matched Boolean Literal: Value='%s'\n", token->value);

        // Add the boolean value to the node
        setNodeValue(literalNode, token->value);

        getNextToken(); // Consume the boolean literal
    } else if (strcmp(token->type, "NULL_LITERAL") == 0) {
        printf("Matched Null Literal: Value='%s'\n", token->value);

        // Add the null value to the node
        setNodeValue(literalNode, token->value);

        getNextToken(); // Consume the null literal
    } else {
        syntaxError("Expected a literal (integer, float, char, string, boolean, or null).");
    }

    return literalNode;
}






ParseTreeNode* parseBoolLiteral() {
    printf("Parsing Boolean Literal...\n");

    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing boolean literal.");
    }

    // Create a node for the boolean literal
    ParseTreeNode* boolLiteralNode = createParseTreeNode("BoolLiteral", "");

    // Match boolean literals ("true" or "false")
    if (strcmp(token->type, "BOOLEAN_LITERAL") == 0) {
        printf("Matched Boolean Literal: Value='%s'\n", token->value);

        // Add the boolean value to the node
        setNodeValue(boolLiteralNode, token->value);

        getNextToken(); // Consume the boolean literal token
    } else {
        syntaxError("Expected a boolean literal ('true' or 'false').");
    }

    return boolLiteralNode;
}








ParseTreeNode* parsePrimaryExpr() {
    printf("Parsing Primary Expression...\n");

    Token* token = getNextToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing a primary expression");
    }

    // Create a node for the primary expression
    ParseTreeNode* primaryNode = createParseTreeNode("PrimaryExpr", "");

    if (strcmp(token->type, "INT_LITERAL") == 0 || 
        strcmp(token->type, "FLOAT_LITERAL") == 0 || 
        strcmp(token->type, "CHAR_LITERAL") == 0 || 
        strcmp(token->type, "STRING_LITERAL") == 0 || 
        strcmp(token->type, "BOOLEAN_LITERAL") == 0) {
        // It's a literal
        printf("Literal detected: Type='%s', Value='%s'\n", token->type, token->value);
        setNodeValue(primaryNode, token->value);
    } else if (strcmp(token->type, "IDENTIFIER") == 0) {
        // It's an identifier
        printf("Identifier detected: %s\n", token->value);
        setNodeValue(primaryNode, token->value);
    } else if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "(") == 0) {
        // It's a grouped expression
        printf("Detected '(' indicating a grouped expression.\n");
        addChild(primaryNode, parseExpression()); // Parse the inner expression

        token = getNextToken();
        if (!token || !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ")") == 0)) {
            syntaxError("Expected ')' to close the grouped expression");
        }
        printf("Detected closing ')' for grouped expression.\n");
    } else {
        syntaxError("Expected a literal, identifier, or grouped expression");
    }

    return primaryNode;
}







ParseTreeNode* parseIfStatement() {
    printf("Parsing If Statement...\n");

    // Create a node for the if statement
    ParseTreeNode* ifNode = createParseTreeNode("IfStatement", "");

    // Match the 'if' keyword
    Token* token = getNextToken();
    if (!token || !(strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "if") == 0)) {
        syntaxError("Expected 'if' keyword.");
    }
    printf("Matched 'if' keyword.\n");
    addChild(ifNode, createParseTreeNode("Keyword", "if"));

    // Match the opening parenthesis '('
    token = getNextToken();
    if (!token || !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "(") == 0)) {
        syntaxError("Expected '(' after 'if'.");
    }
    printf("Matched '(' after 'if'.\n");
    addChild(ifNode, createParseTreeNode("Delimiter", "("));

    // Parse the condition inside the parentheses
    addChild(ifNode, parseExpression());

    // Match the closing parenthesis ')'
    token = getNextToken();
    if (!token || !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ")") == 0)) {
        syntaxError("Expected ')' after condition.");
    }
    printf("Matched ')' after condition.\n");
    addChild(ifNode, createParseTreeNode("Delimiter", ")"));

    // Parse the statement block
    addChild(ifNode, parseStatement());

    // Check for optional 'else' or 'else if'
    token = peekToken();
    if (token && strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "else") == 0) {
        getNextToken(); // Consume 'else'
        printf("Matched 'else' keyword.\n");
        ParseTreeNode* elseNode = createParseTreeNode("ElseClause", "");

        token = peekToken();
        if (token && strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "if") == 0) {
            // Parse an 'else if' statement
            printf("Parsing 'else if' clause.\n");
            addChild(elseNode, parseIfStatement());
        } else {
            // Parse the 'else' statement block
            printf("Parsing 'else' clause.\n");
            addChild(elseNode, parseStatement());
        }

        addChild(ifNode, elseNode);
    }

    return ifNode;
}







ParseTreeNode* parseCaseStatement() {
    printf("Parsing Case Statement...\n");

    // Create a node for the case statement
    ParseTreeNode* caseNode = createParseTreeNode("CaseStatement", "");

    // Match the 'case' keyword
    Token* token = getNextToken();
    if (!token || !(strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "case") == 0)) {
        syntaxError("Expected 'case' keyword.");
    }
    printf("Matched 'case' keyword.\n");
    addChild(caseNode, createParseTreeNode("Keyword", "case"));

    // Match the case value (INT_LITERAL or CHAR_LITERAL)
    token = getNextToken();
    if (!token || !(strcmp(token->type, "INT_LITERAL") == 0 || strcmp(token->type, "CHAR_LITERAL") == 0)) {
        syntaxError("Expected an integer or character literal after 'case'.");
    }
    printf("Matched case value: %s\n", token->value);
    addChild(caseNode, createParseTreeNode("Literal", token->value));

    // Match the colon ':'
    token = getNextToken();
    if (!token || !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ":") == 0)) {
        syntaxError("Expected ':' after case value.");
    }
    printf("Matched ':' after case value.\n");
    addChild(caseNode, createParseTreeNode("Symbol", ":"));

    // Parse the statements inside the case
    while (peekToken() && !(strcmp(peekToken()->type, "Keyword") == 0 && strcmp(peekToken()->value, "break") == 0)) {
        printf("Parsing statement inside case block.\n");
        addChild(caseNode, parseStatement());
    }

    // Match the 'break' keyword
    token = getNextToken();
    if (!token || !(strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "break") == 0)) {
        syntaxError("Expected 'break' in case statement.");
    }
    printf("Matched 'break' keyword.\n");
    addChild(caseNode, createParseTreeNode("Keyword", "break"));

    // Match the semicolon ';'
    token = getNextToken();
    if (!token || !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ";") == 0)) {
        syntaxError("Expected ';' after 'break'.");
    }
    printf("Matched ';' after 'break'.\n");
    addChild(caseNode, createParseTreeNode("Symbol", ";"));

    return caseNode;
}






ParseTreeNode* parseDefaultCase() {
    printf("Parsing Default Case...\n");

    // Create a node for the default case
    ParseTreeNode* defaultNode = createParseTreeNode("DefaultCase", "");

    // Match the 'default' keyword
    Token* token = getNextToken();
    if (!token || !(strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "default") == 0)) {
        syntaxError("Expected 'default' keyword.");
    }
    printf("Matched 'default' keyword.\n");
    addChild(defaultNode, createParseTreeNode("Keyword", "default"));

    // Match the colon ':'
    token = getNextToken();
    if (!token || !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ":") == 0)) {
        syntaxError("Expected ':' after 'default'.");
    }
    printf("Matched ':' after 'default'.\n");
    addChild(defaultNode, createParseTreeNode("Symbol", ":"));

    // Parse the statements inside the default case
    while (peekToken() && !(strcmp(peekToken()->type, "Keyword") == 0 && strcmp(peekToken()->value, "break") == 0)) {
        printf("Parsing statement inside default block.\n");
        addChild(defaultNode, parseStatement());
    }

    // Match the 'break' keyword
    token = getNextToken();
    if (!token || !(strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "break") == 0)) {
        syntaxError("Expected 'break' in default case.");
    }
    printf("Matched 'break' keyword.\n");
    addChild(defaultNode, createParseTreeNode("Keyword", "break"));

    // Match the semicolon ';'
    token = getNextToken();
    if (!token || !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ";") == 0)) {
        syntaxError("Expected ';' after 'break'.");
    }
    printf("Matched ';' after 'break'.\n");
    addChild(defaultNode, createParseTreeNode("Symbol", ";"));

    return defaultNode;
}





ParseTreeNode* parseForInit() {
    printf("Parsing For Loop Initialization...\n");

    // Create a node for the for loop initialization
    ParseTreeNode* forInitNode = createParseTreeNode("ForInit", "");

    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input in 'for' loop initialization.");
    }

    if (strcmp(token->type, "Keyword") == 0 &&
        (strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
         strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
         strcmp(token->value, "string") == 0)) {
        // Variable declaration
        printf("Detected variable declaration in 'for' loop initialization.\n");
        addChild(forInitNode, parseVariableDeclaration());
    } else if (strcmp(token->type, "Identifier") == 0) {
        // Assignment statement
        printf("Detected assignment statement in 'for' loop initialization.\n");
        addChild(forInitNode, parseAssignmentStatement());
    } else {
        syntaxError("Invalid initialization in 'for' loop. Expected variable declaration or assignment statement.");
    }

    return forInitNode;
}





ParseTreeNode* parseStatementBlock() {
    printf("Parsing Statement Block...\n");

    // Create a node for the statement block
    ParseTreeNode* statementBlockNode = createParseTreeNode("StatementBlock", "");

    Token* token = peekToken();
    if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "{") == 0) {
        // Parse a block enclosed in curly braces
        printf("Detected compound statement (block) in statement block.\n");
        addChild(statementBlockNode, parseBlock());
    } else {
        // Parse a single statement
        printf("Detected single statement in statement block.\n");
        addChild(statementBlockNode, parseStatement());
    }

    return statementBlockNode;
}







ParseTreeNode* parseForLoop() {
    printf("Parsing For Loop...\n");

    // Create a parse tree node for the for loop
    ParseTreeNode* forNode = createParseTreeNode("ForLoop", "");

    // Match the 'for' keyword
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "for") != 0) {
        syntaxError("Expected 'for' keyword.");
    }
    addChild(forNode, createParseTreeNode("Keyword", token->value));

    // Match the opening parenthesis '('
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        syntaxError("Expected '(' after 'for'.");
    }
    addChild(forNode, createParseTreeNode("Delimiter", token->value));

    // Parse the optional initialization statement
    if (peekToken() && !(strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, ";") == 0)) {
        addChild(forNode, parseForInit());
    }

    // Match the first semicolon
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        syntaxError("Expected ';' after initialization in 'for' loop.");
    }
    addChild(forNode, createParseTreeNode("Delimiter", token->value));

    // Parse the optional condition
    if (peekToken() && !(strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, ";") == 0)) {
        addChild(forNode, parseExpression());
    }

    // Match the second semicolon
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        syntaxError("Expected ';' after condition in 'for' loop.");
    }
    addChild(forNode, createParseTreeNode("Delimiter", token->value));

    // Parse the optional update statement
    if (peekToken() && !(strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, ")") == 0)) {
        addChild(forNode, parseForUpdate());
    }

    // Match the closing parenthesis ')'
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        syntaxError("Expected ')' after 'for' loop update.");
    }
    addChild(forNode, createParseTreeNode("Delimiter", token->value));

    // Parse the loop body
    addChild(forNode, parseStatementBlock());

    return forNode;
}





ParseTreeNode* parseIncDec() {
    printf("Parsing Increment/Decrement...\n");

    // Create a parse tree node for increment/decrement
    ParseTreeNode* incDecNode = createParseTreeNode("IncDec", "");

    // Match and parse the identifier
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Identifier") != 0) {
        syntaxError("Expected an identifier in increment/decrement statement.");
    }
    addChild(incDecNode, createParseTreeNode("Identifier", token->value));

    // Match and parse the increment/decrement operator
    token = getNextToken();
    if (!token || !(strcmp(token->type, "UnaryOperator") == 0 && 
                   (strcmp(token->value, "++") == 0 || strcmp(token->value, "--") == 0))) {
        syntaxError("Expected '++' or '--' after identifier.");
    }
    addChild(incDecNode, createParseTreeNode("UnaryOperator", token->value));

    return incDecNode;
}







ParseTreeNode* parseForUpdate() {
    printf("Parsing For Loop Update...\n");

    // Create a node for the For Update
    ParseTreeNode* forUpdateNode = createParseTreeNode("ForUpdate", "");

    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input in 'for' loop update.");
    }

    // Match and parse the identifier
    if (strcmp(token->type, "Identifier") == 0) {
        Token* nextToken = peekToken(); // Peek at the next token

        if (nextToken && 
            (strcmp(nextToken->type, "AssignmentOperator") == 0 || 
             strcmp(nextToken->type, "AddAssignOperator") == 0 || 
             strcmp(nextToken->type, "SubAssignOperator") == 0 ||
             strcmp(nextToken->type, "MultAssignOperator") == 0 ||
             strcmp(nextToken->type, "DivAssignOperator") == 0 ||
             strcmp(nextToken->type, "ModAssignOperator") == 0 ||
             strcmp(nextToken->type, "IntDivAssignOperator") == 0)) {
            // Parse an assignment statement and add it as a child
            addChild(forUpdateNode, parseAssignmentStatement());
        } else if (nextToken && 
                   strcmp(nextToken->type, "UnaryOperator") == 0 && 
                   (strcmp(nextToken->value, "++") == 0 || strcmp(nextToken->value, "--") == 0)) {
            // Parse an increment/decrement statement and add it as a child
            addChild(forUpdateNode, parseIncDec());
        } else {
            syntaxError("Invalid update in 'for' loop.");
        }
    } else {
        syntaxError("Expected an identifier in 'for' loop update.");
    }

    return forUpdateNode;
}





ParseTreeNode* parseWhileLoop() {
    printf("Parsing While Loop...\n");

    // Create a parse tree node for the while loop
    ParseTreeNode* whileNode = createParseTreeNode("WhileLoop", "");

    // Match "while"
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "while") != 0) {
        syntaxError("Expected 'while' keyword.");
    }
    addChild(whileNode, createParseTreeNode("Keyword", token->value));

    // Match '('
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        syntaxError("Expected '(' after 'while'.");
    }
    addChild(whileNode, createParseTreeNode("Delimiter", token->value));

    // Parse the condition expression
    addChild(whileNode, parseExpression());

    // Match ')'
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        syntaxError("Expected ')' after condition expression in 'while' loop.");
    }
    addChild(whileNode, createParseTreeNode("Delimiter", token->value));

    // Parse the statement block
    addChild(whileNode, parseStatementBlock());

    return whileNode;
}





ParseTreeNode* parseDoWhileLoop() {
    printf("Parsing Do-While Loop...\n");

    // Create a parse tree node for the do-while loop
    ParseTreeNode* doWhileNode = createParseTreeNode("DoWhileLoop", "");

    // Match "do"
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "do") != 0) {
        syntaxError("Expected 'do' keyword.");
    }
    addChild(doWhileNode, createParseTreeNode("Keyword", token->value));

    // Parse the statement block
    addChild(doWhileNode, parseStatementBlock());

    // Match "while"
    token = getNextToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "while") != 0) {
        syntaxError("Expected 'while' keyword after 'do' block.");
    }
    addChild(doWhileNode, createParseTreeNode("Keyword", token->value));

    // Match '('
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        syntaxError("Expected '(' after 'while' in do-while loop.");
    }
    addChild(doWhileNode, createParseTreeNode("Delimiter", token->value));

    // Parse the condition expression
    addChild(doWhileNode, parseExpression());

    // Match ')'
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        syntaxError("Expected ')' after condition expression in 'do-while' loop.");
    }
    addChild(doWhileNode, createParseTreeNode("Delimiter", token->value));

    // Match ';'
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        syntaxError("Expected ';' at the end of 'do-while' loop.");
    }
    addChild(doWhileNode, createParseTreeNode("Delimiter", token->value));

    return doWhileNode;
}






ParseTreeNode* parseArrayDimensions() {
    printf("Parsing Array Dimensions...\n");

    // Create a parse tree node for array dimensions
    ParseTreeNode* dimensionsNode = createParseTreeNode("ArrayDimensions", "");

    Token* token = peekToken();
    while (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "[") == 0) {
        // Match '['
        token = getNextToken(); // Consume '['
        if (!token) {
            syntaxError("Unexpected end of input while parsing '[' in array dimensions.");
        }
        addChild(dimensionsNode, createParseTreeNode("Delimiter", token->value));

        // Parse the dimension expression
        addChild(dimensionsNode, parseExpression());

        // Match ']'
        token = getNextToken();
        if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "]") != 0) {
            syntaxError("Expected ']' after array dimension expression.");
        }
        addChild(dimensionsNode, createParseTreeNode("Delimiter", token->value));

        // Peek for more dimensions
        token = peekToken();
    }

    return dimensionsNode;
}







ParseTreeNode* parseArrayType() {
    printf("Parsing Array Type...\n");

    // Create a parse tree node for array type
    ParseTreeNode* arrayTypeNode = createParseTreeNode("ArrayType", "");

    // Match the type specifier (e.g., "int", "float", etc.)
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Keyword") != 0 ||
        !(strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
          strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
          strcmp(token->value, "string") == 0)) {
        syntaxError("Expected a valid type specifier (e.g., 'int', 'float', 'char', 'bool', 'string').");
    }

    // Add the matched type to the parse tree
    addChild(arrayTypeNode, createParseTreeNode("TypeSpecifier", token->value));

    return arrayTypeNode;
}









ParseTreeNode* parseArrayInitializer() {
    printf("Parsing Array Initializer...\n");

    // Create a parse tree node for the array initializer
    ParseTreeNode* initializerNode = createParseTreeNode("ArrayInitializer", "");

    // Match the opening curly brace '{'
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "{") != 0) {
        syntaxError("Expected '{' to start array initializer");
    }
    addChild(initializerNode, createParseTreeNode("Delimiter", token->value));

    // Parse initializer values
    do {
        ParseTreeNode* exprNode = parseExpression(); // Parse the initializer value
        addChild(initializerNode, exprNode);

        token = peekToken();
        if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
            getNextToken(); // Consume ','
            addChild(initializerNode, createParseTreeNode("Delimiter", token->value));
        }
    } while (token && !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "}") == 0));

    // Match the closing curly brace '}'
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "}") != 0) {
        syntaxError("Expected '}' to end array initializer");
    }
    addChild(initializerNode, createParseTreeNode("Delimiter", token->value));

    return initializerNode;
}






ParseTreeNode* parseArrayDeclaration() {
    printf("Parsing Array Declaration...\n");

    // Create a parse tree node for the array declaration
    ParseTreeNode* arrayDeclNode = createParseTreeNode("ArrayDeclaration", "");

    // Match the "array" keyword
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "array") != 0) {
        syntaxError("Expected 'array' keyword for array declaration");
    }
    addChild(arrayDeclNode, createParseTreeNode("Keyword", token->value));

    // Parse the array type
    ParseTreeNode* typeNode = parseArrayType();
    addChild(arrayDeclNode, typeNode);

    // Match the identifier (array name)
    token = getNextToken();
    if (!token || strcmp(token->type, "Identifier") != 0) {
        syntaxError("Expected an identifier for the array name");
    }
    addChild(arrayDeclNode, createParseTreeNode("Identifier", token->value));

    // Parse the array dimensions
    ParseTreeNode* dimensionsNode = parseArrayDimensions();
    addChild(arrayDeclNode, dimensionsNode);

    // Check for optional assignment
    token = peekToken();
    if (token && strcmp(token->type, "AssignmentOperator") == 0 && strcmp(token->value, "=") == 0) {
        getNextToken(); // Consume '='
        addChild(arrayDeclNode, createParseTreeNode("AssignmentOperator", token->value));

        // Parse the initializer
        ParseTreeNode* initializerNode = parseArrayInitializer();
        addChild(arrayDeclNode, initializerNode);
    }

    // Match semicolon
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        syntaxError("Expected ';' at the end of the array declaration");
    }
    addChild(arrayDeclNode, createParseTreeNode("Delimiter", token->value));

    return arrayDeclNode;
}






ParseTreeNode* parseArrayOperations() {
    printf("Parsing Array Operations...\n");

    // Create a parse tree node for array operations
    ParseTreeNode* arrayOpsNode = createParseTreeNode("ArrayOperations", "");

    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing array operations");
    }

    // Determine the type of array operation
    if (strcmp(token->type, "Identifier") == 0) {
        token = getNextToken(); // Consume the identifier
        addChild(arrayOpsNode, createParseTreeNode("Identifier", token->value));

        // Check if it's an array access operation
        token = peekToken();
        if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "[") == 0) {
            ParseTreeNode* accessNode = parseArrayAccess();
            addChild(arrayOpsNode, accessNode);
        } else if (token && strcmp(token->type, "AssignmentOperator") == 0 && strcmp(token->value, "=") == 0) {
            // Check if it's an array assignment operation
            token = getNextToken(); // Consume '='
            addChild(arrayOpsNode, createParseTreeNode("AssignmentOperator", token->value));

            // Parse the expression to be assigned
            ParseTreeNode* exprNode = parseExpression();
            addChild(arrayOpsNode, exprNode);

            // Match the semicolon
            token = getNextToken();
            if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
                syntaxError("Expected ';' at the end of array assignment");
            }
            addChild(arrayOpsNode, createParseTreeNode("Delimiter", token->value));
        } else {
            syntaxError("Invalid array operation");
        }
    } else {
        syntaxError("Expected an identifier for array operations");
    }

    return arrayOpsNode;
}







ParseTreeNode* parseArrayAccess() {
    printf("Parsing Array Access...\n");

    // Create a parse tree node for array access
    ParseTreeNode* arrayAccessNode = createParseTreeNode("ArrayAccess", "");

    // Validate the identifier for the array
    Token* token = peekToken();
    if (!token || strcmp(token->type, "Identifier") != 0) {
        syntaxError("Expected an identifier for array access");
    }

    token = getNextToken(); // Consume the identifier
    addChild(arrayAccessNode, createParseTreeNode("Identifier", token->value));

    // Parse array indices enclosed in brackets
    while ((token = peekToken()) && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "[") == 0) {
        token = getNextToken(); // Consume '['
        addChild(arrayAccessNode, createParseTreeNode("Delimiter", "["));

        // Parse the index expression
        ParseTreeNode* indexExprNode = parseExpression();
        addChild(arrayAccessNode, indexExprNode);

        token = getNextToken(); // Consume ']'
        if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "]") != 0) {
            syntaxError("Expected ']' after array index expression");
        }
        addChild(arrayAccessNode, createParseTreeNode("Delimiter", "]"));
    }

    // Check for valid termination of array access
    token = peekToken();
    if (!token || (strcmp(token->type, "AssignmentOperator") != 0 && strcmp(token->type, "Delimiter") != 0)) {
        syntaxError("Invalid token following array access");
    }

    // Handle array assignment if applicable
    if (strcmp(token->type, "AssignmentOperator") == 0 && strcmp(token->value, "=") == 0) {
        token = getNextToken(); // Consume '='
        addChild(arrayAccessNode, createParseTreeNode("AssignmentOperator", token->value));

        // Parse the value to be assigned
        ParseTreeNode* valueNode = parseExpression();
        addChild(arrayAccessNode, valueNode);

        token = getNextToken();
        if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
            syntaxError("Expected ';' after array assignment");
        }
        addChild(arrayAccessNode, createParseTreeNode("Delimiter", ";"));
    } else if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ";") == 0) {
        token = getNextToken(); // Consume the semicolon
        addChild(arrayAccessNode, createParseTreeNode("Delimiter", ";"));
    }

    return arrayAccessNode;
}






ParseTreeNode* parseParameter() {
    printf("Parsing Parameter...\n");

    // Create a parse tree node for the parameter
    ParseTreeNode* parameterNode = createParseTreeNode("Parameter", "");

    // Parse the type specifier
    ParseTreeNode* typeNode = parseTypeSpecifier();
    addChild(parameterNode, typeNode);

    // Match the identifier
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Identifier") != 0) {
        syntaxError("Expected an identifier for parameter");
    }
    addChild(parameterNode, createParseTreeNode("Identifier", token->value));

    return parameterNode;
}







ParseTreeNode* parseFunctionDeclaration() {
    printf("Parsing Function Declaration...\n");

    // Create a parse tree node for the function declaration
    ParseTreeNode* functionNode = createParseTreeNode("FunctionDeclaration", "");

    // Parse the return type
    ParseTreeNode* returnTypeNode = parseTypeSpecifier();
    addChild(functionNode, returnTypeNode);

    // Expect an identifier for the function name
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Identifier") != 0) {
        syntaxError("Expected an identifier for function name");
    }
    addChild(functionNode, createParseTreeNode("Identifier", token->value));

    // Expect '(' to start the parameter list
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        syntaxError("Expected '(' after function name");
    }
    addChild(functionNode, createParseTreeNode("LeftParenthesis", "("));

    // Parse the parameter list
    token = peekToken();
    if (token && !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ")") == 0)) {
        ParseTreeNode* parameterListNode = parseParameterList(); // Parse the parameters if any
        addChild(functionNode, parameterListNode);
    }

    // Expect ')' to end the parameter list
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        syntaxError("Expected ')' after parameter list");
    }
    addChild(functionNode, createParseTreeNode("RightParenthesis", ")"));

    // Check for a semicolon or a compound statement
    token = peekToken();
    if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ";") == 0) {
        getNextToken(); // Consume the semicolon (function declaration only)
        addChild(functionNode, createParseTreeNode("Semicolon", ";"));
    } else {
        ParseTreeNode* blockNode = parseBlock(); // Parse the function body (compound statement)
        addChild(functionNode, blockNode);
    }

    return functionNode;
}






ParseTreeNode* parseFunctionCall() {
    printf("Parsing Function Call...\n");

    // Create a parse tree node for the function call
    ParseTreeNode* functionCallNode = createParseTreeNode("FunctionCall", "");

    // Expect an identifier for the function name
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Identifier") != 0) {
        syntaxError("Expected an identifier for function name");
    }
    addChild(functionCallNode, createParseTreeNode("FunctionName", token->value));

    // Expect '(' to start the argument list
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        syntaxError("Expected '(' after function name");
    }
    addChild(functionCallNode, createParseTreeNode("LeftParenthesis", "("));

    // Parse the argument list if there are any
    token = peekToken();
    if (token && !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ")") == 0)) {
        ParseTreeNode* argumentListNode = parseArgumentList();
        addChild(functionCallNode, argumentListNode);
    }

    // Expect ')' to end the argument list
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        syntaxError("Expected ')' after argument list");
    }
    addChild(functionCallNode, createParseTreeNode("RightParenthesis", ")"));

    // Expect a semicolon to end the function call
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        syntaxError("Expected ';' after function call");
    }
    addChild(functionCallNode, createParseTreeNode("Semicolon", ";"));

    return functionCallNode;
}






ParseTreeNode* parseParameterList() {
    printf("Parsing Parameter List...\n");

    // Create a parse tree node for the parameter list
    ParseTreeNode* parameterListNode = createParseTreeNode("ParameterList", "");

    // Parse the first parameter
    ParseTreeNode* firstParameterNode = parseParameter();
    addChild(parameterListNode, firstParameterNode);

    // Check for additional parameters separated by commas
    Token* token = peekToken();
    while (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
        getNextToken(); // Consume the comma
        addChild(parameterListNode, createParseTreeNode("Comma", ",")); // Add the comma to the tree

        ParseTreeNode* nextParameterNode = parseParameter(); // Parse the next parameter
        addChild(parameterListNode, nextParameterNode);

        token = peekToken(); // Update the token to check for more commas
    }

    return parameterListNode;
}







ParseTreeNode* parseArgumentList() {
    printf("Parsing Argument List...\n");

    // Create a parse tree node for the argument list
    ParseTreeNode* argumentListNode = createParseTreeNode("ArgumentList", "");

    // Parse the first argument
    ParseTreeNode* firstArgumentNode = parseExpression(); // Arguments are typically expressions
    addChild(argumentListNode, firstArgumentNode);

    // Check for additional arguments separated by commas
    Token* token = peekToken();
    while (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
        getNextToken(); // Consume the comma
        addChild(argumentListNode, createParseTreeNode("Comma", ",")); // Add the comma to the tree

        ParseTreeNode* nextArgumentNode = parseExpression(); // Parse the next argument
        addChild(argumentListNode, nextArgumentNode);

        token = peekToken(); // Update the token to check for more commas
    }

    return argumentListNode;
}







ParseTreeNode* parseVariableList() {
    printf("Parsing Variable List...\n");

    // Create a parse tree node for the variable list
    ParseTreeNode* variableListNode = createParseTreeNode("VariableList", "");

    // Parse the first variable (identifier)
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Identifier") != 0) {
        syntaxError("Expected an identifier in variable list");
    }
    addChild(variableListNode, createParseTreeNode("Identifier", token->value));

    // Check for additional variables separated by commas
    while (peekToken() && strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, ",") == 0) {
        getNextToken(); // Consume ',' and add it to the tree
        addChild(variableListNode, createParseTreeNode("Comma", ","));

        token = getNextToken(); // Parse the next variable (identifier)
        if (!token || strcmp(token->type, "Identifier") != 0) {
            syntaxError("Expected an identifier after ',' in variable list");
        }
        addChild(variableListNode, createParseTreeNode("Identifier", token->value));
    }

    return variableListNode;
}




ParseTreeNode* parseAssignmentStatement() {
    printf("Parsing Assignment Statement...\n");

    // Create a parse tree node for the assignment statement
    ParseTreeNode* assignmentNode = createParseTreeNode("AssignmentStatement", "");

    // Parse the identifier at the start of the assignment
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Identifier") != 0) {
        syntaxError("Expected an identifier in assignment statement.");
    }
    addChild(assignmentNode, createParseTreeNode("Identifier", token->value));

    // Parse the assignment operator
    token = getNextToken();
    if (!token || !(strcmp(token->type, "AssignmentOperator") == 0 ||
                    strcmp(token->type, "AddAssignmentOperator") == 0 ||
                    strcmp(token->type, "SubAssignmentOperator") == 0 ||
                    strcmp(token->type, "MultiAssignmentOperator") == 0 ||
                    strcmp(token->type, "DivAssignmentOperator") == 0 ||
                    strcmp(token->type, "ModAssignmentOperator") == 0 ||
                    strcmp(token->type, "IntDivAssignmentOperator") == 0)) {
        syntaxError("Expected an assignment operator.");
    }
    addChild(assignmentNode, createParseTreeNode("AssignmentOperator", token->value));

    // Parse either a variable list or an expression
    token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input in assignment statement.");
    }

    if (strcmp(token->type, "Identifier") == 0) {
        // Handle variable list
        addChild(assignmentNode, parseVariableList());
    } else {
        // Handle expression or literal
        addChild(assignmentNode, parseExpression());
    }

    // Parse the semicolon to terminate the assignment statement
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        syntaxError("Expected ';' at the end of assignment statement.");
    }
    addChild(assignmentNode, createParseTreeNode("Semicolon", ";"));

    return assignmentNode;
}






ParseTreeNode* parseInputStatement() {
    printf("Parsing Input Statement...\n");

    // Create a parse tree node for the input statement
    ParseTreeNode* inputNode = createParseTreeNode("InputStatement", "");

    // Match and add the "input" keyword
    matchToken("Keyword", "input"); // Match "input"
    addChild(inputNode, createParseTreeNode("Keyword", "input"));

    // Match and add the opening parenthesis "("
    matchToken("Delimiter", "("); // Match "("
    addChild(inputNode, createParseTreeNode("Delimiter", "("));

    // Check and parse the input list if it exists
    Token* token = peekToken();
    if (token && strcmp(token->type, "FormatString") == 0) {
        addChild(inputNode, parseInputList());
    }

    // Match and add the closing parenthesis ")"
    matchToken("Delimiter", ")"); // Match ")"
    addChild(inputNode, createParseTreeNode("Delimiter", ")"));

    // Match and add the semicolon ";"
    matchToken("Delimiter", ";"); // Match ";"
    addChild(inputNode, createParseTreeNode("Delimiter", ";"));

    return inputNode;
}






ParseTreeNode* parseInputList() {
    printf("Parsing Input List...\n");

    // Create a parse tree node for the input list
    ParseTreeNode* inputListNode = createParseTreeNode("InputList", "");

    // Parse at least one format-variable pair
    addChild(inputListNode, parseFormatVariablePair());

    // Parse additional format-variable pairs separated by commas
    Token* token = peekToken();
    while (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
        getNextToken(); // Consume the comma
        addChild(inputListNode, createParseTreeNode("Delimiter", ",")); // Add the comma as a child

        // Parse the next format-variable pair
        addChild(inputListNode, parseFormatVariablePair());
        token = peekToken(); // Update token for the next iteration
    }

    return inputListNode;
}








ParseTreeNode* parseFormatVariablePair() {
    printf("Parsing Format-Variable Pair...\n");

    // Create a parse tree node for the format-variable pair
    ParseTreeNode* pairNode = createParseTreeNode("FormatVariablePair", "");

    // Expect a format string
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "FormatString") != 0) {
        syntaxError("Expected a format string");
    }
    addChild(pairNode, createParseTreeNode("FormatString", token->value));

    // Expect a comma
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ",") != 0) {
        syntaxError("Expected a comma after format string");
    }
    addChild(pairNode, createParseTreeNode("Delimiter", ","));

    // Expect an address variable
    token = getNextToken();
    if (!token || strcmp(token->type, "AddressVariable") != 0) {
        syntaxError("Expected an address variable");
    }
    addChild(pairNode, createParseTreeNode("AddressVariable", token->value));

    return pairNode;
}






ParseTreeNode* parseExpressionList() {
    printf("Parsing Expression List...\n");

    // Create a parse tree node for the expression list
    ParseTreeNode* exprListNode = createParseTreeNode("ExpressionList", "");

    // Parse the first expression
    addChild(exprListNode, parseExpression());

    // Parse subsequent expressions separated by commas
    while (peekToken() && strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, ",") == 0) {
        Token* token = getNextToken(); // Consume the comma
        addChild(exprListNode, createParseTreeNode("Delimiter", token->value)); // Add the comma as a child
        addChild(exprListNode, parseExpression()); // Parse the next expression
    }

    return exprListNode;
}





ParseTreeNode* parseOutputStatement() {
    printf("Parsing Output Statement...\n");

    // Create a parse tree node for the output statement
    ParseTreeNode* outputNode = createParseTreeNode("OutputStatement", "");

    // Match and add the "printf" keyword
    matchToken("Keyword", "printf");
    addChild(outputNode, createParseTreeNode("Keyword", "printf"));

    // Match and add the opening parenthesis "("
    matchToken("Delimiter", "(");
    addChild(outputNode, createParseTreeNode("Delimiter", "("));

    // Check and parse the output list if a format string or expression is present
    Token* token = peekToken();
    if (token && 
        (strcmp(token->type, "FormatString") == 0 || 
         strcmp(token->type, "STRING_LITERAL") == 0 || 
         strcmp(token->type, "INT_LITERAL") == 0 ||
         strcmp(token->type, "FLOAT_LITERAL") == 0 ||
         strcmp(token->type, "CHAR_LITERAL") == 0 || 
         strcmp(token->type, "IDENTIFIER") == 0)) {
        addChild(outputNode, parseOutputList());
    }

    // Match and add the closing parenthesis ")"
    matchToken("Delimiter", ")");
    addChild(outputNode, createParseTreeNode("Delimiter", ")"));

    // Match and add the semicolon ";"
    matchToken("Delimiter", ";");
    addChild(outputNode, createParseTreeNode("Delimiter", ";"));

    return outputNode;
}






ParseTreeNode* parseOutputList() {
    printf("Parsing Output List...\n");

    // Create a parse tree node for the output list
    ParseTreeNode* outputListNode = createParseTreeNode("OutputList", "");

    // Check the type of the first token to determine whether to parse a string concatenation or an expression list
    Token* token = peekToken();
    if (token && (strcmp(token->type, "FormatString") == 0 || strcmp(token->type, "STRING_LITERAL") == 0)) {
        // Parse as a string concatenation
        addChild(outputListNode, parseStringConcat());
    } else if (token && 
               (strcmp(token->type, "IDENTIFIER") == 0 || 
                strcmp(token->type, "INT_LITERAL") == 0 || 
                strcmp(token->type, "FLOAT_LITERAL") == 0 || 
                strcmp(token->type, "CHAR_LITERAL") == 0)) {
        // Parse as an expression list
        addChild(outputListNode, parseExpressionList());
    } else {
        syntaxError("Invalid output list. Expected a format string, string literal, or expression.");
    }

    return outputListNode;
}





ParseTreeNode* parseStringConcat() {
    printf("Parsing String Concatenation...\n");

    // Create a parse tree node for string concatenation
    ParseTreeNode* stringConcatNode = createParseTreeNode("StringConcat", "");

    // Parse at least one string literal or format string
    Token* token = getNextToken();
    if (!token || !(strcmp(token->type, "FormatString") == 0 || strcmp(token->type, "STRING_LITERAL") == 0)) {
        syntaxError("Expected a format string or string literal for concatenation");
    }
    addChild(stringConcatNode, createParseTreeNode(token->type, token->value));

    // Parse additional strings separated by ADD_OP ('+') (if any)
    token = peekToken();
    while (token && strcmp(token->type, "ADD_OP") == 0) {
        getNextToken(); // Consume '+'
        addChild(stringConcatNode, createParseTreeNode("AddOp", "+"));

        token = getNextToken();
        if (!token || !(strcmp(token->type, "FormatString") == 0 || strcmp(token->type, "STRING_LITERAL") == 0)) {
            syntaxError("Expected a format string or string literal after '+'");
        }
        addChild(stringConcatNode, createParseTreeNode(token->type, token->value));

        token = peekToken(); // Check for more concatenations
    }

    return stringConcatNode;
}





ParseTreeNode* parseSwitchStatement() {
    printf("Parsing Switch Statement...\n");

    // Create a parse tree node for the switch statement
    ParseTreeNode* switchNode = createParseTreeNode("SwitchStatement", "");

    // Match the 'switch' keyword
    addChild(switchNode, matchToken("Keyword", "switch"));

    // Match the opening parenthesis '('
    addChild(switchNode, matchToken("Delimiter", "("));

    // Parse the controlling expression
    addChild(switchNode, parseExpression());

    // Match the closing parenthesis ')'
    addChild(switchNode, matchToken("Delimiter", ")"));

    // Match the opening curly brace '{'
    addChild(switchNode, matchToken("Delimiter", "{"));

    // Parse case statements
    while (peekToken() && strcmp(peekToken()->value, "case") == 0) {
        addChild(switchNode, parseCaseStatement());
    }

    // Optionally parse the default case
    if (peekToken() && strcmp(peekToken()->value, "default") == 0) {
        addChild(switchNode, parseDefaultCase());
    }

    // Match the closing curly brace '}'
    addChild(switchNode, matchToken("Delimiter", "}"));

    return switchNode;
}





ParseTreeNode* parseIterativeStatement() {
    printf("Parsing Iterative Statement...\n");

    // Create a parse tree node for the iterative statement
    ParseTreeNode* iterativeStatementNode = createParseTreeNode("IterativeStatement", "");

    // Peek at the token to decide which iterative structure to parse
    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing an iterative statement.");
    }

    // Match and parse based on the iterative keyword
    if (strcmp(token->value, "for") == 0) {
        addChild(iterativeStatementNode, parseForLoop());
    } else if (strcmp(token->value, "while") == 0) {
        addChild(iterativeStatementNode, parseWhileLoop());
    } else if (strcmp(token->value, "do") == 0) {
        addChild(iterativeStatementNode, parseDoWhileLoop());
    } else {
        char errorMessage[100];
        snprintf(errorMessage, sizeof(errorMessage), 
                 "Expected 'for', 'while', or 'do' at the start of an iterative statement. Got '%s'.",
                 token->value);
        syntaxError(errorMessage);
    }

    return iterativeStatementNode;
}






ParseTreeNode* parseExpressionStatement() {
    printf("Parsing Expression Statement...\n");

    // Create a parse tree node for the expression statement
    ParseTreeNode* expressionStatementNode = createParseTreeNode("ExpressionStatement", "");

    // Parse the expression and add it as a child
    addChild(expressionStatementNode, parseExpression());

    // Expect a semicolon to terminate the expression statement
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        syntaxError("Expected ';' at the end of an expression statement.");
    }

    // Add the semicolon as a child node
    addChild(expressionStatementNode, createParseTreeNode("Delimiter", token->value));

    return expressionStatementNode;
}






ParseTreeNode* parseJumpStatement() {
    printf("Parsing Jump Statement...\n");

    // Create a parse tree node for the jump statement
    ParseTreeNode* jumpStatementNode = createParseTreeNode("JumpStatement", "");

    Token* token = getNextToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing jump statement.");
    }

    if (strcmp(token->value, "return") == 0) {
        // Add 'return' keyword as a child node
        addChild(jumpStatementNode, createParseTreeNode("Keyword", "return"));

        // Check if there's an expression to return
        if (peekToken() && (strcmp(peekToken()->type, "Identifier") == 0 ||
                            strcmp(peekToken()->type, "INT_LITERAL") == 0 ||
                            strcmp(peekToken()->type, "FLOAT_LITERAL") == 0 ||
                            strcmp(peekToken()->type, "CHAR_LITERAL") == 0 ||
                            strcmp(peekToken()->type, "STRING_LITERAL") == 0)) {
            addChild(jumpStatementNode, parseExpression());
        }

        token = getNextToken();
        if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
            syntaxError("Expected ';' after return statement.");
        }

        // Add semicolon as a child node
        addChild(jumpStatementNode, createParseTreeNode("Delimiter", ";"));

    } else if (strcmp(token->value, "break") == 0 || strcmp(token->value, "continue") == 0) {
        // Add 'break' or 'continue' keyword as a child node
        addChild(jumpStatementNode, createParseTreeNode("Keyword", token->value));

        token = getNextToken();
        if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
            syntaxError("Expected ';' after break/continue statement.");
        }

        // Add semicolon as a child node
        addChild(jumpStatementNode, createParseTreeNode("Delimiter", ";"));

    } else {
        syntaxError("Unrecognized jump statement. Expected 'return', 'break', or 'continue'.");
    }

    return jumpStatementNode;
}




ParseTreeNode* parseConditionalStatement() {
    printf("Parsing Conditional Statement...\n");

    // Create the conditional statement node
    ParseTreeNode* conditionalNode = createParseTreeNode("ConditionalStatement", "");

    // Match "if"
    if (peekToken() && strcmp(peekToken()->type, "Keyword") == 0 && strcmp(peekToken()->value, "if") == 0) {
        addChild(conditionalNode, matchToken("Keyword", "if"));
    } else {
        syntaxError("Expected 'if' keyword.");
    }

    // Match "("
    if (peekToken() && strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, "(") == 0) {
        addChild(conditionalNode, matchToken("Delimiter", "("));
    } else {
        syntaxError("Expected '(' after 'if'.");
    }

    // Parse the condition expression
    addChild(conditionalNode, parseExpression());

    // Match ")"
    if (peekToken() && strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, ")") == 0) {
        addChild(conditionalNode, matchToken("Delimiter", ")"));
    } else {
        syntaxError("Expected ')' after condition.");
    }

    // Parse the statement block
    addChild(conditionalNode, parseStatementBlock());

    // Handle optional "else" or "else if"
    if (peekToken() && strcmp(peekToken()->type, "Keyword") == 0 && strcmp(peekToken()->value, "else") == 0) {
        ParseTreeNode* elseNode = createParseTreeNode("ElseClause", "");
        addChild(conditionalNode, elseNode);
        addChild(elseNode, matchToken("Keyword", "else"));

        // Check if it's an "else if" block
        if (peekToken() && strcmp(peekToken()->type, "Keyword") == 0 && strcmp(peekToken()->value, "if") == 0) {
            addChild(elseNode, parseConditionalStatement());
        } else {
            // Otherwise, parse a regular "else" block
            addChild(elseNode, parseStatementBlock());
        }
    }

    return conditionalNode;
}





ParseTreeNode* parseMainFunction() {
    printf("DEBUG: Parsing Main Function...\n");

    // Create the main function node
    ParseTreeNode* mainFunctionNode = createParseTreeNode("MainFunction", NULL);

    // Match 'void' keyword
    Token* token = peekToken(); // Check the token without consuming
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "void") != 0) {
        syntaxError("Expected 'void' keyword at the beginning of the main function");
        return NULL;
    }
    printf("DEBUG: Peeked and found 'void': Type='%s', Value='%s', Line=%d\n",
           token->type, token->value, token->lineNumber);

    token = getNextToken();  // Consume 'void'
    addChild(mainFunctionNode, createParseTreeNode("Keyword", "void"));

    // Match 'main' identifier
    token = peekToken(); // Check the token without consuming
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "main") != 0) {
        syntaxError("Expected 'main' identifier after 'void'");
        return NULL;
    }
    printf("DEBUG: Peeked and found 'main': Type='%s', Value='%s', Line=%d\n",
           token->type, token->value, token->lineNumber);

    token = getNextToken();  // Consume 'main'
    addChild(mainFunctionNode, createParseTreeNode("Identifier", "main"));

    // Match '(' delimiter
    token = peekToken(); // Check the token without consuming
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        syntaxError("Expected '(' after 'main'");
        return NULL;
    }
    printf("DEBUG: Peeked and found '(': Type='%s', Value='%s', Line=%d\n",
           token->type, token->value, token->lineNumber);

    token = getNextToken();  // Consume '('
    addChild(mainFunctionNode, createParseTreeNode("Delimiter", "("));

    // Match ')' delimiter
    token = peekToken(); // Check the token without consuming
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        syntaxError("Expected ')' after '('");
        return NULL;
    }
    printf("DEBUG: Peeked and found ')': Type='%s', Value='%s', Line=%d\n",
           token->type, token->value, token->lineNumber);

    token = getNextToken();  // Consume ')'
    addChild(mainFunctionNode, createParseTreeNode("Delimiter", ")"));

    // Match '{' delimiter
    token = peekToken(); // Check the token without consuming
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "{") != 0) {
        syntaxError("Expected '{' to start the main function body");
        return NULL;
    }
    printf("DEBUG: Peeked and found '{': Type='%s', Value='%s', Line=%d\n",
           token->type, token->value, token->lineNumber);

    token = getNextToken();  // Consume '{'
    addChild(mainFunctionNode, createParseTreeNode("Delimiter", "{"));

    // Parse the block inside the main function
    printf("DEBUG: Parsing block inside main function...\n");
    ParseTreeNode* blockNode = parseBlock();  // Ensure parseBlock is implemented correctly
    if (!blockNode) {
        syntaxError("Failed to parse main function block");
        return NULL;
    }
    addChild(mainFunctionNode, blockNode);

    // Match '}' delimiter
    token = peekToken(); // Check the token without consuming
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "}") != 0) {
        syntaxError("Expected '}' to close the main function body");
        return NULL;
    }
    printf("DEBUG: Peeked and found '}': Type='%s', Value='%s', Line=%d\n",
           token->type, token->value, token->lineNumber);

    token = getNextToken();  // Consume '}'
    addChild(mainFunctionNode, createParseTreeNode("Delimiter", "}"));

    printf("DEBUG: Completed parsing Main Function.\n");
    return mainFunctionNode;
}

















// Main function
int main(int argc, char* argv[]) {
    printf("\n\nStarting Syntax Analysis...\n");

    // Open the specified directory or current directory to look for .prsm files
    struct dirent *entry;
    DIR *dp = NULL;
    const char* directory = (argc > 1) ? argv[1] : ".";
    dp = opendir(directory);
    if (!dp) {
        printf("Error: Unable to open directory %s.\n", directory);
        return 1;
    }

    // List .prsm files
    char prsmFiles[100][256];
    int fileCount = 0;

    printf("Available .prsm files in directory '%s':\n", directory);
    while ((entry = readdir(dp))) {
        if (strstr(entry->d_name, ".prsm") && strlen(entry->d_name) > 5) {
            printf("%d. %s\n", fileCount + 1, entry->d_name);
            snprintf(prsmFiles[fileCount], sizeof(prsmFiles[fileCount]), "%s/%s", directory, entry->d_name);
            fileCount++;
        }
    }
    closedir(dp);

    if (fileCount == 0) {
        printf("No .prsm files found in directory %s.\n", directory);
        return 1;
    }

    // Prompt user to select a file
    int choice;
    printf("\nEnter the number of the file to analyze (1-%d): ", fileCount);
    scanf("%d", &choice);

    if (choice < 1 || choice > fileCount) {
        printf("Invalid choice. Exiting.\n");
        return 1;
    }

    // Load tokens from the selected .prsm file
    const char* selectedFile = prsmFiles[choice - 1];
    printf("\nSelected file: %s\n", selectedFile);
    loadTokensFromFile(selectedFile);

    // Parse and build the parse tree
    ParseTreeNode* root = parseProgram();

    // Validate the root parse tree
    if (!root) {
        printf("Parsing failed. No parse tree generated.\n");
        return 1;
    }

    // Write the parse tree to a file
    FILE* parseTreeFile = fopen("parse_tree.txt", "w");
    if (!parseTreeFile) {
        printf("Error: Unable to create parse_tree.txt\n");
        freeParseTree(root);
        return 1;
    }

    writeParseTreeToFile(root, parseTreeFile);
    fclose(parseTreeFile);

    printf("\nParse tree written to parse_tree.txt\n");

    // Free the parse tree
    freeParseTree(root);

    printf("\nSyntax Analysis Completed Successfully!\n");
    return 0;
}





