/*
**********************************************************************
*   Copyright (C) 1999, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   Date        Name        Description
*   11/17/99    aliu        Creation.
**********************************************************************
*/
#include "unicode/rbt.h"
#include "rbt_pars.h"
#include "rbt_data.h"
#include "rbt_rule.h"
#include "unicode/rep.h"

void RuleBasedTransliterator::_construct(const UnicodeString& rules,
                                         Direction direction,
                                         UErrorCode& status) {
    data = 0;
    isDataOwned = TRUE;
    if (U_SUCCESS(status)) {
        data = TransliterationRuleParser::parse(rules, direction);
        if (data == 0) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
        } else {
            setMaximumContextLength(data->ruleSet.getMaximumContextLength());
        }
    }
}

RuleBasedTransliterator::RuleBasedTransliterator(const UnicodeString& ID,
                                 const TransliterationRuleData* theData,
                                 UnicodeFilter* adoptedFilter) :
    Transliterator(ID, adoptedFilter),
    data((TransliterationRuleData*)theData), // cast away const
    isDataOwned(FALSE) {
    setMaximumContextLength(data->ruleSet.getMaximumContextLength());
}

/**
 * Copy constructor.  Since the data object is immutable, we can share
 * it with other objects -- no need to clone it.
 */
RuleBasedTransliterator::RuleBasedTransliterator(
        const RuleBasedTransliterator& other) :
    Transliterator(other), data(other.data) {
    // TODO: Finish this -- implement with correct data ownership handling
    if (other.isDataOwned) {
        // TODO: At this point we need to make our own copy of the data.
    } else {
        isDataOwned = FALSE;
    }
}

/**
 * Destructor.  We do NOT own the data object, so we do not delete it.
 */
RuleBasedTransliterator::~RuleBasedTransliterator() {
    if (isDataOwned) {
        delete data;
    }
}

Transliterator* // Covariant return NOT ALLOWED (for portability)
RuleBasedTransliterator::clone(void) const {
    return new RuleBasedTransliterator(*this);
}

/**
 * Transliterates a segment of a string.  <code>Transliterator</code> API.
 * @param text the string to be transliterated
 * @param start the beginning index, inclusive; <code>0 <= start
 * <= limit</code>.
 * @param limit the ending index, exclusive; <code>start <= limit
 * <= text.length()</code>.
 * @return The new limit index
 */
int32_t RuleBasedTransliterator::transliterate(Replaceable& text,
                                               int32_t start,
                                               int32_t limit) const {
    /* When using Replaceable, the algorithm is simpler, since we don't have
     * two separate buffers.  We keep start and limit fixed the entire time,
     * relative to the text -- limit may move numerically if text is
     * inserted or removed.  The cursor moves from start to limit, with
     * replacements happening under it.
     *
     * Example: rules 1. ab>x|y
     *                2. yc>z
     *
     * |eabcd   start - no match, advance cursor
     * e|abcd   match rule 1 - change text & adjust cursor
     * ex|ycd   match rule 2 - change text & adjust cursor
     * exz|d    no match, advance cursor
     * exzd|    done
     */
    int32_t cursor = start;
    while (cursor < limit) {
        TransliterationRule* r =
            data->ruleSet.findMatch(text, start, limit,
                                    cursor, *data,
                                    getFilter());
        if (r == 0) {
            ++cursor;
        } else {
            text.handleReplaceBetween(cursor, cursor + r->getKeyLength(),
                                      r->getOutput());
            limit += r->getOutput().length() - r->getKeyLength();
            cursor += r->getCursorPos();
        }
    }
    return limit;
}

/**
 * Implements {@link Transliterator#handleTransliterate}.
 */
void
RuleBasedTransliterator::handleTransliterate(Replaceable& text, Position& index,
                                             bool_t isIncremental) const {
    int32_t start = index.start;
    int32_t limit = index.limit;
    int32_t cursor = index.cursor;

    bool_t isPartial;

    while (cursor < limit) {
        TransliterationRule* r = data->ruleSet.findIncrementalMatch(
                text, start, limit, cursor,
                *data, isPartial,
                getFilter());
        /* If we match a rule then apply it by replacing the key
         * with the rule output and repositioning the cursor
         * appropriately.  If we get a partial match, then we
         * can't do anything without more text; return with the
         * cursor at the current position.  If we get null, then
         * there is no match at this position, and we can advance
         * the cursor.
         */
        if (r == 0) {
            if (isPartial) {
                break;
            } else {
                ++cursor;
            }
        } else {
            text.handleReplaceBetween(cursor, cursor + r->getKeyLength(),
                                      r->getOutput());
            limit += r->getOutput().length() - r->getKeyLength();
            cursor += r->getCursorPos();
        }
    }

    index.limit = limit;
    index.cursor = cursor;
}
