/*******************************************************************************
*  FILENAME: iActiveObject.h
*
*  DESCRIPTION: Интерфейс для активных объектов. Каждый активный объект должен
*               наследовать этот интерфейс
*
*  Copyright (c) 2015 by SUSU
*
*******************************************************************************/
#ifndef IACTIVEOBJECT_H
#define IACTIVEOBJECT_H

#include "types.h"          //Стандартные типы проекта

class iActiveObject
{
  public:              
    virtual void run(void) = 0;
    void *taskHandle;
};

#endif //IACTIVEOBJECT_H
