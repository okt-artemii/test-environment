/* SPDX-License-Identifier: Apache-2.0 */
/** @file
 * @brief Key-value pairs API
 *
 * @defgroup te_tools_te_kvpair Key-value pairs
 * @ingroup te_tools
 * @{
 *
 * Definition of API for working with key-value pairs
 *
 *
 * Copyright (C) 2004-2022 OKTET Labs Ltd. All rights reserved.
 */

#ifndef __TE_TOOLS_KV_PAIRS_H__
#define __TE_TOOLS_KV_PAIRS_H__

#include "te_config.h"

#include "te_defs.h"
#include "te_errno.h"
#include "te_queue.h"
#include "te_string.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TE_KVPAIR_STR_DELIMITER ":"

/** Element of the list of key-value pair */
typedef struct te_kvpair {
    TAILQ_ENTRY(te_kvpair)   links;
    char                    *key;
    char                    *value;
} te_kvpair;

/** Head of the list of key-value pair */
typedef TAILQ_HEAD(te_kvpair_h, te_kvpair) te_kvpair_h;

/**
 * Initializes empty key-value pairs list.
 *
 * @param head      Head of the list.
 */
extern void te_kvpair_init(te_kvpair_h *head);

/**
 * Free resources allocated for key-value pairs list.
 *
 * @param head      Head of the list.
 */
extern void te_kvpair_fini(te_kvpair_h *head);

/**
 * Add a key-value pair.

 * If there are values bound to @p key, they are shadowed.
 *
 * @param head          Head of the list.
 * @param key           Key of value.
 * @param value_fmt     Format (*printf-like) string for value string.
 * @param ...           Respective parameters for format string.
 */
extern void te_kvpair_push(te_kvpair_h *head, const char *key,
                           const char *value_fmt, ...)
                           __attribute__((format(printf, 3, 4)));

/**
 * Add key-value pair using variadic list.
 *
 * If there are values bound to @p key, they are shadowed.
 *
 * @param head          Head of the list.
 * @param key           Key of value.
 * @param value_fmt     Format (*printf-like) string for value string.
 * @param ap            List of arguments.
 */
extern void te_kvpair_push_va(te_kvpair_h *head, const char *key,
                              const char *value_fmt, va_list ap)
                              __attribute__((format(printf, 3, 0)));


/**
 * Add a key-value pair.
 *
 * Unlike te_kvpair_push(), the function will fail if there are
 * existing bindings for @p key.
 *
 * @param head          Head of the list.
 * @param key           Key of value.
 * @param value_fmt     Format (*printf-like) string for value string.
 * @param ...           Respective parameters for format string.
 *
 * @return Status code
 * @retval TE_EEXIST    The key already exists
 */
extern te_errno te_kvpair_add(te_kvpair_h *head, const char *key,
                              const char *value_fmt, ...)
                              __attribute__((format(printf, 3, 4)));

/**
 * Add key-value pair using variadic list.
 *
 * Unlike te_kvpair_push_va(), the function will fail if there are
 * existing bindings for @p key.
 *
 * @param head          Head of the list.
 * @param key           Key of value.
 * @param value_fmt     Format (*printf-like) string for value string.
 * @param ap            List of arguments.
 *
 * @return Status code.
 * @retval TE_EEXIST    The key already exists
 */
extern te_errno te_kvpair_add_va(te_kvpair_h *head, const char *key,
                                 const char *value_fmt, va_list ap)
                                 __attribute__((format(printf, 3, 0)));

/**
 * Remove the most recently added key-value pair with
 * a given @p key.
 *
 * @param head          Head of the list.
 * @param key           Key of value.
 *
 * @return Status code.
 * @retval TE_ENOENT    No entry with such key.
 * @retval 0            Pair removed successfully.
 */
extern te_errno te_kvpairs_del(te_kvpair_h *head, const char *key);

/**
 * Remove all key-value pairs with a given @p key.
 *
 * @param head          Head of the list.
 * @param key           Key to delete
 *                      (may be @c NULL in which case all pairs are deleted).
 *
 * @return Status code.
 * @retval TE_ENOENT    No entries with a given key.
 * @retval 0            Pair removed successfully.
 */
