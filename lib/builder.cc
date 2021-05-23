#include "builder.hh"
#include <cassert>
#include <ostream>
#include <stack>
#include <iomanip>

namespace kjson {

using namespace std;

class builder::impl {
public:
    using result = results::result<impl *>;

    impl(ostream &out, bool compact)
        : d_out(out)
        , d_compact(compact)
    {}

    ~impl() {
        flush();
    }

    result with_none() {
        return in_sequence()
                .map([](auto *t) { return t->scalar("null"); });
    }
    result with_none(std::string_view key) {
        return result::err("implement me");
    }

    result with_bool(bool v) {
        d_out << boolalpha;
        return in_sequence()
                .map([v](auto *t) { return t->scalar(v); });
    }
    result with_bool(std::string_view key, bool v) {
        return result::err("implement me");
    }

    result with_int(int64_t v) {
        return in_sequence()
                .map([v](auto *t) { return t->scalar(v); });
    }
    result with_int(std::string_view key, int64_t v) {
        return result::err("implement me");
    }

    result with_uint(uint64_t v) {
        return in_sequence()
                .map([v](auto *t) { return t->scalar(v); });
    }
    result with_uint(std::string_view key, uint64_t v) {
        return result::err("implement me");
    }

    result with_float(double v) {
        return in_sequence()
                .map([v](auto *t) { return t->scalar(v); });
    }
    result with_float(std::string_view key, double v) {
        return result::err("implement me");
    }

    result with_string(std::string_view v) {
        return in_sequence()
                .map([v](auto *t) { return t->scalar(std::quoted(v)); });
    }
    result with_string(std::string_view key, std::string_view v) {
        return result::err("implement me");
    }

    result with_mapping() {
        return result::err("implement me");
    }
    result with_mapping(std::string_view key) {
        return result::err("implement me");
    }

    result with_sequence() {
        return in_sequence()
                .map([](auto *t) {
           t->d_out << '[';
           t->d_stack.push(']');
           return t;
        });
    }
    result with_sequence(std::string_view key) {
        return result::err("implement me");
    }

    result pop() {
        if (d_stack.empty()) {
            return result::err("can not pop an empty stack");
        }
        auto c = d_stack.top(); d_stack.pop();
        d_out << c;
        return result::ok(this);
    }

    result flush() {
        while (!d_stack.empty()) {
            pop();
        }
        return result::ok(this);
    }

private:
    template <typename T>
    impl *scalar(const T &v) {
        if (d_needscomma) {
            d_out << (d_compact ? "," : ", ");
        }
        d_out << v;
        d_needscomma = true;

        return this;
    }

    result in_sequence() {
        if (!d_stack.empty() && d_stack.top() != ']') {
            return result::err("top of the stack is not a sequence");
        } else {
            return result::ok(this);
        }
    }

    ostream &d_out;
    bool d_compact{true};

    bool d_needscomma{false};
    stack<char> d_stack;

};

builder::builder(ostream &out, bool compact)
    : d_pimpl(make_unique<impl>(out, compact))
{}

builder::~builder()
{}

builder::result builder::with_none()
{
    assert(d_pimpl);
    return d_pimpl->with_none().map([this](auto) { return this; });
}

builder::result builder::with_none(string_view key)
{
    assert(d_pimpl);
    return d_pimpl->with_none(key).map([this](auto) { return this; });
}

builder::result builder::with_bool(bool v)
{
    assert(d_pimpl);
    return d_pimpl->with_bool(v).map([this](auto) { return this; });
}

builder::result builder::with_bool(string_view key, bool v)
{
    assert(d_pimpl);
    return d_pimpl->with_bool(key, v).map([this](auto) { return this; });
}

builder::result builder::with_int(int64_t v)
{
    assert(d_pimpl);
    return d_pimpl->with_int(v).map([this](auto) { return this; });
}

builder::result builder::with_int(string_view key, int64_t v)
{
    assert(d_pimpl);
    return d_pimpl->with_int(key, v).map([this](auto) { return this; });
}

builder::result builder::with_uint(uint64_t v)
{
    assert(d_pimpl);
    return d_pimpl->with_uint(v).map([this](auto) { return this; });
}

builder::result builder::with_uint(string_view key, uint64_t v)
{
    assert(d_pimpl);
    return d_pimpl->with_uint(key, v).map([this](auto) { return this; });
}

builder::result builder::with_float(double v)
{
    assert(d_pimpl);
    return d_pimpl->with_float(v).map([this](auto) { return this; });
}

builder::result builder::with_float(string_view key, double v)
{
    assert(d_pimpl);
    return d_pimpl->with_float(key, v).map([this](auto) { return this; });
}

builder::result builder::with_string(string_view v)
{
    assert(d_pimpl);
    return d_pimpl->with_string(v).map([this](auto) { return this; });
}

builder::result builder::with_string(string_view key, string_view v)
{
    assert(d_pimpl);
    return d_pimpl->with_string(key, v).map([this](auto) { return this; });
}

builder::result builder::with_mapping()
{
    assert(d_pimpl);
    return d_pimpl->with_mapping().map([this](auto) { return this; });
}

builder::result builder::with_mapping(string_view key)
{
    assert(d_pimpl);
    return d_pimpl->with_mapping(key).map([this](auto) { return this; });
}

builder::result builder::with_sequence()
{
    assert(d_pimpl);
    return d_pimpl->with_sequence().map([this](auto) { return this; });
}

builder::result builder::with_sequence(string_view key)
{
    assert(d_pimpl);
    return d_pimpl->with_sequence(key).map([this](auto) { return this; });
}

builder::result builder::pop()
{
    assert(d_pimpl);
    return d_pimpl->pop().map([this](auto) { return this; });
}

builder::result builder::flush()
{
    assert(d_pimpl);
    return d_pimpl->flush().map([this](auto) { return this; });
}

}
