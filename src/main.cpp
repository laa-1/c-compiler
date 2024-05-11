#include <iostream>
#include <fstream>
#include <string>
#include "preprocessor/Preprocessor.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "ast/visitor/PrintVisitor.h"
#include "ast/visitor/ErrorCheckVisitor.h"
#include "ast/visitor/CodeGenerateVisitor.h"
#include "vm/VirtualMachine.h"

std::vector<char> *preprocess(std::unique_ptr<std::ifstream> file) {
    auto *preprocessor = new Preprocessor(std::move(file));
    std::vector<char> *charList = preprocessor->process();
    delete preprocessor;
    return charList;
}

std::vector<Token> *lexicalAnalyse(std::vector<char> *charList) {
    auto *lexer = new Lexer(charList);
    std::vector<Token> *tokenList = lexer->analysis();
    bool haveError = lexer->isHaveError();
    delete lexer;
    if (haveError) {
        exit(1);
    }
    return tokenList;
}

TranslationUnit *syntaxAnalyse(std::vector<Token> *tokenList) {
    auto *parser = new Parser(tokenList);
    TranslationUnit *translationUnit = parser->analysis();
    bool haveError = parser->isHaveError();
    delete parser;
    if (haveError) {
        exit(1);
    }
    return translationUnit;
}

void printAst(TranslationUnit *translationUnit) {
    auto *printVisitor = new PrintVisitor();
    translationUnit->accept(printVisitor);
    delete printVisitor;
}

void checkError(TranslationUnit *translationUnit, SymbolTable *&symbolTable, StringConstantPool *&stringConstantSequence) {
    auto *errorCheckVisitor = new ErrorCheckVisitor();
    translationUnit->accept(errorCheckVisitor);
    symbolTable = errorCheckVisitor->moveSymbolTable();
    stringConstantSequence = errorCheckVisitor->moveStringConstantPool();
    bool haveError = errorCheckVisitor->isHaveError();
    delete errorCheckVisitor;
    if (haveError) {
        exit(1);
    }
}

void adjustAst(TranslationUnit *translationUnit) {
    // 把函数定义移至最后
    std::deque<Declaration *> declarationDeque;
    for (auto declaration : translationUnit->declarationList) {
        if (declaration->getClass() == DeclarationClass::FUNCTION_DEFINITION) {
            declarationDeque.push_back(declaration);
        } else {
            declarationDeque.push_front(declaration);
        }
    }
    translationUnit->declarationList = std::vector<Declaration *>(declarationDeque.begin(), declarationDeque.end());
}

void calculateAddress(SymbolTable *symbolTable, StringConstantPool *stringConstantPool) {
    std::uint64_t startAddress = 8; // 将内存区的前8个字节空出来
    stringConstantPool->calculateAddress(startAddress);
    startAddress += stringConstantPool->getMemoryUse();
    symbolTable->calculateAddress(startAddress);
}

InstructionSequence *generateCode(TranslationUnit *translationUnit, SymbolTable *symbolTable, StringConstantPool *stringConstantPool) {
    auto *codeGenerateVisitor = new CodeGenerateVisitor(symbolTable, stringConstantPool);
    translationUnit->accept(codeGenerateVisitor);
    InstructionSequence *instructionSequence = codeGenerateVisitor->moveInstructionSequence();
    delete codeGenerateVisitor;
    return instructionSequence;
}

void run(Bytecode *object) {
    VirtualMachine virtualMachine(object);
    virtualMachine.run();
}

