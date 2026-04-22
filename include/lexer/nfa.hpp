#pragma once
#include "dfa.hpp"
#include "set_hash.hpp"
#include <array>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>
struct NFA {
  int num_states;
  int start_state;
  std::vector<std::vector<std::vector<int>>> delta;
  std::vector<Token> token_types;
  NFA(int num_states, int start_state,
      const std::vector<std::vector<std::vector<int>>> &delta,
      std::vector<Token> token_types)
      : num_states(num_states), start_state(start_state), delta(delta),
        token_types(token_types) {}
  NFA union_nfa(const NFA &other) const {
    int num_states_new = num_states + other.num_states + 1;
    std::vector<Token> token_types_new(num_states_new, Token::TOK_ERROR);
    for (int i = 0; i < num_states; i++) {
      token_types_new[i + 1] = token_types[i];
    }
    for (int i = 0; i < other.num_states; i++) {
      token_types_new[i + num_states + 1] = other.token_types[i];
    }
    std::vector<std::vector<std::vector<int>>> delta_new(
        num_states_new, std::vector<std::vector<int>>(ALPHABET_SIZE + 1));
    delta_new[0][ALPHABET_SIZE] = {start_state + 1,
                                   other.start_state + num_states + 1};
    for (int i = 0; i < num_states; i++) {
      for (int alpha = 0; alpha < ALPHABET_SIZE + 1; alpha++) {
        for (int v : delta[i][alpha]) {
          delta_new[i + 1][alpha].push_back(v + 1);
        }
      }
    }
    for (int i = 0; i < other.num_states; i++) {
      for (int alpha = 0; alpha < ALPHABET_SIZE + 1; alpha++) {
        for (int v : other.delta[i][alpha]) {
          delta_new[i + num_states + 1][alpha].push_back(v + num_states + 1);
        }
      }
    }
    NFA ret(num_states_new, 0, delta_new, token_types_new);
    return ret;
  }
  NFA concat_nfa(const NFA &other) const {
    int num_states_new = num_states + other.num_states;
    std::vector<Token> token_types_new(num_states_new, Token::TOK_ERROR);
    for (int i = 0; i < other.num_states; i++) {
      token_types_new[i + num_states] = other.token_types[i];
    }
    std::vector<std::vector<std::vector<int>>> delta_new(
        num_states_new, std::vector<std::vector<int>>(ALPHABET_SIZE + 1));
    for (int i = 0; i < num_states; i++) {
      for (int alpha = 0; alpha < ALPHABET_SIZE + 1; alpha++) {
        for (int v : delta[i][alpha]) {
          delta_new[i][alpha].push_back(v);
        }
      }
      if (token_types[i] != Token::TOK_ERROR) {
        delta_new[i][ALPHABET_SIZE].push_back(num_states + other.start_state);
      }
    }
    for (int i = 0; i < other.num_states; i++) {
      for (int alpha = 0; alpha < ALPHABET_SIZE + 1; alpha++) {
        for (int v : other.delta[i][alpha]) {
          delta_new[i + num_states][alpha].push_back(v + num_states);
        }
      }
    }
    NFA ret(num_states_new, start_state, delta_new, token_types_new);
    return ret;
  }
  NFA kleene_star() const {
    std::vector<Token> token_types_new(num_states + 1, Token::TOK_ERROR);
    for (int i = 0; i < num_states; i++) {
      token_types_new[i] = token_types[i];
    }
    token_types_new[num_states] = Token::TOK_FINAL_PLACEHOLDER;
    std::vector<std::vector<std::vector<int>>> delta_new(
        num_states + 1, std::vector<std::vector<int>>(ALPHABET_SIZE + 1));
    for (int i = 0; i < num_states; i++) {
      for (int alpha = 0; alpha < ALPHABET_SIZE + 1; alpha++) {
        for (int v : delta[i][alpha]) {
          delta_new[i][alpha].push_back(v);
        }
      }
      if (token_types_new[i] != Token::TOK_ERROR) {
        delta_new[i][ALPHABET_SIZE].push_back(num_states);
      }
    }
    delta_new[num_states][ALPHABET_SIZE] = {start_state};
    NFA ret(num_states + 1, num_states, delta_new, token_types_new);
    return ret;
  }
  DFA to_dfa() const {
    std::vector<std::vector<int>> reverse_eps_transition(num_states);
    for (int i = 0; i < num_states; i++) {
      for (int j : delta[i][ALPHABET_SIZE]) {
        reverse_eps_transition[j].push_back(i);
      }
    }
    std::vector<std::unordered_set<int>> e(num_states);
    for (int i = 0; i < num_states; i++) {
      e[i].insert(i);
    }
    std::unordered_set<int> worklist;
    for (int i = 0; i < num_states; i++) {
      worklist.insert(i);
    }
    while (!worklist.empty()) {
      int n = *worklist.begin();
      worklist.erase(n);
      std::unordered_set<int> t;
      t.insert(n);
      for (int v : delta[n][ALPHABET_SIZE]) {
        for (int w : e[v]) {
          t.insert(w);
        }
      }
      if (t != e[n]) {
        e[n] = t;
        for (int v : reverse_eps_transition[n]) {
          worklist.insert(v);
        }
      }
    }
    std::unordered_map<std::unordered_set<int>, int, SetHash> Q;
    std::stack<std::unordered_set<int>> worklist2;
    std::unordered_map<int, std::array<int, ALPHABET_SIZE>> delta_dfa_map;
    std::unordered_set<int> q0 = e[start_state];
    int cnt = 0;
    Q[q0] = cnt++;
    worklist2.push(q0);
    while (!worklist2.empty()) {
      std::unordered_set<int> q = worklist2.top();
      worklist2.pop();
      for (int alpha = 0; alpha < ALPHABET_SIZE; alpha++) {
        std::unordered_set<int> temp;
        for (int state : q) {
          for (int next_state : delta[state][alpha]) {
            temp.insert(next_state);
          }
        }
        std::unordered_set<int> temp2;
        for (int state : temp) {
          for (int state2 : e[state]) {
            temp2.insert(state2);
          }
        }
        if (Q.find(temp2) == Q.end()) {
          Q[temp2] = cnt++;
          worklist2.push(temp2);
        }
        delta_dfa_map[Q[q]][alpha] = Q[temp2];
      }
    }
    std::vector<Token> token_types_dfa(cnt, Token::TOK_ERROR);
    for (const auto &pair : Q) {
      for (int x : pair.first) {
        token_types_dfa[pair.second] =
            max(token_types_dfa[pair.second], token_types[x]);
      }
    }
    std::vector<std::vector<int>> delta_dfa(cnt,
                                            std::vector<int>(ALPHABET_SIZE));
    for (int i = 0; i < cnt; i++) {
      for (int alpha = 0; alpha < ALPHABET_SIZE; alpha++) {
        delta_dfa[i][alpha] = delta_dfa_map[i][alpha];
      }
    }
    DFA ret(cnt, 0, delta_dfa, token_types_dfa);
    return ret;
  }
};
