#include "dataflow1.h"

namespace llvm{

    BitVector DataFlow::meetFunction(std::vector<BitVector> blockoutputs){
        outs()<<MainBlockMap[BB]->blkcount;
        BitVector meetVector;

        meetVector = blockoutputs[0];
        for(int i = 1; i<blockoutputs.size(); i++){
            if(meetop == UNION){
                meetVector |= blockoutputs[i];
            }
            else if(meetop == INTERSECTION){
                meetVector &= blockoutputs[i];
            }
        }

        return meetVector;

        
    }

    void DataFlow::InitializeBlocks(Function &F){
        BitVector zeroVector(instrListSize, false);
        int blkcount = 0;
        std::stack<BasicBlock*> backwardStack;

        for (Function::iterator B = F.begin(), BE = F.end(); B != BE; ++B) {
            
            BlockInfo *temp = new BlockInfo;

            temp->blockindex = blkcount;
            blkcount++;
            temp->currBlock = B;
            temp->inB = zeroVector;
            temp->outB = zeroVector;
            temp->genSet = zeroVector;
            temp->killSet = zeroVector;

            for(BasicBlock* pred : predecessors(B)){
                temp->BlockPredecessors.push_back(pred);
            }

            for(BasicBlock* succ : successors(B)){
                temp->BlockSuccessors.push_back(succ);
            }

            if(direction == FORWARD){
                if(B==F.getEntryBlock()){
                    outs()<<"Entry Block\n";
                    temp->inB = boundary;
                }
                MainBlockList.push_back(B);
                
            }
            else if(direction == BACKWARD){
                for (BasicBlock::iterator I = B->begin(), IE = B->end(); I != IE; ++I){
                    if(dyn_cast<ReturnInst>(I)){
                        outs()<<"Backwards, Setting Output = Boundary\n";
                        temp->outB = boundary;
                    }
                }
                backwardStack.push(B);
                
            }
        }
        if(direction == BACKWARD){
            while(!backwardStack.empty()){
                MainBlockList.push_back(backwardStack.top());
                backwardStack.pop();
            }
        }
    }

    std::vector<BitVector> DataFlow::getBlockOutputs(BlockInfo* temp){

        std::vector<BitVector> blockoutputs;
        if(direction == FORWARD){        
            for(auto i = temp->BlockPredecessors.begin(), i != temp->BlockPredecessors.end(); ++i){
                    blockoutputs.push_back(temp->outB);
                }
            }
        else if(direction == BACKWARD){
            for(auto i = temp->BlockSuccessors.begin(), i != temp->BlockSuccessors.end(); ++i){
                blockoutputs.push_back(temp->inB);
            }
        }
        return blockoutputs;
    }

    std::map<BasicBlock*, BlockInfo*> DataFlow::DFAnalyze(Function &F){

        InitializeBlocks(F);

        bool done = 0;
        int itercount = 0;
        std::map<BasicBlock*, BlockInfo*> Result;

        while(!done){

            std::vector<BitVector> blockoutputs;
            BitVector meetVector;

            for(BasicBlock* B : MainBlockList){

                BlockInfo *temp = MainBlockMap[B];

                blockoutputs = getBlockOutputs(temp);

                
                if(!blockoutputs.empty()){
                    meetVector = meetFunction(blockoutputs);
                }
                if(direction == FORWARD){
                    temp->inB = meetVector;

                }
                else if(direction == BACKWARD){
                    temp->outB = meetVector;
                }

                BlockInfo tfResult = transferFunction(meetVector, B);

                temp->genSet = tfResult.genSet;
                temp->killSet = tfResult.killSet;
                
                BitVector *Output;
                if(direction == FORWARD){
                    if((temp->outB) == (tfResult.tfOut)){
                        done = 1;
                        temp->outB = tfResult.tfOut;
                    }
                }
                else if(direction == BACKWARD){
                    if((temp->inB) == (tfResult.tfOut)){
                        done = 1;
                        temp->inB = tfResult.tfOut;
                    }
                }

            }
            itercount++;
            outs()<<"Iteration: "<<itercount<<" done\n";

        }

        return BlockInfoMap;


    }


    Expression::Expression (Instruction * I) {
        if (BinaryOperator * BO = dyn_cast<BinaryOperator>(I)) {
            this->v1 = BO->getOperand(0);
            this->v2 = BO->getOperand(1);
            this->op = BO->getOpcode();
        }
        else {
            errs() << "We're only considering BinaryOperators\n";
        }
    }

    Expression::Expression () {
    }

    // For two expressions to be equal, they must
    // have the same operation and operands.
    bool Expression::operator== (const Expression &e2) const {
        return this->v1 == e2.v1 && this->v2 == e2.v2 && this->op == e2.op;
    }

