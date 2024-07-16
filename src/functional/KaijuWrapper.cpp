#include "KaijuWrapper.h"
#include "Config.hpp"
#include "bwt/bwt.h"

KaijuWrapper::KaijuWrapper(const LocalParameters & par) : par(par) {
    this->config = new Config();
	config->SEG = false;

    if (par.kaijuMode == "mem") {
        config->mode = MEM;
        config->use_Evalue = false;
    } else if (par.kaijuMode == "greedy") {
        config->mode = GREEDY;
    } else {
        std::cerr << "Unknown mode: " << par.kaijuMode << std::endl;
        exit(1);
    }
	
	std::cout << "Loading Kaiju FMI index...";
    readFMI(par.filenames[3], config);
	std::cout << "done" << std::endl;
    config->init();
    blosum_subst = {
					{'A',{'S', 'V', 'T', 'G', 'C', 'P', 'M', 'K', 'L', 'I', 'E', 'Q', 'R', 'Y', 'F', 'H', 'D', 'N', 'W' }},
					{'R',{'K', 'Q', 'H', 'E', 'N', 'T', 'S', 'M', 'A', 'Y', 'P', 'L', 'G', 'D', 'V', 'W', 'F', 'I', 'C' }},
					{'N',{'S', 'H', 'D', 'T', 'K', 'G', 'E', 'Q', 'R', 'Y', 'P', 'M', 'A', 'V', 'F', 'L', 'I', 'C', 'W' }},
					{'D',{'E', 'N', 'S', 'Q', 'T', 'P', 'K', 'H', 'G', 'R', 'A', 'V', 'Y', 'F', 'M', 'I', 'C', 'W', 'L' }},
					{'C',{'A', 'V', 'T', 'S', 'M', 'L', 'I', 'Y', 'W', 'F', 'P', 'K', 'H', 'G', 'Q', 'D', 'N', 'R', 'E' }},
					{'Q',{'E', 'K', 'R', 'S', 'M', 'H', 'D', 'N', 'Y', 'T', 'P', 'A', 'V', 'W', 'L', 'G', 'F', 'I', 'C' }},
					{'E',{'Q', 'D', 'K', 'S', 'H', 'N', 'R', 'T', 'P', 'A', 'V', 'Y', 'M', 'G', 'W', 'F', 'L', 'I', 'C' }},
					{'G',{'S', 'N', 'A', 'D', 'W', 'T', 'P', 'K', 'H', 'E', 'Q', 'R', 'V', 'Y', 'F', 'M', 'C', 'L', 'I' }},
					{'H',{'Y', 'N', 'E', 'Q', 'R', 'S', 'F', 'K', 'D', 'W', 'T', 'P', 'M', 'G', 'A', 'V', 'L', 'I', 'C' }},
					{'I',{'V', 'L', 'M', 'F', 'Y', 'T', 'C', 'A', 'S', 'W', 'P', 'K', 'H', 'E', 'Q', 'D', 'N', 'R', 'G' }},
					{'L',{'M', 'I', 'V', 'F', 'Y', 'T', 'C', 'A', 'W', 'S', 'K', 'Q', 'R', 'P', 'H', 'E', 'N', 'G', 'D' }},
					{'K',{'R', 'E', 'Q', 'S', 'N', 'T', 'P', 'M', 'H', 'D', 'A', 'V', 'Y', 'L', 'G', 'W', 'F', 'I', 'C' }},
					{'M',{'L', 'V', 'I', 'F', 'Q', 'Y', 'W', 'T', 'S', 'K', 'C', 'R', 'A', 'P', 'H', 'E', 'N', 'G', 'D' }},
					{'F',{'Y', 'W', 'M', 'L', 'I', 'V', 'H', 'T', 'S', 'C', 'A', 'K', 'G', 'E', 'Q', 'D', 'N', 'R', 'P' }},
					{'P',{'T', 'S', 'K', 'E', 'Q', 'D', 'A', 'V', 'M', 'H', 'G', 'N', 'R', 'Y', 'L', 'I', 'C', 'W', 'F' }},
					{'S',{'T', 'N', 'A', 'K', 'G', 'E', 'Q', 'D', 'P', 'M', 'H', 'C', 'R', 'V', 'Y', 'F', 'L', 'I', 'W' }},
					{'T',{'S', 'V', 'N', 'A', 'P', 'M', 'K', 'L', 'I', 'E', 'Q', 'C', 'D', 'R', 'Y', 'W', 'F', 'H', 'G' }},
					{'W',{'Y', 'F', 'M', 'T', 'L', 'H', 'G', 'Q', 'C', 'V', 'S', 'K', 'I', 'E', 'R', 'A', 'P', 'D', 'N' }},
					{'Y',{'F', 'W', 'H', 'V', 'M', 'L', 'I', 'Q', 'T', 'S', 'K', 'E', 'C', 'N', 'R', 'A', 'P', 'G', 'D' }},
					{'V',{'I', 'M', 'L', 'T', 'A', 'Y', 'F', 'C', 'S', 'P', 'K', 'E', 'Q', 'W', 'H', 'G', 'D', 'N', 'R' }} };

	std::memset(nuc2int, std::numeric_limits<uint8_t>::max(), sizeof(nuc2int));
	nuc2int['A'] = nuc2int['a'] = 0;
	nuc2int['C'] = nuc2int['c'] = 1;
	nuc2int['G'] = nuc2int['g'] = 2;
	nuc2int['T'] = nuc2int['t'] = 3;
	nuc2int['U'] = nuc2int['u'] = 3;
	std::memset(compnuc2int, std::numeric_limits<uint8_t>::max(), sizeof(compnuc2int));
	compnuc2int['A'] = compnuc2int['a'] = 3;
	compnuc2int['C'] = compnuc2int['c'] = 2;
	compnuc2int['G'] = compnuc2int['g'] = 1;
	compnuc2int['T'] = compnuc2int['t'] = 0;
	compnuc2int['U'] = compnuc2int['u'] = 0;

	std::memset(aa2int, std::numeric_limits<uint8_t>::min(), sizeof(aa2int));
	aa2int['A'] = 0;
	aa2int['R'] = 1;
	aa2int['N'] = 2;
	aa2int['D'] = 3;
	aa2int['C'] = 4;
	aa2int['Q'] = 5;
	aa2int['E'] = 6;
	aa2int['G'] = 7;
	aa2int['H'] = 8;
	aa2int['I'] = 9;
	aa2int['L'] = 10;
	aa2int['K'] = 11;
	aa2int['M'] = 12;
	aa2int['F'] = 13;
	aa2int['P'] = 14;
	aa2int['S'] = 15;
	aa2int['T'] = 16;
	aa2int['W'] = 17;
	aa2int['Y'] = 18;
	aa2int['V'] = 19;
	blosum62diag[aa2int['A']] = 4;
	blosum62diag[aa2int['R']] = 5;
	blosum62diag[aa2int['N']] = 6;
	blosum62diag[aa2int['D']] = 6;
	blosum62diag[aa2int['C']] = 9;
	blosum62diag[aa2int['Q']] = 5;
	blosum62diag[aa2int['E']] = 5;
	blosum62diag[aa2int['G']] = 6;
	blosum62diag[aa2int['H']] = 8;
	blosum62diag[aa2int['I']] = 4;
	blosum62diag[aa2int['L']] = 4;
	blosum62diag[aa2int['K']] = 5;
	blosum62diag[aa2int['M']] = 5;
	blosum62diag[aa2int['F']] = 6;
	blosum62diag[aa2int['P']] = 7;
	blosum62diag[aa2int['S']] = 4;
	blosum62diag[aa2int['T']] = 5;
	blosum62diag[aa2int['W']] =11;
	blosum62diag[aa2int['Y']] = 7;
	blosum62diag[aa2int['V']] = 4;


	b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'R']]=-1; b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'N']]=-2; b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'D']]=-2; b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'C']]=0; b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'Q']]=-1; b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'E']]=-1; b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'G']]=0; b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'H']]=-2; b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'I']]=-1; b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'L']]=-1; b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'K']]=-1; b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'M']]=-1; b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'F']]=-2; b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'P']]=-1; b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'S']]=1; b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'T']]=0; b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'W']]=-3; b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'Y']]=-2; b62[aa2int[(uint8_t)'A']][aa2int[(uint8_t)'V']]=0; 
	b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'A']]=-1; b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'N']]=0; b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'D']]=-2; b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'C']]=-3; b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'Q']]=1; b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'E']]=0; b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'G']]=-2; b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'H']]=0; b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'I']]=-3; b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'L']]=-2; b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'K']]=2; b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'M']]=-1; b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'F']]=-3; b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'P']]=-2; b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'S']]=-1; b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'T']]=-1; b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'W']]=-3; b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'Y']]=-2; b62[aa2int[(uint8_t)'R']][aa2int[(uint8_t)'V']]=-3; 
	b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'A']]=-2; b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'R']]=0; b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'D']]=1; b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'C']]=-3; b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'Q']]=0; b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'E']]=0; b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'G']]=0; b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'H']]=1; b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'I']]=-3; b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'L']]=-3; b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'K']]=0; b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'M']]=-2; b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'F']]=-3; b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'P']]=-2; b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'S']]=1; b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'T']]=0; b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'W']]=-4; b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'Y']]=-2; b62[aa2int[(uint8_t)'N']][aa2int[(uint8_t)'V']]=-3; 
	b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'A']]=-2; b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'R']]=-2; b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'N']]=1; b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'C']]=-3; b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'Q']]=0; b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'E']]=2; b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'G']]=-1; b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'H']]=-1; b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'I']]=-3; b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'L']]=-4; b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'K']]=-1; b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'M']]=-3; b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'F']]=-3; b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'P']]=-1; b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'S']]=0; b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'T']]=-1; b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'W']]=-4; b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'Y']]=-3; b62[aa2int[(uint8_t)'D']][aa2int[(uint8_t)'V']]=-3; 
	b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'A']]=0; b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'R']]=-3; b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'N']]=-3; b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'D']]=-3; b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'Q']]=-3; b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'E']]=-4; b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'G']]=-3; b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'H']]=-3; b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'I']]=-1; b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'L']]=-1; b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'K']]=-3; b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'M']]=-1; b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'F']]=-2; b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'P']]=-3; b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'S']]=-1; b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'T']]=-1; b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'W']]=-2; b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'Y']]=-2; b62[aa2int[(uint8_t)'C']][aa2int[(uint8_t)'V']]=-1; 
	b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'A']]=-1; b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'R']]=1; b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'N']]=0; b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'D']]=0; b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'C']]=-3; b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'E']]=2; b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'G']]=-2; b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'H']]=0; b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'I']]=-3; b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'L']]=-2; b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'K']]=1; b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'M']]=0; b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'F']]=-3; b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'P']]=-1; b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'S']]=0; b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'T']]=-1; b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'W']]=-2; b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'Y']]=-1; b62[aa2int[(uint8_t)'Q']][aa2int[(uint8_t)'V']]=-2; 
	b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'A']]=-1; b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'R']]=0; b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'N']]=0; b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'D']]=2; b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'C']]=-4; b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'Q']]=2; b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'G']]=-2; b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'H']]=0; b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'I']]=-3; b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'L']]=-3; b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'K']]=1; b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'M']]=-2; b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'F']]=-3; b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'P']]=-1; b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'S']]=0; b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'T']]=-1; b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'W']]=-3; b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'Y']]=-2; b62[aa2int[(uint8_t)'E']][aa2int[(uint8_t)'V']]=-2; 
	b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'A']]=0; b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'R']]=-2; b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'N']]=0; b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'D']]=-1; b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'C']]=-3; b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'Q']]=-2; b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'E']]=-2; b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'H']]=-2; b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'I']]=-4; b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'L']]=-4; b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'K']]=-2; b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'M']]=-3; b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'F']]=-3; b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'P']]=-2; b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'S']]=0; b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'T']]=-2; b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'W']]=-2; b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'Y']]=-3; b62[aa2int[(uint8_t)'G']][aa2int[(uint8_t)'V']]=-3; 
	b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'A']]=-2; b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'R']]=0; b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'N']]=1; b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'D']]=-1; b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'C']]=-3; b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'Q']]=0; b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'E']]=0; b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'G']]=-2; b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'I']]=-3; b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'L']]=-3; b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'K']]=-1; b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'M']]=-2; b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'F']]=-1; b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'P']]=-2; b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'S']]=-1; b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'T']]=-2; b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'W']]=-2; b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'Y']]=2; b62[aa2int[(uint8_t)'H']][aa2int[(uint8_t)'V']]=-3; 
	b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'A']]=-1; b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'R']]=-3; b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'N']]=-3; b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'D']]=-3; b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'C']]=-1; b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'Q']]=-3; b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'E']]=-3; b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'G']]=-4; b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'H']]=-3; b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'L']]=2; b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'K']]=-3; b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'M']]=1; b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'F']]=0; b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'P']]=-3; b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'S']]=-2; b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'T']]=-1; b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'W']]=-3; b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'Y']]=-1; b62[aa2int[(uint8_t)'I']][aa2int[(uint8_t)'V']]=3; 
	b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'A']]=-1; b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'R']]=-2; b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'N']]=-3; b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'D']]=-4; b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'C']]=-1; b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'Q']]=-2; b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'E']]=-3; b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'G']]=-4; b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'H']]=-3; b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'I']]=2; b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'K']]=-2; b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'M']]=2; b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'F']]=0; b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'P']]=-3; b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'S']]=-2; b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'T']]=-1; b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'W']]=-2; b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'Y']]=-1; b62[aa2int[(uint8_t)'L']][aa2int[(uint8_t)'V']]=1; 
	b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'A']]=-1; b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'R']]=2; b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'N']]=0; b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'D']]=-1; b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'C']]=-3; b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'Q']]=1; b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'E']]=1; b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'G']]=-2; b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'H']]=-1; b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'I']]=-3; b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'L']]=-2; b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'M']]=-1; b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'F']]=-3; b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'P']]=-1; b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'S']]=0; b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'T']]=-1; b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'W']]=-3; b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'Y']]=-2; b62[aa2int[(uint8_t)'K']][aa2int[(uint8_t)'V']]=-2; 
	b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'A']]=-1; b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'R']]=-1; b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'N']]=-2; b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'D']]=-3; b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'C']]=-1; b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'Q']]=0; b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'E']]=-2; b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'G']]=-3; b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'H']]=-2; b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'I']]=1; b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'L']]=2; b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'K']]=-1; b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'F']]=0; b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'P']]=-2; b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'S']]=-1; b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'T']]=-1; b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'W']]=-1; b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'Y']]=-1; b62[aa2int[(uint8_t)'M']][aa2int[(uint8_t)'V']]=1; 
	b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'A']]=-2; b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'R']]=-3; b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'N']]=-3; b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'D']]=-3; b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'C']]=-2; b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'Q']]=-3; b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'E']]=-3; b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'G']]=-3; b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'H']]=-1; b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'I']]=0; b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'L']]=0; b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'K']]=-3; b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'M']]=0; b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'P']]=-4; b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'S']]=-2; b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'T']]=-2; b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'W']]=1; b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'Y']]=3; b62[aa2int[(uint8_t)'F']][aa2int[(uint8_t)'V']]=-1; 
	b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'A']]=-1; b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'R']]=-2; b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'N']]=-2; b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'D']]=-1; b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'C']]=-3; b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'Q']]=-1; b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'E']]=-1; b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'G']]=-2; b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'H']]=-2; b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'I']]=-3; b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'L']]=-3; b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'K']]=-1; b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'M']]=-2; b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'F']]=-4; b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'S']]=-1; b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'T']]=-1; b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'W']]=-4; b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'Y']]=-3; b62[aa2int[(uint8_t)'P']][aa2int[(uint8_t)'V']]=-2; 
	b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'A']]=1; b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'R']]=-1; b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'N']]=1; b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'D']]=0; b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'C']]=-1; b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'Q']]=0; b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'E']]=0; b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'G']]=0; b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'H']]=-1; b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'I']]=-2; b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'L']]=-2; b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'K']]=0; b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'M']]=-1; b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'F']]=-2; b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'P']]=-1; b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'T']]=1; b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'W']]=-3; b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'Y']]=-2; b62[aa2int[(uint8_t)'S']][aa2int[(uint8_t)'V']]=-2; 
	b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'A']]=0; b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'R']]=-1; b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'N']]=0; b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'D']]=-1; b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'C']]=-1; b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'Q']]=-1; b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'E']]=-1; b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'G']]=-2; b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'H']]=-2; b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'I']]=-1; b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'L']]=-1; b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'K']]=-1; b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'M']]=-1; b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'F']]=-2; b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'P']]=-1; b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'S']]=1; b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'W']]=-2; b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'Y']]=-2; b62[aa2int[(uint8_t)'T']][aa2int[(uint8_t)'V']]=0; 
	b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'A']]=-3; b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'R']]=-3; b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'N']]=-4; b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'D']]=-4; b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'C']]=-2; b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'Q']]=-2; b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'E']]=-3; b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'G']]=-2; b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'H']]=-2; b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'I']]=-3; b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'L']]=-2; b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'K']]=-3; b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'M']]=-1; b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'F']]=1; b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'P']]=-4; b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'S']]=-3; b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'T']]=-2; b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'Y']]=2; b62[aa2int[(uint8_t)'W']][aa2int[(uint8_t)'V']]=-3; 
	b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'A']]=-2; b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'R']]=-2; b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'N']]=-2; b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'D']]=-3; b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'C']]=-2; b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'Q']]=-1; b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'E']]=-2; b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'G']]=-3; b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'H']]=2; b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'I']]=-1; b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'L']]=-1; b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'K']]=-2; b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'M']]=-1; b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'F']]=3; b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'P']]=-3; b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'S']]=-2; b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'T']]=-2; b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'W']]=2; b62[aa2int[(uint8_t)'Y']][aa2int[(uint8_t)'V']]=-1; 
	b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'A']]=0; b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'R']]=-3; b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'N']]=-3; b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'D']]=-3; b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'C']]=-1; b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'Q']]=-2; b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'E']]=-2; b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'G']]=-3; b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'H']]=-3; b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'I']]=3; b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'L']]=1; b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'K']]=-2; b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'M']]=1; b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'F']]=-1; b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'P']]=-2; b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'S']]=-2; b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'T']]=0; b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'W']]=-3; b62[aa2int[(uint8_t)'V']][aa2int[(uint8_t)'Y']]=-1; 


	// how about converting directly from the codon int representation to the AA int representation used by maxMAtches
	// (stop codons are just the max or min of that type..)
	// which in turn can be used to access blosum matrix directly
	// and could be synced with the aa int representation in bwt
	// then, to display sequences in debug mode, need to make a reverse map from int to char, but thats easy
	// those tables could all be in config and shared between threads, initialized in config constructor

		 std::memset(codon2aa, '*',sizeof(codon2aa));
		 codon2aa[codon_to_int("TCA")] = 'S';
		 codon2aa[codon_to_int("TCC")] = 'S';
		 codon2aa[codon_to_int("TCG")] = 'S';
		 codon2aa[codon_to_int("TCT")] = 'S';
		 codon2aa[codon_to_int("TTC")] = 'F';
		 codon2aa[codon_to_int("TTT")] = 'F';
		 codon2aa[codon_to_int("TTA")] = 'L';
		 codon2aa[codon_to_int("TTG")] = 'L';
		 codon2aa[codon_to_int("TAC")] = 'Y';
		 codon2aa[codon_to_int("TAT")] = 'Y';
		 codon2aa[codon_to_int("TAA")] = '*';
		 codon2aa[codon_to_int("TAG")] = '*';
		 codon2aa[codon_to_int("TGC")] = 'C';
		 codon2aa[codon_to_int("TGT")] = 'C';
		 codon2aa[codon_to_int("TGA")] = '*';
		 codon2aa[codon_to_int("TGG")] = 'W';
		 codon2aa[codon_to_int("CTA")] = 'L';
		 codon2aa[codon_to_int("CTC")] = 'L';
		 codon2aa[codon_to_int("CTG")] = 'L';
		 codon2aa[codon_to_int("CTT")] = 'L';
		 codon2aa[codon_to_int("CCA")] = 'P';
		 codon2aa[codon_to_int("CAT")] = 'H';
		 codon2aa[codon_to_int("CAA")] = 'Q';
		 codon2aa[codon_to_int("CAG")] = 'Q';
		 codon2aa[codon_to_int("CGA")] = 'R';
		 codon2aa[codon_to_int("CGC")] = 'R';
		 codon2aa[codon_to_int("CGG")] = 'R';
		 codon2aa[codon_to_int("CGT")] = 'R';
		 codon2aa[codon_to_int("ATA")] = 'I';
		 codon2aa[codon_to_int("ATC")] = 'I';
		 codon2aa[codon_to_int("ATT")] = 'I';
		 codon2aa[codon_to_int("ATG")] = 'M';
		 codon2aa[codon_to_int("ACA")] = 'T';
		 codon2aa[codon_to_int("ACC")] = 'T';
		 codon2aa[codon_to_int("ACG")] = 'T';
		 codon2aa[codon_to_int("ACT")] = 'T';
		 codon2aa[codon_to_int("AAC")] = 'N';
		 codon2aa[codon_to_int("AAT")] = 'N';
		 codon2aa[codon_to_int("AAA")] = 'K';
		 codon2aa[codon_to_int("AAG")] = 'K';
		 codon2aa[codon_to_int("AGC")] = 'S';
		 codon2aa[codon_to_int("AGT")] = 'S';
		 codon2aa[codon_to_int("AGA")] = 'R';
		 codon2aa[codon_to_int("AGG")] = 'R';
		 codon2aa[codon_to_int("CCC")] = 'P';
		 codon2aa[codon_to_int("CCG")] = 'P';
		 codon2aa[codon_to_int("CCT")] = 'P';
		 codon2aa[codon_to_int("CAC")] = 'H';
		 codon2aa[codon_to_int("GTA")] = 'V';
		 codon2aa[codon_to_int("GTC")] = 'V';
		 codon2aa[codon_to_int("GTG")] = 'V';
		 codon2aa[codon_to_int("GTT")] = 'V';
		 codon2aa[codon_to_int("GCA")] = 'A';
		 codon2aa[codon_to_int("GCC")] = 'A';
		 codon2aa[codon_to_int("GCG")] = 'A';
		 codon2aa[codon_to_int("GCT")] = 'A';
		 codon2aa[codon_to_int("GAC")] = 'D';
		 codon2aa[codon_to_int("GAT")] = 'D';
		 codon2aa[codon_to_int("GAA")] = 'E';
		 codon2aa[codon_to_int("GAG")] = 'E';
		 codon2aa[codon_to_int("GGA")] = 'G';
		 codon2aa[codon_to_int("GGC")] = 'G';
		 codon2aa[codon_to_int("GGG")] = 'G';
		 codon2aa[codon_to_int("GGT")] = 'G';
}

