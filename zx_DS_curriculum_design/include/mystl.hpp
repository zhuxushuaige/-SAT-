#pragma once

#include <cstddef> // for size_t
#include <cstdlib> // for malloc, free, realloc
#include <cstring> // for memset, memcpy
#include <new>     // for std::bad_alloc (optional for error handling)

// 简化的 std::vector<int> 实现 
class MyVectorInt {
private:
    int* data_;
    size_t size_;
    size_t capacity_;

    void reallocate(size_t new_capacity) {
        if (new_capacity <= capacity_) return;
        int* new_data = static_cast<int*>(realloc(data_, new_capacity * sizeof(int)));
        if (!new_data) {
            // Handle realloc failure, do not change state
            // Could throw std::bad_alloc(new) if exceptions are allowed and desired
            return;
        }
        data_ = new_data;
        capacity_ = new_capacity;
    }

public:
    MyVectorInt() : data_(nullptr), size_(0), capacity_(0) {}

    explicit MyVectorInt(size_t initial_capacity) : data_(nullptr), size_(0), capacity_(0) {
        if (initial_capacity > 0) {
            data_ = static_cast<int*>(malloc(initial_capacity * sizeof(int)));
            if (!data_) {
                capacity_ = 0;
            } else {
                capacity_ = initial_capacity;
            }
        }
    }

    ~MyVectorInt() {
        free(data_);
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    }

    // 禁止拷贝构造和赋值（简化处理）
    MyVectorInt(const MyVectorInt&) = delete;
    MyVectorInt& operator=(const MyVectorInt&) = delete;

    void push_back(int value) {
        if (size_ >= capacity_) {
            size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            reallocate(new_capacity);
            if (size_ >= capacity_) return; // realloc failed
        }
        data_[size_] = value;
        size_++;
    }

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    // 访问元素（无边界检查）
    int& operator[](size_t index) { return data_[index]; }
    const int& operator[](size_t index) const { return data_[index]; }

    // 为范围 for 循环提供支持 (C++11)
    int* begin() { return data_; }
    int* end() { return data_ + size_; }
    const int* begin() const { return data_; }
    const int* end() const { return data_ + size_; }
};

//  简化的 std::queue<int> 实现 
// 使用循环数组实现
class MyQueueInt {
private:
    int* data_;
    size_t capacity_;
    size_t front_index_;
    size_t back_index_;
    size_t size_;

public:
    MyQueueInt() : MyQueueInt(10000) {} // 默认容量

    explicit MyQueueInt(size_t max_elements)
        : data_(nullptr), capacity_(max_elements), front_index_(0), back_index_(0), size_(0) {
        if (max_elements > 0) {
            data_ = static_cast<int*>(malloc(max_elements * sizeof(int)));
            if (!data_) {
                capacity_ = 0;
            }
        }
    }

    ~MyQueueInt() {
        free(data_);
        data_ = nullptr;
    }

    // 禁止拷贝构造和赋值
    MyQueueInt(const MyQueueInt&) = delete;
    MyQueueInt& operator=(const MyQueueInt&) = delete;

    void push(int value) {
        if (size_ >= capacity_ || !data_) return; // Queue full or not allocated
        data_[back_index_] = value;
        back_index_ = (back_index_ + 1) % capacity_;
        size_++;
    }

    void pop() {
        if (empty()) return;
        front_index_ = (front_index_ + 1) % capacity_;
        size_--;
    }

    // 获取队首元素（无检查）
    int& front() { return data_[front_index_]; }
    const int& front() const { return data_[front_index_]; }

    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }
};

//  简化的 std::unordered_set<int> 实现 
// 使用开放寻址法 (线性探测) 的哈希表实现
class MyUnorderedSetInt {
private:
    struct Bucket {
        int key;
        bool occupied;
        bool deleted; // Lazy deletion
        Bucket() : key(0), occupied(false), deleted(true) {}
    };

    Bucket* buckets_;
    size_t capacity_;
    size_t size_;
    static const size_t DEFAULT_INITIAL_CAPACITY = 16;
    static const size_t MAX_LOAD_FACTOR_PERCENT = 75;

    size_t hash(int key) const {
        unsigned int ukey = static_cast<unsigned int>(key);
        return ukey % capacity_;
    }

    size_t findIndex(int key) const {
        if (capacity_ == 0) return 0;
        size_t index = hash(key);
        size_t original_index = index;
        do {
            if (!buckets_[index].occupied) {
                return index;
            }
            if (!buckets_[index].deleted && buckets_[index].key == key) {
                return index;
            }
            index = (index + 1) % capacity_;
        } while (index != original_index);
        return index; // Should not happen if load factor is managed
    }

    void grow() {
        if (capacity_ == 0 || size_ * 100 / capacity_ >= MAX_LOAD_FACTOR_PERCENT) {
            size_t new_capacity = (capacity_ == 0) ? DEFAULT_INITIAL_CAPACITY : capacity_ * 2;
            Bucket* new_buckets = static_cast<Bucket*>(calloc(new_capacity, sizeof(Bucket)));
            if (!new_buckets) return;

            for (size_t i = 0; i < capacity_; ++i) {
                if (buckets_[i].occupied && !buckets_[i].deleted) {
                    size_t new_index = static_cast<size_t>(buckets_[i].key) % new_capacity;
                    size_t original_new_index = new_index;
                    bool inserted = false;
                    do {
                        if (!new_buckets[new_index].occupied) {
                            new_buckets[new_index].key = buckets_[i].key;
                            new_buckets[new_index].occupied = true;
                            new_buckets[new_index].deleted = false;
                            inserted = true;
                            break;
                        }
                        new_index = (new_index + 1) % new_capacity;
                    } while (new_index != original_new_index);

                    if (!inserted) {
                        free(new_buckets);
                        return;
                    }
                }
            }
            free(buckets_);
            buckets_ = new_buckets;
            capacity_ = new_capacity;
        }
    }

public:
    MyUnorderedSetInt() : MyUnorderedSetInt(DEFAULT_INITIAL_CAPACITY) {}

