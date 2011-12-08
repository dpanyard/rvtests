/**
   immediately TODO:
   3. support tri-allelic (getAlt())
   4. speed up VCF parsing. (make a separate line buffer).
   5. loading phenotype and covariate (need tests now).
   6. do analysis. (test CMC for now)
   7. VT (combine Collapsor and ModelFitter)

   DONE:
   1. suppport PLINK output
   2. support access INFO tag
   5. give warnings for: Argument.h detect --inVcf --outVcf empty argument value after --inVcf
   8. Make code easy to use ( hide PeopleSet and RangeList)
   9. Inclusion/Exclusion set should be considered sequentially.

   futher TODO:
   1. handle different format GT:GD:DP ... // use getFormatIndex()
   8. force loading index when read by region.
*/
#include "Argument.h"
#include "IO.h"
#include "tabix.h"

#include <cassert>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <algorithm>

#include "Utils.h"
#include "VCFUtil.h"

#include "MathVector.h"
#include "MathMatrix.h"

#include "Analysis.h"

void REQUIRE_STRING_PARAMETER(const std::string& flag, const char* msg){
    if (flag.size() == 0){
        fprintf(stderr, "%s\n", msg);
        abort();
    }
};

int main(int argc, char** argv){
    time_t currentTime = time(0);
    fprintf(stderr, "Analysis started at: %s", ctime(&currentTime));

    ////////////////////////////////////////////////
    BEGIN_PARAMETER_LIST(pl)
        ADD_PARAMETER_GROUP(pl, "Input/Output")
        ADD_STRING_PARAMETER(pl, inVcf, "--inVcf", "input VCF File")
        ADD_STRING_PARAMETER(pl, outVcf, "--outVcf", "output prefix")
        ADD_STRING_PARAMETER(pl, outPlink, "--make-bed", "output prefix")
        ADD_STRING_PARAMETER(pl, pheno, "--pheno", "specify phenotype file")
        ADD_PARAMETER_GROUP(pl, "People Filter")
        ADD_STRING_PARAMETER(pl, peopleIncludeID, "--peopleIncludeID", "give IDs of people that will be included in study")
        ADD_STRING_PARAMETER(pl, peopleIncludeFile, "--peopleIncludeFile", "from given file, set IDs of people that will be included in study")
        ADD_STRING_PARAMETER(pl, peopleExcludeID, "--peopleExcludeID", "give IDs of people that will be included in study")
        ADD_STRING_PARAMETER(pl, peopleExcludeFile, "--peopleExcludeFile", "from given file, set IDs of people that will be included in study")
        ADD_PARAMETER_GROUP(pl, "Site Filter")
        ADD_STRING_PARAMETER(pl, rangeList, "--rangeList", "Specify some ranges to use, please use chr:begin-end format.")
        ADD_STRING_PARAMETER(pl, rangeFile, "--rangeFile", "Specify the file containing ranges, please use chr:begin-end format.")
        END_PARAMETER_LIST(pl)
        ;    

    pl.Read(argc, argv);
    pl.Status();
    
    if (FLAG_REMAIN_ARG.size() > 0){
        fprintf(stderr, "Unparsed arguments: ");
        for (unsigned int i = 0; i < FLAG_REMAIN_ARG.size(); i++){
            fprintf(stderr, " %s", FLAG_REMAIN_ARG[i].c_str());
        }
        fprintf(stderr, "\n");
        abort();
    }

    REQUIRE_STRING_PARAMETER(FLAG_inVcf, "Please provide input file using: --inVcf");

    const char* fn = FLAG_inVcf.c_str(); 
    VCFInputFile vin(fn);

    // set range filters here
    vin.setRangeList(FLAG_rangeList.c_str());
    vin.setRangeList(FLAG_rangeFile.c_str());

    // set people filters here
    if (FLAG_peopleIncludeID.size() || FLAG_peopleIncludeFile.size()) {
        vin.excludeAllPeople();
        vin.includePeople(FLAG_peopleIncludeID.c_str());
        vin.includePeopleFromFile(FLAG_peopleIncludeFile.c_str());
    }
    vin.excludePeople(FLAG_peopleExcludeID.c_str());
    vin.excludePeopleFromFile(FLAG_peopleExcludeFile.c_str());    // now let's finish some statistical tests
    
    // add filters. e.g. put in VCFInputFile is a good method
    // site: DP, MAC, MAF (T3, T5)
    // indv: GD, GQ 

    Collapsor collapsor;
    if (false) {
        // single variant test for a set of markers using collapsing
        collapsor.setSetFileName("set.txt");
    } else {
        // single variant test for each marker
    }
    collapsor.setCollapsingStrategy(Collapsor::NAIVE);
    VCFData data;
    // data.LoadCovariate("cov.txt");
    data.loadPlinkPhenotype(FLAG_pheno.c_str());

    Vector pheno;
    data.extractPhenotype(&pheno);

    LogisticModelScoreTest lmf;
    //PermutateModelFitter lmf;

    FILE* fout = fopen("results.txt", "w");
    // write headers
    //...
#pragma messge "Output headers"                
    while(collapsor.iterateSet(vin, &data)){
        std::string& setName = collapsor.getCurrentSetName();
        collapsor.extractGenotype(&data);
        // for each model, fit the genotype data
        lmf.fit(data.collapsedGenotype, &pheno, data.covariate, fout);
        
        // output 
        data.writeRawData(setName.c_str());
        collapsor.writeOutput(fout);
    }
    fclose(fout);

    // now use analysis module to load data by set
    // load to set
    // for each set:
    //    load Set
    //    collapse
    //    fit model
    //    output datasets and results

    currentTime = time(0);
    fprintf(stderr, "Analysis ended at: %s", ctime(&currentTime));

    return 0;
};
