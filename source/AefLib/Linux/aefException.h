/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   aefException.h
 * Author: yackey
 *
 * Created on August 31, 2017, 3:45 PM
 */

#ifndef AEFEXCEPTION_H
#define AEFEXCEPTION_H

#include <stdexcept>

class CAefException : public std::runtime_error
{
public:
    CAefException();
    CAefException(const char* );
    CAefException(const char*, const int);
    virtual const char* what();
    virtual ~CAefException() throw();
    
private:
    std::string m_sMsg;

};

#endif /* AEFEXCEPTION_H */

