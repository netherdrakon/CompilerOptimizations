////////////////////////////////////////////////////////////////////////////////

#ifndef __CLASSICAL_DATAFLOW_H__
#define __CLASSICAL_DATAFLOW_H__

// #include <stdio.h>
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

namespace llvm {

    typedef struct BlockInfo{
        int blockindex;
        BasicBlock* currBlock;
        BitVector inB;
        BitVector outB;
        BitVector genSet;
        BitVector killSet;
        BitVector tfOut;
        // BitVector boundaryVector;
        std::vector<BasicBlock*> BlockPredecessors;
        std::vector<BasicBlock*> BlockSuccessors;


    }BlockInfo;

    class Expression {
        public:
        Value * v1;
        Value * v2;
        Instruction::BinaryOps op;
        Expression (Instruction * I);
        Expression ();
        bool operator== (const Expression &e2) const;
        bool operator< (const Expression &e2) const;
        std::string toString() const;
        Instruction *instr; //to store instruction for cloning
    };

    enum Direction{
        FORWARD,
        BACKWARD
    };
    enum MeetOperator{
        UNION,
        INTERSECTION
    };
    
    class DataFlow{
        private:
        BitVector boundary;
        enum Direction direction;
        enum MeetOperator meetop;
        int instrListSize;
        int blockListSize;

        public:
        std::vector<BasicBlock*> MainBlockList;
        std::vector<void*> InstrList;
        std::map<BasicBlock*, BlockInfo*> MainBlockMap;
        DataFlow(BitVector i_boundary, std::vector<void*> i_InstrVector, enum Direction i_direction, enum MeetOperator i_meetop, int i_blocklistsize, int i_instrlistsize){
            boundary = i_boundary;
            InstrVector = i_InstrVector;
            direction = i_direction;
            meetop = i_meetop;
            instrListSize = i_instrlistsize;
            blockListSize = i_blocklistsize;
            
        }

        std::vector<BitVector> getBlockOutputs(BlockInfo* temp);
        BitVector meetFunction(std::vector<BitVector> blockoutputs);
        void InitializeBlocks(Function &F);
        virtual BlockInfo transferFunction(BitVector input, BasicBlock* block);
        std::map<BasicBlock*, BlockInfo*> DFAnalyze(Function &F);

    }


}
#endif