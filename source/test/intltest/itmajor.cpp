/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 1998-2003, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

/**
 * MajorTestLevel is the top level test class for everything in the directory "IntlWork".
 */

/***********************************************************************
* Modification history
* Date        Name        Description
* 02/14/2001  synwee      Release collation for testing.
***********************************************************************/

#include "unicode/utypes.h"
#include "itmajor.h"

#include "itutil.h"
#include "tscoll.h"
#include "itformat.h"
#include "ittrans.h"
#include "itrbbi.h"
#include "itrbnf.h"
#include "itrbnfrt.h"
#include "normconf.h"
#include "regextst.h"
#include "tstnorm.h"
#include "canittst.h"
#include "icusvtst.h"
#include "testidna.h"
#include "convtest.h"

#define CASE_SUITE(id, suite) case id:                  \
                          name = #suite;                \
                          if(exec) {                    \
                              logln(#suite "---");      \
                              suite test;               \
                              callTest(test, par);      \
                          }                             \
                          break

void MajorTestLevel::runIndexedTest( int32_t index, UBool exec, const char* &name, char* par )
{
    switch (index) {
        case 0: name = "utility";
                if (exec) {
                    logln("TestSuite Utilities---"); logln();
                    IntlTestUtilities test;
                    callTest( test, par );
                }
                break;

        case 1: name = "normalize";
#if !UCONFIG_NO_NORMALIZATION
                if (exec) {
                    logln("TestSuite Normalize---"); logln();
                    IntlTestNormalize test;
                    callTest( test, par );
                }
#endif
                break;

        case 2: name = "collate";
#if !UCONFIG_NO_COLLATION
                if (exec) {
                    logln("TestSuite Collator---"); logln();
                    IntlTestCollator test;
                    callTest( test, par );
                }
#endif
                break;

        case 3: name = "regex";
#if !UCONFIG_NO_REGULAR_EXPRESSIONS
                if (exec) {
                    logln("TestSuite Regex---"); logln();
                    RegexTest test;
                    callTest( test, par );
                }
#endif
                break;

        case 4: name = "format";
#if !UCONFIG_NO_FORMATTING
                if (exec) {
                    logln("TestSuite Format---"); logln();
                    IntlTestFormat test;
                    callTest( test, par );
                }
#endif
                break;

        case 5: name = "translit";
#if !UCONFIG_NO_TRANSLITERATION
                if (exec) {
                    logln("TestSuite Transliterator---"); logln();
                    IntlTestTransliterator test;
                    callTest( test, par );
                }
#endif
                break;

        case 6: name = "rbbi";
#if !UCONFIG_NO_BREAK_ITERATION
                if (exec) {
                    logln("TestSuite RuleBasedBreakIterator---"); logln();
                    IntlTestRBBI test;
                    callTest( test, par );
                }
#endif
                break;
        case 7: name = "rbnf";
#if !UCONFIG_NO_FORMATTING
                if (exec) {
                    logln("TestSuite RuleBasedNumberFormat----"); logln();
                    IntlTestRBNF test;
                    callTest(test, par);
                }
#endif
                break;
        case 8: name = "rbnfrt";
#if !UCONFIG_NO_FORMATTING
                if (exec) {
                    logln("TestSuite RuleBasedNumberFormat RT----"); logln();
                    RbnfRoundTripTest test;
                    callTest(test, par);
                }
#endif
                break;

        case 9: name = "icuserv";
#if !UCONFIG_NO_SERVICE
                if (exec) {
                    logln("TestSuite ICUService---"); logln();
                    ICUServiceTest test;
                    callTest(test, par);
                }
#endif
                break;
        case 10: name = "idna";
#if !UCONFIG_NO_IDNA  && !UCONFIG_NO_TRANSLITERATION
            if(exec){
                logln("TestSuite IDNA----"); logln();
                TestIDNA test;
                callTest(test,par);
            }
#endif
            break;
        case 11: name = "conversion";
                if (exec) {
                    logln("TestSuite Conversion---"); logln();
                    ConversionTest test;
                    callTest( test, par );
                }
                break;

        default: name = ""; break;
    }
}

void IntlTestNormalize::runIndexedTest( int32_t index, UBool exec, const char* &name, char* par )
{
    if(exec) logln("TestSuite Normalize:");
#if UCONFIG_NO_NORMALIZATION
    name="";
#else
    switch (index) {
        CASE_SUITE(0, BasicNormalizerTest);
        CASE_SUITE(1, NormalizerConformanceTest); // this takes a long time
        CASE_SUITE(2, CanonicalIteratorTest);
        default:
            name="";
            break;
    }
#endif
}