KaijuWrapper::~KaijuWrapper() {
    delete config;
}

void KaijuWrapper::translate(const std::string & cds, std::string & aa) {
	aa.clear();
	if (aa.capacity() < cds.length()/3) {
		aa.reserve(cds.length()/3 + 1);
	}
	const char * c = cds.c_str();
	for (size_t i = 0; i < cds.length() - 3; i+=3) {
		aa += codon2aa[codon_to_int(c+i)];
	}
	aa[0] = 'M'; // start codon
}

void KaijuWrapper::classify_length(std::string cds, std::set<char *> & match_ids, int & maxLength) {
	std::string sequence;
	translate(cds, sequence);
	const unsigned int length = (unsigned int)sequence.length();
    // if(config->debug) { std::cerr << "Searching fragment "<< fragment <<  " (" << length << ")" << "\n"; }
	char * seq = new char[length+1];
	std::strcpy(seq, sequence.c_str());
    translate2numbers((uchar *)seq, length, config->astruct);
    
	SI * si = maxMatches(config->fmi, seq, length, config->min_fragment_length,  1);
    delete[] seq;
	if(!si) {// no match for this fragment
		if(config->debug) std::cerr << "No match for this fragment." << "\n";
        return;
    }
	maxLength = si->ql;

    match_ids.clear();
    ids_from_SI_recursive(si, match_ids);
    recursive_free_SI(si);
}

