cmake_minimum_required(VERSION 3.27)

project(c-compiler)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(src SOURCES)

set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(cc
        src/error/ErrorHandler.cpp
        src/error/ErrorHandler.h
        src/preprocessor/Preprocessor.cpp
        src/preprocessor/Preprocessor.h
        src/lexer/Token.h
        src/lexer/Lexer.cpp
        src/lexer/Lexer.h
        src/ast/node/Node.h
        src/ast/node/Expression.h
        src/ast/node/Type.h
        src/ast/node/Declaration.h
        src/ast/node/Statement.h
        src/ast/node/TranslationUnit.h
        src/ast/node/expression/BinaryExpression.h
        src/ast/node/expression/CallExpression.h
        src/ast/node/expression/CastExpression.h
        src/ast/node/expression/CharacterLiteralExpression.h
        src/ast/node/expression/FloatingPointLiteralExpression.h
        src/ast/node/expression/IdentifierExpression.h
        src/ast/node/expression/IntegerLiteralExpression.h
        src/ast/node/expression/StringLiteralExpression.h
        src/ast/node/expression/TernaryExpression.h
        src/ast/node/expression/UnaryExpression.h
        src/ast/node/type/ScalarType.h
        src/ast/node/type/ArrayType.h
        src/ast/node/type/FunctionType.h
        src/ast/node/type/PointerType.h
        src/ast/node/declaration/FunctionDeclaration.h
        src/ast/node/declaration/FunctionDefinition.h
        src/ast/node/declaration/VariableDeclaration.h
        src/ast/node/statement/BreakStatement.h
        src/ast/node/statement/CaseStatement.h
        src/ast/node/statement/CompoundStatement.h
        src/ast/node/statement/ContinueStatement.h
        src/ast/node/statement/DeclarationStatement.h
        src/ast/node/statement/DefaultStatement.h
        src/ast/node/statement/DoWhileStatement.h
        src/ast/node/statement/ExpressionStatement.h
        src/ast/node/statement/ForStatement.h
        src/ast/node/statement/GotoStatement.h
        src/ast/node/statement/IfStatement.h
        src/ast/node/statement/LabelStatement.h
        src/ast/node/statement/ReturnStatement.h
        src/ast/node/statement/SwitchStatement.h
        src/ast/node/statement/WhileStatement.h
        src/parser/Parser.cpp
        src/parser/Parser.h
        src/ast/visitor/Visitor.cpp
        src/ast/visitor/Visitor.h
        src/ast/visitor/PrintVisitor.cpp
        src/ast/visitor/PrintVisitor.h
        src/symbol/Symbol.h
        src/symbol/ScalarSymbol.h
        src/symbol/PointerSymbol.h
        src/symbol/ArraySymbol.h
        src/symbol/FunctionSymbol.h
        src/symbol/StatementSymbol.h
        src/symbol/Scope.h
        src/symbol/SymbolTable.cpp
        src/symbol/SymbolTable.h
        src/symbol/SymbolTableBuilder.cpp
        src/symbol/SymbolTableBuilder.h
        src/symbol/SymbolTableIterator.cpp
        src/symbol/SymbolTableIterator.h
        src/constant/StringConstant.h
        src/constant/StringConstantPool.cpp
        src/constant/StringConstantPool.h
        src/builtin/BuiltInFunctionInserter.cpp
        src/builtin/BuiltInFunctionInserter.h
        src/ast/visitor/ErrorCheckVisitor.cpp
        src/ast/visitor/ErrorCheckVisitor.h
        src/instruction/BinaryDataType.h
        src/instruction/Instruction.h
        src/instruction/InstructionSequence.cpp
        src/instruction/InstructionSequence.h
        src/instruction/InstructionSequenceBuilder.cpp
        src/instruction/InstructionSequenceBuilder.h
        src/ast/visitor/CodeGenerateVisitor.cpp
        src/ast/visitor/CodeGenerateVisitor.h
        src/bytecode/Bytecode.h
        src/bytecode/Bytecode.cpp
        src/vm/VirtualMachine.cpp
        src/vm/VirtualMachine.h
        src/main.cpp
        src/address/AddressCalculator.cpp
        src/address/AddressCalculator.h
)

target_include_directories(cc PUBLIC ${PROJECT_BINARY_DIR})