    explicit MyUnorderedSetInt(size_t expected_size) : buckets_(nullptr), capacity_(0), size_(0) {
        size_t initial_capacity = expected_size * 100 / MAX_LOAD_FACTOR_PERCENT;
        if (initial_capacity < DEFAULT_INITIAL_CAPACITY) {
            initial_capacity = DEFAULT_INITIAL_CAPACITY;
        }
        buckets_ = static_cast<Bucket*>(calloc(initial_capacity, sizeof(Bucket)));
        if (buckets_) {
            capacity_ = initial_capacity;
        }
    }

    ~MyUnorderedSetInt() {
        free(buckets_);
        buckets_ = nullptr;
    }

    // 禁止拷贝构造和赋值
    MyUnorderedSetInt(const MyUnorderedSetInt&) = delete;
    MyUnorderedSetInt& operator=(const MyUnorderedSetInt&) = delete;

    void insert(int key) {
        if (size_ * 100 / capacity_ >= MAX_LOAD_FACTOR_PERCENT) {
            grow();
        }
        if (capacity_ == 0) return;

        size_t index = findIndex(key);
        if (!buckets_[index].occupied || buckets_[index].deleted) {
            buckets_[index].key = key;
            buckets_[index].occupied = true;
            buckets_[index].deleted = false;
            size_++;
        }
    }

    // 返回 0 或 1
    size_t count(int key) const {
        if (capacity_ == 0) return 0;
        size_t index = findIndex(key);
        if (buckets_[index].occupied && !buckets_[index].deleted && buckets_[index].key == key) {
            return 1;
        }
        return 0;
    }

    // 更直观的接口
    bool contains(int key) const {
        return count(key) > 0;
    }

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void clear() {
        for (size_t i = 0; i < capacity_; ++i) {
            buckets_[i].occupied = false;
            buckets_[i].deleted = true;
        }
        size_ = 0;
    }
};

//  简化的 std::map<int, int> 实现 
// 使用平行数组实现
class MyMapIntInt {
private:
    int* keys_;
    int* values_;
    size_t size_;
    size_t capacity_;

    void reallocate(size_t new_capacity) {
        if (new_capacity <= capacity_) return;
        int* new_keys = static_cast<int*>(realloc(keys_, new_capacity * sizeof(int)));
        int* new_values = static_cast<int*>(realloc(values_, new_capacity * sizeof(int)));
        if (!new_keys || !new_values) {
            // Handle failure
            free(new_keys);   // realloc already freed old ptr if it failed
            free(new_values); // realloc already freed old ptr if it failed
            return;
        }
        keys_ = new_keys;
        values_ = new_values;
        capacity_ = new_capacity;
    }

    // 返回索引或 -1
    long long findIndex(int key) const {
        for (size_t i = 0; i < size_; ++i) {
            if (keys_[i] == key) {
                return static_cast<long long>(i);
            }
        }
        return -1LL;
    }

public:
    MyMapIntInt() : MyMapIntInt(100) {} // 默认容量

    explicit MyMapIntInt(size_t initial_capacity)
        : keys_(nullptr), values_(nullptr), size_(0), capacity_(0) {
        if (initial_capacity > 0) {
            keys_ = static_cast<int*>(malloc(initial_capacity * sizeof(int)));
            values_ = static_cast<int*>(malloc(initial_capacity * sizeof(int)));
            if (!keys_ || !values_) {
                free(keys_);
                free(values_);
                keys_ = values_ = nullptr;
                capacity_ = 0;
            } else {
                capacity_ = initial_capacity;
            }
        }
    }

    ~MyMapIntInt() {
        free(keys_);
        free(values_);
        keys_ = nullptr;
        values_ = nullptr;
    }

    // 禁止拷贝构造和赋值
    MyMapIntInt(const MyMapIntInt&) = delete;
    MyMapIntInt& operator=(const MyMapIntInt&) = delete;

    // 插入或访问
    int& operator[](int key) {
        long long index = findIndex(key);
        if (index != -1) {
            return values_[index];
        }
        // Key not found, insert
        if (size_ >= capacity_) {
            size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            reallocate(new_capacity);
            if (size_ >= capacity_) {
                // realloc failed, cannot insert
                static int error_value = 0; // Unsafe but simple
                return error_value;
            }
        }
        keys_[size_] = key;
        values_[size_] = 0; // 默认值
        size_++;
        return values_[size_ - 1];
    }

    // 返回 0 或 1
    size_t count(int key) const {
        return (findIndex(key) != -1) ? 1 : 0;
    }

    void clear() {
        size_ = 0; // 逻辑清空
    }

    // 提供访问内部数组的方法以便遍历
    const int* keys() const { return keys_; }
    const int* values() const { return values_; }
    size_t size() const { return size_; }
};
