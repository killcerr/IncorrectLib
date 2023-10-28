#pragma once
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>
namespace TextReplacer {
template <typename T>
constexpr bool IsText = false;
template <typename... Ts>
constexpr bool IsText<std::basic_string<Ts...>> = true;
template <size_t I>
constexpr auto getArgAt(auto &&...args) {
    return std::get<I>(std::make_tuple(std::forward<decltype(args)>(args)...));
}
template <typename RT, size_t I = 0>
constexpr void args2functions(std::vector<std::function<RT()>> &res, auto &&...args) {
    auto arg = getArgAt<I * 2>(std::forward<decltype(args)>(args)...);
    auto &&fn = getArgAt<I * 2 + 1>(std::forward<decltype(args)>(args)...);
    res.push_back(std::bind(std::forward<decltype(fn)>(fn), std::move(arg)));
    if constexpr ((I + 1) * 2 + 1 < sizeof...(args)) args2functions<RT, I + 1>(res, std::forward<decltype(args)>(args)...);
}

template <typename StrT>
    requires IsText<StrT>
constexpr StrT textReplacer(StrT &&fmt, auto &&...args) {
    std::vector<std::function<StrT()>> fs;
    args2functions(fs, std::forward<decltype(args)>(args)...);
    StrT r, t;
    using CharT = StrT::value_type;
    CharT lc = 0;
    auto fi = fs.begin();
    for (auto c : fmt) {
        if (c == '$' && lc == '$') {
            if (!t.empty()) t.pop_back();
            r += t;
            r += (*fi)();
            t.clear();
            fi++;
            if (fi == fs.end()) fi = fs.begin();
            lc = 0;
        } else {
            t += c;
            lc = c;
        }
    }
    return r;
}

namespace fn {
auto rtarg = [](const auto &arg) { return arg; };
auto argts = [](const auto &arg) { return std::to_string(arg); };
auto rtcall = [](const auto &arg) { return arg(); };
}  // namespace fn

}  // namespace TextProcesser