extern te_errno te_kvpairs_del_all(te_kvpair_h *head, const char *key);


/**
 * Get the @p index'th value associated with the @p key.
 *
 * The most recently added value has the index @c 0.
 *
 * @param head   Head of the list.
 * @param key    Key of required value.
 * @param index  Ordinal index of a value to fetch.
 *
 * @return       Requested @p key value or @c NULL if there is no such key.
 */
extern const char *te_kvpairs_get_nth(const te_kvpair_h *head, const char *key,
                                      unsigned int index);

/**
 * Get the most recent value associated with the @p key.
 *
 * @param head   Head of the list.
 * @param key    Key of required value.
 *
 * @return       Requested @p key value or @c NULL if there is no such key.
 */
static inline const char *
te_kvpairs_get(const te_kvpair_h *head, const char *key)
{
    return te_kvpairs_get_nth(head, key, 0);
}

/**
 * Get all the values associated with @p key.
 *
 * The values are appended to the vector @p result.
 *
 * @note The values are not strduped, so @p result must not
 *       be freed with te_vec_deep_free().
 *
 * @param[in]  head   Head of the list.
 * @param[in]  key    Key (may be @c NULL, in which case
 *                    all values are returned).
 * @param[out] result The output vector.
 *
 * @return Status code.
 * @retval TE_ENOENT  No value with a given key exists in @p head.
 */
extern te_errno te_kvpairs_get_all(const te_kvpair_h *head, const char *key,
                                   te_vec *result);

/**
 * Count the number of values associated with @p key.
 *
 * @param head   Head of the list.
 * @param key    Key (may be @c NULL in which case all values are counted).
 *
 * @return The number of values associated with @p key.
 */
extern unsigned int te_kvpairs_count(const te_kvpair_h *head, const char *key);

/**
 * Function type of callbacks used by te_kvpairs_foreach().
 *
 * @param key    Current key.
 * @param value  Current value.
 * @param user   User data.
 *
 * @return Status code.
 * @retval TE_EOK te_kvpairs_foreach() will stop immediately and
 *                return success to the caller.
 */
typedef te_errno te_kvpairs_iter_fn(const char *key, const char *value,
                                    void *user);

/**
 * Call @p callback for all values bound to @p key.
 *
 * If @p callback returns non-zero status, the function
 * returns immediately, but TE_EOK status is treated as success.
 *
 * @param head     Head of the list.
 * @param callback Callback function.
 * @param key      Key to scan (may be @c NULL, in which case
 *                 all key-value pairs are scanned).
 * @param user     User data for @p callback.
 *
 * @return Status code.
 * @retval TE_ENOENT No values have been processed.
 */
extern te_errno te_kvpairs_foreach(const te_kvpair_h *head,
                                   te_kvpairs_iter_fn *callback,
                                   const char *key,
                                   void *user);

/**
 * Convert list of kv_pairs to string representation (i.e. key1=val1:key2=val2).
 *
 * If there are multiple values for the same key, they all be represented
 * as separate @c key=value pairs.
 *
 * @sa TE_KVPAIR_STR_DELIMITER
 *
 * @param[in]  head Head of the list.
 * @param[out] str  Pointer to string.
 *
 * @return          Status code.
 */
extern te_errno te_kvpair_to_str(const te_kvpair_h *head, te_string *str);

/**
 * Convert a list of kv pairs to a valid URI query string.
 *
 * If @p str is not empty, @c & separator is added, so that a query
 * string may be assembled incrementally from several kvpairs.
 *
 * @param[in]  head  Head of the list
 * @param[out] str   TE string
 *
 * @sa TE_STRING_URI_ESCAPE_QUERY_VALUE
 */
extern void te_kvpair_to_uri_query(const te_kvpair_h *head, te_string *str);

/**
 * Convert string to list of kv_pairs,
 * @sa TE_KVPAIR_STR_DELIMITER
 *
 * @param[in] str   Pointer to string
 * @param[out] head Head of the kv_pair list
 *
 * @retval          Status code
 */
extern te_errno te_kvpair_from_str(const char *str, te_kvpair_h *head);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* !__TE_TOOLS_KV_PAIRS_H__ */
/**@} <!-- END te_tools_te_kvpair --> */
