#pragma once
#include <cmath>

class BM25
{
public:
    BM25(double k1 = 1.5, double b = 0.75)
        : k1(k1), b(b) {}

    double score(int tf, int df, int doc_len,
                 double avgdl, int N) const
    {
        double idf = log((N - df + 0.5) / (df + 0.5));
        return idf *
               ((tf * (k1 + 1)) /
                (tf + k1 * (1 - b + b * doc_len / avgdl)));
    }

private:
    double k1;
    double b;
};
