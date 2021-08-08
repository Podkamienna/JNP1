#ifndef PLAYLIST_SHUFFLE_PLAY_MODE_H
#define PLAYLIST_SHUFFLE_PLAY_MODE_H

#include "play_mode.h"
#include "playlist_element.h"

#include <memory>
#include <vector>
#include <random>

class ShufflePlayMode : public PlayMode {
public:
	explicit ShufflePlayMode(size_t seed) : randomEngine(seed) {}

	void play(const std::vector<std::shared_ptr<PlaylistElement>> &playlist) override;

private:
	std::default_random_engine randomEngine;
};

#endif //PLAYLIST_SHUFFLE_PLAY_MODE_H
