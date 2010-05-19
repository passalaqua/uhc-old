%%[8
#include "../../rts.h"
%%]



%%[8

struct FrameMap
{
    int32_t NumRoots;               //< Number of roots in stack frame.
    int32_t NumMeta;                //< Number of metadata entries. May be < NumRoots.
    const void *Meta[0];            //< Metadata for each root.
};

struct StackEntry
{
    struct StackEntry *Next;        //< Link to next stack entry (the caller's).
    const struct FrameMap *Map;     //< Pointer to constant FrameMap.
    void *Roots[0];                 //< Stack roots (in-place array).
};

struct StackEntry *llvm_gc_root_chain;

%%]

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Internal functions
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Interface functions
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%[8
void mm_traceSupplyStack_llvm_Init( MM_TraceSupply* traceSupply, MM_Malloc* memmgt, MM_Mutator* mutator ) 
{
    printf("mm_traceSupplyStack_llvm_Init\n");

	MM_TraceSupply_Stack_Data* stackData = memmgt->malloc( sizeof(MM_TraceSupply_Stack_Data) ) ;
	stackData->trace = mutator->trace ;
	traceSupply->data = (MM_TraceSupply_Data_Priv*)stackData ;

}

void mm_traceSupplyStack_llvm_Reset( MM_TraceSupply* traceSupply, Word gcStackInfo ) 
{
    printf("mm_traceSupplyStack_llvm_Reset\n");

	MM_TraceSupply_Stack_Data* stackData = (MM_TraceSupply_Stack_Data*)traceSupply->data ;
	stackData->gcStackInfo = (GCStackInfo*)gcStackInfo ;

}

void mm_traceSupplyStack_llvm_Run( MM_TraceSupply* traceSupply )
{

    printf("mm_traceSupplyStack_llvm_Run\n");

	MM_TraceSupply_Stack_Data* stackData;
	MM_Trace *trace;

	stackData = (MM_TraceSupply_Stack_Data*) traceSupply->data ;
	trace     =                              stackData->trace;


    int32_t    i, num_roots;
    Word       *root;
    Word       con;
    struct StackEntry   *entry = llvm_gc_root_chain;

	
    printf("|*****************************************************************\n");
    printf("|** Running a stack walk \n");
    printf("| [0x%016llx] llvm_gc_root_chain\n", entry);
    
    while (entry)
    {
        num_roots = entry->Map->NumRoots;
        printf("| [0x%016llx] %d root(s)\n", entry, num_roots);
        for (i = 0; i < num_roots; i++)
        {
            root = (Word *) entry->Roots[i];
   
            if (root == NULL) {
                printf("| ... [%d] 0x%016llx\n", i, root );

            } else {
                printf("| ... [%d] 0x%016llx, con: %lld \n", i, root, *root );
           	    root = mm_Trace_TraceObject( trace, root );

            }

        }
        printf("\n");


        entry = entry->Next;
    }

    printf("|*****************************************************************\n");


    return;
    //exit(1);
}
%%]

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% MM_TraceSupply interface object
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%[8
MM_TraceSupply mm_traceSupplyStack_llvm =
	{ NULL
	, &mm_traceSupplyStack_llvm_Init
	, MM_Undefined
	, &mm_traceSupplyStack_llvm_Reset
	, &mm_traceSupplyStack_llvm_Run
	, MM_Undefined
	} ;
%%]
