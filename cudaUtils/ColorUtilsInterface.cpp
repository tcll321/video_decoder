#include "ColorUtilsInterface.h"
#include "ColorUtils.h"

ColorUtilsInterface::ColorUtilsInterface(void)
{

}

std::shared_ptr<ColorUtilsInterface> ColorUtilsInterface::Create(int deviceID)
{
	std::shared_ptr<ColorUtilsInterface> p(new CColorUtils(deviceID));
	return p;
}
