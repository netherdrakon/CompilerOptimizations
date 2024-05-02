#ifndef __AVAILABLE_H__
#define __AVAILABLE_H__

#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "dataflow.h"

using namespace llvm;

class WillBeAvailable : public FunctionPass{
    private:
    std::vector<BasicBlock*, BlockInfo*> willbe_result;

    public:
    


}