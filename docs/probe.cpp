class P {
public:
    int f(int arg);
    int m;
};

// locals chosen to span od_slots buckets: standing=12, stepCount=9, pathIndex=9
int P::f(int arg) {
    int standing;
    int stepCount;
    int pathIndex;
    pathIndex = arg;
    stepCount = arg + 1;
    standing = arg + 2;
    m = pathIndex + stepCount + standing;
    return m;
}
