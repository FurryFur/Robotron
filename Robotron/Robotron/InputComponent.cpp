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

InBufferStream& operator>>(InBufferStream& ibs, InputComponent& input)
{
	ibs >> input.axis >> input.orientationDelta;

	std::uint8_t boolPack = 0;
	ibs >> boolPack;
	input.btn1Down = (boolPack & (1 << 0)) == (1 << 0);
	input.btn2Down = (boolPack & (1 << 1)) == (1 << 1);
	input.btn3Down = (boolPack & (1 << 2)) == (1 << 2);
	input.btn4Down = (boolPack & (1 << 3)) == (1 << 3);

	return ibs;
}
