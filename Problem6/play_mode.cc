#include "play_mode.h"
#include "odd_even_play_mode.h"
#include "sequence_play_mode.h"
#include "shuffle_play_mode.h"

#include <memory>
#include <vector>


std::shared_ptr<PlayMode> createSequenceMode() {
	return std::make_shared<SequencePlayMode>();
}

std::shared_ptr<PlayMode> createShuffleMode(size_t seed) {
	return std::make_shared<ShufflePlayMode>(seed);
}

std::shared_ptr<PlayMode> createOddEvenMode() {
	return std::make_shared<OddEvenPlayMode>();
}