
////////////////////////////////////////////////////////////////////////////////

#ifndef __CLASSICAL_DATAFLOW_H__
#define __CLASSICAL_DATAFLOW_H__

#include <stdio.h>
#include <iostream>
#include <queue>
#include <vector>

#include "llvm/IR/Instructions.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/ValueMap.h"
#include "llvm/IR/CFG.h"

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/ADT/PostOrderIterator.h"
#include <algorithm>
#include <stack>
#include <map>
#include <string>

namespace llvm{

    enum MeetOperator
	{
		UNION,
	    INTERSECTION
	};
	enum Direction
	{	        	
		FORWARD,
    	BACKWARD	    	
	};
    enum BlockType
    {
        ENTRY,
        MIDDLE,
        EXIT
    }


    struct BlockInfo{
        enum BlockType btype;
        BitVector input;
        BitVector output;
        BitVector genSet;
        BitVector killSet;
        BitVector tfOut;
        std::vector<BasicBlock*> prevblock;
        std::vector<BasicBlock*> nextblock;
    }

    class DataFlow
    {
        private:
            int size;
            BitVector boundary;
            BitVector initial;
            enum MeetOp meetOp;
            enum Direction direction;
            std::vector<BasicBlock*> instr_forward;
            std::vector<BasicBlock*> instr_back;
        
        
        public:
            std::vector<void*> domain;
            std::map<BasicBlock*, BlockInfo*> BlockInfoMap;
            std::map<BasicBlock*, std::vector<llvm::BitVector>*> BlockInstrMap;


            DataFlow(int size, std::vector<void*> dom, enum MeetOperator m, enum Direction dir, BitVector b, BitVector in)
            {
                size = size;
                domain = dom;
                meetOp = m;
                direction = dir;
                boundary = b;
                initial = in;
            }

            void GetPrevBlocks(BasicBlock* currblock, struct BlockInfo* BInfo);
            void GetNextBlocks(BasicBlock* currblock, struct BlockInfo* BInfo);




            void InitializeBlocks(Function &F);


    }

    

}