
#include "script/ScriptedItem.h"

#include "graphics/Math.h"
#include "io/Logger.h"
#include "platform/String.h"
#include "scene/Interactive.h"
#include "script/ScriptEvent.h"
#include "script/ScriptUtils.h"

using std::string;

namespace script {

namespace {

class RepairCommand : public Command {
	
public:
	
	RepairCommand() : Command("repair") { }
	
	Result execute(Context & context) {
		
		string target = context.getLowercase();
		long t = GetTargetByNameTarget(target);
		if(t == -2) {
			t = GetInterNum(context.getIO()); //self
		}
		
		float val = clamp(context.getFloat(), 0.f, 100.f);
		
		if(ValidIONum(t)) {
			ARX_DAMAGES_DurabilityRestore(inter.iobj[t], val);
		}
		
		LogDebug << "repair " << target << ' ' << val;
		
		return Success;
	}
	
	~RepairCommand() { }
	
};

class SetPoisonousCommand : public Command {
	
public:
	
	SetPoisonousCommand() : Command("setpoisonous", ANY_IO) { }
	
	Result execute(Context & context) {
		
		float poisonous = context.getFloat();
		float poisonous_count = context.getFloat();
		
		LogDebug << "setpoisonous " << poisonous << ' ' << poisonous_count;
		
		INTERACTIVE_OBJ * io = context.getIO();
		if(poisonous_count == 0) {
			io->poisonous_count = 0;
		} else {
			ARX_CHECK_SHORT(poisonous);
			ARX_CHECK_SHORT(poisonous_count);
			io->poisonous = static_cast<short>(poisonous);
			io->poisonous_count = static_cast<short>(poisonous_count);
		}
		
		return Success;
	}
	
	~SetPoisonousCommand() { }
	
};

class SetStealCommand : public Command {
	
public:
	
	SetStealCommand() : Command("setsteal", IO_ITEM) { }
	
	Result execute(Context & context) {
		
		string stealvalue = context.getLowercase();
		
		LogDebug << "setsteal " << stealvalue;
		
		INTERACTIVE_OBJ * io = context.getIO();
		if(stealvalue == "off") {
			io->_itemdata->stealvalue = -1;
		} else {
			io->_itemdata->stealvalue = clamp((int)context.getFloatVar(stealvalue), -1, 100);
			if(io->_itemdata->stealvalue == 100) {
				io->_itemdata->stealvalue = -1;
			}
		}
		
		return Success;
	}
	
	~SetStealCommand() { }
	
};

class SetLightCommand : public Command {
	
public:
	
	SetLightCommand() : Command("setlight", IO_ITEM) { }
	
	Result execute(Context & context) {
		
		string lightvalue = context.getLowercase();
		
		LogDebug << "setlight " << lightvalue;
		
		if(lightvalue == "off") {
			context.getIO()->_itemdata->LightValue = -1;
		} else {
			context.getIO()->_itemdata->LightValue = clamp((int)context.getFloatVar(lightvalue), -1, 1);
		}
		
		return Success;
	}
	
	~SetLightCommand() { }
	
};

class SetFoodCommand : public Command {
	
public:
	
	SetFoodCommand() : Command("setfood", IO_ITEM) { }
	
	Result execute(Context & context) {
		
		float food_value = context.getFloat();
		
		LogDebug << "setfood " << food_value;
		
		context.getIO()->_itemdata->food_value = (char)food_value;
		
		return Success;
	}
	
	~SetFoodCommand() { }
	
};

class SetObjectTypeCommand : public Command {
	
public:
	
	SetObjectTypeCommand() : Command("setobjecttype", ANY_IO) { }
	
	Result execute(Context & context) {
		
		bool set = true;
		string options = context.getFlags();
		if(!options.empty()) {
			u64 flg = flags(options);
			set = !(flg & flag('r'));
			if(!flg || (flg & ~flag('r'))) {
				LogWarning << "unexpected flags: setobjecttype " << options;
			}
		}
		
		string type = context.getLowercase();
		if(type.length() != 1) {
			LogWarning << "setobjecttype: expected only one character, got " << type;
		}
		
		LogDebug << "setobjecttype " << type << ' ' << set;
		
		if(!ARX_EQUIPMENT_SetObjectType(*context.getIO(), type, set)) {
			LogWarning << "setobjecttype: unknown object type: " << type;
			return Failed;
		}
		
		return Success;
	}
	
	~SetObjectTypeCommand() { }
	
};

}

void setupScriptedItem() {
	
	ScriptEvent::registerCommand(new RepairCommand);
	ScriptEvent::registerCommand(new SetPoisonousCommand);
	ScriptEvent::registerCommand(new SetStealCommand);
	ScriptEvent::registerCommand(new SetLightCommand);
	ScriptEvent::registerCommand(new SetFoodCommand);
	ScriptEvent::registerCommand(new SetObjectTypeCommand);
	
}

} // namespace script
