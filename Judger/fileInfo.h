#pragma once

#include "stdafx.h"
#include "program.h"

extern const string infoFile;

bool CheckFile(const string &file);
void InitFile(Program *&prog, Program *&force, string &inputFile, string &outputFile);