/* This file is part of Kaiju, Copyright 2015-2018 Peter Menzel and Anders Krogh,
 * Kaiju is licensed under the GPLv3, see the file LICENSE. */

#include "ConsumerThread.hpp"

ConsumerThread::ConsumerThread(ProducerConsumerQueue<ReadItem*>* workQueue, Config * config) {

	myWorkQueue = workQueue;
	this->config = config;
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


	for(unsigned int i = 0; i<=5; i++) {
		translations[i].reserve(1000);
	}
}


void ConsumerThread::getAllFragmentsBits(const std::string & line) {

	for(unsigned int i = 0; i<=2; i++) {
		translations[i].clear();
	}
	const char * c = line.c_str();
	for(size_t count = 0; count < line.length()-2; count++) {
		//printf("%i.",nuc2int[(uint8_t)line[count]]);
		//printf("=%i.",codon_to_int(codon));
		char aa = codon2aa[codon_to_int(c++)];
		if(aa == '*') {
			size_t index = count%3;
			// finished one of the translations, so add it to fragments
			if(translations[index].length() >= config->min_fragment_length) {
				if(config->mode==GREEDY) {
					unsigned int score = calcScore(translations[index]);
					if(score >= config->min_score)
						fragments.emplace(score,new Fragment(translations[index]));
				}
				else {
					fragments.emplace(translations[index].length(),new Fragment(translations[index]));
				}
			}
			translations[index].clear();
		}
		else {
			translations[count%3].push_back(aa);
		}
	}
	for(unsigned int i = 0; i<=2; i++) {
		//add remaining stuff to fragments
		if(translations[i].length() >= config->min_fragment_length) {
			if(config->mode==GREEDY) {
				unsigned int score = calcScore(translations[i]);
				if(score >= config->min_score)
					fragments.emplace(score,new Fragment(translations[i]));
			}
			else {
				fragments.emplace(translations[i].length(),new Fragment(translations[i]));
			}
		}
		translations[i].clear();
	}
//cerr << "\n";
	for(int count=(int)line.length()-2; count >=0; count--) { // count needs to be an int here and not size_t
	//	printf("=%i.",revcomp_codon_to_int(codon));
		char aa = codon2aa[revcomp_codon_to_int(c--)];
		if(aa == '*') {
			size_t index = count%3;
			// finished one of the translations, so add it to fragments
			if(translations[index].length() >= config->min_fragment_length) {
				if(config->mode==GREEDY) {
					unsigned int score = calcScore(translations[index]);
					if(score >= config->min_score)
						fragments.emplace(score,new Fragment(translations[index]));
				}
				else {
					fragments.emplace(translations[index].length(),new Fragment(translations[index]));
				}
			}
			translations[index].clear();
		}
		else {
			translations[count%3].push_back(aa);
		}
	}
	for(unsigned int i = 0; i<=2; i++) {
		//add remaining stuff to fragments
		if(translations[i].length() >= config->min_fragment_length) {
			if(config->mode==GREEDY) {
				unsigned int score = calcScore(translations[i]);
				if(score >= config->min_score)
					fragments.emplace(score,new Fragment(translations[i]));
			}
			else {
				fragments.emplace(translations[i].length(),new Fragment(translations[i]));
			}
		}
	}

}

Fragment * ConsumerThread::getNextFragment(unsigned int min_score) {
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

	while(config->SEG && f != NULL && !f->SEGchecked) {
		std::string convertedseq = f->seq;
		for(size_t i = 0; i < convertedseq.length(); i++) {
			convertedseq[i] = AMINOACID_TO_NCBISTDAA[(int)convertedseq[i]];
		}
		BlastSeqLoc *seg_locs = NULL;
		SeqBufferSeg((Uint1*)(convertedseq.data()), (Int4)convertedseq.length(), 0, config->blast_seg_params, &seg_locs);
		if(seg_locs) { // SEG found region(s)
			BlastSeqLoc * curr_loc = seg_locs;
			size_t start = 0; //start of non-SEGged piece
			do {
				size_t length = curr_loc->ssr->left - start;
				if(config->debug) std::cerr << "SEG region: " << curr_loc->ssr->left << " - " << curr_loc->ssr->right << " = " << f->seq.substr(curr_loc->ssr->left,curr_loc->ssr->right - curr_loc->ssr->left + 1) << std::endl;
				if(length > config->min_fragment_length) {
					if(config->mode == GREEDY) {
						unsigned int score = calcScore(f->seq,start,length,0);
						if(score >= config->min_score) {
							fragments.emplace(score,new Fragment(f->seq.substr(start,length),true));
						}
					}
					else {
						fragments.emplace(length,new Fragment(f->seq.substr(start,length),true));
					}
				}
				start = curr_loc->ssr->right + 1;
			} while((curr_loc=curr_loc->next) != NULL);
			size_t len_last_piece = f->seq.length() - start;
			if(len_last_piece > config->min_fragment_length) {
				if(config->mode == GREEDY) {
					unsigned int score = calcScore(f->seq,start,len_last_piece,0);
					if(score >= config->min_score) {
						fragments.emplace(score,new Fragment(f->seq.substr(start,len_last_piece),true));
					}
				}
				else {
					fragments.emplace(len_last_piece,new Fragment(f->seq.substr(start,len_last_piece),true));
				}
			}

			BlastSeqLocFree(seg_locs);
			delete f;
			f = NULL;
			if(!fragments.empty()) {
				it = fragments.begin();
				if(it->first >= min_score) {
					f = it->second;
					fragments.erase(it);
					// next iteration of while loop
				}
			}
		}
		else { // no SEG regions found
			return f;
		}
	}

	return f;
}


