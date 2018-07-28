#ifndef BSRAGDOLLCONTACTLISTENERMODIFIER_H_
#define BSRAGDOLLCONTACTLISTENERMODIFIER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

struct bsragdollcontactlistenermodifier
{
public:
	bsragdollcontactlistenermodifier(std::string filepath, std::string id, std::string preaddress, int functionlayer, bool compare);

	void nonCompare(std::string filepath, std::string id);
	void Compare(std::string filepath, std::string id);
	void Dummy(std::string id);

	std::string GetBone();
	bool IsBoneNull();

	std::string GetPayload();
	bool IsPayloadNull();

	std::string GetVariableBindingSet();
	bool IsBindingNull();

	std::string GetAddress();
	bool IsNegate();

private:
	std::string address;
	std::string tempaddress;
	std::string payload;
	std::string variablebindingset;
	std::string bones;
	bool IsNegated = false;
};

extern void BSRagdollContactListenerModifierExport(std::string id);

#endif