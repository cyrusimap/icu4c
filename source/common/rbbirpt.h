//---------------------------------------------------------------------------------
//
// Generated Header File.  Do not edit by hand.
//    This file contains the state table for the ICU Rule Based Break Iterator
//    rule parser.
//    It is generated by the Perl script "rbbicst.pl" from
//    the rule parser state definitions file "rbbirpt.txt".
//
//   Copyright (C) 2002 International Business Machines Corporation 
//   and others. All rights reserved.  
//
//---------------------------------------------------------------------------------
#ifndef RBBIRPT_H
#define RBBIRPT_H

U_NAMESPACE_BEGIN
//
// Character classes for RBBI rule scanning.
//
    static const uint8_t kRuleSet_digit_char = 128;
    static const uint8_t kRuleSet_white_space = 129;
    static const uint8_t kRuleSet_rule_char = 130;
    static const uint8_t kRuleSet_name_start_char = 131;
    static const uint8_t kRuleSet_name_char = 132;


enum RBBI_RuleParseAction {
    doExprOrOperator,
    doOptionEnd,
    doRuleErrorAssignExpr,
    doTagValue,
    doEndAssign,
    doRuleError,
    doVariableNameExpectedErr,
    doRuleChar,
    doLParen,
    doSlash,
    doStartTagValue,
    doDotAny,
    doExprFinished,
    doScanUnicodeSet,
    doExprRParen,
    doStartVariableName,
    doTagExpectedError,
    doTagDigit,
    doUnaryOpStar,
    doEndVariableName,
    doNOP,
    doUnaryOpQuestion,
    doExit,
    doStartAssign,
    doEndOfRule,
    doUnaryOpPlus,
    doExprStart,
    doOptionStart,
    doExprCatOperator,
    doReverseDir,
    doCheckVarDef,
    rbbiLastAction};

//-------------------------------------------------------------------------------
//
//  RBBIRuleTableEl    represents the structure of a row in the transition table
//                     for the rule parser state machine.
//-------------------------------------------------------------------------------
struct RBBIRuleTableEl {
    RBBI_RuleParseAction          fAction;
    uint8_t                       fCharClass;       // 0-127:    an individual ASCII character
                                                    // 128-255:  character class index
    uint8_t                       fNextState;       // 0-250:    normal next-stat numbers
                                                    // 255:      pop next-state from stack.
    uint8_t                       fPushState;
    UBool                         fNextChar;
};

