%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Memory management: roots
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%[8
#include "../rts.h"
%%]

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Interface for global roots
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%[8
MM_FlexArray mm_Roots ;
%%]

%%[8
void mm_Roots_RegisterNWithFlag( WPtr toObj, HalfWord nr, MM_Trace_Flg flg ) {
	if ( nr > 0 ) {
		MM_FlexArray_Inx i = mm_flexArray_NewSlot( &mm_Roots ) ;
		MM_Roots_Entry* r = (MM_Roots_Entry*)mm_flexArray_At( &mm_Roots, i ) ;
		r->ptrToObj = toObj ;
		r->nrObjs = nr ;
		r->flags = flg ;
	}
}

%%]

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Interface for local roots
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%[8
MM_LclRoot_Grp*		mm_LclRoots = NULL ;
%%]

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Initialization
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%[8
void mm_init_roots() {
	mm_flexArray_New( &mm_malloc_LOF, &mm_Roots, sizeof(MM_Roots_Entry), 100, 0 ) ;
}
%%]
