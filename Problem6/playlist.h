#ifndef PLAYLIST_PLAYLIST_H
#define PLAYLIST_PLAYLIST_H

#include "play_mode.h"
#include "playlist_element.h"

#include <cstddef>
#include <vector>
#include <memory>
#include <string_view>

class Playlist : public PlaylistElement {
public:
	explicit Playlist(std::string_view playlistName);

	void add(const std::shared_ptr<PlaylistElement> &playlistElement);

	void add(const std::shared_ptr<PlaylistElement> &playlistElement, size_t position);

	void remove();

	void remove(size_t position);

	void setMode(const std::shared_ptr<PlayMode> &newPlayMode);

	void play() const override;

	bool contains(PlaylistElement *playlistElement) const override;

private:
	std::string name;

	std::vector<std::shared_ptr<PlaylistElement>> playlist{};

	std::shared_ptr<PlayMode> playMode;
};


#endif //PLAYLIST_PLAYLIST_H
