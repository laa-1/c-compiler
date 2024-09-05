# 技术文档

本文档主要针对于该项目的各个模块的技术原理进行介绍。

介绍的顺序将按照代码中各个模块的执行顺序进行，针对于每个流程所涉及的数据结构，如果有必要，将会在介绍对应模块之前对它们进行介绍。

## 预处理

Preprocessor 预处理类。

主要负责将文件所有字符读入内存、去除注释、去除换行符，最终会输出 `std::vector<std::vector<char>>`

每个 std::vector<char> 对应一行，不包含换行符，但对于单换行符的行也要一个行，会保留一个空的 std::vector<char>，方便词法分析解析行列号

## 词法分析

Lexer 词法分析类。

主要负责解析所有字符解析为一个一个 token，并标识好每个 token 的行列号

词法规则的说明位于 [lexicon.txt](lexicon.txt)，其中包含了34个关键词和45个运算符。

最终会输出 `std::vector<Token>`

## 语法分析

Parser 语法分析类。

主要负责解析所有 token，构建出 AST，同时还会把 token 中的行列号信息传递到 AST 中，最终会输出 AST 的根节点 TranslationUnit

语法规则的说明位于 [grammar.txt](grammar.txt)，其语法规则是基于 ISO-IEC 9899-1999 (E) 标准进行修改的，主要改动是去除了如 struct、union、elem 此类不支持的语法结构。

由于C语言的语法结构并不是完全契合 LL(1)、LR(1) 等简单的上下文无关文法，复杂文法的解析手写会十分复杂，故 Parser 类中采用的解析方法是手写的递归下降。


### AST 设计

普通的语法树（具体语法树）的结构与语言的语法结构是高度吻合的，且存在大量的很长的单链式结构，不利于后面的解析。

```text
additive-expression +
    multiplicative-expression
        cast-expression
            unary-expression
                postfix-expression
                    primary-expression: 1
    multiplicative-expression
        cast-expression
            unary-expression
                postfix-expression
                    primary-expression: 2
```

由于树的结构本身就隐含了优先级，因此语法树的节点类可以进行缩减和合并，此外，通过合理的继承和多态，可以消除语法树的长单链式结构，更加方便后面的解析，由此便得到了抽象语法树 AST。

AST 的节点类除了 TranslationUnit 是用于作为整个 AST 的根节点以外，会分为下面几大类：
* Expression
* Type
* Declaration
* Statement

每一种都有一个与之对应的抽象基类，每个 AST 节点的具体子类都继承于它们的其中之一。

每个 AST 节点具体子类的成员变量都是由普通类型或上面的抽象基类指针组成，通过多态的特性来指针真正的子节点，而不是直接由 AST 节点的具体子类直接进行组合。

这样可以消除长单链式结构，同时也会让 AST 的结构非常地灵活，可以进行各种嵌套，表达能力更强

### 左递归

以 additive-expression 的产生式为例：

```text
additive-expression:
	additive-expression + multiplicative-expression
	additive-expression - multiplicative-expression
	multiplicative-expression
```

下面是一个完全按照产生式规则的普通递归下降法解析左递归的函数（已经简化过的）：

```c++
void parseAdditiveExpression() {
    parseAdditiveExpression();
    if (nextToken == "+") {
        parseMultiplicativeExpression();
        return;
    }
    rollback();
    parseAdditiveExpression();
    if (nextToken == "-") {
        parseMultiplicativeExpression();
        return;
    }
    rollback();
    parseMultiplicativeExpression();
}
```

显然，上面的函数会陷入到无限递归当中，但实际上只需要稍微进行修改，也能在继续使用递归下降法的前提下实现对左递归产生式进行解析。

注意到，上面的 additive-expression 产生式实际等价于以至少有一个 `multiplicative-expression` 作为前缀，后续紧接零个或多个 `+ multiplicative-expression` 或 `- multiplicative-expression`，这实际上也是消除左递归的原理。

根据于此，可修改得到下面能正常解析左递归的递归下降函数（已经简化过的）：

```c++
void parseAdditiveExpression() {
    parseMultiplicativeExpression();
    while (true) {
        if (nextToken == "+") {
            parseMultiplicativeExpression();
        } else if (nextToken == "-") {
            parseMultiplicativeExpression();
        } else {
            return;
        }
    }
}
```

Parser 类中针对所有的左递归产生式均采用上面方法来消除左递归。

### 嵌套式复杂声明的语法解析

C 语言的变量和函数声明分为两部分组成。

前半部分由下面的其中之一组成：
* void
* char
* short
* int
* long
* long long
* float
* double
* signed int
* signed long
* signed long long
* unsigned int
* unsigned long
* unsigned long long

后半部分则由下面的任意嵌套组成（xxx代表的是更里面一层的类型声明）：

* \*xxx
* xxx[]
* xxx()

它们分别对应指针、数组、函数的类型声明

函数和数组的优先级的一致的，指针的优先级低一级，可以通过括号控制优先级。

