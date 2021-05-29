#include "builder.hh"
#include <cassert>
#include <iomanip>
#include <limits>
#include <ostream>
#include <stack>

namespace kjson {

using namespace std;

class builder::impl {
  public:
    impl(ostream& out, bool compact)
      : d_out(out)
      , d_compact(compact) {
    }

    ~impl() {
        flush();
    }

    void key(string_view key) {
        if(!d_expect_key) {
            throw builder_error("not expecting a key");
        }

        in_mapping();
        comma();

        d_out << quoted(key);
        d_out << (d_compact ? ":" : ": ");
        d_needscomma = false;
        d_expect_key = false;
    }

    void with_none() {
        scalar("null");
    }

    void with_bool(bool v) {
        d_out << boolalpha;
        scalar(v);
    }

    void with_int(int64_t v) {
        scalar(v);
    }

    void with_uint(uint64_t v) {
        scalar(v);
    }

    void with_float(double v) {
        d_out.precision(std::numeric_limits<double>::max_digits10);
        scalar(v);
    }

    void with_string(std::string_view v) {
        scalar(quoted(v));
    }

    void push_mapping() {
        expect_value();
        d_expect_key = true;
        push('{', '}');
    }

    void push_sequence() {
        expect_value();
        d_expect_key = false;
        push('[', ']');
    }

    void pop() {
        if(d_stack.empty()) {
            throw builder_error("can not pop an empty stack");
        }
        auto c = d_stack.top();
        d_stack.pop();
        newline();
        d_out << c;

        if(is_mapping()) {
            d_expect_key = true;
        }
    }

    void flush() {
        while(!d_stack.empty()) {
            pop();
        }
    }

  private:
    bool is_mapping() const {
        return !d_stack.empty() && d_stack.top() == '}';
    }

    template <typename T>
    void scalar(const T& v) {
        expect_value();

        comma();

        d_out << v;
        d_needscomma = true;

        if(is_mapping()) {
            d_expect_key = true;
        }
    }

    void expect_value() {
        if(is_mapping() && d_expect_key) {
            throw builder_error("not expecting a value");
        }
    }

    void in_mapping() {
        if(!is_mapping()) {
            throw builder_error("top of the stack is not a mapping");
        }
    }

    void push(char b, char e) {
        comma();
        d_out << b;
        d_stack.push(e);
        newline();
        d_needscomma = false;
    }

    void comma() {
        if(d_needscomma) {
            d_out << ',';
            newline();
        }
    }

    void newline() {
        if(!d_compact) {
            d_out << '\n';
            for(size_t i = 0; i < d_stack.size(); ++i) {
                d_out << "  ";
            }
        }
    }

    ostream& d_out;
    bool     d_compact{true};

    bool        d_needscomma{false};
    bool        d_expect_key{false};
    stack<char> d_stack;
};

builder::builder(ostream& out, bool compact)
  : d_pimpl(make_unique<impl>(out, compact)) {
}

builder::~builder() {
}

builder& builder::key(string_view k) {
    assert(d_pimpl);
    d_pimpl->key(k);
    return *this;
}

builder& builder::with_none() {
    assert(d_pimpl);
    d_pimpl->with_none();
    return *this;
}

builder& builder::with_bool(bool v) {
    assert(d_pimpl);
    d_pimpl->with_bool(v);
    return *this;
}

builder& builder::with_int(int64_t v) {
    assert(d_pimpl);
    d_pimpl->with_int(v);
    return *this;
}

builder& builder::with_uint(uint64_t v) {
    assert(d_pimpl);
    d_pimpl->with_uint(v);
    return *this;
}

builder& builder::with_float(double v) {
    assert(d_pimpl);
    d_pimpl->with_float(v);
    return *this;
}

builder& builder::with_string(string_view v) {
    assert(d_pimpl);
    d_pimpl->with_string(v);
    return *this;
}

builder& builder::push_mapping() {
    assert(d_pimpl);
    d_pimpl->push_mapping();
    return *this;
}

builder& builder::push_sequence() {
    assert(d_pimpl);
    d_pimpl->push_sequence();
    return *this;
}

builder& builder::pop() {
    assert(d_pimpl);
    d_pimpl->pop();
    return *this;
}

builder& builder::flush() {
    assert(d_pimpl);
    d_pimpl->flush();
    return *this;
}

} // namespace kjson
