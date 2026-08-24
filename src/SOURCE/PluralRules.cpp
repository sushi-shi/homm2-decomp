#include <SOURCE/PluralRules.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <limits>
#include <string>

namespace localization::plural {
namespace {

class ExpressionParser {
public:
    ExpressionParser(const std::string& expression, std::uint32_t count)
        : m_expression(expression), m_count(count) {}

    bool Evaluate(std::int64_t& value) {
        value = Conditional();
        SkipSpace();
        return m_valid && m_position == m_expression.size();
    }

private:
    void SkipSpace() {
        while (m_position < m_expression.size()
               && std::isspace(static_cast<unsigned char>(m_expression[m_position]))) {
            ++m_position;
        }
    }

    bool Consume(const char* token) {
        SkipSpace();
        const std::size_t length = std::char_traits<char>::length(token);
        if (m_expression.compare(m_position, length, token) != 0) {
            return false;
        }
        m_position += length;
        return true;
    }

    std::int64_t Conditional() {
        std::int64_t condition = LogicalOr();
        if (Consume("?")) {
            const std::int64_t whenTrue = Conditional();
            if (!Consume(":")) {
                m_valid = false;
                return 0;
            }
            const std::int64_t whenFalse = Conditional();
            return condition != 0 ? whenTrue : whenFalse;
        }
        return condition;
    }

    std::int64_t LogicalOr() {
        std::int64_t value = LogicalAnd();
        while (Consume("||")) {
            const std::int64_t right = LogicalAnd();
            value = value != 0 || right != 0;
        }
        return value;
    }

    std::int64_t LogicalAnd() {
        std::int64_t value = Equality();
        while (Consume("&&")) {
            const std::int64_t right = Equality();
            value = value != 0 && right != 0;
        }
        return value;
    }

    std::int64_t Equality() {
        std::int64_t value = Relational();
        while (true) {
            if (Consume("==")) {
                value = value == Relational();
            } else if (Consume("!=")) {
                value = value != Relational();
            } else {
                return value;
            }
        }
    }

    std::int64_t Relational() {
        std::int64_t value = Additive();
        while (true) {
            if (Consume("<=")) {
                value = value <= Additive();
            } else if (Consume(">=")) {
                value = value >= Additive();
            } else if (Consume("<")) {
                value = value < Additive();
            } else if (Consume(">")) {
                value = value > Additive();
            } else {
                return value;
            }
        }
    }

    std::int64_t Additive() {
        std::int64_t value = Multiplicative();
        while (true) {
            if (Consume("+")) {
                value += Multiplicative();
            } else if (Consume("-")) {
                value -= Multiplicative();
            } else {
                return value;
            }
        }
    }

    std::int64_t Multiplicative() {
        std::int64_t value = Unary();
        while (true) {
            if (Consume("*")) {
                value *= Unary();
            } else if (Consume("/")) {
                const std::int64_t divisor = Unary();
                if (divisor == 0) {
                    m_valid = false;
                    return 0;
                }
                value /= divisor;
            } else if (Consume("%")) {
                const std::int64_t divisor = Unary();
                if (divisor == 0) {
                    m_valid = false;
                    return 0;
                }
                value %= divisor;
            } else {
                return value;
            }
        }
    }

    std::int64_t Unary() {
        if (Consume("!")) {
            return Unary() == 0;
        }
        if (Consume("-")) {
            return -Unary();
        }
        if (Consume("+")) {
            return Unary();
        }
        return Primary();
    }

    std::int64_t Primary() {
        if (Consume("(")) {
            const std::int64_t value = Conditional();
            if (!Consume(")")) {
                m_valid = false;
            }
            return value;
        }
        SkipSpace();
        if (m_position < m_expression.size() && m_expression[m_position] == 'n') {
            ++m_position;
            return m_count;
        }
        if (m_position >= m_expression.size()
            || !std::isdigit(static_cast<unsigned char>(m_expression[m_position]))) {
            m_valid = false;
            return 0;
        }
        std::uint64_t value = 0;
        while (m_position < m_expression.size()
               && std::isdigit(static_cast<unsigned char>(m_expression[m_position]))) {
            const unsigned digit = static_cast<unsigned>(m_expression[m_position] - '0');
            if (value > (std::numeric_limits<std::uint64_t>::max() - digit) / 10) {
                m_valid = false;
                return 0;
            }
            value = value * 10 + digit;
            ++m_position;
        }
        if (value > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max())) {
            m_valid = false;
            return 0;
        }
        return static_cast<std::int64_t>(value);
    }

    const std::string& m_expression;
    std::uint32_t m_count;
    std::size_t m_position = 0;
    bool m_valid = true;
};

std::string Trim(std::string value) {
    const auto nonSpace = [](unsigned char character) {
        return std::isspace(character) == 0;
    };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), nonSpace));
    value.erase(std::find_if(value.rbegin(), value.rend(), nonSpace).base(), value.end());
    return value;
}

bool Field(const std::string& header, const std::string& name, std::string& value) {
    std::size_t position = 0;
    while (position < header.size()) {
        const std::size_t end = header.find(';', position);
        const std::string field = Trim(header.substr(position, end - position));
        const std::size_t equals = field.find('=');
        if (equals != std::string::npos && Trim(field.substr(0, equals)) == name) {
            value = Trim(field.substr(equals + 1));
            return true;
        }
        if (end == std::string::npos) {
            break;
        }
        position = end + 1;
    }
    return false;
}

}

bool Parse(const std::string& header, Rules& rules, std::string& error) {
    std::string countText;
    std::string expression;
    if (!Field(header, "nplurals", countText) || !Field(header, "plural", expression)) {
        error = "Plural-Forms must define nplurals and plural";
        return false;
    }
    if (countText.empty()
        || !std::all_of(countText.begin(), countText.end(), [](unsigned char character) {
            return std::isdigit(character) != 0;
        })) {
        error = "nplurals is not a positive integer";
        return false;
    }
    std::uint64_t formCount = 0;
    for (char character : countText) {
        formCount = formCount * 10 + static_cast<unsigned>(character - '0');
        if (formCount > 16) {
            break;
        }
    }
    if (formCount == 0 || formCount > 16 || expression.empty()) {
        error = "nplurals must be between 1 and 16 and plural must not be empty";
        return false;
    }

    Rules parsed;
    parsed.formCount = static_cast<std::size_t>(formCount);
    parsed.expression = expression;
    for (std::uint32_t sample = 0; sample < 1000; ++sample) {
        std::int64_t selected = 0;
        ExpressionParser parser(parsed.expression, sample);
        if (!parser.Evaluate(selected) || selected < 0
            || static_cast<std::uint64_t>(selected) >= formCount) {
            error = "plural expression is invalid or selects a form outside nplurals";
            return false;
        }
    }
    rules = std::move(parsed);
    return true;
}

std::size_t Select(const Rules& rules, std::uint32_t count) {
    std::int64_t selected = 0;
    ExpressionParser parser(rules.expression, count);
    if (!parser.Evaluate(selected) || selected < 0) {
        return count == 1 ? 0 : std::min<std::size_t>(1, rules.formCount - 1);
    }
    return std::min<std::size_t>(static_cast<std::size_t>(selected), rules.formCount - 1);
}

}
