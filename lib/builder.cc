#include "builder.hh"
#include <cassert>
#include <ostream>
#include <stack>
#include <iomanip>
#include <limits>

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
        return with_key(key).map([](auto *t) { return t->scalar("null"); });
    }

    result with_bool(bool v) {
        d_out << boolalpha;
        return in_sequence()
                .map([v](auto *t) { return t->scalar(v); });
    }
    result with_bool(std::string_view key, bool v) {
        d_out << boolalpha;
        return with_key(key).map([v](auto *t) { return t->scalar(v); });
    }

    result with_int(int64_t v) {
        return in_sequence()
                .map([v](auto *t) { return t->scalar(v); });
    }
    result with_int(std::string_view key, int64_t v) {
        return with_key(key).map([v](auto *t) { return t->scalar(v); });
    }

    result with_uint(uint64_t v) {
        return in_sequence()
                .map([v](auto *t) { return t->scalar(v); });
    }
    result with_uint(std::string_view key, uint64_t v) {
        return with_key(key).map([v](auto *t) { return t->scalar(v); });
    }

    result with_float(double v) {
        d_out.precision(std::numeric_limits<double>::max_digits10);
        return in_sequence()
                .map([v](auto *t) { return t->scalar(v); });
    }
    result with_float(std::string_view key, double v) {
        d_out.precision(std::numeric_limits<double>::max_digits10);
        return with_key(key).map([v](auto *t) { return t->scalar(v); });
    }

    result with_string(std::string_view v) {
        return in_sequence()
                .map([v](auto *t) { return t->scalar(quoted(v)); });
    }
    result with_string(std::string_view key, std::string_view v) {
       return with_key(key).map([v](auto *t) { return t->scalar(quoted(v)); });
    }

    result with_mapping() {
        return in_sequence()
                .map([](auto *t) { return t->do_mapping(); });
    }
    result with_mapping(std::string_view key) {
        return with_key(key).map([](auto *t) { return t->do_mapping(); });
    }

    result with_sequence() {
        return in_sequence()
                .map([](auto *t) { return t->do_sequence(); });
    }
    result with_sequence(std::string_view key) {
        return with_key(key).map([](auto *t) { return t->do_sequence(); });
    }

    result pop() {
        if (d_stack.empty()) {
            return result::err("can not pop an empty stack");
        }
        auto c = d_stack.top(); d_stack.pop();
        newline();
        d_out << c;
        return result::ok(this);
    }

    void flush() {
        while (!d_stack.empty()) {
            pop().unwrap();
        }
    }

private:
    template <typename T>
    impl *scalar(const T &v) {
        comma();

        d_out << v;
        d_needscomma = true;

        return this;
    }

    result with_key(string_view key) {
        return in_mapping()
                .map([this, key](auto *t) {
            assert(this == t);
            comma();

            d_out << quoted(key);
            d_out << (d_compact ? ":" : ": ");
            d_needscomma = false;
            return this;
        });
    }

    result in_sequence() {
        if (!d_stack.empty() && d_stack.top() != ']') {
            return result::err("top of the stack is not a sequence");
        } else {
            return result::ok(this);
        }
    }

    result in_mapping() {
        if (d_stack.empty() || d_stack.top() != '}') {
            return result::err("top of the stack is not a mapping");
        } else {
            return result::ok(this);
        }
    }

    impl *do_mapping() {
        return push('{', '}');
    }

    impl *do_sequence() {
        return push('[', ']');
    }

    impl *push(char b, char e) {
        comma();
        d_out << b;
        d_stack.push(e);
        newline();
        d_needscomma = false;
        return this;
    }

    void comma() {
        if (d_needscomma) {
            d_out << ',';
            newline();
        }
    }

    void newline() {
        if (!d_compact) {
            d_out << '\n';
            for (size_t i = 0; i < d_stack.size(); ++i) {
                d_out << "  ";
            }
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

void builder::flush()
{
    assert(d_pimpl);
    d_pimpl->flush();
}

}
