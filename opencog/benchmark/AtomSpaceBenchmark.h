#ifndef _OPENCOG_AS_BENCHMARK_H
#define _OPENCOG_AS_BENCHMARK_H

#include <opencog/atomspace/types.h>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/util/mt19937ar.h>
#include <boost/tuple/tuple.hpp>

using boost::tuple;

namespace opencog
{

class AtomSpaceBenchmark
{
    // size of AtomSpace, time taken for operation, rss memory max
    typedef tuple<size_t,clock_t,long> record_t;

    struct TimeStats {
        clock_t t_total;
        clock_t t_max;
        clock_t t_min;
        clock_t t_mean;
        clock_t t_std;
        long t_N;
        TimeStats(const std::vector<record_t> records);
        void print();
    };


    void dumpToCSV(std::string filename, std::vector<record_t> records) const;

    float linkSize_mean;
    float linkSize_std;

    Type defaultLinkType;
    float chanceOfNonDefaultLink;
    Type defaultNodeType;
    float chanceOfNonDefaultNode;

    float maxSize; //! never make the atomspace bigger than this while building it

    AtomSpace a;
    MT19937RandGen* rng;

    Type randomType(Type t);

    clock_t makeRandomNode(const std::string& s);
    clock_t makeRandomLink();

    long getMemUsage();
    int counter;

    std::string methodName;
public:
    int N;
    int sizeIncrease;
    bool saveToFile;
    int saveInterval;
    bool buildTestData;
    typedef clock_t (AtomSpaceBenchmark::*BMFn)();
    BMFn methodToTest;

    float percentLinks;
    long atomCount; //! number of nodes to build atomspace with before testing

    bool showTypeSizes;
    void printTypeSizes();

    AtomSpaceBenchmark();
    ~AtomSpaceBenchmark();

    void setMethod(std::string method);
    void showMethods();
    void startBenchmark(int numThreads=1);

    void buildAtomSpace(long atomspaceSize=(1 << 16), float percentLinks = 0.1);

    clock_t bm_addNode();
    clock_t bm_addLink();

    void bm_getType() {};
    void bm_getHandleSet() {};
    void bm_getHandleNode() {};
    void bm_getHandleLink() {};
    void bm_getName() {};

    // Get and set TV and AV
    void bm_TruthValue() {};
    void bm_TV() {};

    void bm_erase() {};

    size_t estimateOfAtomSize(Handle h);

};

} // namespace opencog

#endif //_OPENCOG_AS_BENCHMARK_H
