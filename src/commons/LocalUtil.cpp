#include "LocalUtil.h"
#include <cctype>


std::string LocalUtil::getQueryBaseName(const std::string & queryPath) {
    std::vector<std::string> splits = Util::split(queryPath, ".");
    std::string baseName;
    int extentionNum = 1;
    if (Util::endsWith(".gz", queryPath)) {
        extentionNum = 2;
    }
    for (size_t i = 0; i < splits.size() - extentionNum; ++i) {
        if (i == splits.size() - extentionNum - 1) {
            baseName += splits[i];
        } else {
            baseName += splits[i] + ".";
        }
    }
    return baseName;
}


void LocalUtil::splitQueryFile(std::vector<SequenceBlock> & sequences, const std::string &queryPath) {
    KSeqWrapper* kseq = nullptr;
    kseq = KSeqFactory(queryPath.c_str());
    while (kseq->ReadEntry()) {
        const KSeqWrapper::KSeqEntry & e = kseq->entry;
        sequences.emplace_back(e.headerOffset - 1,
                               e.sequenceOffset + e.sequence.l,
                               e.sequenceOffset + e.sequence.l - e.headerOffset + 2,
                               e.sequence.l);
    }
    delete kseq;
}

int LocalUtil::getMaxCoveredLength(int queryLength) {
    if (queryLength % 3 == 2) {
        return queryLength - 2; 
    } else if (queryLength % 3 == 1) {
        return queryLength - 4; 
    } else {
        return queryLength - 3; 
    }
}

int LocalUtil::getFirstWhiteSpacePos(const std::string &str) {
    for (size_t i = 0; i < str.size(); ++i) {
        if (isspace(int(str[i]))) {
            return i;
        }
    }
    return str.size();
}

void LocalUtil::loadUnorderedSetFromFile(std::unordered_set<int> &set, const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filePath);
    }
    std::string line;
    while (std::getline(file, line)) {
        set.insert(std::stoi(line));
    }
}