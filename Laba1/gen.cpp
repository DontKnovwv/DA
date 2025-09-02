// gen.cpp — генератор тестов для ЛР (почтовые индексы + uint64)
#include <bits/stdc++.h>
using namespace std;

static inline uint64_t xorshift64(uint64_t &s){
    s ^= s << 13; s ^= s >> 7; s ^= s << 17; return s;
}
int main(int argc, char** argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    if (argc < 2) { cerr << "Usage: ./gen N [seed]\n"; return 1; }
    uint64_t N = strtoull(argv[1], nullptr, 10);
    uint64_t seed = (argc >= 3 ? strtoull(argv[2], nullptr, 10)
                               : chrono::high_resolution_clock::now().time_since_epoch().count());
    uint64_t s = seed ? seed : 88172645463393265ull;

    for (uint64_t i = 0; i < N; ++i) {
        uint32_t key = (uint32_t)(xorshift64(s) % 1'000'000u);
        uint64_t val = xorshift64(s); // 0..2^64-1
        // печать ключа с ведущими нулями
        char buf[7];
        uint32_t k = key;
        for (int j = 5; j >= 0; --j) { buf[j] = char('0' + (k % 10)); k /= 10; }
        buf[6] = '\0';
        cout << buf << '\t' << val << '\n';
    }
    return 0;
}
