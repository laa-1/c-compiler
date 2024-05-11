/**
 * 交通灯状态机，测试复杂声明
 */

// 定义每个状态的操作函数
void redLightAction() {
    print_s("Red Light - Stop\n");
}

void yellowLightAction() {
    print_s("Yellow Light - Caution\n");
}

void greenLightAction() {
    print_s("Green Light - Go\n");
}

// 定义状态转换函数
void (*toYellow())() {
    return yellowLightAction;
}

void (*toGreen())() {
    return greenLightAction;
}

void (*toRed())() {
    return redLightAction;
}

int main() {
    // 当前状态
    void (*currentState)() = redLightAction;
    // 状态转换函数数组（返回函数指针的函数指针数组）
    void (*(*transitions[3])())() = {toYellow, toGreen, toRed};
    // 模拟状态机的运行
    for (int i = 0; i < 6; i++) {
        // 执行当前状态的操作
        currentState();
        // 使用状态转换函数数组来决定下一个状态
        currentState = transitions[i % 3](); // 交替转换状态
    }
    return 0;
}

