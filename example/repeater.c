/**
 * 复读机，测试输入和输出
 */

int main() {
    char buffer[100];
    while (buffer[0] != 'q' || buffer[1] != '\0') {
        scan_s(buffer);
        print_s(buffer);
        print_s("\n");
    }
    return 0;
}