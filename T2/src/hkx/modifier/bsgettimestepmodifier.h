#ifndef BSGETTIMESTEPMODIFIER_H_
#define BSGETTIMESTEPMODIFIER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "src\hkx\hkbobject.h"
#include "src\hkx\modifier\hkbmodifier.h"

struct bsgettimestepmodifier : public hkbmodifier, std::enable_shared_from_this<bsgettimestepmodifier>
{
public:
	bsgettimestepmodifier() {}

	std::string GetAddress();

	std::string tempaddress;

	// variableBindingSet from hkbmodifier
	// userData from hkbmodifier
	// name from hkbmodifier
	// enable from hkbmodifier
	double timeStep;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
};

extern safeStringUMap<std::shared_ptr<bsgettimestepmodifier>> bsgettimestepmodifierList;
extern safeStringUMap<std::shared_ptr<bsgettimestepmodifier>> bsgettimestepmodifierList_E;

#endif