void KaijuWrapper::classify_greedyblosum(std::string cds, std::set<char *> & match_ids) {
	std::string sequence;
	translate(cds, sequence);

	// Make fragments from sequence
	std::multimap<unsigned int,Fragment *,std::greater<unsigned int>> fragments;
	const unsigned int score = calcScore(sequence);
	if(score >= config->min_score) {
		fragments.insert(std::pair<unsigned int,Fragment *>(score,new Fragment(sequence)));
	}

	std::vector<SI *> best_matches_SI;
	std::vector<std::string> best_matches;
	unsigned int best_match_score = 0;
	double query_len = static_cast<double>(sequence.length());

	while(1) {
		Fragment * t = getNextFragment(fragments, best_match_score);
		if(!t) break;
		const std::string fragment = t->seq;
		const size_t length = fragment.length();
		const unsigned int num_mm = t->num_mm;

		if(config->debug) { std::cerr << "Searching fragment "<< fragment <<  " (" << length << ","<< num_mm << "," << t->diff << ")" << "\n"; }
		char * seq = new char[length+1];
		std::strcpy(seq, fragment.c_str());

		translate2numbers((uchar *)seq, (unsigned int)length, config->astruct);

		SI * si = NULL;
		if(num_mm > 0) {
			if(num_mm == config->mismatches) { //after last mm has been done, we need to have at least reached the min_length
				si = maxMatches_withStart(config->fmi, seq, (unsigned int)length, config->min_fragment_length, 1,t->si0,t->si1,t->matchlen);
			}
			else {
				si = maxMatches_withStart(config->fmi, seq, (unsigned int)length, t->matchlen, 1,t->si0,t->si1,t->matchlen);
			}
		}
		else {
			si = maxMatches(config->fmi, seq, (unsigned int)length, config->seed_length, 0); //initial matches
		}

		if(!si) {// no match for this fragment
			if(config->debug) std::cerr << "No match for this fragment." << "\n";
			delete[] seq;
			delete t;
			continue; // continue with the next fragment
		}
		if(config->debug) std::cerr << "Longest match has length " << (unsigned int)si->ql <<  "\n";

		if(config->mismatches > 0 && num_mm < config->mismatches) {
			SI * si_it = si;
			while(si_it) {
				unsigned int match_right_end = si_it->qi + si_it->ql - 1;
				if(num_mm > 0) assert(match_right_end  == length - 1); // greedy matches end always at the end
				if(config->debug) std::cerr << "Match from " << si_it->qi << " to " << match_right_end << ": " << fragment.substr(si_it->qi, match_right_end -  si_it->qi +1)  << " (" << si_it->ql << ")\n";
				if(si_it->qi > 0 && match_right_end + 1 >= config->min_fragment_length) {
					//1. match must end before beginning of fragment, i.e. it is extendable
					//2. remaining fragment, from zero to end of current match, must be longer than minimum length of accepted matches
					const size_t erase_pos = (match_right_end < length - 1) ? match_right_end + 1 : std::string::npos;
					addAllMismatchVariantsAtPosSI(t, fragments,(unsigned int)(si_it->qi - 1), best_match_score, erase_pos,si_it);
				}
				si_it = si_it->samelen ? si_it->samelen : si_it->next;
			}
		}
		
		if((unsigned int)si->ql < config->min_fragment_length) { // match was too short
			if(config->debug) { std::cerr << "Match of length " << si->ql << " is too short\n"; }
			delete[] seq;
			delete t;
			recursive_free_SI(si);
			continue; // continue with the next fragment
		}

		eval_match_scores(si, t, best_matches_SI, best_match_score, best_matches);

		delete[] seq;
		delete t;
	} // end current fragment

	if(best_matches_SI.empty()) {
		return;
	}

	if(config->use_Evalue) {
		//calc e-value and only return match if > cutoff
		double bitscore = (LAMBDA * best_match_score - LN_K) / LN_2;
		double Evalue = config->db_length * query_len * pow(2, -1 * bitscore);
		if(config->debug) std::cerr << "E-value = " << Evalue << std::endl;
		if(Evalue > config->min_Evalue) {
			for(auto itm : best_matches_SI) {
				free(itm);
			}
			return;
		}
	}
		
	match_ids.clear();

	for(auto itm : best_matches_SI) {
		ids_from_SI(itm, match_ids);
	}
	for(auto itm : best_matches_SI) {
			//recursive_free_SI(itm);
		free(itm);
	}
}

