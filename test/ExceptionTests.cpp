/*
 * ExceptionTests.cpp
 *
 *  Created on: March 29, 2018
 *      Author: yackey
 */

#include <stdio.h>
#include <string>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gtest/gtest-spi.h>

#include "aefException.h"

TEST(ExceptionTest, Create_WithNoMessageNoValue_ReturnDefaultMessage)
{
    std::string sExceptionMessage("no error value provided");
	CAefException e(sExceptionMessage.c_str());
    try
    {
        throw e;
    } catch (CAefException& ex)
    {
        std::string sWhat = ex.what();
        std::size_t found = sWhat.find(sExceptionMessage);
        ASSERT_TRUE(found!=std::string::npos);
    }
}

TEST(ExceptionTest, Create_WithMessageNoValue_ReturnMessage)
{
    std::string sExceptionMessage("test exception message");
	CAefException e(sExceptionMessage.c_str());
    try
    {
        throw e;
    } catch (CAefException& ex)
    {
        std::string sWhat = ex.what();
        std::size_t found = sWhat.find(sExceptionMessage);
        ASSERT_TRUE(found!=std::string::npos);
    }
}


TEST(ExceptionTest, Create_WithNoMessageNoValue_ReturnDefaultMessageWithValue)
{
    std::string sExceptionMessage("1234");
	CAefException e(sExceptionMessage.c_str());
    try
    {
        throw e;
    } catch (CAefException& ex)
    {
        std::string sWhat = ex.what();
        std::size_t found = sWhat.find(sExceptionMessage);
        ASSERT_TRUE(found!=std::string::npos);
    }
}

TEST(ExceptionTest, Create_WithMessageWithValue_ReturnMessageWithValue)
{
    std::string sExceptionMessage("5678");
	CAefException e(sExceptionMessage.c_str());
    try
    {
        throw e;
    } catch (CAefException& ex)
    {
        std::string sWhat = ex.what();
        std::size_t found = sWhat.find(sExceptionMessage);
        ASSERT_TRUE(found!=std::string::npos);
    }
}


