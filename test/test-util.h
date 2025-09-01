/**
 * Utility test macros and functions.
 */

#define add_test_case(item, testCase)                                          \
    g_test_add_func("/ibus-chewing/" item "/" QUOTE_ME(testCase), testCase)
