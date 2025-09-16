#include <iostream>

namespace NDict {

const int MAX_WORD_LEN = 256;
const unsigned int RNG_DEFAULT_SEED = 2463534242U;

const unsigned int XORSHIFT_L1 = 13U;
const unsigned int XORSHIFT_R  = 17U;
const unsigned int XORSHIFT_L2 = 5U;
const int DEC_BASE = 10;

inline bool IsLetter(int ch) {
    if (ch >= 'A' && ch <= 'Z') {
        return true;
    }
    if (ch >= 'a' && ch <= 'z') {
        return true;
    }
    return false;
}

inline char ToLowerChar(char ch) {
    if (ch >= 'A' && ch <= 'Z') {
        return static_cast<char>(ch - 'A' + 'a');
    }
    return ch;
}

inline bool IsHSpace(int ch) {
    if (ch == ' ') {
        return true;
    }
    if (ch == '\t') {
        return true;
    }
    return false;
}

void SkipHSpaces() {
    while (true) {
        int ch = std::cin.peek();
        if (!std::cin) {
            return;
        }
        if (!IsHSpace(ch)) {
            return;
        }
        std::cin.get();
    }
}

inline bool IsSpace(int ch) {
    if (ch == ' ') {
        return true;
    }
    if (ch == '\n') {
        return true;
    }
    if (ch == '\r') {
        return true;
    }
    if (ch == '\t') {
        return true;
    }
    return false;
}

void SkipSpaces() {
    while (true) {
        int ch = std::cin.peek();
        if (!std::cin) {
            return;
        }
        if (!IsSpace(ch)) {
            return;
        }
        std::cin.get();
    }
}

void SkipToLineEnd() {
    while (true) {
        int ch = std::cin.peek();
        if (!std::cin) {
            return;
        }
        if (ch == '\n') {
            std::cin.get();
            return;
        }
        if (ch == -1) {
            return;
        }
        std::cin.get();
    }
}

int CompareStrings(const char *a, const char *b) {
    int i = 0;
    while (true) {
        char ca = a[i];
        char cb = b[i];
        if (ca == 0 && cb == 0) {
            return 0;
        }
        if (ca == 0) {
            return -1;
        }
        if (cb == 0) {
            return 1;
        }
        if (ca < cb) {
            return -1;
        }
        if (ca > cb) {
            return 1;
        }
        i = i + 1;
    }
}

int StringLength(const char *s) {
    int n = 0;
    while (s[n] != 0) {
        n = n + 1;
    }
    return n;
}

char *DuplicateString(const char *s) {
    int n = StringLength(s);
    char *p = new char[n + 1];
    for (int i = 0; i < n; i = i + 1) {
        p[i] = s[i];
    }
    p[n] = 0;
    return p;
}

bool ReadWord(char *out) {
    SkipHSpaces();
    int len = 0;
    while (true) {
        int ch = std::cin.peek();
        if (!std::cin) {
            break;
        }
        if (!IsLetter(ch)) {
            break;
        }
        std::cin.get();
        if (len < MAX_WORD_LEN) {
            out[len] = ToLowerChar(static_cast<char>(ch));
            len = len + 1;
        }
    }
    out[len] = 0;
    return len > 0;
}

bool ReadUnsignedLongLong(unsigned long long &value) {
    SkipHSpaces();
    bool have = false;
    unsigned long long v = 0ULL;
    while (true) {
        int ch = std::cin.peek();
        if (!std::cin) {
            break;
        }
        if (ch < '0' || ch > '9') {
            break;
        }
        have = true;
        std::cin.get();
        unsigned long long digit = static_cast<unsigned long long>(ch - '0');
        v = v * static_cast<unsigned long long>(DEC_BASE) + digit;
    }
    value = v;
    return have;
}

class TTreap {
private:
    class TNode {
    public:
        char *Key;
        unsigned long long Value;
        unsigned int Prior;
        TNode *Left;
        TNode *Right;

        TNode(char *key, unsigned long long value, unsigned int prior)
            : Key(key), Value(value), Prior(prior), Left(nullptr), Right(nullptr) {
        }
    };

    TNode *Root;
    unsigned int RngState;

    unsigned int NextRand() {
        unsigned int x = RngState;
        x = x ^ (x << XORSHIFT_L1);
        x = x ^ (x >> XORSHIFT_R);
        x = x ^ (x << XORSHIFT_L2);
        RngState = x;
        if (x == 0U) {
            return 1U;
        }
        return x;
    }