//to be used by greedyblosum
void KaijuWrapper::addAllMismatchVariantsAtPosSI(
	const Fragment * f,
	std::multimap<unsigned int,Fragment *,std::greater<unsigned int>> & fragments,
	unsigned int pos,
	unsigned int best_match_score,
	size_t erase_pos = std::string::npos,
	SI * si = NULL) 
	{

	assert(config->mode==GREEDY);
	assert(pos < erase_pos);
	assert(f->num_mm == 0 || pos < f->pos_lastmm);

	std::string fragment = f->seq; // make a copy to modify the sequence at pos
	assert(fragment.length() >= config->min_fragment_length);
	char origchar = fragment[pos];
	assert(blosum_subst.count(origchar) > 0);

	if(erase_pos != std::string::npos && erase_pos < fragment.length()) {
		if(config->debug)	std::cerr << "Deleting from position " << erase_pos  << "\n";
		fragment.erase(erase_pos);
	}

	//calc score for whole sequence, so we can substract the diff for each substitution
	unsigned int score = calcScore(fragment,f->diff) - blosum62diag[aa2int[(uint8_t)origchar]];
	IndexType siarray[2], siarrayupd[2];
	siarray[0] = si->start;
	siarray[1] = si->start+(IndexType)si->len;

	for(auto itv : blosum_subst.at(origchar)) {
		// we know the difference between score of original aa and substitution score, this
		// has to be subtracted when summing over all positions later
		// so we add this difference to the fragment
		int score_after_subst = score + b62[aa2int[(uint8_t)origchar]][aa2int[(uint8_t)itv]];
		if(score_after_subst >= (int)best_match_score && score_after_subst >= (int)config->min_score) {
			if(UpdateSI(config->fmi, config->astruct->trans[(size_t)itv], siarray, siarrayupd) != 0) {
				fragment[pos] = itv;
				int diff = b62[aa2int[(uint8_t)origchar]][aa2int[(uint8_t)itv]] - blosum62diag[aa2int[(uint8_t)itv]];
				if(config->debug) std::cerr << "Adding fragment   " << fragment << " with mismatch at pos " << pos << " ,diff " << f->diff+diff << ", max score " << score_after_subst << "\n";
				fragments.emplace(score_after_subst,new Fragment(fragment,f->num_mm+1, pos, f->diff + diff,siarrayupd[0],siarrayupd[1],si->ql+1));
			}
			else if(config->debug) {
				fragment[pos] = itv;
				std::cerr << "Skipping fragment " << fragment << " mismatch at pos " << pos << ", because " << itv << " is not a valid extension\n";
			}

		}
		else {
			if(config->debug) {
				fragment[pos] = itv;
				std::cerr << "Skipping fragment "<< fragment <<" and following fragments, because score is too low: " << score_after_subst << " < " << std::max(best_match_score,config->min_score) << "\n";
			}
			break;
		}
	}

}

