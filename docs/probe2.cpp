class P {
public:
    int f(int arg);
    int m;
};
// SAME decl order, wildly different names/buckets
int P::f(int arg) {
    int zzz;
    int aaaaaa;
    int qq;
    qq = arg;
    aaaaaa = arg + 1;
    zzz = arg + 2;
    m = qq + aaaaaa + zzz;
    return m;
}
