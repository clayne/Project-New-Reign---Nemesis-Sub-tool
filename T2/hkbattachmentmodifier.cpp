#include "hkbattachmentmodifier.h"
#include "Global.h"

using namespace std;

hkbattachmentmodifier::hkbattachmentmodifier(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + "ap" + to_string(functionlayer) + ">";

	if ((!IsExist[id]) && (!Error))
	{
		if (compare)
		{
			Compare(filepath, id);
		}
		else
		{
			nonCompare(filepath, id);
		}
	}
	else if (!Error)
	{
		bool statusChange = false;

		if (IsForeign[id])
		{
			statusChange = true;
		}

		string dummyID = CrossReferencing(id, address, functionlayer, compare);

		if (compare)
		{
			if (statusChange)
			{
				Dummy(dummyID);
			}

			if (IsForeign[id])
			{
				address = preaddress;
			}
			else if (!statusChange)
			{
				IsNegated = true;
			}
		}
		else
		{
			IsNegated = true;
		}
	}
	else
	{
		return;
	}
}

void hkbattachmentmodifier::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbAttachmentModifier(ID: " << id << ") has been initialized!" << endl;
	}

	string line;
	payload.reserve(4);

	if (!FunctionLineOriginal[id].empty())
	{
		usize size = FunctionLineOriginal[id].size();

		for (usize i = 0; i < size; ++i)
		{
			line = FunctionLineOriginal[id][i];

			if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
			{
				variablebindingset = line.substr(38, line.find("</hkparam>") - 38);

				if (variablebindingset != "null")
				{
					referencingIDs[variablebindingset].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(29, line.find("</hkparam>") - 29));

				if (payload.back() != "null")
				{
					referencingIDs[payload.back()].push_back(id);
				}
			}
		}
	}
	else
	{
		cout << "ERROR: hkbAttachmentModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	FunctionLineTemp[id] = FunctionLineOriginal[id];
	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbAttachmentModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbattachmentmodifier::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbAttachmentModifier(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vector<string> newline;
	string line;
	payload.reserve(4);

	if (!FunctionLineEdited[id].empty())
	{
		usize size = FunctionLineEdited[id].size();

		for (usize i = 0; i < size; ++i)
		{
			line = FunctionLineEdited[id][i];

			if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
			{
				variablebindingset = line.substr(38, line.find("</hkparam>") - 38);

				if (variablebindingset != "null")
				{
					if (!exchangeID[variablebindingset].empty())
					{
						int tempint = line.find(variablebindingset);
						variablebindingset = exchangeID[variablebindingset];
						line.replace(tempint, line.find("</hkparam>") - tempint, variablebindingset);
					}

					parent[variablebindingset] = id;
					referencingIDs[variablebindingset].push_back(id);
				}
			}
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(29, line.find("</hkparam>") - 29));

				if (payload.back() != "null")
				{
					if (!exchangeID[payload.back()].empty())
					{
						int tempint = line.find(payload.back());
						payload.back() = exchangeID[payload.back()];
						line.replace(tempint, line.find("</hkparam>") - tempint, payload.back());
					}

					parent[payload.back()] = id;
					referencingIDs[payload.back()].push_back(id);
				}
			}

			newline.push_back(line);
		}
	}
	else
	{
		cout << "ERROR: hkbAttachmentModifier Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2
	if (IsOldFunction(filepath, id, address)) // is this new function or old
	{
		IsForeign[id] = false;
		string tempid;

		if (addressChange.find(address) != addressChange.end())
		{
			tempaddress = addressChange[address];
			addressChange.erase(addressChange.find(address));
			address = tempaddress;
		}

		tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbAttachmentModifier(newID: " << id << ") with hkbAttachmentModifier(oldID: " << tempid << ")" << endl;
		}

		if (variablebindingset != "null")
		{
			referencingIDs[variablebindingset].pop_back();
			referencingIDs[variablebindingset].push_back(tempid);
			parent[variablebindingset] = tempid;
		}

		for (unsigned int i = 0; i < payload.size(); i++)
		{
			if (payload[i] != "null")
			{
				referencingIDs[payload[i]].pop_back();
				referencingIDs[payload[i]].push_back(tempid);
				parent[payload[i]] = tempid;
			}
		}

		{
			vector<string> emptyVS;
			FunctionLineNew[tempid] = emptyVS;
		}

		FunctionLineNew[tempid].push_back(FunctionLineTemp[tempid][0]);

		for (unsigned int i = 1; i < newline.size(); i++)
		{
			FunctionLineNew[tempid].push_back(newline[i]);
		}

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbAttachmentModifier(newID: " << id << ") with hkbAttachmentModifier(oldID: " << tempid << ") is complete!" << endl;
		}
	}
	else
	{
		IsForeign[id] = true;
		FunctionLineNew[id] = newline;
		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbAttachmentModifier(ID: " << id << ") is complete!" << endl;
	}
}

void hkbattachmentmodifier::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbAttachmentModifier(ID: " << id << ") has been initialized!" << endl;
	}

	payload.reserve(4);
	string line;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (line.find("<hkparam name=\"variableBindingSet\">", 0) != string::npos)
			{
				variablebindingset = line.substr(38, line.find("</hkparam>") - 38);

				if (variablebindingset != "null")
				{
					if (!exchangeID[variablebindingset].empty())
					{
						variablebindingset = exchangeID[variablebindingset];
					}

					parent[variablebindingset] = id;
				}
			}
			else if (line.find("<hkparam name=\"payload\">", 0) != string::npos)
			{
				payload.push_back(line.substr(29, line.find("</hkparam>") - 29));

				if (payload.back() != "null")
				{
					if (!exchangeID[payload.back()].empty())
					{
						payload.back() = exchangeID[payload.back()];
					}

					parent[payload.back()] = id;
				}
			}
		}
	}
	else
	{
		cout << "ERROR: Dummy hkbAttachmentModifier Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbAttachmentModifier(ID: " << id << ") is complete!" << endl;
	}
}

