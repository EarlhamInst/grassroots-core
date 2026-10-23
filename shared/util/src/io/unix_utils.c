/*
 * unix_utils.c
 *
 *  Created on: 21 Oct 2026
 *      Author: tyrrells
 */

#include <fnmatch.h>

#include "unix_utils.h"

#include "string_linked_list.h"
#include "filesystem_utils.h"



LinkedList *GetUnixMatchingFiles (const char ** const patterns_ss, const bool full_path_flag, const bool recurse_flag, bool (*match_fn) (const void *data_p))
{
	LinkedList *list_p = AllocateLinkedList (FreeStringListNode);

	if (list_p)
		{
			const char **pattern_ss = patterns_ss;

			while (*pattern_ss)
				{
					char *filename_p = GetFilenameOnly (*pattern_ss);

					if (filename_p)
						{
							char *path_p = GetPathOnly (*pattern_ss);

							if (path_p)
								{
									DIR *dir_p = opendir (path_p);

									if (dir_p)
										{
											struct dirent entry;
											struct dirent *entry_p = &entry;
											const int flags = FNM_CASEFOLD;

											while ((entry_p = readdir (dir_p)) != NULL)
												{
													if ((fnmatch (filename_p, entry_p -> d_name, flags)) == 0)
														{
															if ((match_fn == NULL) || (match_fn (entry_p)))
																{
																	StringListNode *node_p = NULL;

																	if (full_path_flag)
																		{
																			char *full_filename_s = MakeFilename (path_p, entry_p -> d_name);

																			if (full_filename_s)
																				{
																					node_p = AllocateStringListNode (full_filename_s, MF_SHALLOW_COPY);
																				}
																		}
																	else
																		{
																			node_p = AllocateStringListNode (entry_p -> d_name, MF_DEEP_COPY);
																		}

																	if (node_p)
																		{
																			LinkedListAddTail (list_p, (ListItem *) node_p);
																		}
																}


														}
												}

											closedir (dir_p);
										}

									FreeMemory (path_p);
								}

							FreeMemory (filename_p);
						}

					++ pattern_ss;
				}		/* while (*pattern_ss) */



			if (list_p -> ll_size > 0)
				{
					return list_p;
				}
			else
				{
					FreeLinkedList (list_p);
				}
		}

	return NULL;
}

