#include "InputComponent.h"

#include "BufferStream.h"

OutBufferStream& InputComponent::serialize(OutBufferStream& obs) const
{
	obs << axis << orientationDelta;

	std::uint16_t boolPack = 0;
	if (btn1Down)
		boolPack |= 1 << 0;
	if (btn2Down)
		boolPack |= 1 << 1;
	if (btn3Down)
		boolPack |= 1 << 2;
	if (btn4Down)
		boolPack |= 1 << 3;
	if (shootLeftDown)
		boolPack |= 1 << 4;
	if (shootDown)
		boolPack |= 1 << 5;
	if (shootRightDown)
		boolPack |= 1 << 6;
	if (shootLeft)
		boolPack |= 1 << 7;
	if (shootRight)
		boolPack |= 1 << 8;
	if (shootLeftUp)
		boolPack |= 1 << 9;
	if (shootUp)
		boolPack |= 1 << 10;
	if (shootRightUp)
		boolPack |= 1 << 11;

	return obs << boolPack;
}

InBufferStream& InputComponent::deserialize(InBufferStream& ibs)
{
	std::uint16_t boolPack;
	ibs >> axis >> orientationDelta >> boolPack;

	btn1Down = (boolPack & (1 << 0)) != 0;
	btn2Down = (boolPack & (1 << 1)) != 0;
	btn3Down = (boolPack & (1 << 2)) != 0;
	btn4Down = (boolPack & (1 << 3)) != 0;
	shootLeftDown = (boolPack & (1 << 4)) != 0;
	shootDown = (boolPack & (1 << 5)) != 0;
	shootRightDown = (boolPack & (1 << 6)) != 0;
	shootLeft = (boolPack & (1 << 7)) != 0;
	shootRight = (boolPack & (1 << 8)) != 0;
	shootLeftUp = (boolPack & (1 << 9)) != 0;
	shootUp = (boolPack & (1 << 10)) != 0;
	shootRightUp = (boolPack & (1 << 11)) != 0;

	return ibs;
}

OutBufferStream& operator<<(OutBufferStream& obs, const InputComponent& input)
{
	return input.serialize(obs);
}

InBufferStream& operator>>(InBufferStream& ibs, InputComponent& input)
{
	return input.deserialize(ibs);
}
