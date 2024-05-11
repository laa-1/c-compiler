/**
 * 文档中的示例程序
 */

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
    print_i64(result);
    return 0;
}