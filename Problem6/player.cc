#include "player.h"
#include "playlist_element.h"
#include "song.h"
#include "movie.h"
#include "playlist.h"
#include "player_exception.h"

#include <string_view>
#include <string>
#include <memory>

using std::shared_ptr;
using std::string;
using std::string_view;
using std::make_shared;

shared_ptr<PlaylistElement> Player::openFile(const File &file) {
	const string &fileType = file.getType();

	if (fileType == "audio") {
		return make_shared<Song>(file);
	}

	if (fileType == "video") {
		return make_shared<Movie>(file);
	}

	throw UnsupportedTypeException();
}

shared_ptr<Playlist> Player::createPlaylist(string_view name) {
	return make_shared<Playlist>(name);
}