void KaijuWrapper::ids_from_SI_recursive(SI *si, std::set<char *> & match_ids){
	SI * si_it = si;
	while(si_it) {
		IndexType k, pos;
		int iseq;
		for (k=si_it->start; k<si_it->start+si_it->len; ++k) {
			if(match_ids.size() > config->max_match_ids) {
				break;
			}
			get_suffix(config->fmi, config->bwt->s, k, &iseq, &pos);
			match_ids.insert(config->bwt->s->ids[iseq]);
		} // end for
		si_it = si_it->samelen;
	} // end while all SI with same length    
}

unsigned int KaijuWrapper::calcScore(const std::string & s, size_t start, size_t len, int diff) {
	int score = 0;
	for(size_t i=start; i < start+len; ++i) {
		score += blosum62diag[aa2int[(uint8_t)s[i]]];
	}
	score += diff;
	return score > 0 ? score : 0;
}

unsigned int KaijuWrapper::calcScore(const std::string & s, int diff) {
	int score = 0;
	for(size_t i=0; i < s.length(); ++i) {
		score += blosum62diag[aa2int[(uint8_t)s[i]]];
	}
	score += diff;
	return score > 0 ? score : 0;
}

unsigned int KaijuWrapper::calcScore(const std::string & s) {
	unsigned int score = 0;
	for(size_t i=0; i < s.length(); ++i) {
		score += blosum62diag[aa2int[(uint8_t)s[i]]];
	}
	return score;
}

