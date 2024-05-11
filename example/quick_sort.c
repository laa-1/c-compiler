/**
 * 快速排序
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