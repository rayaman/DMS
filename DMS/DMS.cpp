// DMS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
//#include <string>
//#include <exception>
#include "value.h"
#include "codes.h"
#include "cmd.h"
#include "dms_exceptions.h"
#include "errors.h"
#include "utils.h"
#include "number_utils.h"
#include "string_utils.h"
#include "LineParser.h"
#include "dms.h"
#include "chunk.h"
#include "token.h"
using namespace dms;
using namespace dms::utils;
int main()
{
    LineParser parser = LineParser("test.dms");
    dms_state* state = parser.Parse();
    state->dump();
}