与人类的阅读习惯不同的是，在类型声明的嵌套中，里层才是顶层类型声明，而外层反而是底层类型声明，这也是分析 C 语言声明要先找到里层的第一个标识符，再逐层往外分析的原因。

观察下面的一个复杂声明以及它所解析出来的 AST 就可以印证上面的说法：

```c
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

这种结构实际上完美契合自底向上的解析过程，更方便自底向上类型的解析器的实现。

但 Parser 类中采用的是递归下降法，是自顶向下的，如果采用普通的解析方法，内层的类型声明会先解析出来作为子树，导致内层的类型声明成为底层，刚好反过来。

解决办法是借助栈来临时存储解析出来的类型，再由上层来构造嵌套顺序正确的 AST 子树。

## 错误检查

ErrorCheckVisitor 错误检查类。

主要负责类型推导、类型检查、赋值检查、标识符和作用域检查、控制流检查等，同时还要构建出符号表和字符串常量池，最终输出符号表和字符串常量池

表达式的计算结果也是具有类型的，类型检查需要依赖于表达式的结果的类型推导，因此在类型检查的过程中需要对表达式的 AST 节点的结果类型进行补充。

### Visitor 模式

在构造好的 AST 中，存在大量的不同类型的节点类，后续的针对 AST 处理逻辑如果写成每个节点类虚继承的成员函数，会导致这部分处理逻辑分裂在各个类中，而且状态类型的数据很难在它们之间进行传递。

而 Visitor 模式能很好地将 AST 的复杂数据结构（节点类）与复杂处理逻辑进行分离，传递状态数据也更加容易。

因此后续的处理逻辑如果涉及多个不同类型的节点类进行操作，都以 Visitor 模式进行处理，如 AST 打印类 PrintVisitor，错误检查类 ErrorCheckVisitor 和 代码生成类 CodeGenerateVisitor。

### 符号表

在 C 语言中，每个符号都有属于自己的作用域，而不同的作用域存在并列及包含关系，是符合多叉树的结构的，因此 Scope 类也是按照多叉树的结构来进行设计的。

由于在查询一个符号时需要在当前作用域和所有上层作用域进行查找，因此 Scope 类额外添加了父节点指针，并且重载了下标运算符来实现从当前作用域及递归向上的所有上层作用域中查找符号。

考虑到作用域嵌套的复杂性，设计了 SymbolTableBuilder 类来辅助符号表的构建。

### 运行时类型识别

前面提到 AST 的节点中的成员是以多态形式，即基类指针指向子类对象，而后续的处理逻辑需要识别是哪个具体子类，且是运行时才能确定的，需要运行时类型识别的机制，即 RTTI。

由于 C++ 的 RTTI 机制并不完善，ISO 标准只规定了对应的接口，各大编译器之间的实现和所展现出来的功能也有所差异，故没有采用 typeid、type_info、dynamic_cast 等 RTTI 相关特性，而是通过一个简单的类类型枚举来实现运行时的类型识别，并配合 C 风格的强制类型转换得到真正子类对象，因为实际上也用不上过多的运行时对象信息，只需要知道其类型就足够了。

### 左值和右值

对于表达式的计算结果，有一部分是在内存中有对应值的，如 `a[1]`，而另一部分则是未必是如此，如 `a + b`，通常是存储在寄存器等临时存储中（这里则是操作数栈中），前者被称为左值，后者则是右值，前者可以被赋值，后者则不能。判断一个表达式能否被赋值和取地址也依赖于此，也需要在类型检查中进行推导和检查。

## 虚拟机设计

采用的架构是栈式虚拟机

虚拟机在加载字节码时会初始化数据区和代码区，同时支持 C 语言代码利用函数指针在运行时对代码区中函数首地址所对应的指令进行实时译指令并执行

### 数据类型

C 语言中的数据类型存在虽然在不同平台上具体的位数并不确定，但目前主流是分为8-64位整形，8-64位无符号整形，32位和64位浮点数，这里的虚拟机的数据区也会划分为这 10 种数据类型。

操作数栈中的数据类型则只分为64位整形、64位无符号整形、64位浮点数这三种，主要是为了缩减指令数量，因为同一功能的指令有针对不同数据类型的版本，数据类型过多会导致指令数量暴增。

数据区和操作数栈之间的数据转移由 load 指令和 store 指令完成，而数据类型的扩展和压缩也由它们来完成。

### 指令设计

栈式虚拟机所有的指令的操作数几乎都隐含在操作数栈中，只有 push 指令需要从外部获取操作数，需要将操作数编码到指令当中

除了常见的运算指令外，有部分特殊的指令，如针对于部分指令对操作数顺序敏感的问题，设计了 swap 指令用于交换栈顶两个操作数的顺序。针对于某些特殊的需要，设计了 copy 指令用于复制栈顶值等。

### 内存设计

C 语言中是可以直接操作到内存地址的，并且指针可以参与运算，类似于 JVM 虚拟机的那种划分多个局部变量表的模式并不适合。因此内存模型的设计更加偏向于平坦设计，只划分了代码区和数据区，所有的变量均放在同一块连续的内存区域中。

对于全局作用域的变量，它的地址是在编译期就能确定的，它在指令中编码的是绝对地址

对于局部变量，由于函数的调用层级无法在编译期确定，因此它的地址也就无法在编译期确定，但是可以在编译期确定它相对于函数调用时的起始地址的相对地址，它在指令中编码的是相对地址

函数调用时的起始地址称为基地址，通过下面公式可以计算出局部变量的绝对地址

```text
基地址(bp) + 局部变量相对地址 = 局部变量绝对地址
```

以下面的一段简单的递归求阶乘程序来分析虚拟机是如何利用基地址(bp)确定变量地址的：

```c
int num = 2;

