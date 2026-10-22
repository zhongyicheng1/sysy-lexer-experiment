// extension/dfa.cpp
//
// DFA 化简（Moore 等价类划分）与模拟执行。
// 编译：g++ -std=c++17 -Wall -Wextra -pedantic extension/dfa.cpp -o extension/dfa
// 运行：./extension/dfa
//
// 约定：
//   - 状态编号 0 .. trans.size()-1；start 默认为 0。
//   - trans[s][c] 是状态 s 在字符 c 上的后继，-1 表示死状态（无转移）。
//   - 字母表假定为 'a','b',...；字符 'a'+c 的下标为 c。
//   - alphabet_size 可省略，实际按 max(alphabet_size, 最长行) 推导。

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

namespace {

struct DFA {
    int start = 0;
    int alphabet_size = 2;
    std::vector<std::vector<int>> trans;
    std::vector<int> accept_states;

    int state_count() const { return static_cast<int>(trans.size()); }

    bool is_accepting(int state) const {
        return std::find(accept_states.begin(), accept_states.end(), state) !=
               accept_states.end();
    }
};

// 从 start 做 BFS，返回按发现顺序排列的可达状态（start 在首位）。
// 不可达状态会被化简丢弃，因此必须先求可达集，避免等价类合并把编号弄乱。
std::vector<int> reachable_states(const DFA& dfa) {
    std::vector<int> order;
    const int n = dfa.state_count();

    if (n == 0 || dfa.start < 0 || dfa.start >= n) {
        return order;
    }

    std::vector<char> seen(static_cast<std::size_t>(n), 0);
    std::queue<int> work;
    seen[static_cast<std::size_t>(dfa.start)] = 1;
    work.push(dfa.start);

    while (!work.empty()) {
        const int u = work.front();
        work.pop();
        order.push_back(u);

        for (std::size_t c = 0; c < dfa.trans[static_cast<std::size_t>(u)].size(); ++c) {
            const int v = dfa.trans[static_cast<std::size_t>(u)][c];
            if (v >= 0 && v < n && !seen[static_cast<std::size_t>(v)]) {
                seen[static_cast<std::size_t>(v)] = 1;
                work.push(v);
            }
        }
    }
    return order;
}

// Moore 算法：
//   1. 初始划分：接受态 / 非接受态；
//   2. 反复用 (接受性, 各字符目标所在块) 作为签名细分，直到块数不再增加。
// 只对可达状态做划分，最后按块重建 DFA（start = start 所在块）。
DFA minimize_dfa(const DFA& input) {
    DFA result;
    const std::vector<int> order = reachable_states(input);
    const int m = static_cast<int>(order.size());

    if (m == 0) {
        return result;  // 空自动机：无状态、无接受态。
    }

    const int n = input.state_count();
    std::vector<int> old_to_sub(static_cast<std::size_t>(n), -1);
    for (int i = 0; i < m; ++i) {
        old_to_sub[static_cast<std::size_t>(order[static_cast<std::size_t>(i)])] = i;
    }

    int alpha = std::max(1, input.alphabet_size);
    for (int s : order) {
        alpha = std::max(alpha, static_cast<int>(input.trans[static_cast<std::size_t>(s)].size()));
    }

    // 把可达子图压成 0..m-1 的连续编号。
    std::vector<std::vector<int>> sub(static_cast<std::size_t>(m),
                                      std::vector<int>(static_cast<std::size_t>(alpha), -1));
    std::vector<char> sub_accept(static_cast<std::size_t>(m), 0);

    for (int i = 0; i < m; ++i) {
        const int os = order[static_cast<std::size_t>(i)];
        const std::vector<int>& row = input.trans[static_cast<std::size_t>(os)];
        for (int c = 0; c < static_cast<int>(row.size()) && c < alpha; ++c) {
            const int t = row[static_cast<std::size_t>(c)];
            sub[static_cast<std::size_t>(i)][static_cast<std::size_t>(c)] =
                (t >= 0 && t < n) ? old_to_sub[static_cast<std::size_t>(t)] : -1;
        }
        sub_accept[static_cast<std::size_t>(i)] = input.is_accepting(os) ? 1 : 0;
    }

    std::vector<int> block(static_cast<std::size_t>(m), 0);
    int block_count = 0;
    bool has_accept = false;
    bool has_reject = false;
    for (int i = 0; i < m; ++i) {
        if (sub_accept[static_cast<std::size_t>(i)]) {
            block[static_cast<std::size_t>(i)] = 1;
            has_accept = true;
        } else {
            block[static_cast<std::size_t>(i)] = 0;
            has_reject = true;
        }
    }
    block_count = (has_accept ? 1 : 0) + (has_reject ? 1 : 0);

    // 细分到块数不再增长为止（划分只会变细，因此一定终止）。
    while (true) {
        std::map<std::vector<int>, int> id_of;
        std::vector<int> next_block(static_cast<std::size_t>(m), 0);
        int next_count = 0;

        for (int i = 0; i < m; ++i) {
            std::vector<int> signature;
            signature.reserve(static_cast<std::size_t>(alpha) + 1);
            signature.push_back(sub_accept[static_cast<std::size_t>(i)] ? 1 : 0);
            for (int c = 0; c < alpha; ++c) {
                const int t = sub[static_cast<std::size_t>(i)][static_cast<std::size_t>(c)];
                signature.push_back(t < 0 ? -1 : block[static_cast<std::size_t>(t)]);
            }

            std::map<std::vector<int>, int>::iterator it = id_of.find(signature);
            if (it == id_of.end()) {
                id_of.insert(std::make_pair(signature, next_count));
                next_block[static_cast<std::size_t>(i)] = next_count;
                ++next_count;
            } else {
                next_block[static_cast<std::size_t>(i)] = it->second;
            }
        }

        block.swap(next_block);
        if (next_count == block_count) {
            break;
        }
        block_count = next_count;
    }

    result.start = block[0];  // order[0] 恒为 start
    result.alphabet_size = alpha;
    result.trans.assign(static_cast<std::size_t>(block_count),
                        std::vector<int>(static_cast<std::size_t>(alpha), -1));

    std::vector<char> block_accept(static_cast<std::size_t>(block_count), 0);
    for (int i = 0; i < m; ++i) {
        const int b = block[static_cast<std::size_t>(i)];
        if (sub_accept[static_cast<std::size_t>(i)]) {
            block_accept[static_cast<std::size_t>(b)] = 1;
        }
        for (int c = 0; c < alpha; ++c) {
            const int t = sub[static_cast<std::size_t>(i)][static_cast<std::size_t>(c)];
            result.trans[static_cast<std::size_t>(b)][static_cast<std::size_t>(c)] =
                (t < 0) ? -1 : block[static_cast<std::size_t>(t)];
        }
    }
    for (int b = 0; b < block_count; ++b) {
        if (block_accept[static_cast<std::size_t>(b)]) {
            result.accept_states.push_back(b);
        }
    }
    return result;
}

// 逐字符模拟；遇到死状态、越界字符或非法状态直接拒绝。
bool simulate_dfa(const DFA& dfa, const std::string& input) {
    const int n = dfa.state_count();
    if (n == 0 || dfa.start < 0 || dfa.start >= n) {
        return false;
    }

    int state = dfa.start;
    for (std::size_t i = 0; i < input.size(); ++i) {
        const int idx = static_cast<unsigned char>(input[i]) - 'a';
        if (idx < 0 || idx >= dfa.alphabet_size ||
            idx >= static_cast<int>(dfa.trans[static_cast<std::size_t>(state)].size())) {
            return false;  // 字母表之外的字符：没有转移
        }
        const int next = dfa.trans[static_cast<std::size_t>(state)][static_cast<std::size_t>(idx)];
        if (next < 0 || next >= n) {
            return false;  // 死状态
        }
        state = next;
    }
    return dfa.is_accepting(state);
}

void print_dfa(const DFA& dfa, const std::string& title) {
    std::cout << title << "\n";
    std::cout << "  states=" << dfa.trans.size() << ", start=q" << dfa.start
              << ", accepting={";
    for (std::size_t i = 0; i < dfa.accept_states.size(); ++i) {
        if (i != 0) {
            std::cout << ",";
        }
        std::cout << "q" << dfa.accept_states[i];
    }
    std::cout << "}\n";

    for (std::size_t s = 0; s < dfa.trans.size(); ++s) {
        std::cout << "  q" << s << ":";
        for (std::size_t c = 0; c < dfa.trans[s].size(); ++c) {
            std::cout << " " << static_cast<char>('a' + static_cast<int>(c)) << "->";
            if (dfa.trans[s][c] < 0) {
                std::cout << "-";
            } else {
                std::cout << "q" << dfa.trans[s][c];
            }
        }
        std::cout << "\n";
    }
}

}  // namespace