    // Less than is provided here in case you want
    // to use STL maps, which use less than for
    // equality checking by default
    bool Expression::operator< (const Expression &e2) const {
        if (this->v1 == e2.v1) {
            if (this->v2 == e2.v2) {
            if (this->op == e2.op) {
                return false;
            } else {
                return this->op < e2.op;
            }
            } else {
            return this->v2 < e2.v2;
            }
        } else {
            return this->v1 < e2.v1;
        }
    }

      std::string getShortValueName(Value * v) {
    if (v->getName().str().length() > 0) {
      return "%" + v->getName().str();
    }
    else if (isa<Instruction>(v)) {
      std::string s = "";
      raw_string_ostream * strm = new raw_string_ostream(s);
      v->print(*strm);
      std::string inst = strm->str();
      size_t idx1 = inst.find("%");
      size_t idx2 = inst.find(" ",idx1);
      if (idx1 != std::string::npos && idx2 != std::string::npos) {
          return inst.substr(idx1,idx2-idx1);
      }
      else {
        // return "\"" + inst + "\"";
        return "";
      }
    }
    else {
      std::string s = "";
      raw_string_ostream * strm = new raw_string_ostream(s);
      v->print(*strm);
      std::string inst = strm->str();
      return "\"" + inst + "\"";
    }
  }

  // A pretty printer for Expression objects
  // Feel free to alter in any way you like
  std::string Expression::toString() const {
    std::string op = "?";
    switch (this->op) {
    case Instruction::Add:
    case Instruction::FAdd: op = "+"; break;
    case Instruction::Sub:
    case Instruction::FSub: op = "-"; break;
    case Instruction::Mul:
    case Instruction::FMul: op = "*"; break;
    case Instruction::UDiv:
    case Instruction::FDiv:
    case Instruction::SDiv: op = "/"; break;
    case Instruction::URem:
    case Instruction::FRem:
    case Instruction::SRem: op = "%"; break;
    case Instruction::Shl: op = "<<"; break;
    case Instruction::AShr:
    case Instruction::LShr: op = ">>"; break;
    case Instruction::And: op = "&"; break;
    case Instruction::Or: op = "|"; break;
    case Instruction::Xor: op = "xor"; break;
    }
    return getShortValueName(v1) + " " + op + " " + getShortValueName(v2);
  }

  void copyExpression (Expression *e1, Expression *e2) {
    e1->v1 = e2->v1;
    e1->v2 = e2->v2;
    e1->op = e2->op;
  }

  void printSet(std::vector<Expression> * x) {
    bool first = true;
    outs() << "{";

    for (std::vector<Expression>::iterator it=x->begin(), iend=x->end(); it!=iend; ++it) {
      if (!first) {
        outs() << ", ";
      }
      else {
        first = false;
      }
      outs() << (it->toString());
    }
    outs() << "}\n";
  }

  void printSetBB(std::vector<void *> * x) {
    bool first = true;
    outs() << "{";

    for (std::vector<void *>::iterator it=x->begin(), iend=x->end(); it!=iend; ++it) {
      if (!first) {
        outs() << ", ";
      }
      else {
        first = false;
      }
      if (((BasicBlock*)*it)->hasName())
        outs() << ((BasicBlock *)*it)->getName();
      else
        ((BasicBlock *)*it)->printAsOperand(outs(), false);
    }
    outs() << "}\n";
  }

  void printSetValue(std::vector<void *> * x) {
    bool first = true;
    outs() << "{";

    for (std::vector<void *>::iterator it=x->begin(), iend=x->end(); it!=iend; ++it) {
      if (!first) {
        outs() << ", ";
      }
      else {
        first = false;
      }
      outs() << (getShortValueName((Value *)(*it)));
    }
    outs() << "}\n";
  }

  void printSetInstr(std::vector<void *> * x) {
    bool first = true;
    outs() << "{";

    for (std::vector<void *>::iterator it=x->begin(), iend=x->end(); it!=iend; ++it) {
      if (!first) {
        outs() << ", ";
      }
      else {
        first = false;
      }
      ((Instruction *)(*it))->print(outs());
    }
    outs() << "}\n";
  }

  void printString(std::vector<std::string> *x){
    bool first = true;
    outs() << "{";

    for (std::vector<std::string>::iterator it=x->begin(), iend=x->end(); it!=iend; ++it) {
      if (!first) {
        outs() << ", ";
      }
      else {
        first = false;
      }
      outs() << (*it);
    }
    outs() << "}\n";
  }

  void printBitVector(BitVector bv)
  {
      for(int i = 0 ; i<bv.size(); ++i)
      {
          outs() << (bv[i]);
      }
      outs() << "\n";
  }




}