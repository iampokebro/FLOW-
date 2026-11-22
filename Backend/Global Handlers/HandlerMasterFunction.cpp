#include "HandlerMasterFunction.h"

//IMPORTANT UPDATE THIS EVERY TIME WHEN CREATEING A FUNCTION LOCALS
//Always Register Your Local Function No Other Way Devloped yet

//include for Global
#include "../Externals/Global Base/G1.hpp"

void Global(HandlerMasterFunction& mhf) {
	mhf.addHandler("Global");

	ManagerBase &Handler = mhf.Handler["Global"];

	Handler.Register("Addition", Addition);
	Handler.Register("Substract", Substract);
	Handler.Register("Multiply", Multiply);
	Handler.Register("Divide", Divide);
}

void HandlerMasterFunction::defineGlobal() {
	Global(*this);
}