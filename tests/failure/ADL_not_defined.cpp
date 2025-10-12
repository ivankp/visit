// error.*static.*assert.*VisitADL is not specialized for these types.

#include "Visit.hpp"
using visit::Visit;

struct S { int i; };

int main() {
    S s;
    Visit(s, [&](int s) { });
}