int factorial(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

int main() {
    int a;
    int result = factorial(num);
    return 0;
}
```

数据区的前 8 个字节是保留的，不存放数据

全局变量 num 的绝对地址为 8

局部变量 n 的相对地址为 0，局部变量 a 的相对地址为 0，局部变量 result 的相对地址为 4。

假设 factorial 函数入口地址为 72，main 函数的入口地址为 226，可以得到下面的函数内存使用映射表（不包含内建函数）。

```text
-----MEMORY USE MAP-----
0                   12
72                  4
226                 8
```

它代表的是全局作用域和每个函数所需要的最大内存使用，其中 0 是指全局作用域（至于为什么是 0，后面会进行解释）。

全局作用域使用了 12 字节，factorial 函数使用了 4 字节，main 函数使用了 8 字节。

执行流程如下（由于 factorial 函数是递归调用，故使用单引号来区分不同层级的函数和形参）：

+ bp 初始化为 0
+ num 实际地址 = 8
+ 调用 main 函数，bp += 12
+ a 实际地址 = 12
+ result 实际地址 = 16
+ 调用 factorial' 函数，bp += 8
+ n' 实际地址 = 20
+ 调用 factorial'' 函数，bp += 4
+ n'' 实际地址 = 24
+ factorial'' 函数返回，bp -= 4
+ n' 实际地址 = 20
+ factorial' 函数返回，bp -= 8
+ result 实际地址 = 16
+ main 函数返回
+ 程序结束

上面的流程很清晰地展示了 functionMemoryUseMap 和 bp 如何配合来实现局部变量的寻址，而 fbp 指令就是用于获取 bp 的值的指令。

## 代码生成

CodeGenerateVisitor 代码生成类。

主要负责解析 AST、符号表和字符串常量池，最终输出指令序列

### 指令执行流程

虚拟机会从指令序列的第一条开始顺序执行。

以上面的递归求阶乘程序所生成的指令序列的开头一个小片段为例：

```text
0                   push_64             8
10                  fbp
12                  add_u64
14                  push_64             2
24                  store_i32
26                  push_64             226
36                  call
38                  hlt
……                  ……
```

可以看到在执行 main 函数之前需要对全局变量进行初始化，之后才是调用 main 函数，返回后就停机。

因此需要在代码生成之前，需要将所有的函数定义移至后半部分，其他的外部声明会集中到前半部分，并在函数定义的代码之前插入调用 main 函数的指令以及停机指令。

前面的函数内存使用映射表中，起始地址对应为 0 的函数实际上是在 main 函数前面执行的用于初始化全局变量的函数

```text
-----MEMORY USE MAP-----
0                   12
72                  4
226                 8
```

### 往后跳转的跳转指令

跳转指令的跳转分为往前跳转和往后跳转，对于前者，实现起来很简单，但对于后者，跳转到的指令的地址是无法提前确定的，解决办法是通过临时设置占位地址的方式，等到被跳转的指令地址确定后，再回过来打补丁，修改前面设置的占位地址。

## 字节码

字节码由函数内存使用映射表、数据区、代码区组成

函数内存使用映射表，前面已经介绍过了

数据区，由字符串常量池序列化而成，二进制形式存储（代码中表示为`std::vector<std::uint8_t>`），按顺序存储所有字符串常量，在虚拟机加载字节码时会将这一部分放到虚拟机的数据区开头

代码区，由指令序列序列化而成，二进制形式存储（代码中表示为`std::vector<std::uint8_t>`），按顺序存储所有指令，在虚拟机加载字节码时会将这一部分放到虚拟机的代码区

## 内建函数

C 语言本身是没有支持输入和输出的相关语法的，scanf 和 printf 是封装系统调用的库函数，而这里的虚拟机并没有设计类似于 JVM 的 JNI 机制，无法直接与系统调用进行交互。

为了实现输入和输出的功能的同时又不修改 C 语言本身的语法规则，通过修改符号表和指令序列的方式内建了 8 个函数，分别是：
* scan_i64
* scan_u64
* scan_f64
* scan_s
* print_i64
* print_u64
* print_f64
* print_s

由于它们的功能十分简单，通过优化传参的过程，实际上每个函数算上 ret 指令只用了两条指令。

## 最后

更多的细节可以查看本项目的源代码。