    void Split(TNode *node, const char *key, TNode *&left, TNode *&right) {
        if (node == nullptr) {
            left = nullptr;
            right = nullptr;
            return;
        }
        int cmp = CompareStrings(node->Key, key);
        if (cmp < 0) {
            TNode *tleft;
            TNode *tright;
            Split(node->Right, key, tleft, tright);
            node->Right = tleft;
            left = node;
            right = tright;
            return;
        } else {
            TNode *tleft;
            TNode *tright;
            Split(node->Left, key, tleft, tright);
            node->Left = tright;
            left = tleft;
            right = node;
            return;
        }
    }

    TNode *Merge(TNode *left, TNode *right) {
        if (left == nullptr) {
            return right;
        }
        if (right == nullptr) {
            return left;
        }
        if (left->Prior >= right->Prior) {
            left->Right = Merge(left->Right, right);
            return left;
        } else {
            right->Left = Merge(left, right->Left);
            return right;
        }
    }

    bool Insert(TNode *&node, char *key, unsigned long long value, unsigned int prior) {
        if (node == nullptr) {
            node = new TNode(key, value, prior);
            return true;
        }
        int cmp = CompareStrings(key, node->Key);
        if (cmp == 0) {
            return false;
        }
        if (prior > node->Prior) {
            TNode *l;
            TNode *r;
            Split(node, key, l, r);
            TNode *n = new TNode(key, value, prior);
            n->Left = l;
            n->Right = r;
            node = n;
            return true;
        }
        if (cmp < 0) {
            return Insert(node->Left, key, value, prior);
        } else {
            return Insert(node->Right, key, value, prior);
        }
    }

    bool Remove(TNode *&node, const char *key) {
        if (node == nullptr) {
            return false;
        }
        int cmp = CompareStrings(key, node->Key);
        if (cmp == 0) {
            TNode *old = node;
            node = Merge(node->Left, node->Right);
            delete[] old->Key;
            delete old;
            return true;
        }
        if (cmp < 0) {
            return Remove(node->Left, key);
        } else {
            return Remove(node->Right, key);
        }
    }

    bool Find(TNode *node, const char *key, unsigned long long &value) const {
        TNode *cur = node;
        while (cur != nullptr) {
            int cmp = CompareStrings(key, cur->Key);
            if (cmp == 0) {
                value = cur->Value;
                return true;
            }
            if (cmp < 0) {
                cur = cur->Left;
            } else {
                cur = cur->Right;
            }
        }
        return false;
    }

    void Destroy(TNode *node) {
        if (node == nullptr) {
            return;
        }
        Destroy(node->Left);
        Destroy(node->Right);
        delete[] node->Key;
        delete node;
    }

public:
    TTreap() : Root(nullptr), RngState(RNG_DEFAULT_SEED) {
    }

    ~TTreap() {
        Destroy(Root);
        Root = nullptr;
    }

    bool Insert(char *key, unsigned long long value) {
        unsigned long long tmp = 0ULL;
        if (Find(Root, key, tmp)) {
            return false;
        }
        unsigned int p = NextRand();
        return Insert(Root, key, value, p);
    }

    bool Remove(const char *key) {
        return Remove(Root, key);
    }

    bool Find(const char *key, unsigned long long &value) const {
        return Find(Root, key, value);
    }
};

}

int main() {
    using namespace NDict;

    TTreap dict;

    char word[MAX_WORD_LEN + 1];

    while (true) {
        SkipSpaces();

        int ch = std::cin.peek();
        if (!std::cin) {
            break;
        }

        if (ch == '+') {
            std::cin.get();
            bool okw = ReadWord(word);
            unsigned long long val = 0ULL;
            bool okv = ReadUnsignedLongLong(val);
            SkipToLineEnd();
            if (!okw || !okv) {
                continue;
            }
            char *key = DuplicateString(word);
            bool inserted = dict.Insert(key, val);
            if (inserted) {
                std::cout << "OK" << '\n';
            } else {
                delete[] key;
                std::cout << "Exist" << '\n';
            }
            continue;
        }

        if (ch == '-') {
            std::cin.get();
            bool okw = ReadWord(word);
            SkipToLineEnd();
            if (!okw) {
                continue;
            }
            bool removed = dict.Remove(word);
            if (removed) {
                std::cout << "OK" << '\n';
            } else {
                std::cout << "NoSuchWord" << '\n';
            }
            continue;
        }

        if (IsLetter(ch)) {
            bool okw = ReadWord(word);
            SkipToLineEnd();
            if (!okw) {
                continue;
            }
            unsigned long long val = 0ULL;
            bool found = dict.Find(word, val);
            if (found) {
                std::cout << "OK: " << val << '\n';
            } else {
                std::cout << "NoSuchWord" << '\n';
            }
            continue;
        }

        std::cin.get();
    }

    return 0;
}
