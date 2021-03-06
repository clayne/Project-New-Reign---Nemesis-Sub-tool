#include "hkbcombinetransformsmodifier.h"

using namespace std;

namespace combinetransformsmodifier
{
	const string key = "ar";
	const string classname = "hkbCombineTransformsModifier";
	const string signature = "0xfd1f0b79";
}

string hkbcombinetransformsmodifier::GetAddress()
{
	return address;
}

safeStringUMap<shared_ptr<hkbcombinetransformsmodifier>> hkbcombinetransformsmodifierList;
safeStringUMap<shared_ptr<hkbcombinetransformsmodifier>> hkbcombinetransformsmodifierList_E;

void hkbcombinetransformsmodifier::regis(string id, bool isEdited)
{
	isEdited ? hkbcombinetransformsmodifierList_E[id] = shared_from_this() : hkbcombinetransformsmodifierList[id] = shared_from_this();
	isEdited ? hkbmodifierList_E[id] = shared_from_this() : hkbmodifierList[id] = shared_from_this();
	ID = id;
}

void hkbcombinetransformsmodifier::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
	{
		if (line.find("<hkparam name=\"") != string::npos)
		{
			switch (type)
			{
				case 0:
				{
					string output;

					if (readParam("variableBindingSet", line, output))
					{
						variableBindingSet = (isEdited ? hkbvariablebindingsetList_E : hkbvariablebindingsetList)[output];
						++type;
					}

					break;
				}
				case 1:
				{
					if (readParam("userData", line, userData)) ++type;

					break;
				}
				case 2:
				{
					if (readParam("name", line, name)) ++type;

					break;
				}
				case 3:
				{
					if (readParam("enable", line, enable)) ++type;

					break;
				}
				case 4:
				{
					if (readParam("translationOut", line, translationOut)) ++type;

					break;
				}
				case 5:
				{
					if (readParam("rotationOut", line, rotationOut)) ++type;

					break;
				}
				case 6:
				{
					if (readParam("leftTranslation", line, leftTranslation)) ++type;

					break;
				}
				case 7:
				{
					if (readParam("leftRotation", line, leftRotation)) ++type;

					break;
				}
				case 8:
				{
					if (readParam("rightTranslation", line, rightTranslation)) ++type;

					break;
				}
				case 9:
				{
					if (readParam("rightRotation", line, rightRotation)) ++type;

					break;
				}
				case 10:
				{
					if (readParam("invertLeftTransform", line, invertLeftTransform)) ++type;

					break;
				}
				case 11:
				{
					if (readParam("invertRightTransform", line, invertRightTransform)) ++type;

					break;
				}
				case 12:
				{
					if (readParam("invertResult", line, invertResult)) ++type;
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << combinetransformsmodifier::classname + " (ID: " << ID << ") is complete!" << endl;
	}
}

void hkbcombinetransformsmodifier::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::unique_lock<boost::mutex> curLock(nodeMutex);
	address = preaddress + combinetransformsmodifier::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			RecordID(ID, address);
			string curadd = address;		// protect changing address upon release lock
			curLock.unlock();

			if (variableBindingSet) variableBindingSet->connect(filepath, curadd, functionlayer + 1, false, graphroot);
		}
		else
		{
			// existed
			if (IsOldFunction(filepath, shared_from_this(), address))
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbcombinetransformsmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbcombinetransformsmodifierList_E[ID] = protect;
				hkbmodifierList_E[ID] = protect;
				editedBehavior[ID] = protect;
				RecordID(ID, address, true);
				nextNode(filepath, functionlayer, true, graphroot);
			}
			// newly created
			else
			{
				IsForeign[ID] = true;
				address = preaddress;
				RecordID(ID, address, true);
				nextNode(filepath, functionlayer, false, graphroot);
			}
		}
	}
	else if (!compare)
	{
		CrossReferencing(shared_from_this(), address, functionlayer, compare);
	}
	else if (IsForeign.find(ID) != IsForeign.end())
	{
		string newID = CrossReferencing(shared_from_this(), address, functionlayer, compare);

		// comparing
		if (compare)
		{
			if (ID != newID)
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto foreign_itr = IsForeign.find(ID);

				if (foreign_itr != IsForeign.end()) IsForeign.erase(foreign_itr);

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbcombinetransformsmodifierList_E.erase(ID);
				hkbmodifierList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbcombinetransformsmodifierList_E[ID] = protect;
				hkbmodifierList_E[ID] = protect;
				editedBehavior[ID] = protect;
				RecordID(ID, address, true);
				nextNode(filepath, functionlayer, true, graphroot);
			}
			else
			{
				address = preaddress;
				nextNode(filepath, functionlayer, false, graphroot);
			}
		}
	}
	else if (IsForeign.find(hkb_parent[shared_from_this()]->ID) != IsForeign.end())
	{
		if (count(address.begin(), address.end(), '>') == 3)
		{
			if (address.find("(cj", 0) != string::npos || address.find("(i", 0) != string::npos)
			{
				IsOldFunction(filepath, shared_from_this(), address);
			}
		}
	}
}

