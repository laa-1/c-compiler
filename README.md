# C Compiler

一个带有虚拟机的 C 语言编译器，支持除了结构体以外的大部分语法，附带输入和输出的库函数。

完全使用 C++ 手写实现，不使用任何编译器构建工具，没有任何除了 C++ 标准库外的依赖。

附带了详细的技术文档介绍本项目的设计和原理，你可以[点击这里](docs/doc.md)找到它。

## 安装

### 二进制可执行文件

目前只提供 Windows 平台的二进制可执行文件，更推荐从源代码构建。

你可以在 release 中找到构建好的二进制可执行文件。

### 从源代码构建

环境要求：

* C++14 及以上（使用了 C++14 的 std::make_unique 特性）

由于没有库依赖，使用 CMake 等进行构建的步骤很简单（下面以 Linux 系统为例）：

```shell
mkdir build
cd build
cmake ..
make
```

Windows 平台可以直接利用 IDE 等方式来构建本项目。

## 使用

编译源文件并运行。

```shell
cc -c main.c
```

直接运行字节码文件。

```shell
cc -vm main.bin
```

编译源文件并输出人类可阅读形式的字节码文件。

```shell
cc -c main.c -obr main.txt
```

使用 `cc -h` 可以获取帮助以查看更多用法。

```shell
Usage:
   cc -c <input_file> [options]                 Load the input file as a source file, compile it and perform other operations according to the options
   cc -vm <input_file>                          Load the input file as a bytecode file and run it
   cc -h                                        Get help, display this information
Options:
                                                Defaults to compile and run when no option is selected
   -ast                                         Print abstract syntax tree
   -ob <output_file>                            Output bytecode file
   -obr <output_file>                           Output bytecode file in human-readable form
   -r                                           Run
Examples:
   cc -c main.c                                 Compile source file and run
   cc -c main.c -r                              Compile source file and run
   cc -c main.c -ob main.bin                    Compile source file and output bytecode file
   cc -c main.c -ast -obr main.txt -r           Compile source file, print abstract syntax tree and output bytecode file in human-readable form
   cc -vm main.bin                              Run bytecode file
```

## 示例

一个简单的快速排序：

```c
void swap(int *p1, int *p2) {
    int tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}

void quick_sort(int *nums, int start, int end) {
    if (start >= end) {
        return;
    }
    int base = start;
    int left = start;
    int right = end;
    while (left < right) {
        while (nums[right] >= nums[base] && left < right) {
            right--;
        }
        while (nums[left] <= nums[base] && left < right) {
            left++;
        }
        swap(nums + left, nums + right);
    }
    swap(nums + base, nums + left);
    quick_sort(nums, start, left - 1);
    quick_sort(nums, left + 1, end);
}

int main() {
    int nums[10] = {13, 17, 15, 19, 18, 10, 14, 12, 16, 11};
    print_s("unsorted: ");
    for (int i = 0; i < 10; i++) {
        print_i64(nums[i]);
        print_s(" ");
    }
    print_s("\n");
    quick_sort(nums, 0, 9);
    print_s("sorted: ");
    for (int i = 0; i < 10; i++) {
        print_i64(nums[i]);
        print_s(" ");
    }
    return 0;
}
``` 

```text
unsorted: 13 17 15 19 18 10 14 12 16 11 
sorted: 10 11 12 13 14 15 16 17 18 19
```

打印一个复杂声明的 AST：

```c
int main() {
    char (*(*p[3])())[5];
    return 0;
}
```

```text
TranslationUnit
    declaration: FunctionDefinition
        functionType: FunctionType
            returnType: ScalarType
                baseType: int
        identifier: main
        body: CompoundStatement
            statement: DeclarationStatement
                declaration: VariableDeclaration
                    variableType: ArrayType
                        elemType: PointerType
                            sourceType: FunctionType
                                returnType: PointerType
                                    sourceType: ArrayType
                                        elemType: ScalarType
                                            baseType: char
                                        size: 5
                        size: 3
                    identifier: p
            statement: ReturnStatement
                value: IntLiteralExpression
                    value: 0
```

更多的示例可以在 example 目录下找到。

## 内建函数

编译器中内建 8 个函数，用于输入和输出。

无需引入头文件或添加额外声明，直接调用即可。

内建函数也会参与类型检查，需要注意传参类型。

下面是它们的声明原型：

```c
void scan_i64(long long int *address);
void scan_u64(unsigned long long int *address);
void scan_f64(double *address);
void scan_s(char *address);
void print_i64(long long int value);
void print_u64(unsigned long long int value);
void print_f64(double value);
void print_s(char *address);
```

## 不支持的语法

本项目的语法是根据 ISO-IEC 9899-1999 (E) 标准进行设计，实际的经过修改后语法规则可以查看[grammar.txt](docs/grammar.txt)。

* 不支持 struct、enum、typedef 相关的语法特性。
* 不支持变长参数，如 `(int a, ...)`
* 不支持动态数组，如 `a[n]`。
* 不支持 auto、register、restrict、volatile 关键词。（这几个关键词十分冷门，几乎没有人会使用）
* extern、static、inline 修饰符不会生效，但不影响正常编译。
* sizeof 运算符不支持对类型求字节数，但可以对表达式求字节数。
* 后置递增实际上与前置递增行为一致。

除了上面提及的以外，其他语法特性都是支持的，如函数指针、逗号表达式、for 语句空条件、指针偏移运算、字符串初始化、隐式类型转换等等均支持。
