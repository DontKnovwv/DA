#include <iostream>
#include <cstdint>

using std::cin;
using std::cout;

constexpr uint32_t KEY_DIGITS = 6;
constexpr uint32_t KEY_RANGE = 1'000'000u;
constexpr uint32_t START_CAPACITY = 1u << 16;
constexpr char     FIELD_SEPARATOR = '\t';
constexpr char     ZERO_CHAR = '0';

struct TRec {
    uint32_t Key;
    uint64_t Value;
};

static inline uint32_t ParseKey(const char* s) {
    uint32_t x = 0;
    for (uint32_t i = 0; i < KEY_DIGITS; ++i) {
        x = x * 10u + static_cast<uint32_t>(s[i] - ZERO_CHAR);
    }
    return x;
}

static inline void PrintKey(uint32_t key) {
    char buf[KEY_DIGITS + 1];
    for (int i = static_cast<int>(KEY_DIGITS) - 1; i >= 0; --i) {
        buf[i] = static_cast<char>(ZERO_CHAR + (key % 10u));
        key /= 10u;
    }
    buf[KEY_DIGITS] = '\0';
    cout << buf;
}

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);

    uint32_t* countPerKey = new uint32_t[KEY_RANGE];
    for (uint32_t i = 0; i < KEY_RANGE; ++i) {
        countPerKey[i] = 0u;
    }

    uint64_t size = 0;
    uint64_t capacity = START_CAPACITY;
    TRec* records = new TRec[capacity];

    char keyBuf[KEY_DIGITS + 8];
    uint64_t value;
    while (cin >> keyBuf >> value) {
        if (size == capacity) {
            uint64_t newCapacity = capacity << 1;
            TRec* tmp = new TRec[newCapacity];
            for (uint64_t i = 0; i < size; ++i) {
                tmp[i] = records[i];
            }
            delete[] records;
            records = tmp;
            capacity = newCapacity;
        }
        uint32_t key = ParseKey(keyBuf);
        records[size].Key = key;
        records[size].Value = value;
        ++countPerKey[key];
        ++size;
    }

    for (uint32_t i = 1; i < KEY_RANGE; ++i) {
        countPerKey[i] = countPerKey[i] + countPerKey[i - 1];
    }

    TRec* sorted = new TRec[size];
    for (int64_t i = static_cast<int64_t>(size) - 1; i >= 0; --i) {
        uint32_t key = records[i].Key;
        uint32_t pos = --countPerKey[key];
        sorted[pos] = records[i];
    }

    for (uint64_t i = 0; i < size; ++i) {
        PrintKey(sorted[i].Key);
        cout << FIELD_SEPARATOR << sorted[i].Value << '\n';
    }

    delete[] sorted;
    delete[] records;
    delete[] countPerKey;
    return 0;
}
