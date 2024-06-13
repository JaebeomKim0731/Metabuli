#ifndef ADKMER4_INDEXCREATOR_H
#define ADKMER4_INDEXCREATOR_H
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <ctime>
#include <fstream>
#include "printBinary.h"
#include "Mmap.h"
#include "Kmer.h"
#include "SeqIterator.h"
#include "BitManipulateMacros.h"
#include "common.h"
#include "NcbiTaxonomy.h"
#include "FastSort.h"
#include "LocalParameters.h"
#include "NucleotideMatrix.h"
#include "SubstitutionMatrix.h"
#include "tantan.h"
#include "LocalUtil.h"
#include "ProteinDbIndexer.h"


#ifdef OPENMP
#include <omp.h>
#endif


struct TaxId2Fasta{
    TaxID species;
    TaxID taxid;
    string fasta;
    TaxId2Fasta(TaxID sp, TaxID ssp, string fasta): species(sp), taxid(ssp), fasta(std::move(fasta)) {}
};

using namespace std;

class IndexCreator{
protected:
    const LocalParameters &par;
    uint64_t MARKER;
    BaseMatrix *subMat;

    // Parameters
    int threadNum;
    size_t bufferSize;
    int reducedAA;
    string spaceMask;
    int accessionLevel;
    int lowComplexityMasking;
    float lowComplexityMaskingThreshold;
    string dbName;
    string dbDate;
    
    // Inputs
    NcbiTaxonomy * taxonomy;
    string dbDir;
    string fnaListFileName;
    string taxonomyDir;
    string acc2taxidFileName;

    // Outputs
    string taxidListFileName;
    string taxonomyBinaryFileName;
    string versionFileName;
    string paramterFileName;

    struct FASTA {
        string path;
        TaxID speciesID;
        size_t trainingSeqIdx;
        vector<SequenceBlock> sequences;
    };

    TaxID newTaxID;
    vector<FASTA> fastaList;
    vector<TaxID> taxIdList;
    vector<size_t> processedSeqCnt; // Index of this vector is the same as the index of fnaList
    std::unordered_map<std::string, TaxID> foundAcc2taxid;

    struct FnaSplit{
        // species, file_idx, training, offset, cnt
        size_t training;
        size_t offset;
        size_t cnt;
        TaxID speciesID;
        int file_idx;
        FnaSplit(size_t training, size_t offset, size_t cnt, TaxID speciesID, int file_idx):
                training(training), offset(offset), cnt(cnt), speciesID(speciesID), file_idx(file_idx) {}
    };
    vector<FnaSplit> fnaSplits;

    struct FastaSplit{
        FastaSplit(size_t training, uint32_t offset, uint32_t cnt, TaxID taxid):
            training(training), offset(offset), cnt(cnt), taxid(taxid) {}
        size_t training;
        uint32_t offset;
        uint32_t cnt;
        TaxID taxid;
    };

    size_t numOfFlush=0;

    void writeTaxIdList();

    void writeTargetFiles(TargetKmer * kmerBuffer, size_t & kmerNum, const LocalParameters & par, const size_t * uniqeKmerIdx, size_t & uniqKmerCnt);

    void writeTargetFilesAndSplits(TargetKmer * kmerBuffer, size_t & kmerNum, const LocalParameters & par, const size_t * uniqeKmerIdx, size_t & uniqKmerCnt);

    void writeTaxonomyDB();

    void writeDbParameters();

    static bool compareForDiffIdx(const TargetKmer & a, const TargetKmer & b);
    
    static bool compareForDiffIdx2(const TargetKmer & a, const TargetKmer & b);

    size_t fillTargetKmerBuffer(Buffer<TargetKmer> &kmerBuffer,
                                bool *checker,
                                size_t &processedSplitCnt);

    int selectReadingFrame(priority_queue<uint64_t> & observedKmers, const char * seq, const SeqIterator & seqIt);

    void makeBlocksForParallelProcessing();

    void makeBlocksForParallelProcessing_accession_level();

    void splitFastaForProdigalTraining(int file_idx, TaxID speciesID);

    void unzipAndList(const string & folder, const string & fastaList_fname){
        system(("./../../util/unzip_and_list.sh " + folder + " " + fastaList_fname).c_str());
    }

    void load_assacc2taxid(const string & mappingFile, unordered_map<string, int> & assacc2taxid);


    static TaxID load_accession2taxid(const string & mappingFile, unordered_map<string, int> & assacc2taxid);

    TaxID getMaxTaxID();

    void editTaxonomyDumpFiles(const vector<pair<string, pair<TaxID, TaxID>>> & newAcc2taxid);

    void reduceRedundancy(Buffer<TargetKmer> & kmerBuffer, size_t * uniqeKmerIdx, size_t & uniqKmerCnt,
                          const LocalParameters & par);
    size_t AminoAcidPart(size_t kmer) {
        return (kmer) & MARKER;
    }

    int getNumberOfLines(const string & filename){
        ifstream file(filename);
        int cnt = 0;
        string line;
        while (getline(file, line)) {
            cnt++;
        }
        file.close();
        return cnt;
    }

public:
    static void splitSequenceFile(vector<SequenceBlock> & seqSegments, MmapedData<char> seqFile);

    static void printIndexSplitList(DiffIdxSplit * splitList) {
        for (int i = 0; i < 4096; i++) {
            cout << splitList[i].infoIdxOffset << " " << 
                    splitList[i].diffIdxOffset << " " << 
                    splitList[i].ADkmer << endl;
        }
    }

    string getSeqSegmentsWithHead(vector<SequenceBlock> & seqSegments,
                                  const string & seqFileName,
                                  const unordered_map<string, TaxID> & acc2taxid,
                                  vector<pair<string, pair<TaxID, TaxID>>> & newAcc2taxid);

    string getSeqSegmentsWithHead(vector<SequenceBlock> & seqSegments,
                                  const string & seqFileName,
                                  const unordered_map<string, TaxID> & acc2taxid,
                                  unordered_map<string, TaxID> & foundAcc2taxid);

    static void getSeqSegmentsWithHead(vector<SequenceBlock> & seqSegments, const char * seqFileName);
    IndexCreator(const LocalParameters & par);
    ~IndexCreator();
    int getNumOfFlush();

    static void writeDiffIdx(uint16_t *buffer, FILE* handleKmerTable, uint16_t *toWrite, size_t size, size_t & localBufIdx, size_t bufferSize);

    void createIndex();

    static void getDiffIdx(uint64_t lastKmer, uint64_t entryToWrite, FILE* handleKmerTable,
                    uint16_t *kmerBuf, size_t bufferSize, size_t & localBufIdx);

    static void getDiffIdx(uint64_t lastKmer, uint64_t entryToWrite, FILE* handleKmerTable,
                    uint16_t *kmerBuf, size_t bufferSize, size_t & localBufIdx, size_t & totalBufferIdx);

    void writeInfo(TargetKmerInfo * entryToWrite, FILE * infoFile, TargetKmerInfo * infoBuffer, size_t & infoBufferIdx);
    static void flushKmerBuf(uint16_t *buffer, FILE *handleKmerTable, size_t & localBufIdx);
    static void flushInfoBuf(TargetKmerInfo * buffer, FILE * infoFile, size_t & localBufIdx);

};
#endif //ADKMER4_INDEXCREATOR_H