Fragment * KaijuWrapper::getNextFragment(
	std::multimap<unsigned int,Fragment *,std::greater<unsigned int>> & fragments,
	unsigned int min_score) 
	{
	if(fragments.empty()) {
		return NULL;
	}
	auto it = fragments.begin();
	if(config->debug) std::cerr << "max fragment score/length = " << it->first << "\n";
	if(it->first < min_score) { //the highest scoring fragment in the sorted list is below threshold, then search stops
		return NULL;
	}
	Fragment * f = it->second;
	if(config->debug) std::cerr <<  "Fragment = " << f->seq << "\n";
	fragments.erase(it);

	// while(config->SEG && f != NULL && !f->SEGchecked) {
	// 	std::string convertedseq = f->seq;
	// 	for(size_t i = 0; i < convertedseq.length(); i++) {
	// 		convertedseq[i] = AMINOACID_TO_NCBISTDAA[(int)convertedseq[i]];
	// 	}
	// 	BlastSeqLoc *seg_locs = NULL;
	// 	SeqBufferSeg((Uint1*)(convertedseq.data()), (Int4)convertedseq.length(), 0, config->blast_seg_params, &seg_locs);
	// 	if(seg_locs) { // SEG found region(s)
	// 		BlastSeqLoc * curr_loc = seg_locs;
	// 		size_t start = 0; //start of non-SEGged piece
	// 		do {
	// 			size_t length = curr_loc->ssr->left - start;
	// 			if(config->debug) std::cerr << "SEG region: " << curr_loc->ssr->left << " - " << curr_loc->ssr->right << " = " << f->seq.substr(curr_loc->ssr->left,curr_loc->ssr->right - curr_loc->ssr->left + 1) << std::endl;
	// 			if(length > config->min_fragment_length) {
	// 				if(config->mode == GREEDY) {
	// 					unsigned int score = calcScore(f->seq,start,length,0);
	// 					if(score >= config->min_score) {
	// 						fragments.emplace(score,new Fragment(f->seq.substr(start,length),true));
	// 					}
	// 				}
	// 				else {
	// 					fragments.emplace(length,new Fragment(f->seq.substr(start,length),true));
	// 				}
	// 			}
	// 			start = curr_loc->ssr->right + 1;
	// 		} while((curr_loc=curr_loc->next) != NULL);
	// 		size_t len_last_piece = f->seq.length() - start;
	// 		if(len_last_piece > config->min_fragment_length) {
	// 			if(config->mode == GREEDY) {
	// 				unsigned int score = calcScore(f->seq,start,len_last_piece,0);
	// 				if(score >= config->min_score) {
	// 					fragments.emplace(score,new Fragment(f->seq.substr(start,len_last_piece),true));
	// 				}
	// 			}
	// 			else {
	// 				fragments.emplace(len_last_piece,new Fragment(f->seq.substr(start,len_last_piece),true));
	// 			}
	// 		}

	// 		BlastSeqLocFree(seg_locs);
	// 		delete f;
	// 		f = NULL;
	// 		if(!fragments.empty()) {
	// 			it = fragments.begin();
	// 			if(it->first >= min_score) {
	// 				f = it->second;
	// 				fragments.erase(it);
	// 				// next iteration of while loop
	// 			}
	// 		}
	// 	}
	// 	else { // no SEG regions found
	// 		return f;
	// 	}
	// }

	return f;
}

