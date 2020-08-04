// DMS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <exception>
#include "dms.h"
using namespace dms;
using namespace dms::utils;

int main()
{
    /*
        TODO

        Work on LineParser, chunk and state
        We need to parse the dms file.
        Might need to rework the syntax a bit 
    */
    LineParser parser = LineParser("test.dms");
    parser.Parse();
    print("Goodbye!");
}