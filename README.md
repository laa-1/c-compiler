# c-compiler

一个 C 语言编译器和虚拟机，支持除结构体以外的语法，支持字节码导出和直接运行字节码，内建I/O函数，

包含了词法分析、语法分析、构建 AST、类型推导和检查、错误检查、错误信息输出、字节码生成、虚拟机执行等多个模块。

完全使用 C++ 手写实现，不使用任何编译器构建工具，没有任何除了 C++ 标准库外的依赖。

[点击这里](doc/doc.md) 可以查看介绍本项目的设计和原理的技术文档

## 安装

### 二进制可执行文件

[点击这里](https://github.com/laa-1/c-compiler/releases) 可以跳转 Releases 找到构建好的 Windows 平台的二进制可执行文件。

### 从源代码构建

环境要求：

* C++20

由于没有除标准库以外的依赖，根据 CMakeLists.txt 的内容进行构建即可 

## 使用

使用 `cc -h` 可以获取帮助来查看用法。

```text
Usage:
   cc -cl <input_file> [options]                        Compile mode, compile source file and performing other operations depending on the options
   cc -vm <input_file>                                  Virtual machine mode, run binary bytecode file
   cc -h                                                Get help, display this information
Options:
                                                        Defaults to run when no option is selected
   -r                                                   Run
   -o <output_file>                                     Output binary bytecode file
   -oh <output_file>                                    Output human-readable bytecode file
   -ast                                                 Print abstract syntax tree
Examples:
   cc -c main.c                                         Compile source file and run
   cc -c main.c -r                                      Compile source file and run
   cc -c main.c -ast -o main.bin -oh main.txt -r        Compile source file, print abstract syntax tree, output binary bytecode file, output human-readable bytecode file and run
   cc -vm main.bin                                      Run binary bytecode file
```

## 示例

一个简单的快速排序和二分查找：

```c
/**
 * 二分查找
 */

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

int binary_search(int *nums, int start, int end, int value) {
    for (int left = start, right = end, middle = left + (right - left) / 2; left <= right; middle = left + (right - left) / 2) {
        if (nums[middle] < value) {
            left = middle + 1;
        } else if (nums[middle] > value) {
            right = middle - 1;
        } else {
            return middle;
        }
    }
    return -1;
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
    print_s("\n");
    int location = binary_search(nums, 0, 9, 13);
    print_s("location: ");
    print_i64(location);
    print_s("\n");
    return 0;
}
``` 

```text
unsorted: 13 17 15 19 18 10 14 12 16 11 
sorted: 10 11 12 13 14 15 16 17 18 19 
location: 3
```

打印一个复杂声明的 AST：

```c
/**
 * 文档中的示例程序
 */

int main() {
    char (*(*p[3])())[5];
    return 0;
}
```

```text
TranslationUnit 5:1
    declaration: FunctionDefinition 5:1
        functionType: FunctionType 5:5
            returnType: ScalarType 5:1
                baseType: int
        identifier: main
        body: CompoundStatement 5:12
            statement: DeclarationStatement 6:5
                declaration: VariableDeclaration 6:5
                    variableType: ArrayType 6:14
                        elemType: PointerType 6:13
                            sourceType: FunctionType 6:12
                                returnType: PointerType 6:11
                                    sourceType: ArrayType 6:10
                                        elemType: ScalarType 6:5
                                            baseType: char
                                        size: 5
                        size: 3
                    identifier: p
            statement: ReturnStatement 7:5
                value: IntLiteralExpression 7:12
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

本项目的语法是根据 ISO-IEC 9899-1999 (E) 标准进行设计，实际的经过修改后语法规则可以查看 [grammar.txt](doc/grammar.txt)。

* 不支持 struct、union、enum、typedef 相关的语法特性。
* 不支持变长参数，如 `(int a, ...)`
* 不支持动态数组，如 `a[n]`。
* typedef、extern、static、auto、register、inline、restrict、volatile 修饰符无实际效果，但不影响正常编译。
* sizeof 运算符不支持对类型求字节数，但可以对表达式求字节数。

其他语法特性都是支持的，如任意形式嵌套的声明、函数指针、指针运算、多维数组、字符串初始化、隐式类型转换、后置递增、逗号表达式、for 语句空条件等等均支持。