void KaijuWrapper::ids_from_SI(SI *si, std::set<char *> & match_ids) {
	IndexType k, pos;
	int iseq;
	for (k=si->start; k<si->start+si->len; ++k) {
		if(match_ids.size() > config->max_match_ids) {
			break;
		}
		get_suffix(config->fmi, config->bwt->s, k, &iseq, &pos);
		match_ids.insert(config->bwt->s->ids[iseq]);
	}
}


void KaijuWrapper::eval_match_scores(
	SI *si,
	Fragment * frag, 
	std::vector<SI *> & best_matches_SI, 
	unsigned int best_match_score,
	std::vector<std::string> & best_matches) 
	{

	if(!si) return;

	// eval the remaining same-length and shorter matches
	if(si->samelen)
		eval_match_scores(si->samelen, frag, best_matches_SI, best_match_score, best_matches);
	if(si->next && si->next->ql >= (int)config->min_fragment_length)
		eval_match_scores(si->next, frag, best_matches_SI, best_match_score, best_matches);
	else if(si->next)
		recursive_free_SI(si->next);

	unsigned int score = calcScore(frag->seq,si->qi,si->ql,frag->diff);

	if(config->debug) std::cerr << "Match " <<frag->seq.substr(si->qi,si->ql) << " (length=" << (unsigned int)si->ql << " score=" << score << " num_mm=" << frag->num_mm<< ")\n";

	if(score < config->min_score) {
		free(si);
		si = NULL;
		return;
	}

	if(score > best_match_score) {
		for(auto itm : best_matches_SI) {
			//recursive_free_SI(itm);
			free(itm);
		}
		best_matches_SI.clear();
		best_matches_SI.push_back(si);
		best_match_score = score;
		if(config->verbose) {
			best_matches.clear();
			best_matches.push_back(frag->seq.substr(si->qi,si->ql));
		}
	}
	else if(score == best_match_score && best_matches_SI.size() < config->max_matches_SI) {
		best_matches_SI.push_back(si);
		if(config->verbose)
			best_matches.push_back(frag->seq.substr(si->qi,si->ql));
	}
	else {
		free(si);
		si = NULL;
	}


}