string hkbcombinetransformsmodifier::getClassCode()
{
	return combinetransformsmodifier::key;
}

void hkbcombinetransformsmodifier::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(15);
	output.reserve(15);
	usize base = 2;
	hkbcombinetransformsmodifier* ctrpart = static_cast<hkbcombinetransformsmodifier*>(counterpart.get());

	output.push_back(openObject(base, ID, combinetransformsmodifier::classname, combinetransformsmodifier::signature));		// 1
	paramMatch("variableBindingSet", variableBindingSet, ctrpart->variableBindingSet, output, storeline, base, false, open, isEdited);
	paramMatch("userData", userData, ctrpart->userData, output, storeline, base, false, open, isEdited);
	paramMatch("name", name, ctrpart->name, output, storeline, base, false, open, isEdited);
	paramMatch("enable", enable, ctrpart->enable, output, storeline, base, false, open, isEdited);
	paramMatch("translationOut", translationOut, ctrpart->translationOut, output, storeline, base, false, open, isEdited);
	paramMatch("rotationOut", rotationOut, ctrpart->rotationOut, output, storeline, base, false, open, isEdited);
	paramMatch("leftTranslation", leftTranslation, ctrpart->leftTranslation, output, storeline, base, false, open, isEdited);
	paramMatch("leftRotation", leftRotation, ctrpart->leftRotation, output, storeline, base, false, open, isEdited);
	paramMatch("rightTranslation", rightTranslation, ctrpart->rightTranslation, output, storeline, base, false, open, isEdited);
	paramMatch("rightRotation", rightRotation, ctrpart->rightRotation, output, storeline, base, false, open, isEdited);
	paramMatch("invertLeftTransform", invertLeftTransform, ctrpart->invertLeftTransform, output, storeline, base, false, open, isEdited);
	paramMatch("invertRightTransform", invertRightTransform, ctrpart->invertRightTransform, output, storeline, base, false, open, isEdited);
	paramMatch("invertResult", invertResult, ctrpart->invertResult, output, storeline, base, true, open, isEdited);
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", combinetransformsmodifier::classname, output, isEdited);
}

void hkbcombinetransformsmodifier::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(15);

	output.push_back(openObject(base, ID, combinetransformsmodifier::classname, combinetransformsmodifier::signature));		// 1
	output.push_back(autoParam(base, "variableBindingSet", variableBindingSet));
	output.push_back(autoParam(base, "userData", userData));
	output.push_back(autoParam(base, "name", name));
	output.push_back(autoParam(base, "enable", enable));
	output.push_back(autoParam(base, "translationOut", translationOut));
	output.push_back(autoParam(base, "rotationOut", rotationOut));
	output.push_back(autoParam(base, "leftTranslation", leftTranslation));
	output.push_back(autoParam(base, "leftRotation", leftRotation));
	output.push_back(autoParam(base, "rightTranslation", rightTranslation));
	output.push_back(autoParam(base, "rightRotation", rightRotation));
	output.push_back(autoParam(base, "invertLeftTransform", invertLeftTransform));
	output.push_back(autoParam(base, "invertRightTransform", invertRightTransform));
	output.push_back(autoParam(base, "invertResult", invertResult));
	output.push_back(closeObject(base));		// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, combinetransformsmodifier::classname, output, true);
}

void hkbcombinetransformsmodifier::parentRefresh()
{
	if (variableBindingSet) hkb_parent[variableBindingSet] = shared_from_this();
}

void hkbcombinetransformsmodifier::nextNode(string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot)
{
	if (isOld) ++functionlayer;

	if (variableBindingSet)
	{
		parentRefresh();
		variableBindingSet->connect(filepath, address, functionlayer, true, graphroot);
	}
}
