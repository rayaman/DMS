// DMS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <exception>
#include <algorithm>
#include "dms.h"
using namespace dms;
using namespace dms::utils;
std::string random_string(size_t length)
{
    auto randchar = []() -> char
    {
        const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

int main()
{
    dms_string* test;
    test = buildString("Hello world!");
    std::cout << test->getValue() << std::endl;
    dms_string* test3 = string_utils::reverse(*test);
    std::cout << test3->getValue() << std::endl;
    try {
        dms_string* test2 = string_utils::sub(*test, 6, 6);
        std::cout << test2->getValue() << std::endl;
    } catch (std::exception e) {
        std::cout << e.what();
    }
    dms_string* test4 = dms::buildString("Hello world!");
    test4 = string_utils::upper(*test4);
    print(test4->getValue());
    test4 = string_utils::lower(*test4);
    print(test4->getValue());
    dms_string* test5 = buildString("Hi folks!");
    dms_string* test6 = buildString("Hi");
    dms_string* test7 = buildString("folks!");
    dms_string* test8 = buildString("fo!");
    print("Test Starts with");
    print(string_utils::startsWith(*test5, *test6)->getValue());
    print(string_utils::startsWith(*test5, *test7)->getValue());
    print("Test Ends with");
    print(string_utils::endsWith(*test5, *test7)->getValue());
    print(string_utils::endsWith(*test5, *test6)->getValue());
    print("Index of");
    print(string_utils::indexOf(*test5, *test6)->getValue());
    print(string_utils::indexOf(*test5, *test7)->getValue());
    print(string_utils::indexOf(*test5, *test8)->getValue());
    print(string_utils::indexOf(*test4, *test7)->getValue());
    print("Includes");
    print(string_utils::includes(*test5, *test6)->getValue());
    print(string_utils::includes(*test8, *test6)->getValue());
    dms_string* test9 = buildString("Hello");
    print(string_utils::repeat(*test9,*buildNumber(5))->getValue());


    dms_number* num = dms::buildNumber(12345);
    value val = value{};
    value v_n = value{};
    value v_s = value{};
    print("Val: ", val);
    val.set(test4);
    print("Val: ",val);
    val.set(num);
    print("Val: ", val);
    v_n.set(num);
    v_s.set(buildString("Hello World!!!"));

    dms_env* env = new dms_env{};
    print("Huh: ",v_n," | ",v_s);
    env->pushValue(v_n,v_s);
    value v = env->getValue(v_n);
    print("ENV: ",v);

    for (int i = 0; i < 100; i++) {
        value* tempval = new value{};
        tempval->set(buildString(random_string(8)));
        env->pushValue(*tempval);
    }
    for (auto const& x : env->ipart)
    {
        print(x.first, ':', x.second);
    }
    dms_args args = dms_args{};
    args.args.push_back(v_n);
    args.args.push_back(v_s);
    print("Assertion: ", utils::typeassert(args, number, string));
    print("Assertion: ", utils::typeassert(args,number,number));
    print("Goodbye!");
}