//to be used by greedyblosum
void ConsumerThread::addAllMismatchVariantsAtPosSI(const Fragment * f, unsigned int pos, size_t erase_pos = std::string::npos,SI * si = NULL) {

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

unsigned int ConsumerThread::calcScore(const std::string & s, size_t start, size_t len, int diff) {
	int score = 0;
	for(size_t i=start; i < start+len; ++i) {
		score += blosum62diag[aa2int[(uint8_t)s[i]]];
	}
	score += diff;
	return score > 0 ? score : 0;
}

unsigned int ConsumerThread::calcScore(const std::string & s, int diff) {
	int score = 0;
	for(size_t i=0; i < s.length(); ++i) {
		score += blosum62diag[aa2int[(uint8_t)s[i]]];
	}
	score += diff;
	return score > 0 ? score : 0;
}

unsigned int ConsumerThread::calcScore(const std::string & s) {
	unsigned int score = 0;
	for(size_t i=0; i < s.length(); ++i) {
		score += blosum62diag[aa2int[(uint8_t)s[i]]];
	}
	return score;
}


uint64_t ConsumerThread::classify_greedyblosum() {

		best_matches_SI.clear();
		best_matches.clear();
		best_match_score = 0;

		while(1) {
			Fragment * t = getNextFragment(best_match_score);
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
			if(config->debug) std::cerr << "Longest match is length " << (unsigned int)si->ql <<  "\n";

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
						addAllMismatchVariantsAtPosSI(t,(unsigned int)(si_it->qi - 1),erase_pos,si_it);
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

			eval_match_scores(si, t);
			delete[] seq;
			delete t;

		} // end current fragment

		if(best_matches_SI.empty()) {
			return 0;
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
				return 0;
			}
		}

		match_ids.clear();
		match_dbnames.clear();

		for(auto itm : best_matches_SI) {
			ids_from_SI(itm);
		}
		for(auto itm : best_matches_SI) {
			//recursive_free_SI(itm);
			free(itm);
		}


		if(config->verbose)  {
			std::stringstream ss;
			ss << best_match_score << "\t" ;
			for(auto it : match_ids) ss << it << ",";
			ss  << "\t";
			for(auto it : match_dbnames) ss << it << ",";
			ss  << "\t";
			for(auto it : best_matches) ss << it << ",";
			extraoutput = ss.str();
		}

		uint64_t lca = (match_ids.size()==1) ?  *(match_ids.begin()) : lca_from_ids(config,node2depth, match_ids);
		return lca;

}

