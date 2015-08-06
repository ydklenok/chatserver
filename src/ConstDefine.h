#ifndef __CONST_DEFINE_H_H__
#define __CONST_DEFINE_H_H__

#include <string>
#include "StructUtil.h"

#define LEAF_READ_SIZE_MAX (2048)

using namespace std;

typedef void (*ProcessRoutine)(BufferEvent *bev, string &content);

#endif