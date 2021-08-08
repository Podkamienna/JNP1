#ifndef PLAYLIST_PLAYER_H
#define PLAYLIST_PLAYER_H

#include "file.h"
#include "playlist.h"
#include "playlist_element.h"

#include <string_view>
#include <memory>

class Player {
public:
	static std::shared_ptr<PlaylistElement> openFile(const File &file);

	static std::shared_ptr<Playlist> createPlaylist(std::string_view name);
};


#endif //PLAYLIST_PLAYER_H