uint64_t ConsumerThread::classify_length() {

		unsigned int longest_match_length = 0;
		longest_matches_SI.clear();
		longest_fragments.clear();

		while(1) {
			Fragment * t = getNextFragment(longest_match_length);
			if(!t) break;// searched all fragments that are longer than best match length
			const std::string fragment = t->seq;
			const size_t length = fragment.length();

			if(config->debug) { std::cerr << "Searching fragment "<< fragment <<  " (" << length << ")" << "\n"; }
			char * seq = new char[length+1];
			std::strcpy(seq, fragment.c_str());

			translate2numbers((uchar *)seq, (unsigned int)length, config->astruct);
			//use longest_match_length here too:
			//SI * si = maxMatches(config->fmi, seq, length, max(config->min_fragment_length,longest_match_length),  1);
			SI * si = greedyExact(config->fmi, seq, (unsigned int)length, std::max(config->min_fragment_length,longest_match_length),  -1);

			if(!si) {// no match for this fragment
				if(config->debug) std::cerr << "No match for this fragment." << "\n";
				delete[] seq;
				delete t;
				continue; // continue with the next fragment
			}


			// just get length here and save si when it is longest
			if(config->debug) std::cerr << "Longest match is length " << (unsigned int)si->ql << "\n";
			if((unsigned int)si->ql > longest_match_length) {
				for(auto itm : longest_matches_SI) {
					recursive_free_SI(itm);
				}
				longest_matches_SI.clear();
				longest_matches_SI.push_back(si);
				longest_match_length = (unsigned int)si->ql;
				if(config->verbose) {
					longest_fragments.clear();
					longest_fragments.push_back(fragment.substr(si->qi,si->ql));
				}
			}
			else if((unsigned int)si->ql == longest_match_length) {
				longest_matches_SI.push_back(si);
				if(config->verbose) {
					longest_fragments.push_back(fragment.substr(si->qi,si->ql));
				}
			}
			else {
				recursive_free_SI(si);
				si = NULL;
			}
			delete[] seq;
			delete t;

		} // end current fragment

		if(longest_matches_SI.empty()) {
			return 0;
		}
		match_ids.clear();
		match_dbnames.clear();
		for(auto itm : longest_matches_SI) {
			ids_from_SI_recursive(itm);
		}
		for(auto itm : longest_matches_SI) {
			recursive_free_SI(itm);
		}


		if(config->verbose) {
			std::stringstream ss;
			ss << longest_match_length << "\t";
			for(auto it : match_ids) ss << it << ",";
			ss  << "\t";
			for(auto it : match_dbnames) ss << it << ",";
			ss  << "\t";
			for(auto it : longest_fragments) ss << it << ",";
			extraoutput = ss.str();
		}

		uint64_t lca = (match_ids.size()==1) ?  *(match_ids.begin()) : lca_from_ids(config,node2depth, match_ids);
		return lca;

}

void ConsumerThread::doWork() {
	ReadItem * item = NULL;
	while(myWorkQueue->pop(&item)) {
		assert(item != NULL);
		read_count++;
		if(read_count > 20000) {
			flush_output();
			read_count = 0;
		}

		if(config->input_is_protein) {
			if(item->sequence1.length() < config->min_fragment_length) {
				output << "U\t" << item->name << "\t0\n";
				delete item;
				continue;
			}
		}
		else {
			if((!item->paired && item->sequence1.length() < config->min_fragment_length*3) ||
				(item->paired && item->sequence1.length() < config->min_fragment_length*3 && item->sequence2.length() < config->min_fragment_length*3)) {
				output << "U\t" << item->name << "\t0\n";
				delete item;
				continue;
			}
		}

		uint64_t lca = 0;
		extraoutput = "";

		if(config->input_is_protein) {
			query_len = static_cast<double>(item->sequence1.length());
			for (auto & c: item->sequence1) {
				c = (char)toupper(c);
			}
			size_t start = 0;
			size_t pos = item->sequence1.find_first_not_of("ACDEFGHIKLMNPQRSTVWY");
			while(pos != std::string::npos) {
				if(pos-start >= config->min_fragment_length) {
					std::string subseq =  item->sequence1.substr(start,pos-start);
					//std::cerr << "subseq=" << subseq << endl;
					if(config->mode==GREEDY) {
						unsigned int score = calcScore(subseq);
						if(score >= config->min_score) {
							fragments.emplace(score,new Fragment(subseq));
						}
					}
					else {
						fragments.emplace((unsigned int)subseq.length(),new Fragment(subseq));
					}
				}
				start = pos+1;
				pos = item->sequence1.find_first_not_of("ACDEFGHIKLMNPQRSTVWY", pos + 1);
			}
			//add remaining sequence, which corresponds to the whole sequence if no invalid char was found
			std::string subseq = item->sequence1.substr(start,item->sequence1.length()-start);
			if(subseq.length() >= config->min_fragment_length) {
				if(config->mode==GREEDY) {
					unsigned int score = calcScore(subseq);
					if(score >= config->min_score) {
						fragments.emplace(score,new Fragment(subseq));
					}
				}
				else {
					fragments.emplace((unsigned int)subseq.length(),new Fragment(subseq));
				}
			}
		}
		else { // normal mode with DNA input
			query_len = static_cast<double>(item->sequence1.length()) / 3.0;
			if(item->sequence1.length() >= config->min_fragment_length*3) {
				if(config->debug) std::cerr << "Getting fragments for read: "<< item->sequence1 << "\n";
				getAllFragmentsBits(item->sequence1);
			}
			if(item->paired) {
				query_len += static_cast<double>(item->sequence2.length()) / 3.0;
				if(item->sequence2.length() >= config->min_fragment_length*3) {
					if(config->debug) std::cerr << "Getting fragments for 2nd read: " << item->sequence2 << "\n";
					getAllFragmentsBits(item->sequence2);
				}
			}
		}

		if(config->debug) std::cerr << fragments.size()  << " fragments found in the read."<< "\n";

		if(config->mode == MEM) {
			lca = classify_length();
		}
		else if(config->mode == GREEDY) {
			lca = classify_greedyblosum();
		}
		else { // this should not happen
			assert(false);
		}

		if(lca > 0) {
			output << "C\t" << item->name << "\t" << lca;
			if(config->verbose) output << "\t" << extraoutput;
			output << "\n";
			if(config->debug) {
				std::cerr << "C\t" << item->name << "\t" << lca << "\t" << extraoutput << "\n";
			}

		}
		else  {
			output << "U\t" << item->name << "\t0\n";
			if(config->debug) {
				std::cerr << "U\t" << item->name << "\t0\n";
			}

		}

		delete item;

		clearFragments();

	}

	flush_output();

}

