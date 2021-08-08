#ifndef PLAYLIST_PLAYLIST_ELEMENT_H
#define PLAYLIST_PLAYLIST_ELEMENT_H

class PlaylistElement {
public:
	virtual ~PlaylistElement() = default;

	virtual void play() const = 0;

	virtual bool contains(PlaylistElement *playlistElement) const = 0;
};

#endif //PLAYLIST_PLAYLIST_ELEMENT_H