void parseCommandLineArguments(int argc, char *argv[], bool &needCompile, bool &needPrintAst, bool &needOutputBytecodeFile, bool &needOutputBytecodeReadableFile, bool &needRun, std::string &sourceFilePath, std::string &bytecodeFilePath, std::string &bytecodeReadableFilePath) {
    std::string usage = "Usage:\n"
                        "   cc -c <input_file> [options]                 Load the input file as a source file, compile it and perform other operations according to the options\n"
                        "   cc -vm <input_file>                          Load the input file as a bytecode file and run it\n"
                        "   cc -h                                        Get help, display this information\n"
                        "Options:\n"
                        "                                                Defaults to compile and run when no option is selected\n"
                        "   -ast                                         Print abstract syntax tree\n"
                        "   -ob <output_file>                            Output bytecode file\n"
                        "   -obr <output_file>                           Output bytecode file in human-readable form\n"
                        "   -r                                           Run\n"
                        "Examples:\n"
                        "   cc -c main.c                                 Compile source file and run\n"
                        "   cc -c main.c -r                              Compile source file and run\n"
                        "   cc -c main.c -ob main.bin                    Compile source file and output bytecode file\n"
                        "   cc -c main.c -ast -obr main.txt -r           Compile source file, print abstract syntax tree and output bytecode file in human-readable form\n"
                        "   cc -vm main.bin                              Run bytecode file\n";
    if (argc < 2) {
        std::cout << "Missing command-line option and argument" << std::endl;
        std::cout << usage << std::endl;
        exit(1);
    }
    if (std::string(argv[1]) == "-c") {
        if (argc < 3) {
            std::cout << "Missing command-line option and argument" << std::endl;
            std::cout << usage << std::endl;
            exit(1);
        }
        needCompile = true;
        sourceFilePath = argv[2];
        if (argc < 4) {
            needRun = true;
            return;
        }
        int argIndex = 3;
        while (argIndex < argc) {
            if (std::string(argv[argIndex]) == "-r") {
                needRun = true;
                argIndex += 1;
            } else if (std::string(argv[argIndex]) == "-ast") {
                needPrintAst = true;
                argIndex += 1;
            } else if (std::string(argv[argIndex]) == "-ob") {
                if (argc == argIndex + 1) {
                    std::cout << "Missing argument for '-ob' option" << std::endl;
                    std::cout << usage << std::endl;
                    exit(1);
                }
                needOutputBytecodeFile = true;
                bytecodeFilePath = argv[argIndex + 1];
                argIndex += 2;
            } else if (std::string(argv[argIndex]) == "-obr") {
                if (argc == argIndex + 1) {
                    std::cout << "Missing argument for '-obr' option" << std::endl;
                    std::cout << usage << std::endl;
                    exit(1);
                }
                needOutputBytecodeReadableFile = true;
                bytecodeReadableFilePath = argv[argIndex + 1];
                argIndex += 2;
            } else {
                std::cout << "Unrecognized command-line option '" + std::string(argv[argIndex]) + "'" << std::endl;
                std::cout << usage << std::endl;
                exit(1);
            }
        }
    } else if (std::string(argv[1]) == "-vm") {
        if (argc < 3) {
            std::cout << "Missing command-line option and argument" << std::endl;
            std::cout << usage << std::endl;
            exit(1);
        }
        needRun = true;
        bytecodeFilePath = argv[2];
    } else if (std::string(argv[1]) == "-h") {
        std::cout << usage << std::endl;
        exit(0);
    }
    else {
        std::cout << usage << std::endl;
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    bool needCompile = false;
    bool needPrintAst = false;
    bool needOutputBytecodeFile = false;
    bool needOutputBytecodeReadableFile = false;
    bool needRun = false;
    std::string sourceFilePath;
    std::string bytecodeFilePath;
    std::string bytecodeReadableFilePath;
    parseCommandLineArguments(argc, argv, needCompile, needPrintAst, needOutputBytecodeFile, needOutputBytecodeReadableFile, needRun, sourceFilePath, bytecodeFilePath, bytecodeReadableFilePath);
    Bytecode *bytecode;
    if (needCompile) {
        std::unique_ptr<std::ifstream> sourceFile = std::make_unique<std::ifstream>(sourceFilePath);
        if (sourceFile->fail()) {
            std::cout << "Source file open failure" << std::endl;
            exit(1);
        }
        std::vector<char> *charList = preprocess(std::move(sourceFile));
        std::vector<Token> *tokenList = lexicalAnalyse(charList);
        TranslationUnit *translationUnit = syntaxAnalyse(tokenList);
        if (needPrintAst) {
            printAst(translationUnit);
        }
        SymbolTable *symbolTable = nullptr;
        StringConstantPool *stringConstantPool = nullptr;
        checkError(translationUnit, symbolTable, stringConstantPool);
        adjustAst(translationUnit);
        calculateAddress(symbolTable, stringConstantPool);
        InstructionSequence *instructionSequence = generateCode(translationUnit, symbolTable, stringConstantPool);
        bytecode = new Bytecode(symbolTable, stringConstantPool, instructionSequence);
        if (needOutputBytecodeFile) {
            std::unique_ptr<std::ofstream> bytecodeFile = std::make_unique<std::ofstream>(bytecodeFilePath, std::ios::binary);
            if (bytecodeFile->fail()) {
                std::cout << "Bytecode file open failure" << std::endl;
                exit(1);
            }
            bytecode->outputToFile(std::move(bytecodeFile));
        }
        if (needOutputBytecodeReadableFile) {
            std::unique_ptr<std::ofstream> bytecodeReadableFile = std::make_unique<std::ofstream>(bytecodeReadableFilePath, std::ios::binary);
            if (bytecodeReadableFile->fail()) {
                std::cout << "Bytecode file open failure" << std::endl;
                exit(1);
            }
            bytecode->outputToReadableFile(std::move(bytecodeReadableFile));
        }
        delete charList;
        delete tokenList;
        delete translationUnit;
        delete symbolTable;
        delete stringConstantPool;
        delete instructionSequence;
    } else {
        std::unique_ptr<std::ifstream> bytecodeFile = std::make_unique<std::ifstream>(bytecodeFilePath, std::ios::binary);
        if (bytecodeFile->fail()) {
            std::cout << "Bytecode file open failure" << std::endl;
            exit(1);
        }
        bytecode = new Bytecode();
        bytecode->loadFromFile(std::move(bytecodeFile));
    }
    if (needRun) {
        run(bytecode);
    }
    delete bytecode;
    return 0;
}