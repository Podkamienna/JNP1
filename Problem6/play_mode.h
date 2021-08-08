#ifndef PLAYLIST_PLAY_MODE_H
#define PLAYLIST_PLAY_MODE_H

#include "playlist_element.h"

#include <vector>
#include <memory>

class PlayMode {
public:
	PlayMode() = default;

	virtual ~PlayMode() = default;

	virtual void play(const std::vector<std::shared_ptr<PlaylistElement>> &playlist) = 0;
};

std::shared_ptr<PlayMode> createSequenceMode();

std::shared_ptr<PlayMode> createShuffleMode(size_t seed);

std::shared_ptr<PlayMode> createOddEvenMode();

#endif //PLAYLIST_PLAY_MODE_H
