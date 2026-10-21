/*
 * unix_utils.h
 *
 *  Created on: 21 Oct 2026
 *      Author: tyrrells
 */

#ifndef CORE_SHARED_UTIL_INCLUDE_IO_UNIX_UTILS_H_
#define CORE_SHARED_UTIL_INCLUDE_IO_UNIX_UTILS_H_

#include <dirent.h>


#include "grassroots_util_library.h"
#include "linked_list.h"


#ifdef __cplusplus
extern "C"
{
#endif


GRASSROOTS_UTIL_LOCAL LinkedList *GetUnixMatchingFiles (const char ** const patterns_ss, const bool full_path_flag, const bool recurse_flag, bool (*match_fn) (const void *data_p));


#ifdef __cplusplus
}
#endif


#endif /* CORE_SHARED_UTIL_INCLUDE_IO_UNIX_UTILS_H_ */
