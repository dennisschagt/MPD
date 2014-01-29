/*
 * Copyright (C) 2003-2014 The Music Player Daemon Project
 * http://www.musicpd.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef MPD_UPDATE_WALK_HXX
#define MPD_UPDATE_WALK_HXX

#include "check.h"
#include "Editor.hxx"

#include <sys/stat.h>

struct stat;
struct Directory;
struct archive_plugin;
class ExcludeList;

class UpdateWalk final {
#ifdef ENABLE_ARCHIVE
	friend class UpdateArchiveVisitor;
#endif

#ifndef WIN32
	static constexpr bool DEFAULT_FOLLOW_INSIDE_SYMLINKS = true;
	static constexpr bool DEFAULT_FOLLOW_OUTSIDE_SYMLINKS = true;

	bool follow_inside_symlinks;
	bool follow_outside_symlinks;
#endif

	bool walk_discard;
	bool modified;

	DatabaseEditor editor;

public:
	UpdateWalk(EventLoop &_loop);

	/**
	 * Returns true if the database was modified.
	 */
	bool Walk(const char *path, bool discard);

private:
	gcc_pure
	bool SkipSymlink(const Directory *directory,
			 const char *utf8_name) const;

	void RemoveExcludedFromDirectory(Directory &directory,
					 const ExcludeList &exclude_list);

	void PurgeDeletedFromDirectory(Directory &directory);

	void UpdateSongFile2(Directory &directory,
			     const char *name, const char *suffix,
			     const struct stat *st);

	bool UpdateSongFile(Directory &directory,
			    const char *name, const char *suffix,
			    const struct stat *st);

	bool UpdateContainerFile(Directory &directory,
				 const char *name, const char *suffix,
				 const struct stat *st);


#ifdef ENABLE_ARCHIVE
	void UpdateArchiveTree(Directory &parent, const char *name);

	bool UpdateArchiveFile(Directory &directory,
			       const char *name, const char *suffix,
			       const struct stat *st);

	void UpdateArchiveFile(Directory &directory, const char *name,
			       const struct stat *st,
			       const archive_plugin &plugin);


#else
	bool UpdateArchiveFile(gcc_unused Directory &directory,
			       gcc_unused const char *name,
			       gcc_unused const char *suffix,
			       gcc_unused const struct stat *st) {
		return false;
	}
#endif

	bool UpdatePlaylistFile(Directory &directory,
				const char *name, const char *suffix,
				const struct stat *st);

	bool UpdateRegularFile(Directory &directory,
			       const char *name, const struct stat *st);

	void UpdateDirectoryChild(Directory &directory,
				  const char *name, const struct stat *st);

	bool UpdateDirectory(Directory &directory, const struct stat *st);

	/**
	 * Create the specified directory object if it does not exist
	 * already or if the #stat object indicates that it has been
	 * modified since the last update.  Returns nullptr when it
	 * exists already and is unmodified.
	 *
	 * The caller must lock the database.
	 */
	Directory *MakeDirectoryIfModified(Directory &parent, const char *name,
					   const struct stat *st);

	Directory *DirectoryMakeChildChecked(Directory &parent,
					     const char *name_utf8);

	Directory *DirectoryMakeUriParentChecked(const char *uri);

	void UpdateUri(const char *uri);
};

#endif
