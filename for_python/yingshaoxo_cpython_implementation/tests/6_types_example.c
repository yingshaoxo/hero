#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 类型标记枚举
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_LIST,
    TYPE_DICT
} ValueType;

// 通用值结构体
typedef struct {
    ValueType type;
    union {
        int64_t int_val;
        double float_val;
        _Bool bool_val;
        char* string_val;      // 堆分配字符串
        struct List* list_val; // 嵌套列表
        struct Dict* dict_val; // 嵌套字典
    } data;
} Any;

// 动态数组结构体
typedef struct {
    Any* items;
    size_t length;
    size_t capacity;
} List;

// 创建新列表
List* list_new(size_t capacity) {
    List* list = malloc(sizeof(List));
    list->items = malloc(sizeof(Any) * capacity);
    list->length = 0;
    list->capacity = capacity;
    return list;
}

// 添加元素到列表
void list_append(List* list, Any item) {
    if (list->length >= list->capacity) {
        list->capacity *= 2;
        list->items = realloc(list->items, sizeof(Any) * list->capacity);
    }
    list->items[list->length++] = item;
}

// 创建字符串类型
Any create_string(const char* s) {
    Any any;
    any.type = TYPE_STRING;
    any.data.string_val = strdup(s); // 堆内存拷贝
    return any;
}

// 释放内存（简化版）
void free_any(Any any) {
    if (any.type == TYPE_STRING) {
        free(any.data.string_val);
    }
    // 其他类型处理省略...
}

int main() {
    List* mylist = list_new(2);
    
    // 添加不同类型
    Any num = {.type = TYPE_INT, .data.int_val = 42};
    list_append(mylist, num);
    
    list_append(mylist, create_string("world"));
    
    // 类型检查访问
    for (size_t i=0; i<mylist->length; i++) {
        Any item = mylist->items[i];
        switch(item.type) {
            case TYPE_INT:
                printf("Int: %ld\n", item.data.int_val);
                break;
            case TYPE_STRING:
                printf("String: %s\n", item.data.string_val);
                break;
            // 其他类型处理省略...
        }
    }
    
    // 清理内存
    for (size_t i=0; i<mylist->length; i++) {
        free_any(mylist->items[i]);
    }
    free(mylist->items);
    free(mylist);
    
    return 0;
}