static const struct RBBIRuleTableEl gRuleParseStateTable[] = {
    {doNOP, 0, 0, 0, TRUE}
    , {doExprStart, 254, 21, 8, FALSE}     //  1      start
    , {doNOP, 129, 1,0,  TRUE}     //  2 
    , {doExprStart, 36 /* $ */, 80, 90, FALSE}     //  3 
    , {doNOP, 33 /* ! */, 11,0,  TRUE}     //  4 
    , {doNOP, 59 /* ; */, 1,0,  TRUE}     //  5 
    , {doNOP, 252, 0,0,  FALSE}     //  6 
    , {doExprStart, 255, 21, 8, FALSE}     //  7 
    , {doEndOfRule, 59 /* ; */, 1,0,  TRUE}     //  8      break-rule-end
    , {doNOP, 129, 8,0,  TRUE}     //  9 
    , {doRuleError, 255, 95,0,  FALSE}     //  10 
    , {doNOP, 33 /* ! */, 13,0,  TRUE}     //  11      rev-option
    , {doReverseDir, 255, 20, 8, FALSE}     //  12 
    , {doOptionStart, 131, 15,0,  TRUE}     //  13      option-scan1
    , {doRuleError, 255, 95,0,  FALSE}     //  14 
    , {doNOP, 132, 15,0,  TRUE}     //  15      option-scan2
    , {doOptionEnd, 255, 17,0,  FALSE}     //  16 
    , {doNOP, 59 /* ; */, 1,0,  TRUE}     //  17      option-scan3
    , {doNOP, 129, 17,0,  TRUE}     //  18 
    , {doNOP, 255, 95,0,  FALSE}     //  19 
    , {doExprStart, 255, 21, 8, FALSE}     //  20      reverse-rule
    , {doRuleChar, 254, 30,0,  TRUE}     //  21      term
    , {doNOP, 129, 21,0,  TRUE}     //  22 
    , {doRuleChar, 130, 30,0,  TRUE}     //  23 
    , {doNOP, 91 /* [ */, 86, 30, FALSE}     //  24 
    , {doLParen, 40 /* ( */, 21, 30, TRUE}     //  25 
    , {doNOP, 36 /* $ */, 80, 29, FALSE}     //  26 
    , {doDotAny, 46 /* . */, 30,0,  TRUE}     //  27 
    , {doRuleError, 255, 95,0,  FALSE}     //  28 
    , {doCheckVarDef, 255, 30,0,  FALSE}     //  29      term-var-ref
    , {doNOP, 129, 30,0,  TRUE}     //  30      expr-mod
    , {doUnaryOpStar, 42 /* * */, 35,0,  TRUE}     //  31 
    , {doUnaryOpPlus, 43 /* + */, 35,0,  TRUE}     //  32 
    , {doUnaryOpQuestion, 63 /* ? */, 35,0,  TRUE}     //  33 
    , {doNOP, 255, 35,0,  FALSE}     //  34 
    , {doExprCatOperator, 254, 21,0,  FALSE}     //  35      expr-cont
    , {doNOP, 129, 35,0,  TRUE}     //  36 
    , {doExprCatOperator, 130, 21,0,  FALSE}     //  37 
    , {doExprCatOperator, 91 /* [ */, 21,0,  FALSE}     //  38 
    , {doExprCatOperator, 40 /* ( */, 21,0,  FALSE}     //  39 
    , {doExprCatOperator, 36 /* $ */, 21,0,  FALSE}     //  40 
    , {doExprCatOperator, 46 /* . */, 21,0,  FALSE}     //  41 
    , {doExprCatOperator, 47 /* / */, 47,0,  FALSE}     //  42 
    , {doExprCatOperator, 123 /* { */, 59,0,  TRUE}     //  43 
    , {doExprOrOperator, 124 /* | */, 21,0,  TRUE}     //  44 
    , {doExprRParen, 41 /* ) */, 255,0,  TRUE}     //  45 
    , {doExprFinished, 255, 255,0,  FALSE}     //  46 
    , {doSlash, 47 /* / */, 49,0,  TRUE}     //  47      look-ahead
    , {doNOP, 255, 95,0,  FALSE}     //  48 
    , {doExprCatOperator, 254, 21,0,  FALSE}     //  49      expr-cont-no-slash
    , {doNOP, 129, 35,0,  TRUE}     //  50 
    , {doExprCatOperator, 130, 21,0,  FALSE}     //  51 
    , {doExprCatOperator, 91 /* [ */, 21,0,  FALSE}     //  52 
    , {doExprCatOperator, 40 /* ( */, 21,0,  FALSE}     //  53 
    , {doExprCatOperator, 36 /* $ */, 21,0,  FALSE}     //  54 
    , {doExprCatOperator, 46 /* . */, 21,0,  FALSE}     //  55 
    , {doExprOrOperator, 124 /* | */, 21,0,  TRUE}     //  56 
    , {doExprRParen, 41 /* ) */, 255,0,  TRUE}     //  57 
    , {doExprFinished, 255, 255,0,  FALSE}     //  58 
    , {doNOP, 129, 59,0,  TRUE}     //  59      tag-open
    , {doStartTagValue, 128, 62,0,  FALSE}     //  60 
    , {doTagExpectedError, 255, 95,0,  FALSE}     //  61 
    , {doNOP, 129, 66,0,  TRUE}     //  62      tag-value
    , {doNOP, 125 /* } */, 66,0,  FALSE}     //  63 
    , {doTagDigit, 128, 62,0,  TRUE}     //  64 
    , {doTagExpectedError, 255, 95,0,  FALSE}     //  65 
    , {doNOP, 129, 66,0,  TRUE}     //  66      tag-close
    , {doTagValue, 125 /* } */, 69,0,  TRUE}     //  67 
    , {doTagExpectedError, 255, 95,0,  FALSE}     //  68 
    , {doExprCatOperator, 254, 21,0,  FALSE}     //  69      expr-cont-no-tag
    , {doNOP, 129, 69,0,  TRUE}     //  70 
    , {doExprCatOperator, 130, 21,0,  FALSE}     //  71 
    , {doExprCatOperator, 91 /* [ */, 21,0,  FALSE}     //  72 
    , {doExprCatOperator, 40 /* ( */, 21,0,  FALSE}     //  73 
    , {doExprCatOperator, 36 /* $ */, 21,0,  FALSE}     //  74 
    , {doExprCatOperator, 46 /* . */, 21,0,  FALSE}     //  75 
    , {doExprCatOperator, 47 /* / */, 47,0,  FALSE}     //  76 
    , {doExprOrOperator, 124 /* | */, 21,0,  TRUE}     //  77 
    , {doExprRParen, 41 /* ) */, 255,0,  TRUE}     //  78 
    , {doExprFinished, 255, 255,0,  FALSE}     //  79 
    , {doStartVariableName, 36 /* $ */, 82,0,  TRUE}     //  80      scan-var-name
    , {doNOP, 255, 95,0,  FALSE}     //  81 
    , {doNOP, 131, 84,0,  TRUE}     //  82      scan-var-start
    , {doVariableNameExpectedErr, 255, 95,0,  FALSE}     //  83 
    , {doNOP, 132, 84,0,  TRUE}     //  84      scan-var-body
    , {doEndVariableName, 255, 255,0,  FALSE}     //  85 
    , {doScanUnicodeSet, 91 /* [ */, 255,0,  TRUE}     //  86      scan-unicode-set
    , {doScanUnicodeSet, 112 /* p */, 255,0,  TRUE}     //  87 
    , {doScanUnicodeSet, 80 /* P */, 255,0,  TRUE}     //  88 
    , {doNOP, 255, 95,0,  FALSE}     //  89 
    , {doNOP, 129, 90,0,  TRUE}     //  90      assign-or-rule
    , {doStartAssign, 61 /* = */, 21, 93, TRUE}     //  91 
    , {doNOP, 255, 29, 8, FALSE}     //  92 
    , {doEndAssign, 59 /* ; */, 1,0,  TRUE}     //  93      assign-end
    , {doRuleErrorAssignExpr, 255, 95,0,  FALSE}     //  94 
    , {doExit, 255, 95,0,  TRUE}     //  95      errorDeath
 };
static const char * const RBBIRuleStateNames[] = {    0,
     "start",
    0,
    0,
    0,
    0,
    0,
    0,
     "break-rule-end",
    0,
    0,
     "rev-option",
    0,
     "option-scan1",
    0,
     "option-scan2",
    0,
     "option-scan3",
    0,
    0,
     "reverse-rule",
     "term",
    0,
    0,
    0,
    0,
    0,
    0,
    0,
     "term-var-ref",
     "expr-mod",
    0,
    0,
    0,
    0,
     "expr-cont",
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
     "look-ahead",
    0,
     "expr-cont-no-slash",
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
     "tag-open",
    0,
    0,
     "tag-value",
    0,
    0,
    0,
     "tag-close",
    0,
    0,
     "expr-cont-no-tag",
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
     "scan-var-name",
    0,
     "scan-var-start",
    0,
     "scan-var-body",
    0,
     "scan-unicode-set",
    0,
    0,
    0,
     "assign-or-rule",
    0,
    0,
     "assign-end",
    0,
     "errorDeath",
    0};

U_NAMESPACE_END
#endif
