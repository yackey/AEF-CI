/* 
 * File:   aefException.cpp
 * Author: yackey
 *
 * Created on August 31, 2017, 3:45 PM
 */

#include "aefException.h"
#include <sstream>

CAefException::CAefException()
    : std::runtime_error("")
{
    m_sMsg = "no message provided";
}

CAefException::CAefException(const char* cpMessage)
    : std::runtime_error("")
{
    std::stringstream ss;
    ss << cpMessage << ": no error value provided";
    m_sMsg = ss.str();    
}

CAefException::CAefException(const char* cpMessage, const int nVal)
    : std::runtime_error("")
{
    std::stringstream ss;
    ss << cpMessage << ": " << nVal;
    m_sMsg = ss.str();
}

const char* CAefException::what() const throw()
{
    return m_sMsg.c_str();
}

CAefException::~CAefException() throw()
{

}
