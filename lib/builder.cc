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
    using result = results::result<impl*>;

    impl(ostream& out, bool compact)
      : d_out(out)
      , d_compact(compact) {
    }

    ~impl() {
        flush();
    }

    result key(string_view k) {
        return with_key(k);
    }

    result with_none() {
        return scalar("null");
    }

    result with_bool(bool v) {
        d_out << boolalpha;
        return scalar(v);
    }

    result with_int(int64_t v) {
        return scalar(v);
    }

    result with_uint(uint64_t v) {
        return scalar(v);
    }

    result with_float(double v) {
        d_out.precision(std::numeric_limits<double>::max_digits10);
        return scalar(v);
    }

    result with_string(std::string_view v) {
        return scalar(quoted(v));
    }

    result push_mapping() {
        return expect_value()
            .map([](auto* t) { return t->do_mapping(); });
    }

    result push_sequence() {
        return expect_value()
            .map([](auto* t) { return t->do_sequence(); });
    }

    result pop() {
        if(d_stack.empty()) {
            return result::err("can not pop an empty stack");
        }
        auto c = d_stack.top();
        d_stack.pop();
        newline();
        d_out << c;

        in_mapping()
                .map([this](auto) { d_expect_key = true; return 0; });

        return result::ok(this);
    }

    void flush() {
        while(!d_stack.empty()) {
            pop().unwrap();
        }
    }

  private:
    template <typename T>
    result scalar(const T& v) {
        return expect_value()
            .map([this, &v](auto* t) {
                assert(this == t);

                comma();

                d_out << v;
                d_needscomma = true;

                in_mapping()
                        .map([this](auto) { d_expect_key = true; return 0; });

                return this;
            });
    }

    result with_key(string_view key) {
        if (!d_expect_key) {
            return result::err("not expecting a key");
        }

        return in_mapping()
            .map([this, key](auto* t) {
                assert(this == t);
                comma();

                d_out << quoted(key);
                d_out << (d_compact ? ":" : ": ");
                d_needscomma = false;
                d_expect_key = false;
                return this;
            });
    }

    result expect_value() {
        if(!d_stack.empty() && d_stack.top() == '}' && d_expect_key) {
            return result::err("not expecting a value");
        } else {
            return result::ok(this);
        }
    }

    result in_mapping() {
        if(d_stack.empty() || d_stack.top() != '}') {
            return result::err("top of the stack is not a mapping");
        } else {
            return result::ok(this);
        }
    }

    impl* do_mapping() {
        d_expect_key = true;
        return push('{', '}');
    }

    impl* do_sequence() {
        d_expect_key = false;
        return push('[', ']');
    }

    impl* push(char b, char e) {
        comma();
        d_out << b;
        d_stack.push(e);
        newline();
        d_needscomma = false;
        return this;
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

builder::result builder::key(string_view k) {
    assert(d_pimpl);
    return d_pimpl->key(k).map([this](auto) { return this; });
}

builder::result builder::with_none() {
    assert(d_pimpl);
    return d_pimpl->with_none().map([this](auto) { return this; });
}

builder::result builder::with_bool(bool v) {
    assert(d_pimpl);
    return d_pimpl->with_bool(v).map([this](auto) { return this; });
}

builder::result builder::with_int(int64_t v) {
    assert(d_pimpl);
    return d_pimpl->with_int(v).map([this](auto) { return this; });
}

builder::result builder::with_uint(uint64_t v) {
    assert(d_pimpl);
    return d_pimpl->with_uint(v).map([this](auto) { return this; });
}

builder::result builder::with_float(double v) {
    assert(d_pimpl);
    return d_pimpl->with_float(v).map([this](auto) { return this; });
}

builder::result builder::with_string(string_view v) {
    assert(d_pimpl);
    return d_pimpl->with_string(v).map([this](auto) { return this; });
}

builder::result builder::push_mapping() {
    assert(d_pimpl);
    return d_pimpl->push_mapping().map([this](auto) { return this; });
}

builder::result builder::push_sequence() {
    assert(d_pimpl);
    return d_pimpl->push_sequence().map([this](auto) { return this; });
}

builder::result builder::pop() {
    assert(d_pimpl);
    return d_pimpl->pop().map([this](auto) { return this; });
}

void builder::flush() {
    assert(d_pimpl);
    d_pimpl->flush();
}

} // namespace kjson