void ConsumerThread::eval_match_scores(SI *si, Fragment * frag) {

	if(!si) return;

	// eval the remaining same-length and shorter matches
	if(si->samelen)
		eval_match_scores(si->samelen, frag);
	if(si->next && si->next->ql >= (int)config->min_fragment_length)
		eval_match_scores(si->next, frag);
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

void ConsumerThread::ids_from_SI(SI *si) {
	IndexType k, pos;
	int iseq;
	for (k=si->start; k<si->start+si->len; ++k) {

		// too many match ids affect AM and runtime, so use a limit now
		if(match_ids.size() > config->max_match_ids) {
			break;
		}

		get_suffix(config->fmi, config->bwt->s, k, &iseq, &pos);
		uint64_t id = ULONG_MAX;

		// we can have either  AX1235.1_4567, WP_12345.1_987 (Acc.Ver_taxonid) or 987 (only taxonid) as database names
		// look for the last occurence of _
		char * pch = strrchr(config->bwt->s->ids[iseq],'_');
		if(pch != NULL)  { //found _, then use number after _
			id = strtoul(pch+1,NULL,10);
			if(id == ULONG_MAX) {
				std::cerr << "Found bad number (out of range error) in database sequence name: " << config->bwt->s->ids[iseq] << std::endl;
				continue;
			}
			// extract db name
			if(config->verbose && match_dbnames.size() < config->max_match_acc) {
				match_dbnames.emplace(config->bwt->s->ids[iseq],pch-config->bwt->s->ids[iseq]);
			}
		}
		else { // no _ found, use the whole database name as taxonid
			id = strtoul(config->bwt->s->ids[iseq],NULL,10);
			if(id == ULONG_MAX) {
				std::cerr << "Found bad number (out of range error) in database sequence name: " << config->bwt->s->ids[iseq] << std::endl;
				continue;
			}
		}
		match_ids.insert(id);
	}
}

void ConsumerThread::ids_from_SI_recursive(SI *si) {
	SI * si_it = si;
	while(si_it) {

		ids_from_SI(si_it);

		si_it = si_it->samelen;
	}
}

void ConsumerThread::flush_output() {
	static std::mutex m;

	{
	std::lock_guard<std::mutex> out_lock(m);
	*(config->out_stream) << output.str();
	}

	output.str("");
}

void ConsumerThread::clearFragments() {
	while(!fragments.empty()) {
		auto it = fragments.begin();
		Fragment * f = it->second;
		delete f;
		fragments.erase(it);
	}
	fragments.clear();
}


inline uint8_t ConsumerThread::codon_to_int(const char* codon)  {
 return (uint8_t)(nuc2int[(uint8_t)codon[0]] << 4 | nuc2int[(uint8_t)codon[1]]  << 2 | nuc2int[(uint8_t)codon[2]]);
}

inline uint8_t ConsumerThread::revcomp_codon_to_int(const char* codon)  {
 return (uint8_t)(compnuc2int[(uint8_t)codon[2]] << 4 | compnuc2int[(uint8_t)codon[1]]  << 2 | compnuc2int[(uint8_t)codon[0]]);
}


