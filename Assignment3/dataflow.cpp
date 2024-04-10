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
        BitVector empty(size, false);

        for (BasicBlock &BB : F)
        {
            struct BlockInfo *temp_info = new BlockInfo();

            temp_info->block = &BB;
            temp_info->input = empty;
            temp_info->output = empty;
            temp_info->genSet = empty;
            temp_info->killSet = empty;
            temp_info->tfOut = empty;

            GetPrevBlocks(&BB, temp_info);
            GetNextBlocks(&BB, temp_info);

            BasicBlock &entryblock = F.getEntryBlock();
            if (&BB == &entryblock)
            {
                temp_info->btype = ENTRY;
            }
            else
            {
                temp_info->btype = MIDDLE;
            }

            for (Instruction &intsr : BB)
            {
                Instruction *I = &instr;
                if (dyn_cast<ReturnInst>(I))
                {
                temp_info->btype = EXIT;
                }
            }
            BlockInfoMap[&BB] = temp_info;
        }

    }

    void DataFlow::InitializeBoundary(Function &F)
    {
        std::map<BasicBlock*, BBAttributes*>::iterator i;

        struct BlockInfo *tmp;
        for (i = BlockInfoMap.begin(); i != BlockInfoMap.end(); i++)
        {
            tmp = BlockInfoMap[i->first];
            if (direction == FORWARD)
            {
                if (tmp->BBType == ENTRY)
                {
                    tmp->input = boundary;
                }
                tmp->output = initial;
            }
            else if (direction == BACKWARD)
            {
                if (tmp->BBType == EXIT)
                {
                    tmp->output = boundary;
                }
                tmp->input = initial;
            }
        }
    }


}