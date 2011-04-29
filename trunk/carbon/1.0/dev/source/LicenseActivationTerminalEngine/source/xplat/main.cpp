

#include "Constants.h"
#include "Log.h"
#include "MachineIdManager.h"

int main()
{

	::CoInitialize(NULL);

	MachineIdMgr _obj;
	std::string temp;
	_obj.generateMachineId(temp);

	bool ret = _obj.validateMachineId(temp);

	int i;
	cout<<"hello world";
//	cin>>i;

	return 0;
}

