#ifndef PLAYLIST_SEQUENCE_PLAY_MODE_H
#define PLAYLIST_SEQUENCE_PLAY_MODE_H

#include "play_mode.h"
#include "playlist_element.h"

#include <memory>
#include <vector>

class SequencePlayMode : public PlayMode {
public:
	void play(const std::vector<std::shared_ptr<PlaylistElement>> &playlist) override;
};

#endif //PLAYLIST_SEQUENCE_PLAY_MODE_H
