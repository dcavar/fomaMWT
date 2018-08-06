/*
 * Author: Damir Cavar (http://damir.cavar.me/)
 * Date: 10/04/2016 - 08/06/2018
 *
 * Purpose:
 * Process efficiently multi-word expressions with a morphology implemented
 * as a Finite State Transducer using Lexc and Foma.
 *
 *
 * Copyright 2016-2018 by Damir Cavar
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


typedef bool _Bool; // Foma uses C99 _Bool, Mac clang has trouble with that

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <sstream>
#include <stdbool.h> // Foma uses _Bool from C99, this is necessary
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <ctype.h>
#include <fomalib.h>


namespace fs = boost::filesystem;
namespace po = boost::program_options;

using namespace std;

// multigram holds the from, to, string, and vector of morphological analyses for the mgram
typedef tuple<unsigned long, unsigned long, string, vector<string>> multigram;


namespace {
    const size_t ERROR_IN_COMMAND_LINE = 1;
    const size_t SUCCESS = 0;
    const size_t ERROR_XML = 2;
    const size_t ERROR_UNHANDLED_EXCEPTION = 3;
    const size_t ERROR_MISSING_FILE = 4;
} // namespace


vector<multigram> getMultiGrams(vector<string> tokens, unsigned long maxn) {
    // accumulate all ngrams here:
    vector<multigram> res;

    // loop over n-gram length 1 to maxn
    for (unsigned long x = 1; x <= min(maxn, tokens.size()); ++x) {
        // loop from beg. of token list to end
        for (unsigned long i = 0; i < (tokens.size() - x + 1); ++i) {
            if ((tokens[i + x - 1].size() == 1) & ispunct(tokens[i + x - 1][0]))
                continue;

            vector<string> temp;
            copy(tokens.begin() + i, tokens.begin() + i + x, back_inserter(temp));
            multigram mtmp(i, i+x, boost::algorithm::join(temp, " "), vector<string>());
            res.push_back(mtmp);
        }
    }

    return res;
}



void prettyPrintToken(multigram token) {
    cout << get<0>(token) << "\t" << get<1>(token) << "\t" << get<2>(token) ;
    // if unigram, print always newline after this above
    if (get<3>(token).size() == 0) {
        cout << endl;
        return; // there is nothing to loop over
    }
    bool first = true;
    for (auto x : get<3>(token)) {
        if (first) {
            first = false;
            cout << "\t" << x ;
        } else {
            cout << ", " << x ;
        }
    }
    cout << endl;
}


void processTokens(string fstfname, vector<string> fnames, int maxn) {

    // necessary data types for Foma
    struct fsm *net;
    struct apply_handle *ah;
    char *result;
    vector<multigram> finalResult; // storage for all analyses

    // read the FST from binary file
    net = fsm_read_binary_file((char *) fstfname.c_str());
    cout << "Loaded binary fst: " << fstfname << endl;
    ah = apply_init(net);

    // for filename
    for (auto file : fnames) {
        finalResult.clear();

        // read file
        ifstream infile(file);
        // assume that every line is a sentence
        string line;
        while (getline(infile, line)) {
            // if no line, continue
            if (line.size() == 0)
                continue;

            cout << endl << line << endl;

            vector<string> tokens;
            boost::split(tokens, line, boost::is_any_of(" "));

            for (const auto &token : getMultiGrams(tokens, maxn)) { //
                // if line is a single punctuation mark, continue
                if ((get<2>(token).size() == 1) & ispunct(get<2>(token)[0])) {
                    continue;
                }

                vector<string> resbuffer = get<3>(token);
                result = apply_up(ah, (char *)get<2>(token).c_str());
                if (result == NULL) {
                    // append to result only if this is a unigram
                    if (get<1>(token) - get<0>(token) == 1) {
                        finalResult.push_back(multigram(get<0>(token), get<1>(token), get<2>(token), resbuffer));
                        // pretty print token
                        prettyPrintToken(multigram(get<0>(token), get<1>(token), get<2>(token), resbuffer));
                    }
                    continue;
                }
                // catch all resulting analyses from the morphology
                while (result != NULL) {
                    resbuffer.push_back(result);
                    result = apply_up(ah, NULL);
                }

                // pretty print token
                prettyPrintToken(multigram(get<0>(token), get<1>(token), get<2>(token), resbuffer));

                // append to result, there was an analysis
                finalResult.push_back(multigram(get<0>(token), get<1>(token), get<2>(token), resbuffer));
            }
        }
        // close file when done
        infile.close();
    }
    apply_clear(ah);
    fsm_destroy(net);
}



/*! \fn void usage( )
    \brief Print out the usage.

    Prints out the copyright without option descriptions.
*/
void usage() {
    cout << "Usage: morphotagger [OPTION]... [SENTENCES]..." << endl << endl
         << "For help:" << endl << "mwtagger --help" << endl
         << endl
         << "(C) 2015-2018 by Damir Cavar <dcavar@indiana.edu>" << endl << endl;
}


int main(int argc, char *argv[]) {

    bool quiet = false;
    vector<string> inputFiles;
    string fstFilename = "english.fst";
    int n = 3;

    try {
        po::options_description desc("mwtagger options");
        desc.add_options()
                ("quiet,q", "Quiet operations")
                ("help,h", "Quiet operations")
                ("maxn,n", po::value<int>(&n), "Set maximal n-gram size")
                ("fst,f", po::value<string>(&fstFilename), "Set FST filename")
                ("input-files", po::value<vector<string>>(&inputFiles), "Word list input files");
        // ("input-files", po::value<vector<string>>(&inputFiles)->required(), "Word list input files");

        po::positional_options_description p;
        p.add("input-files", -1);
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);

        if (vm.count("help")) {
            cout << "Usage: mwtagger [OPTION]... [SENTENCES]..." << endl << endl
                 << "Process words " << endl
                 << endl
                 << "(C) 2015-2018 by Damir Cavar <damir@linguistlist.org>" << endl << endl
                 << desc << endl;
            return SUCCESS;
        }
        po::notify(vm);
        if (vm.count("quiet")) {
            quiet = true;
        }
        if (vm.count("help")) {
            usage();
            cout << desc << endl;
        }
        if (!vm.count("input-files")) {
            usage();
            cout << desc << endl;
        }
    }
        // Catch command line error
    catch (exception &e) {
        cerr << "Error: " << e.what() << endl;
        return ERROR_IN_COMMAND_LINE;
    }
        // catch any other error
    catch (...) {
        cerr << "Unknown error!" << endl;
        return ERROR_UNHANDLED_EXCEPTION;
    }
    // fire up the analyzer
    if (fstFilename.size() & inputFiles.size())
        processTokens(fstFilename, inputFiles, n);


    return SUCCESS;
} // main
