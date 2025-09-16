#include <iostream>
#include <cstdio>
#include <cstdlib>

// Простой xorshift64* PRNG для воспроизводимости
static unsigned long long rngState = 0x9E3779B97F4A7C15ULL;

inline unsigned long long NextRand() {
    unsigned long long x = rngState;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    rngState = x;
    return x * 0x2545F4914F6CDD1DULL;
}

inline unsigned long long RandIn(unsigned long long maxv) {
    return NextRand() % (maxv + 1ULL);
}

inline char RandLetter(bool upper) {
    unsigned long long r = NextRand();
    char base = upper ? 'A' : 'a';
    return static_cast<char>(base + (r % 26ULL));
}

void MakeWord(char *buf, int len, bool mixCase) {
    for (int i = 0; i < len; ++i) {
        bool upper = mixCase && ((NextRand() >> 5) & 1ULL);
        buf[i] = RandLetter(upper);
    }
    buf[len] = 0;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        std::fprintf(stderr, "Usage: %s <commands_count> <output_file>\n", argv[0]);
        return 1;
    }

    unsigned long long n = 0ULL;
    {
        const char *s = argv[1];
        while (*s) {
            if (*s < '0' || *s > '9') { std::fprintf(stderr, "Bad number\n"); return 1; }
            n = n * 10ULL + static_cast<unsigned long long>(*s - '0');
            ++s;
        }
    }

    const char *outName = argv[2];
    std::FILE *out = std::fopen(outName, "wb");
    if (!out) {
        std::perror("fopen");
        return 1;
    }

    // Пул вставленных ключей для корректных удалений/поиска
    // Делаем простой динамический массив указателей на C-строки
    const unsigned int MAX_WORD_LEN = 256U;
    unsigned long long cap = (n > 0ULL ? (n / 2ULL + 16ULL) : 16ULL);
    char **keys = (char**)std::malloc(sizeof(char*) * cap);
    unsigned long long size = 0ULL;

    // Доля операций
    const unsigned long long P_INSERT = 40ULL;  // %
    const unsigned long long P_DELETE = 10ULL;  // %
    // Остальное — поиск

    // Иногда создаём экстремально длинные слова
    auto sometimesLong = []() { return (NextRand() % 97ULL) == 0ULL; };

    for (unsigned long long i = 0; i < n; ++i) {
        unsigned long long r = NextRand() % 100ULL;
        if (r < P_INSERT) {
            // INSERT
            unsigned int len = sometimesLong() ? MAX_WORD_LEN : (unsigned int)(1ULL + (NextRand() % 12ULL));
            char buf[257];
            MakeWord(buf, (int)len, true);
            unsigned long long val = NextRand();
            std::fprintf(out, "+ %s %llu\n", buf, (unsigned long long)val);

            // Сохраняем ключ для будущих delete/find
            if (size == cap) {
                cap = cap * 2ULL;
                keys = (char**)std::realloc(keys, sizeof(char*) * cap);
            }
            char *store = (char*)std::malloc(len + 1U);
            for (unsigned int j = 0; j < len; ++j) store[j] = buf[j];
            store[len] = 0;
            keys[size++] = store;
        } else if (r < P_INSERT + P_DELETE && size > 0ULL) {
            // DELETE существующего
            unsigned long long idx = NextRand() % size;
            std::fprintf(out, "- %s\n", keys[idx]);
            // Удалим из пула (перенос последнего на место idx)
            std::free(keys[idx]);
            keys[idx] = keys[size - 1ULL];
            --size;
        } else {
            // FIND — существующий или случайный
            if (size > 0ULL && (NextRand() & 1ULL)) {
                unsigned long long idx = NextRand() % size;
                std::fprintf(out, "%s\n", keys[idx]);
            } else {
                unsigned int len = (unsigned int)(1ULL + (NextRand() % 12ULL));
                char buf[257];
                MakeWord(buf, (int)len, true);
                std::fprintf(out, "%s\n", buf);
            }
        }
    }

    // Освобождение памяти
    for (unsigned long long i2 = 0ULL; i2 < size; ++i2) {
        std::free(keys[i2]);
    }
    std::free(keys);

    std::fclose(out);
    return 0;
}