int hkbattachmentmodifier::GetPayloadCount()
{
	return int(payload.size());
}

string hkbattachmentmodifier::GetPayload(int child)
{
	return "#" + boost::regex_replace(string(payload[child]), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbattachmentmodifier::IsPayloadNull(int child)
{
	if (payload[child].find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbattachmentmodifier::GetVariableBindingSet()
{
	return "#" + boost::regex_replace(string(variablebindingset), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
}

bool hkbattachmentmodifier::IsBindingNull()
{
	if (variablebindingset.find("null", 0) != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

string hkbattachmentmodifier::GetAddress()
{
	return address;
}

bool hkbattachmentmodifier::IsNegate()
{
	return IsNegated;
}

void hkbAttachmentModifierExport(string id)
{
	//stage 1 reading
	vector<string> storeline1;
	storeline1.reserve(FunctionLineTemp[id].size());
	string line;

	if (FunctionLineTemp[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			line = FunctionLineTemp[id][i];

			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline1.push_back(line);
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbAttachmentModifier Input Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	//stage 2 reading and identifying edits
	vector<string> storeline2;
	storeline2.reserve(FunctionLineNew[id].size());
	bool open = false;
	bool IsEdited = false;
	int curline = 0;
	int openpoint;
	int closepoint;

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				if ((line.find("sendTo", 0) != string::npos) || (line.find("attachmentSetup", 0) != string::npos))
				{
					if (open)
					{
						closepoint = curline;

						if (closepoint != openpoint)
						{
							storeline2.push_back("<!-- ORIGINAL -->");

							for (int j = openpoint; j < closepoint; j++)
							{
								storeline2.push_back(storeline1[j]);
							}
						}

						storeline2.push_back("<!-- CLOSE -->");
						open = false;
					}

					if (line.find("attachmentSetup", 0) != string::npos)
					{
						if ((line.find(storeline1[curline], 0) != string::npos) && (line.length() == storeline1[curline].length()))
						{
							if (open)
							{
								closepoint = curline;

								if (closepoint != openpoint)
								{
									storeline2.push_back("<!-- ORIGINAL -->");

									for (int j = openpoint; j < closepoint; j++)
									{
										storeline2.push_back(storeline1[j]);
									}
								}

								storeline2.push_back("<!-- CLOSE -->");
								open = false;
							}
						}
						else
						{
							if (!open)
							{
								storeline2.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
								openpoint = curline;
								open = true;
							}

							IsEdited = true;
						}

						storeline2.push_back(line);
					}
					else
					{
						storeline2.push_back(storeline1[curline]);
					}
				}
				else
				{
					if ((line.find(storeline1[curline], 0) != string::npos) && (line.length() == storeline1[curline].length()))
					{
						if (open)
						{
							closepoint = curline;

							if (closepoint != openpoint)
							{
								storeline2.push_back("<!-- ORIGINAL -->");

								for (int j = openpoint; j < closepoint; j++)
								{
									storeline2.push_back(storeline1[j]);
								}
							}

							storeline2.push_back("<!-- CLOSE -->");
							open = false;
						}
					}
					else
					{
						if (!open)
						{
							storeline2.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							openpoint = curline;
							open = true;
						}

						IsEdited = true;
					}

					storeline2.push_back(line);
				}

				curline++;
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbAttachmentModifier Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	NemesisReaderFormat(storeline2, true);

	if (open) // close unclosed edits
	{
		closepoint = curline;

		if (closepoint != openpoint)
		{
			storeline2.push_back("<!-- ORIGINAL -->");

			for (int j = openpoint; j < closepoint; j++)
			{
				storeline2.push_back(storeline1[j]);
			}
		}

		storeline2.push_back("<!-- CLOSE -->");
		open = false;
	}

	// stage 3 output if it is edited
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";

	if (IsEdited)
	{
		ofstream output(filename);

		if (output.is_open())
		{
			FunctionWriter fwrite(&output);

			for (unsigned int i = 0; i < storeline2.size(); i++)
			{
				if ((storeline2[i].find("OPEN", 0) != string::npos) && ((storeline2[i + 1].find("sendTo", 0) != string::npos) || (storeline2[i + 1].find("attachmentSetup", 0) != string::npos)))
				{
					fwrite << "				</hkobject>" << "\n";
					fwrite << "			</hkparam>" << "\n";
				}

				fwrite << storeline2[i] << "\n";

				if (storeline2[i].find("sendTo", 0) != string::npos)
				{
					fwrite << "				<hkobject>" << "\n";
				}

				if (i < storeline2.size() - 1)
				{
					if (((storeline2[i].find("OPEN", 0) == string::npos) && (storeline2[i].find("ORIGINAL", 0) == string::npos)) && (((storeline2[i + 1].find("sendTo", 0) != string::npos) && (storeline2[i + 1].find("sendToAttacherOnAttach", 0) == string::npos)) || (storeline2[i + 1].find("attachmentSetup", 0) != string::npos)))
					{
						fwrite << "				</hkobject>" << "\n";
						fwrite << "			</hkparam>" << "\n";
					}
				}
			}

			fwrite << "		</hkobject>" << "\n";
			output.close();
		}
		else
		{
			cout << "ERROR: Edit hkbAttachmentModifier Output Not Found (File: " << filename << ")" << endl;
			Error = true;
		}
	}
	else
	{
		if (IsFileExist(filename))
		{
			if (remove(filename.c_str()) != 0)
			{
				perror("Error deleting file");
				Error = true;
			}
		}
	}
}