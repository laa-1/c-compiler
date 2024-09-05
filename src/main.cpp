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
#include "error/ErrorHandler.h"
#include "address/AddressCalculator.h"

enum class Mode {
    COMPILE,
    VIRTUAL_MACHINE
};

void parseCommandLineArguments(int argc, char *argv[], Mode &mode, bool &needRun, bool &needOutputBinaryBytecodeFile, bool &needOutputHumanReadableBytecodeFile, bool &needPrintAst, std::string &inputFilePath, std::string &binaryBytecodeOutputFilePath, std::string &humanReadableBytecodeOutputFilePath) {
    std::string usage = "Usage:\n"
                        "   cc -cl <input_file> [options]                        Compile mode, compile source file and performing other operations depending on the options\n"
                        "   cc -vm <input_file>                                  Virtual machine mode, run binary bytecode file\n"
                        "   cc -h                                                Get help, display this information\n"
                        "Options:\n"
                        "                                                        Defaults to run when no option is selected\n"
                        "   -r                                                   Run\n"
                        "   -o <output_file>                                     Output binary bytecode file\n"
                        "   -oh <output_file>                                    Output human-readable bytecode file\n"
                        "   -ast                                                 Print abstract syntax tree\n"
                        "Examples:\n"
                        "   cc -c main.c                                         Compile source file and run\n"
                        "   cc -c main.c -r                                      Compile source file and run\n"
                        "   cc -c main.c -ast -o main.bin -oh main.txt -r        Compile source file, print abstract syntax tree, output binary bytecode file, output human-readable bytecode file and run\n"
                        "   cc -vm main.bin                                      Run binary bytecode file\n";
    if (argc < 2) {
        std::cout << "Missing command-line option and argument" << std::endl;
        std::cout << usage << std::endl;
        exit(1);
    }
    if (std::string(argv[1]) == "-cl") {
        mode = Mode::COMPILE;
        if (argc < 3) {
            std::cout << "Missing command-line option and argument" << std::endl;
            std::cout << usage << std::endl;
            exit(1);
        }
        inputFilePath = argv[2];
        if (argc < 4) {
            needRun = true;
            return;
        }
        int argIndex = 3;
        while (argIndex < argc) {
            if (std::string(argv[argIndex]) == "-r") {
                needRun = true;
                argIndex += 1;
            } else if (std::string(argv[argIndex]) == "-o") {
                if (argc == argIndex + 1) {
                    std::cout << "Missing argument for '-o' option" << std::endl;
                    std::cout << usage << std::endl;
                    exit(1);
                }
                needOutputBinaryBytecodeFile = true;
                binaryBytecodeOutputFilePath = argv[argIndex + 1];
                argIndex += 2;
            } else if (std::string(argv[argIndex]) == "-oh") {
                if (argc == argIndex + 1) {
                    std::cout << "Missing argument for '-oh' option" << std::endl;
                    std::cout << usage << std::endl;
                    exit(1);
                }
                needOutputHumanReadableBytecodeFile = true;
                humanReadableBytecodeOutputFilePath = argv[argIndex + 1];
                argIndex += 2;
            } else if (std::string(argv[argIndex]) == "-ast") {
                needPrintAst = true;
                argIndex += 1;
            } else {
                std::cout << "Unknown command-line option '" + std::string(argv[argIndex]) + "'" << std::endl;
                std::cout << usage << std::endl;
                exit(1);
            }
        }
    } else if (std::string(argv[1]) == "-vm") {
        mode = Mode::VIRTUAL_MACHINE;
        if (argc < 3) {
            std::cout << "Missing command-line option and argument" << std::endl;
            std::cout << usage << std::endl;
            exit(1);
        }
        needRun = true;
        inputFilePath = argv[2];
    } else if (std::string(argv[1]) == "-h") {
        std::cout << usage << std::endl;
        exit(0);
    } else {
        std::cout << usage << std::endl;
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    Mode mode;
    bool needRun = false;
    bool needOutputBinaryBytecodeFile = false;
    bool needOutputHumanReadableBytecodeFile = false;
    bool needPrintAst = false;
    std::string inputFilePath;
    std::string binaryBytecodeOutputFilePath;
    std::string humanReadableBytecodeOutputFilePath;
    parseCommandLineArguments(argc, argv, mode, needRun, needOutputBinaryBytecodeFile, needOutputHumanReadableBytecodeFile, needPrintAst, inputFilePath, binaryBytecodeOutputFilePath, humanReadableBytecodeOutputFilePath);
    switch (mode) {
        case Mode::COMPILE: {
            std::unique_ptr<std::ifstream> sourceFile = nullptr;
            std::vector<std::vector<char>> *charLineList = nullptr;
            std::vector<Token> *tokenList = nullptr;
            TranslationUnit *translationUnit = nullptr;
            SymbolTable *symbolTable = nullptr;
            StringConstantPool *stringConstantPool = nullptr;
            InstructionSequence *instructionSequence = nullptr;
            Bytecode *bytecode = nullptr;
            sourceFile = std::make_unique<std::ifstream>(inputFilePath);
            if (sourceFile->fail()) {
                std::cout << "Source file open failure" << std::endl;
                exit(1);
            }
            charLineList = Preprocessor::process(std::move(sourceFile));
            tokenList = Lexer::analysis(charLineList);
            translationUnit = Parser::analysis(tokenList);
            if (needPrintAst) {
                PrintVisitor::print(translationUnit);
            }
            ErrorCheckVisitor::checkError(translationUnit, symbolTable, stringConstantPool);
            AddressCalculator::calculate(symbolTable, stringConstantPool);
            instructionSequence = CodeGenerateVisitor::generateCode(translationUnit, symbolTable, stringConstantPool);
            bytecode = Bytecode::build(symbolTable, stringConstantPool, instructionSequence);
            if (needOutputBinaryBytecodeFile) {
                std::unique_ptr<std::ofstream> binaryBytecodeFile = std::make_unique<std::ofstream>(binaryBytecodeOutputFilePath, std::ios::binary);
                if (binaryBytecodeFile->fail()) {
                    std::cout << "Bytecode file open failure" << std::endl;
                    exit(1);
                }
                bytecode->outputToBinaryFile(std::move(binaryBytecodeFile));
            }
            if (needOutputHumanReadableBytecodeFile) {
                std::unique_ptr<std::ofstream> humanReadableBytecodeFile = std::make_unique<std::ofstream>(humanReadableBytecodeOutputFilePath, std::ios::binary);
                if (humanReadableBytecodeFile->fail()) {
                    std::cout << "Bytecode file open failure" << std::endl;
                    exit(1);
                }
                bytecode->outputToHumanReadableFile(std::move(humanReadableBytecodeFile));
            }
            if (needRun) {
                VirtualMachine::run(bytecode);
            }
            delete charLineList;
            delete tokenList;
            delete translationUnit;
            delete symbolTable;
            delete stringConstantPool;
            delete instructionSequence;
            delete bytecode;
            break;
        }
        case Mode::VIRTUAL_MACHINE: {
            Bytecode *bytecode = nullptr;
            std::unique_ptr<std::ifstream> bytecodeFile = std::make_unique<std::ifstream>(inputFilePath, std::ios::binary);
            if (bytecodeFile->fail()) {
                std::cout << "Bytecode file open failure" << std::endl;
                exit(1);
            }
            bytecode = Bytecode::build(std::move(bytecodeFile));
            VirtualMachine::run(bytecode);
            delete bytecode;
            break;
        }
    }
    return 0;
}