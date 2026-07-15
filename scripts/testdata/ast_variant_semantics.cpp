struct AstRecord {
    int value;
    int *pointer;
};

struct AstOwner {
    int value;

    int Read(int scalar)
    {
        return value + scalar;
    }
};

enum AstChoice {
    AST_FIRST,
    AST_SECOND
};

void ConsumeReference(int &value);

int SafeHelpers(AstRecord record, AstRecord &recordRef, int scalar, AstChoice choice)
{
    int local = record.value + scalar;
    int nested = (recordRef.value + scalar) * static_cast<int>(choice);
    int pointerValue = *record.pointer + scalar;
    return local + nested + pointerValue;
}

int SafeIntegerOperators(int value, int divisor)
{
    int quotient = value / divisor;
    int difference = value - divisor;
    return quotient + difference;
}

int RejectedMemberContexts(AstRecord record, volatile AstRecord &volatileRecord, int scalar)
{
    record.value = scalar;
    int *address = &record.value;
    ConsumeReference(record.value);
    const int &alias = record.value;
    int observed = volatileRecord.value + scalar;
    return *address + alias + observed;
}

int SafeReadAdvance(int *cursor)
{
    int value;
    value = *cursor;
    cursor = cursor + 1;
    return value;
}

int RejectedReadAdvance(AstRecord record, int *cursor, volatile int *volatileCursor)
{
    record.value = *cursor;
    cursor = cursor + 1;
    int value;
    value = *volatileCursor;
    volatileCursor = volatileCursor + 1;
    return record.value + value;
}

int RejectedAliasingStores(int *p, int *q)
{
    *p = 1;
    q[0] = 2;
    return *p + *q;
}

int SafeStatementOrder(int firstInput, int secondInput)
{
    int first;
    int second;
    first = firstInput;
    second = secondInput;
    return first + second;
}

int SafeDeclarations()
{
    int first = 1, second = 2;
    int third = 3;
    int fourth = 4;
    return first + second + third + fourth;
}

int RejectedVolatileOrder(volatile AstRecord &record, int scalar)
{
    int first;
    int second;
    first = record.value;
    second = scalar;
    return first + second;
}

int RejectedLineMacros()
{
    int first = __LINE__, second = __LINE__;
    int third = __LINE__;
    int fourth = __LINE__;
    third = __LINE__;
    fourth = __LINE__;
    return first + second + third + fourth;
}

float RejectedFloatingCommutative(float left, float right)
{
    return left + right;
}
