#include "InputComponent.h"

#include "BufferStream.h"
#include "GLMUtils.h"

OutBufferStream& operator<<(OutBufferStream& obs, const InputComponent& input)
{
	obs << input.axis << input.orientationDelta;

	std::uint8_t boolPack = 0;
	boolPack |= input.btn1Down << 0;
	boolPack |= input.btn2Down << 1;
	boolPack |= input.btn3Down << 2;
	boolPack |= input.btn4Down << 3;

	obs << boolPack;
	return obs;
}
