#ifndef PLAYLIST_SONG_H
#define PLAYLIST_SONG_H

#include "playlist_element.h"
#include "file.h"

#include <string>

class Song : public PlaylistElement {
public:
	explicit Song(const File &file);

	void play() const override;

	bool contains(PlaylistElement *playlistElement) const override;

private:
    std::string title;

    std::string artist;

    std::string content;
};


#endif //PLAYLIST_SONG_H
