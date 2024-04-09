////////////////////////////////////////////////////////////////////////////////
//ECE5544: Compiler Optimization
//Group : Akhilesh Marathe, Alexander Owens
////////////////////////////////////////////////////////////////////////////////

#include "dataflow.h"


namespace llvm {

    void DataFlow::GetPrevBlocks(BasicBlock* currblock, struct BlockInfo* BInfo)
    {
        for (BasicBlock *pred : predecessors(currblock))
        {
            BInfo->prevblock.push_back(pred);
        }
    }

    void DataFlow::GetNextBlocks(BasicBlock* currblock, struct BlockInfo* BInfo)
    {
        for (BasicBlock *next : successors(currblock))
        {
            BInfo->nextblock.push_back(next);
        }
    }



    void DataFlow::InitializeBlocks(Function &F)
    {

    }


}