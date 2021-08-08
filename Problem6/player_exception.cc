#include "player_exception.h"

const char *UnsupportedTypeException::what() const noexcept {
	return "unsupported type";
}

const char *InvalidMetadataException::what() const noexcept {
	return "invalid metadata";
}

const char *LoopException::what() const noexcept {
	return "loop";
}

const char *CorruptFileException::what() const noexcept {
	return "corrupt file";
}

const char *CorruptContentException::what() const noexcept {
	return "corrupt content";
}

const char *OutOfBoundsException::what() const noexcept {
	return "out of bounds";
}