int main() {
    // 识别 ab*：q0 --a--> q1 --b--> q2 --b--> q2，q1/q2 接受，q3 为死状态。
    DFA dfa;
    dfa.start = 0;
    dfa.alphabet_size = 2;
    dfa.trans = {
        {1, 3},  // q0: a->q1, b->q3
        {3, 2},  // q1: a->q3, b->q2
        {3, 2},  // q2: a->q3, b->q2
        {3, 3},  // q3: 死状态
    };
    dfa.accept_states = {1, 2};

    print_dfa(dfa, "DFA (ab*) before minimization:");
    const DFA minimized = minimize_dfa(dfa);
    print_dfa(minimized, "DFA (ab*) after minimization:");

    const char* cases[] = {
        "", "a", "ab", "abb", "abbb", "b", "ba", "aa", "aba", "abab",
    };

    bool consistent = true;
    std::cout << "\nsimulation (before vs after):\n";
    for (const char* text : cases) {
        const std::string input(text);
        const bool before = simulate_dfa(dfa, input);
        const bool after = simulate_dfa(minimized, input);
        std::cout << "  \"" << input << "\" -> before="
                  << (before ? "accept" : "reject") << ", after="
                  << (after ? "accept" : "reject") << "\n";
        if (before != after) {
            consistent = false;
        }
    }

    if (!consistent) {
        std::cout << "FAIL: minimization changed the language\n";
        return 1;
    }
    std::cout << "OK: minimization preserves the language\n";
    return 0;
}
