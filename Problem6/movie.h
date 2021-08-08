#ifndef PLAYLIST_MOVIE_H
#define PLAYLIST_MOVIE_H

#include "playlist_element.h"
#include "file.h"

#include <string>

const int ALPHABET_SIZE = 26;

class Movie : public PlaylistElement {
public:
    explicit Movie(const File &file);

    void play() const override;

	bool contains(PlaylistElement *playlistElement) const override;

private:
	void decipherROT13();

	static const char rot13[ALPHABET_SIZE];

	std::string title;

	std::string year;

	std::string content;
};

#endif //PLAYLIST_MOVIE_H
