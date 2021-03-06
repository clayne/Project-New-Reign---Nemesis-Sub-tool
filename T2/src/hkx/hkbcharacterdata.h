#ifndef HKBCHARACTERDATA_H_
#define HKBCHARACTERDATA_H_

#include <iostream>
#include <fstream>
#include <cstdio>

#include "src\utilities\coordinate.h"
#include "src\utilities\variableinfopack.h"

#include "src\hkx\hkbobject.h"

struct hkbvariablevalueset;
struct hkbfootikdriverinfo;
struct hkbhandikdriverinfo;
struct hkbcharacterstringdata;
struct hkbmirroredskeletoninfo;

struct hkpcharactercontrollercinfo : public hkbobject, std::enable_shared_from_this<hkpcharactercontrollercinfo>
{
	hkpcharactercontrollercinfo() {}

	void regis(std::string id, bool isEdited);
};

extern safeStringUMap<std::shared_ptr<hkpcharactercontrollercinfo>> hkpcharactercontrollercinfoList;
extern safeStringUMap<std::shared_ptr<hkpcharactercontrollercinfo>> hkpcharactercontrollercinfoList_E;

struct hkbcharacterdata : public hkbobject, std::enable_shared_from_this<hkbcharacterdata>
{
public:
	hkbcharacterdata() {}

	std::string GetAddress();

	std::string tempaddress;

	struct charactercontrollerinfo
	{
		double capsuleHeight;
		double capsuleRadius;
		int collisionFilterInfo;
		std::shared_ptr<hkpcharactercontrollercinfo> characterControllerCinfo;
	};

	charactercontrollerinfo characterControllerInfo;
	coordinate modelUpMS;
	coordinate modelForwardMS;
	coordinate modelRightMS;
	std::shared_ptr<variableinfopack> characterPropertyInfos;
	std::vector<int> numBonesPerLod;
	std::shared_ptr<hkbvariablevalueset> characterPropertyValues;
	std::shared_ptr<hkbfootikdriverinfo> footIkDriverInfo;
	std::shared_ptr<hkbhandikdriverinfo> handIkDriverInfo;
	std::shared_ptr<hkbcharacterstringdata> stringData;
	std::shared_ptr<hkbmirroredskeletoninfo> mirroredSkeletonInfo;
	double scale;

	void regis(std::string id, bool isEdited);
	void dataBake(std::string filepath, vecstr& nodelines, bool isEdited);
	void connect(std::string filepath, std::string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot);
	std::string getClassCode();
	void match(std::shared_ptr<hkbobject> counterpart);
	void newNode();

private:
	void parentRefresh();
	void nextNode(std::string filepath, int functionlayer, bool isOld, hkbbehaviorgraph* graphroot);
	void threadedNextNode(std::shared_ptr<hkbobject> hkb_obj, std::string filepath, std::string address, int functionlayer, hkbbehaviorgraph* graphroot);
};

extern safeStringUMap<std::shared_ptr<hkbcharacterdata>> hkbcharacterdataList;
extern safeStringUMap<std::shared_ptr<hkbcharacterdata>> hkbcharacterdataList_E;

#endif
