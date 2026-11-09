/*
 * unix_utils.c
 *
 *  Created on: 21 Oct 2026
 *      Author: tyrrells
 */

#include <errno.h>
#include <fnmatch.h>

#include "unix_utils.h"

#include "string_linked_list.h"
#include "filesystem_utils.h"
#include "streams.h"


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






bool UnixCopyToNewFile (const char * const src_filename_s, const char * const dest_filename_s, void (*callback_fn) (void))
{
	FILE *in_f = fopen (src_filename_s, "rb");
	bool success_flag = false;
	int saved_errno = 0;

	if (in_f)
		{
			FILE *out_f = fopen (dest_filename_s, "wb");

			if (out_f)
				{

					char buffer [32768];		/* 64 * 512 */
					success_flag = true;
					bool loop_flag = true;

					while (success_flag && loop_flag)
						{
							size_t num_objs_read = fread (buffer, 1, 32768, in_f);

							if (num_objs_read > 0)
								{
									size_t num_objs_written = fwrite (buffer, 1, num_objs_read, out_f);

									if (num_objs_read != num_objs_written)
										{
											success_flag = false;
											saved_errno = errno;
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "CopyToNewFile (): Read " SIZET_FMT " blocks but wrote " SIZET_FMT, num_objs_read, num_objs_written);
										}
								}
							else
								{
									loop_flag = false;
								}

						}		/* while (success_flag) */

					if (success_flag)
						{
							success_flag = (feof (in_f) != 0) ? true : false;
						}

					fclose (out_f);

				}		/* if (out_f) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "CopyToNewFile (): Failed to open destination file \"%s\"", dest_filename_s);
				}

			fclose (in_f);

		}		/* if (in_f) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "CopyToNewFile (): Failed to open source file \"%s\"", src_filename_s);
		}

	/* In case closing one of the files overwrote it, restore errno */
	if (saved_errno != 0)
		{
			errno = saved_errno;
		}


	if (!success_flag)
		{
			FileInformation info = { };

			if (CalculateFileInformation (src_filename_s, &info))
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "CopyToNewFile (): Source file \"%s\" size " SIZET_FMT, src_filename_s, info.fi_size);
				}

			if (CalculateFileInformation (dest_filename_s, &info))
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "CopyToNewFile (): Destination file \"%s\" size " SIZET_FMT, dest_filename_s, info.fi_size);
				}

		}

	return success_flag;
}

